/*-----------------------------------------------------------------------------------
File:			collisions.cpp
Authors:		Steve Costa
Description:	This class is used for calling the proper geometric tests to
				determine if there is a collision and then calling the correct
				collision reponse functions to determine the new state of
				the colliding objects.
-----------------------------------------------------------------------------------*/

#include "collisions.h"

#include "geoMath.h"						// Geometric math tests
using namespace geomath;

#include "physics.h"						// Physics calculations
using namespace physics;

#include "vector.h"							// Vector data type
using namespace vec;

#include "commonUtil.h"

/*-----------------------------------------------------------------------------------
The world object reference that is passed to the constructor is used to create
pointers to all the world object in the CWorld class so that their state variables
can be visible for performing geometric tests and applying physics responses.
-----------------------------------------------------------------------------------*/

CCollisions::CCollisions(CWorld &world)
{
	num_walls = world.num_walls;
	num_balls = world.num_balls;
	num_boxes = world.num_boxes;

	p_walls = world.p_walls;
	p_balls = world.p_balls;
	p_boxes = world.p_boxes;

	// Size array large enough for max # of simultaneous collisions
	p_cdata = new colldata[num_balls + num_boxes];
}

/*-----------------------------------------------------------------------------------
The collision tests performed return the time at which a collision will occur.  Each
frame is alotted a time value of 1.  All collision tests are performed and the
collision which occurs the soones and is less than 1 is chosen.  The simulation
advances forward in time until the first collision at which time the collision
response effects are calculated for the objects that have collided.  The initial
time value of 1 is decremented by that collision time (i.e. 1 - collision time where
collision time was 0.75 = 0.25 left).  If there is still time left the collision
tests are conducted again to see if there will be a collision within that time
step (i.e. 0.25) if there is (the first one of course) the same process of
advancing to that time and calculating the collision response is conducted.  Oncse
the time value reaches 0 no more collisions within the time interval will occur
and the simulation will advance to the next frame.  If no collisions occured
within 1 from the beginning, all the objects are simply advanced by this time
step.

Note: The value ZERO can be considered an 'epsilon' value.  Any collisions that
occur within 'epsilon' time of eachother are considered to be simultaneous
collisions.  (This has to be done to avoid rounding errors)
-----------------------------------------------------------------------------------*/

void CCollisions::Test(float dt)
{
	t_left = 1.0f;						// All time values normalized between 0 and 1
	
	while (t_left > 0.0f)
	{
		min_time = 1000.0f;
		num_sim_collisions = 0;

		TestBallBall(dt);				// Test for collisions between balls
		TestBallWall(dt);				// Test for collisions between balls and walls
		TestBoxWall(dt);				// Test for collisions between boxes and walls
		TestBoxBox(dt);					// Test for collisions between boxes
		TestBoxBall(dt);				// Test for collisions between boxes and balls
				
		if (num_sim_collisions)				// There was a collision
		{
			// Advance objects according to displacement vectors 
			// until first collision time 
			for (int i = 0; i < num_balls; i++)
			{
				p_balls[i].center += p_balls[i].vel * dt * min_time;
			}
			for (int i = 0; i < num_boxes; i++)
			{
				p_boxes[i].maxv += p_boxes[i].vel * dt * min_time;
				p_boxes[i].minv += p_boxes[i].vel * dt * min_time;
			}

			// Calculate collision reponse for any objects that may have collided
			// (there could be some "simultaneous" collisions so any that occur
			// at the same time are taken into account
			for (int i = 0; i < num_sim_collisions; i++)
			{
				if (p_cdata[i].collID == BALL_BALL_COLLISION)
					BallBallResponse(i);
				else if (p_cdata[i].collID == BALL_WALL_COLLISION)
					BallWallResponse(i);
				else if (p_cdata[i].collID == BOX_WALL_COLLISION)
					BoxWallResponse(i);
				else if (p_cdata[i].collID == BOX_BOX_COLLISION)
					BoxBoxResponse(i);
				else if (p_cdata[i].collID == BALL_BOX_COLLISION)
					BallBoxResponse(i);
			} // End for

			t_left -= min_time;
		} // End if (num_sim_collisions)
		else											// No more collisions
		{
			for (int i = 0; i < num_balls; i++)
			{
				p_balls[i].center += p_balls[i].vel * dt * t_left;
			}
			for (int i = 0; i < num_boxes; i++)
			{
				p_boxes[i].maxv += p_boxes[i].vel * dt * t_left;
				p_boxes[i].minv += p_boxes[i].vel * dt * t_left;
			}
			t_left = 0.0f;
		}
	}
}

