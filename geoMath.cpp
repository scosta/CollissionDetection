/*-----------------------------------------------------------------------------------
File:			geoMath.cpp
Authors:		Steve Costa
Description:	Geometric test functions for determining collisions between
				different 3D objects.
-----------------------------------------------------------------------------------*/

#include "geoMath.h"

#include "commonUtil.h"

/*-----------------------------------------------------------------------------------
Dynamic test for intersection between the plane and a sphere given a plane
and sphere data type.
t = (d_ - (c.n) + r) / (d.n)
where:  d_= point.normal for plane
		c = center of sphere
		r = radius of sphere
		n = normal of the plane (normalized)
		d = direction vector of motion for sphere (normalized)
Returns time at which sphere and plane intersect.  Returns a negative number
if there is no intersection.
-----------------------------------------------------------------------------------*/

float geomath::IntersectBallPlane(const TVector& ball_center,
								  float ball_radius,
								  const TVector& ball_vel,
								  const TVector& plane_point,
								  const TVector& plane_norm)
{
	// Ensure that the plane normal is normalized
	assert(fabs(plane_norm * plane_norm - 1.0) < 0.01f);

	// Distance from plane to origin
	float d_ = plane_point * plane_norm;

	// Direction of motion
	TVector d = Normalized(ball_vel);

	// Calculate denominator for equation
	// If the result is 0 then the plane normal and sphere trajectory are perpindicular
	// If the result is > 0 the sphere is travelling away from the wall
	float denominator = d * plane_norm;
	if (denominator >= 0.0f) return -1.0f;

	// Calculate time before collision
	float t = (d_ - (ball_center * plane_norm) + ball_radius) / denominator;

	// We used a normalized velocity vector for the test, we must scale t further
	// by deviding by the magnitude of the actual velocity vector
	return t / Magnitude(ball_vel);
}

/*-----------------------------------------------------------------------------------
Once a collision with a wall surface is established, there must be a check done
to see if that collision was within the wall coordinates.
If the dot product between the normal of a vertex and the the vector between the
vertex and the point is < 0 for any of the vertices, the point is outside the
square.
-----------------------------------------------------------------------------------*/

bool geomath::IsBallOnWall(	const TBall& ball, const TWall& wall)
{
	// Transform from local to world coordinates
	TVector coords[4];
	coords[0] = (wall.GetVertex(0) * wall.trans);
	coords[1] = (wall.GetVertex(1) * wall.trans);
	coords[2] = (wall.GetVertex(3) * wall.trans);
	coords[3] = (wall.GetVertex(2) * wall.trans);

	// Project the center of the ball onto the wall plane
	TVector c = ball.center;
	TVector point = c + (-1 * wall.normal * (c - coords[0])) * wall.normal;

	int i = 3;

	for (int j = 0; j < 4; i = j, j++)
	{
		TVector E = coords[j] - coords[i];
		TVector E_normal = CrossProduct(wall.normal, E);
		TVector D = point - coords[i];
		if (D * E_normal < 0.0f)
			return false;
	}
	return true;
}

/*-----------------------------------------------------------------------------------
Project the min and max coordinates onto the plane of the wall and test to see
if either of those projected coordinates are within the confines of the wall.
-----------------------------------------------------------------------------------*/

bool geomath::IsBoxOnWall(const TBox& box, const TWall& wall)
{
	bool p1_hitwall = true, p2_hitwall = true;

	// Transform from local to world coordinates
	TVector coords[4];
	coords[0] = (wall.GetVertex(0) * wall.trans);
	coords[1] = (wall.GetVertex(1) * wall.trans);
	coords[2] = (wall.GetVertex(3) * wall.trans);
	coords[3] = (wall.GetVertex(2) * wall.trans);

	// Project min vertex onto the wall
	TVector c = box.minv;
	TVector point = c + (-1 * wall.normal * (c - coords[0])) * wall.normal;

	// Test if it is within wall limits
	int i = 3;

	for (int j = 0; j < 4; i = j, j++)
	{
		TVector E = coords[j] - coords[i];
		TVector E_normal = CrossProduct(wall.normal, E);
		TVector D = point - coords[i];
		if (D * E_normal < 0.0f)
			p1_hitwall = false;
	}

	// Project max vertex onto the wall
	c = box.maxv;
	point = c + (-1 * wall.normal * (c - coords[0])) * wall.normal;

	// Test if it is within wall limits
	i = 3;

	for (int j = 0; j < 4; i = j, j++)
	{
		TVector E = coords[j] - coords[i];
		TVector E_normal = CrossProduct(wall.normal, E);
		TVector D = point - coords[i];
		if (D * E_normal < 0.0f)
			p2_hitwall = false;
	}

	return (p1_hitwall || p2_hitwall);
}

/*-----------------------------------------------------------------------------------
Dynamic test for intersection between two spheres
t = e.d - sqrt((e.d)^2 + r^2 - e.e)
where:	e = vector from sphere2 center to sphere1 center
		d = direction sphere2 is travelling in
		dn = direction sphere2 is travelling in (normalized)
		r = sum or radii of two spheres
		t = time at which collision occurs
Returns the time at which the spheres intersect, if there is no intersection
the time value is negative.
-----------------------------------------------------------------------------------*/

float geomath::IntersectBallBall(	const TVector& ball_center1,
									float ball_radius1,
									const TVector& ball_vel1,
									const TVector& ball_center2,
									float ball_radius2,
									const TVector& ball_vel2)
{
	float r = ball_radius1 + ball_radius2;				// Sum of radii

	TVector e(	ball_center1.x - ball_center2.x,		// Vector between centres
				ball_center1.y - ball_center2.y,
				ball_center1.z - ball_center2.z);
				
	// ball 1 is treated as if it is stationary and ball 2 has a combined vel vector
	TVector d = ball_vel2 - ball_vel1;			// Velocity vector
	TVector dn = Normalized(d);					// Normalized velocity vector
	float dm = Magnitude(d);					// Magnitude of velocity vector

	if (dm > r) return -1.0f;				// No intersection

	float eq1 = (e * dn) * (e * dn) + r * r - (e * e);

	if (eq1 < 0) return -1.0f;				// No intersection

	float t = (e * dn) -  sqrt(eq1);

	// We used a normalized velocity vector for the test, we must scale t further
	// by deviding by the magnitude of the actual velocity vector
	return t / dm;
}

/*-----------------------------------------------------------------------------------
Dynamic test for intersection between a box and a plane.
Return the time at which the collision occurs (between 0 and 1).
A negative number is returned if there is no collision.
-----------------------------------------------------------------------------------*/

float geomath::IntersectBoxPlane(const TVector& box_min, const TVector& box_max, 
								 const TVector& box_vel, const TVector& wall_point,
								 const TVector& wall_normal)
{
	// Ensure that the plane normal is normalized
	assert(fabs(wall_normal * wall_normal - 1.0) < 0.01f);

	// Compute the normalized vector of the box velocity
	TVector dn = Normalized(box_vel);

	// Compute distance from plane to origin
	float plane_distance = wall_normal * wall_point;

	// Compute glancing angle, if 0 or positive then
	// it is travelling parallel to plane or away from it
	float theta = wall_normal * dn;
	if (theta >= 0.0f) return -1.0f;		// No collision

	// Compute min and max D values

	float minD, maxD;

	if (wall_normal.x > 0.0f) {
		minD = wall_normal.x * box_min.x;
		maxD = wall_normal.x * box_max.x;
	} else {
		minD = wall_normal.x * box_max.x;
		maxD = wall_normal.x * box_min.x;
	}

	if (wall_normal.y > 0.0f) {
		minD += wall_normal.y * box_min.y;
		maxD += wall_normal.y * box_max.y;
	} else {
		minD += wall_normal.y * box_max.y;
		maxD += wall_normal.y * box_min.y;
	}

	if (wall_normal.z > 0.0f) {
		minD += wall_normal.z * box_min.z;
		maxD += wall_normal.z * box_max.z;
	} else {
		minD += wall_normal.z * box_max.z;
		maxD += wall_normal.z * box_min.z;
	}

	// If maxD <= distance from plance to origin, it is on other side
	if (maxD <= plane_distance) return -1.0f;

	// Using the maximum coordinate of the bounding box
	// use raytracing to find time of intersection
	float t = (plane_distance - minD) / theta;

	// If t is negative, collision has already occurred
	if (t < 0.0f) return 0.0f;

	// We used a normalized velocity vector for the test, we must scale t further
	// by deviding by the magnitude of the actual velocity vector
	return t / Magnitude(box_vel);
}