/*-----------------------------------------------------------------------------------
Test for collisions between balls.
-----------------------------------------------------------------------------------*/

void CCollisions::TestBallBall(float dt)
{
	float temp_time;

	for (int t = 0; t < num_balls - 1; t++)
	{
		TVector ball_vel1 = p_balls[t].vel;
		float rad_1 = p_balls[t].radius;
		TVector center1 = p_balls[t].center;

		for (int i = t + 1; i < num_balls; i++)
		{
			TVector ball_vel2 = p_balls[i].vel;
			float rad_2 = p_balls[i].radius;
			TVector center2 = p_balls[i].center;

			// Get time of collision
			temp_time = IntersectBallBall(	center1, rad_1, ball_vel1 * dt, 
											center2, rad_2, ball_vel2 * dt);

			// Ensure collision is between 0 and t_left
			if (temp_time >= 0.0f && temp_time <= t_left)
			{
				// If collision is at the same time as another collision add it to the list
				if (abs(temp_time - min_time) <= ZERO)
				{
					num_sim_collisions++;
					p_cdata[num_sim_collisions - 1].collID = BALL_BALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
				// If collision is sooner than those previous
				else if (temp_time < min_time)
				{
					num_sim_collisions = 1;
					min_time = temp_time;
					p_cdata[num_sim_collisions - 1].collID = BALL_BALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
			} // End if		
		} // End for
	} // End for
}

/*-----------------------------------------------------------------------------------
Test for collisions between balls and walls
-----------------------------------------------------------------------------------*/

void CCollisions::TestBallWall(float dt)
{
	float temp_time;

	for (int t = 0; t < num_walls; t++)
	{
		TVector wall_point = p_walls[t].point1 * p_walls[t].trans;
		TVector wall_normal = p_walls[t].normal;
		
		for (int i = 0; i < num_balls; i++)
		{
			TVector ball_vel = p_balls[i].vel;
			float rad = p_balls[i].radius;
			TVector center = p_balls[i].center;

			// Check that the ball would make contact with the wall then check if it
			// will do so within the alloted time slice
			if (IsBallOnWall(p_balls[i], p_walls[t])) {
				temp_time = IntersectBallPlane(	center, rad, ball_vel * dt,
												wall_point, wall_normal);
			}
			else {
				temp_time = -1.0f;
			}

			// Ensure collision is between 0 and t_left
			if (temp_time >= 0.0f && temp_time <= t_left)
			{
				// If collision is at the same time as another collision add it to the list
				if (abs(temp_time - min_time) <= ZERO)
				{
					num_sim_collisions++;
					p_cdata[num_sim_collisions - 1].collID = BALL_WALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = i;
					p_cdata[num_sim_collisions - 1].object2 = t;
				}
				// If collision is sooner than those previous
				else if (temp_time < min_time)
				{
					num_sim_collisions = 1;
					min_time = temp_time;
					p_cdata[num_sim_collisions - 1].collID = BALL_WALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = i;
					p_cdata[num_sim_collisions - 1].object2 = t;
				}
			} // End if		
		} // End for
	} // End for
}

/*-----------------------------------------------------------------------------------
Test for collisions between boxes and walls
-----------------------------------------------------------------------------------*/

void CCollisions::TestBoxWall(float dt)
{
	float temp_time;

	for (int t = 0; t < num_boxes; t++)
	{
		TVector box_min = p_boxes[t].minv;
		TVector box_max = p_boxes[t].maxv;
		TVector box_vel = p_boxes[t].vel;
		for (int i = 0; i < num_walls; i++)
		{
			TVector wall_point = p_walls[i].point1 * p_walls[i].trans;
			TVector wall_normal = p_walls[i].normal;
			wall_normal.Normalize();

			// Check that the box would make contact with the wall then check if it
			// will do so within the alloted time slice
			if (IsBoxOnWall(p_boxes[t], p_walls[i])) {
				temp_time = IntersectBoxPlane(	box_min, box_max, box_vel * dt,
												wall_point, wall_normal);
			}
			else {
				temp_time = -1.0f;
			}

			if (temp_time >= 0.0f && temp_time <= t_left)
			{
				// If collision is at the same time as another collision add it to the list
				if (abs(temp_time - min_time) <= ZERO)
				{
					num_sim_collisions++;
					p_cdata[num_sim_collisions - 1].collID = BOX_WALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
				// If collision is sooner than those previous
				else if (temp_time < min_time)
				{
					num_sim_collisions = 1;
					min_time = temp_time;
					p_cdata[num_sim_collisions - 1].collID = BOX_WALL_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
			} // End if		
		} // End for
	} // End for
}

/*-----------------------------------------------------------------------------------
Test for collisions between boxes
-----------------------------------------------------------------------------------*/

void CCollisions::TestBoxBox(float dt)
{
	float temp_time;

	for (int t = 0; t < num_boxes - 1; t++)
	{
		TVector box_min1 = p_boxes[t].minv;
		TVector box_max1 = p_boxes[t].maxv;
		TVector box_vel1 = p_boxes[t].vel;

		for (int i = t + 1; i < num_boxes; i++)
		{
			TVector box_min2 = p_boxes[i].minv;
			TVector box_max2 = p_boxes[i].maxv;
			TVector box_vel2 = p_boxes[i].vel;

			temp_time = IntersectBoxBox(	box_min1, box_max1, box_vel1 * dt,
											box_min2, box_max2, box_vel2 * dt);

			if (temp_time >= 0.0f && temp_time <= t_left)
			{
				// If collision is at the same time as another collision add it to the list
				if (abs(temp_time - min_time) <= ZERO)
				{
					num_sim_collisions++;
					p_cdata[num_sim_collisions - 1].collID = BOX_BOX_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
				// If collision is sooner than those previous
				else if (temp_time < min_time)
				{
					num_sim_collisions = 1;
					min_time = temp_time;
					p_cdata[num_sim_collisions - 1].collID = BOX_BOX_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = t;
					p_cdata[num_sim_collisions - 1].object2 = i;
				}
			} // End if		
		} // End for
	} // End for
}

/*-----------------------------------------------------------------------------------
Test for collisions between boxes and balls.  In order to achieve simmulation
collision detection where we obtain the precise time of collision the box
is decomposed into triangles and each triangle is tested for a collision
-----------------------------------------------------------------------------------*/

void CCollisions::TestBoxBall(float dt)
{
	float temp_time;
	float t_min;								// Find fastest time
	TVector vert[3];							// 3 vertices of a triangle
	TVector ep1, ep2;							// Edge vertices
	bool v_collision;							// true if vertex collision
	bool edge_collision = false;				// Used when balls hit edges of blocks
	bool top_collision = false;					// When balls hit top of blocks

	for (int t = 0; t < num_boxes; t++)
	{
		TVector box_min = p_boxes[t].minv;
		TVector box_max = p_boxes[t].maxv;
		TVector box_vel = p_boxes[t].vel;

		for (int i = 0; i < num_balls; i++)
		{
			TVector ball_vel = p_balls[i].vel;
			float rad = p_balls[i].radius;
			TVector center = p_balls[i].center;

			// We need to test for collision with the 4 sides of the cube and 
			// the top, since balls will never make contact with the bottom.
			// Each side will be composed of 2 triangles, resulting in 8 tests
			// the smallest positive time value will be the resultant time of
			// collision

			temp_time = 1000.0f;
			
			// Front Face Triangle 1
			vert[0] = p_boxes[t].GetVertex(4);
			vert[1] = p_boxes[t].GetVertex(5);
			vert[2] = p_boxes[t].GetVertex(6);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Front Face Triangle 2
			vert[0] = p_boxes[t].GetVertex(5);
			vert[1] = p_boxes[t].GetVertex(7);
			vert[2] = p_boxes[t].GetVertex(6);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Left Face Triangle 1
			vert[0] = p_boxes[t].GetVertex(6);
			vert[1] = p_boxes[t].GetVertex(0);
			vert[2] = p_boxes[t].GetVertex(2);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Left Face Triangle 2
			vert[0] = p_boxes[t].GetVertex(6);
			vert[1] = p_boxes[t].GetVertex(0);
			vert[2] = p_boxes[t].GetVertex(4);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Right Face Triangle 1
			vert[0] = p_boxes[t].GetVertex(1);
			vert[1] = p_boxes[t].GetVertex(3);
			vert[2] = p_boxes[t].GetVertex(5);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Right Face Triangle 2
			vert[0] = p_boxes[t].GetVertex(5);
			vert[1] = p_boxes[t].GetVertex(3);
			vert[2] = p_boxes[t].GetVertex(7);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Top Face Triangle 1
			vert[0] = p_boxes[t].GetVertex(7);
			vert[1] = p_boxes[t].GetVertex(3);
			vert[2] = p_boxes[t].GetVertex(2);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f) {
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
			}
			
			// Top Face Triangle 2
			vert[0] = p_boxes[t].GetVertex(6);
			vert[1] = p_boxes[t].GetVertex(7);
			vert[2] = p_boxes[t].GetVertex(2);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f) {
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
			}
			
			// Back Face Triangle 1
			vert[0] = p_boxes[t].GetVertex(2);
			vert[1] = p_boxes[t].GetVertex(3);
			vert[2] = p_boxes[t].GetVertex(0);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
			if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			// Back Face Triangle 2
			vert[0] = p_boxes[t].GetVertex(3);
			vert[1] = p_boxes[t].GetVertex(1);
			vert[2] = p_boxes[t].GetVertex(0);

			t_min = IntersectBallTriangle(	center, ball_vel * dt, rad,
											box_vel * dt, vert, 1.0f, 
											v_collision, ep1, ep2);
			
            if (t_min < temp_time && t_min >= 0.0f)
			{
				temp_time = t_min;
				tri_coords[0] = vert[0]; tri_coords[1] = vert[1]; tri_coords[2] = vert[2];
				edge_collision = v_collision;
			}

			if (temp_time >= 0.0f && temp_time <= t_left)
			{
				// If collision is at the same time as another collision add it to the list
				if (abs(temp_time - min_time) <= ZERO)
				{
					num_sim_collisions++;
					p_cdata[num_sim_collisions - 1].collID = BALL_BOX_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = i;
					p_cdata[num_sim_collisions - 1].object2 = t;
					p_cdata[num_sim_collisions - 1].edge_collision = edge_collision;
					p_cdata[num_sim_collisions - 1].v1 = tri_coords[0];
					p_cdata[num_sim_collisions - 1].v2 = tri_coords[1];
					p_cdata[num_sim_collisions - 1].v3 = tri_coords[2];
					p_cdata[num_sim_collisions - 1].edge_p1 = ep1;
					p_cdata[num_sim_collisions - 1].edge_p2 = ep2;

				}
				// If collision is sooner than those previous
				else if (temp_time < min_time)
				{
					num_sim_collisions = 1;
					min_time = temp_time;
					p_cdata[num_sim_collisions - 1].collID = BALL_BOX_COLLISION;
					p_cdata[num_sim_collisions - 1].object1 = i;
					p_cdata[num_sim_collisions - 1].object2 = t;
					p_cdata[num_sim_collisions - 1].edge_collision = edge_collision;
					p_cdata[num_sim_collisions - 1].v1 = tri_coords[0];
					p_cdata[num_sim_collisions - 1].v2 = tri_coords[1];
					p_cdata[num_sim_collisions - 1].v3 = tri_coords[2];
					p_cdata[num_sim_collisions - 1].edge_p1 = ep1;
					p_cdata[num_sim_collisions - 1].edge_p2 = ep2;
				}
			} // End if		
			
		} // End for
	}
}

/*-----------------------------------------------------------------------------------
Apply collision response between two balls
-----------------------------------------------------------------------------------*/

void CCollisions::BallBallResponse(int i)
{
	// Apply collision effects to both balls
	float mass1 = 1.0f, mass2 = 1.0f;
	int ball1_id = p_cdata[i].object1;
	int ball2_id = p_cdata[i].object2;

	TVector vel1 = p_balls[p_cdata[i].object1].vel;
	TVector vel2 = p_balls[p_cdata[i].object2].vel;

	MObjMObjEffects(vel1, p_balls[ball1_id].center,
					vel2, p_balls[ball2_id].center);

	// With the new velocity vectors we can adjust the
	// speed, and axis of rotation for each ball
	p_balls[ball1_id].vel = vel1;
	p_balls[ball1_id].axis.x = vel1.z;
	p_balls[ball1_id].axis.z = -vel1.x;
	p_balls[ball1_id].axis.Normalize();
					
	p_balls[ball2_id].vel = vel2;
	p_balls[ball2_id].axis.x = vel2.z;
	p_balls[ball2_id].axis.z = -vel2.x;
	p_balls[ball2_id].axis.Normalize();
}

/*-----------------------------------------------------------------------------------
Apply collision response between ball and wall
-----------------------------------------------------------------------------------*/

void CCollisions::BallWallResponse(int i)
{
	int ball_id = p_cdata[i].object1;
	int wall_id = p_cdata[i].object2;

	TVector norm = p_walls[wall_id].normal;
	TVector initial_vec = p_balls[ball_id].vel;
	
	MObjSObjEffects(initial_vec, norm, 0.5f);
	
	// We now change the direction that the ball is moving in
	// we do not change speed (we must also change the axis of rotation)
	p_balls[ball_id].vel = initial_vec;
	p_balls[ball_id].axis.x = initial_vec.z;
	p_balls[ball_id].axis.z = -initial_vec.x;
	p_balls[ball_id].axis.Normalize();
}

/*-----------------------------------------------------------------------------------
Apply collision response between box and wall
-----------------------------------------------------------------------------------*/

void CCollisions::BoxWallResponse(int i)
{
	int box_id = p_cdata[i].object1;
	int wall_id = p_cdata[i].object2;

	TVector initial_vec = p_boxes[box_id].vel;
	TVector norm = p_walls[wall_id].normal;

	MObjSObjEffects(initial_vec, norm, 0.5f);
	
	// We now change the direction that the box is moving in
	// we do not change speed
	p_boxes[box_id].vel = initial_vec;
}

/*-----------------------------------------------------------------------------------
Apply collision response between boxes
-----------------------------------------------------------------------------------*/

void CCollisions::BoxBoxResponse(int i)
{
	// Apply collision effects to both boxes
	float mass1 = 1.0f, mass2 = 1.0f;
	int box1_id = p_cdata[i].object1;
	int box2_id = p_cdata[i].object2;

	TVector vel1 = p_boxes[box1_id].vel;
	TVector vel2 = p_boxes[box2_id].vel;

	TVector center1 = (p_boxes[box1_id].maxv + p_boxes[box1_id].minv) * 0.5f;
	TVector center2 = (p_boxes[box2_id].maxv + p_boxes[box2_id].minv) * 0.5f;

	MObjMObjEffects2(vel1, center1, vel2, center2);

	// With the new velocity vectors we can adjust the
	// direction and speed of the boxes
	p_boxes[box1_id].vel = vel1;					
	p_boxes[box2_id].vel = vel2;
}

/*-----------------------------------------------------------------------------------
Apply collision response between ball and box
-----------------------------------------------------------------------------------*/

void CCollisions::BallBoxResponse(int i)
{
	float mass1 = 1.0f, mass2 = 1.0f;
	int ball_id = p_cdata[i].object1;
	int box_id = p_cdata[i].object2;

	TVector vel1 = p_balls[ball_id].vel;
	TVector vel2 = p_boxes[box_id].vel;

	TVector center1 = p_balls[ball_id].center;

	if (p_cdata[i].edge_collision) {
		// We must first project the centre of the box onto the edge of collision
		TVector v = p_balls[ball_id].center - p_cdata[i].edge_p1;		// Vector between edge base and center
		TVector edge = p_cdata[i].edge_p2 - p_cdata[i].edge_p1;			// Edge vector
		float proj = v * edge;											// v projected on edge
		TVector edge_point = p_cdata[i].edge_p1 + proj * Normalized(edge);

		// Now we find the collision normal
		TVector n_col = p_balls[ball_id].center - edge_point;
		n_col.Normalize();

		// Move the ball a little bit away
		p_balls[ball_id].center += (n_col * 0.001f);

		// If the dot product of the normal of collision and the
		// vector of the ball is less then 0, the objects are in compacted
		float vb = p_balls[ball_id].vel * n_col;

		if (vb < 0.0f)
			p_balls[ball_id].vel += (-2.0f * vb) * n_col;

		n_col *= -1.0f;

		// Move the box a little bit away
		p_boxes[box_id].minv += (n_col * 0.001f);

		vb = p_boxes[box_id].vel * n_col;

		if (vb < 0.0f)
			p_boxes[box_id].vel += (-2.0f * vb) * n_col;
	}
	else {
		// Simmulate a collision between the 
		// ball and the box, we can't use the center of the box
		// we must use the closest point on the box to the center
		// of the sphere so that the function will calculate an
		// axis that is orthogonal to the wall of the box
		TVector v1 = p_cdata[i].v1 - p_cdata[i].v2;
		TVector v2 = p_cdata[i].v2 - p_cdata[i].v3;
		TVector n = CrossProduct(v1, v2);
		n.Normalize();

		// project center of ball onto plane
		TVector center2 = center1 + (-1 * n * (center1 - tri_coords[0])) * n;

		MObjMObjEffects(vel1, center1, vel2, center2);
		
		// With the new velocity vectors we can adjust the
		// direction, speed, and axis of rotation for the ball
		// and the direction and speed of the box
		p_balls[ball_id].vel = vel1;
		p_balls[ball_id].axis.x = p_balls[ball_id].vel.z;
		p_balls[ball_id].axis.y = 0.0f;
		p_balls[ball_id].axis.z = -p_balls[ball_id].vel.x;
		if (Magnitude(p_balls[ball_id].axis) > 0.0f) {
			p_balls[ball_id].axis.Normalize();
		}
		else {
			p_balls[ball_id].axis.x = -1.0f;
			p_balls[ball_id].axis.y = 0.0f;
			p_balls[ball_id].axis.z = 0.0f;
		}
						
		p_boxes[box_id].vel = vel2;
	} // End else
}

CCollisions::~CCollisions()
{
	delete [] p_cdata;
}