/*-----------------------------------------------------------------------------------
Dynamic test for intersection between two boxes.
Return the time at which the collision occurs (between 0 and 1).
A negative number is returned if there is no collision.
-----------------------------------------------------------------------------------*/

float geomath::IntersectBoxBox(	const TVector& box_min1, const TVector& box_max1, 
								const TVector& box_vel1, const TVector& box_min2,
								const TVector& box_max2, const TVector& box_vel2)
{
	// We are considering time interval of intersection between 0 and 1
	float t_enter = 0.0f;
	float t_exit = 1.0f;

	// Assume that box1 is stationary and box2 is moving
	TVector d = box_vel2 - box_vel1;

	// Check for an overlap between the two boxes in each of the
	// three dimensions.  The enter and exit times are updated
	// until and enter and exit value is found satisfying an
	// intersection in all three dimesions.  If the enter time
	// is greater than the exit time, then there is no intersection

	// x-axis test

	if (d.x == 0.0f)		// No movement along x
	{
		if ((box_min1.x >= box_max2.x) || (box_max1.x <= box_min2.x))
			return -1.0f;	// No intersection
	}
	else
	{
		float d_flipped = 1.0f / d.x;

		// Compute time of start and end of intersection
		float x_enter = (box_min1.x - box_max2.x) * d_flipped;
		float x_exit = (box_max1.x - box_min2.x) * d_flipped;

		// If enter is greater than swap values
		float temp;
		if (x_enter > x_exit) SWAP(x_enter, x_exit, temp);

		// Shorten the time interval
		if (x_enter > t_enter) t_enter = x_enter;
		if (x_exit < t_exit) t_exit = x_exit;

		// If enter is greater than exit there is no intersection
		if (t_enter > t_exit) return -1.0f;
	}

	// y-axis test

	if (d.y == 0.0f)		// No movement along x
	{
		if ((box_min1.y >= box_max2.y) || (box_max1.y <= box_min2.y))
			return -1.0f;	// No intersection
	}
	else
	{
		float d_flipped = 1.0f / d.y;

		// Compute time of start and end of intersection
		float y_enter = (box_min1.y - box_max2.y) * d_flipped;
		float y_exit = (box_max1.y - box_min2.y) * d_flipped;

		// If enter is greater than exit, swap values
		float temp;
		if (y_enter > y_exit) SWAP(y_enter, y_exit, temp);

		// Shorten the time interval
		if (y_enter > t_enter) t_enter = y_enter;
		if (y_exit < t_exit) t_exit = y_exit;

		// If enter is greater than exit there is no intersection
		if (t_enter > t_exit) return -1.0f;
	}

	// z-axis test

	if (d.z == 0.0f)		// No movement along x
	{
		if ((box_min1.z >= box_max2.z) || (box_max1.z <= box_min2.z))
			return -1.0f;	// No intersection
	}
	else
	{
		float d_flipped = 1.0f / d.z;

		// Compute time of start and end of intersection
		float z_enter = (box_min1.z - box_max2.z) * d_flipped;
		float z_exit = (box_max1.z - box_min2.z) * d_flipped;

		// If enter is greater than exit, swap values
		float temp;
		if (z_enter > z_exit) SWAP(z_enter, z_exit, temp);

		// Shorten the time interval
		if (z_enter > t_enter) t_enter = z_enter;
		if (z_exit < t_exit) t_exit = z_exit;

		// If enter is greater than exit there is no intersection
		if (t_enter > t_exit) return -1.0f;
	}

	// Return time at which intersection occurs
	if (t_enter > 0.0f)
		return t_enter;
	else
		return -1.0f;
}

/*-----------------------------------------------------------------------------------
The following methods are adapted from code presented by Olivier Renault in
http://www.gamedev.net on how to determing the time at which a collision
between a sphere and a triangle occurs.
-----------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------
Test for triangle and sphere collision and return time of collision
-----------------------------------------------------------------------------------*/

float geomath::IntersectBallTriangle(const TVector& center, const TVector& ball_vel,
									 float radius, const TVector& tri_vel,
									 const TVector* vertices, float t_left, 
									 bool& edge_collision, TVector& edge_p1, TVector& edge_p2)
{
	// Assume that the triangle is stationary and ball is moving
	TVector velocity = ball_vel - tri_vel;

	// Calculate triangle normal
	TVector vec1 = vertices[1] - vertices[0];
	TVector vec2 = vertices[2] - vertices[1];
	TVector normal = CrossProduct(vec1, vec2);
	normal.Normalize();

	// Get time of collision between sphere and plane
	float t = IntersectBallPlane(center, radius, velocity, vertices[0], normal);

	// Calculate the point of collision on the plane
	TVector point = center + velocity * t;

	// Check to see that this point is in the triangle if so then there is a collision
	if (IsPointInTriangle(point, normal, vertices)) {
		edge_collision = false;
		return t;
	}

	edge_collision = true;

	// Check for collision time with edges and vertices and return smallest collision time
	// as well as the two edge vertices involved in the collision
	t = 1000.0f;
	float t_temp;

	t_temp = IntersectBallEdge(center, velocity, radius, vertices[0], vertices[1], true, t_left);
	if (t_temp < t && t_temp >= 0.0f)
	{
		t = t_temp;
		edge_p1 = vertices[0];
		edge_p2 = vertices[1];
	}

	t_temp = IntersectBallEdge(center, velocity, radius, vertices[1], vertices[2], true, t_left);
	if (t_temp < t && t_temp >= 0.0f)
	{
		t = t_temp;
		edge_p1 = vertices[1];
		edge_p2 = vertices[2];
	}

	t_temp = IntersectBallEdge(center, velocity, radius, vertices[2], vertices[0], true, t_left);
	if (t_temp < t && t_temp >= 0.0f)
	{
		t = t_temp;
		edge_p1 = vertices[2];
		edge_p2 = vertices[0];
	}

	return t;
}

/*-----------------------------------------------------------------------------------
This test is used to verify if a point lies within a triangle.
-----------------------------------------------------------------------------------*/

bool geomath::IsPointInTriangle(const TVector& point, const TVector& normal,
								const TVector* vertices)
{
	int i = 2;

	for (int j = 0; j < 3; i = j, j++)
	{
		TVector E = vertices[j] - vertices[i];
		TVector E_normal = CrossProduct(normal, E);
		TVector D = point - vertices[i];
		if (D * E_normal < 0.0f)
			return false;
	}
	return true;
}

/*-----------------------------------------------------------------------------------
Edge (epoint1, epoint2) vs Swept Sphere (center, velocity, radius)
ray equation: point = center + velocity * time, where 0 <= time <= t_left
cylinder equation: ((point - O) x L)^2 = r^2
where L = (epoint2 - epoint1) / |epoint2 - epoint1|, O = epoint1

substituting ray equation in cylinder equation:
((center - O) x L + (velocity x L) * time)^2 = r^2

this gives us a 2ODE: a*t^2 + b*t + c = 0
a = (V x L) * (V x L)
b = 2 * (V x L) * ((C - O) x L)
c = ((C - O) x L) * ((C - O) x L) - r * r
-----------------------------------------------------------------------------------*/

float geomath::IntersectBallEdge(const TVector& center, const TVector& velocity, float radius,
								 const TVector& point1, const TVector& point2, bool test_vertices,
								 float t_left)
{
	// Set up the equation
	TVector D = center - point1;
	TVector L = point2 - point1;
	float l2 = L * L;
	TVector VxL = CrossProduct(velocity, L);
	TVector DxL = CrossProduct(D, L);
	float a = (VxL * VxL);
	float b = (DxL * VxL) * 2.0f;
	float c = (DxL * DxL) - (radius * radius * l2);

	// If b > 0, object are separating
	if (b > 0.0f) return -1.0f;

	// If c < 0, there is a collision at t = 0
	// Check if a test with end points is required if not return 0
	float t = 0.0f;
	if (c > 0.0f)
		t = Solve2ODE(a, b, c, t_left);
	
	// In case of no intersection
	if (t < 0.0f) return -1.0f;

	// Get collision point
	TVector col_point = center + velocity * t;

	// Check how far we are on the edge at time of collision
	D = col_point - point1;
	float l = (D * L) / l2;

	// If we're before the start vertex, we need to test with start vertex
	if (l < 0.0f)
	{
		if (test_vertices) return IntersectBallVertex(center, velocity, radius, point1, t_left);
		else return -1.0f;
	}

	// If we're after the end vertex, need to test for collision with end vertex
	if (l > 1.0f)
	{
		if (test_vertices) return IntersectBallVertex(center, velocity, radius, point2, t_left);
		else return -1.0f;
	}

	// Collision with the edge
	return t;
}

/*-----------------------------------------------------------------------------------
This method is used for solving second order differential equations of the form
a*t^2 + b*t + c = 0
The resulting roots are calculated as follows:
t = -b +/- sqrt(b^2 - 4*a*c) / (2 * a)
-----------------------------------------------------------------------------------*/

float geomath::Solve2ODE(float a, float b, float c, float t_left)
{
	// Calculate portion to square root
	float d = b * b - 4 * a * c;

	// A negative value being square rooted results in no root
	if (d < 0.0f) return -1.0f;

	// Calculate the two roots
	d = sqrt(d);
	float den = 1.0f / (2.0f * a);

	float t0 = (-b - d) * den;
	float t1 = (-b + d) * den;

	// Return the first positive root
	float temp;
	if (t0 < t1) SWAP(t0, t1, temp);
	if (t1 < 0.0f || t0 > t_left) return -1.0f;
	if (t0 > 0.0f) return t0;

	return t1;
}

/*-----------------------------------------------------------------------------------
Vertex (vertex) vs swept sphere (center, velocity, radius)

ray equation: point = center + velocity * time, where 0 <= time <= t_left
sphere equation: (point - vertex)^2 = r^2

subsituting ray equation into sphere:
((center - vertex) + velocity * time)^2 = r^2

this gives us a 2ODE: a*t^2 + b*t + c = 0
a = velocity * velocity
b = 2 * velocity * (center - vertex)
c = (center - vertex) * (center - vertex) - radius^2
-----------------------------------------------------------------------------------*/

float geomath::IntersectBallVertex(const TVector& center, const TVector& velocity,
								   float radius, const TVector& vertex, float t_left)
{
	// Set up the 20DE
	TVector D = center - vertex;
	float a = velocity * velocity;
	float b = 2.0f * (velocity * D);
	float c = D * D - radius * radius;

	// If b > 0 no collision
	if (b > 0.0f) return -1.0f;

	// If c < 0, there is a collision at t = 0
	if (c < 0.0f) return 0.0f;

	// Solve the roots to get time of intersection
	float t = Solve2ODE(a, b, c, t_left);

	// If t < 0 there is no intersection
	if (t < 0.0f) return -1.0f;

	return t;
}