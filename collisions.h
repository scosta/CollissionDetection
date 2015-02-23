/*-----------------------------------------------------------------------------------
File:			collisions.h
Authors:		Steve Costa
Description:	Header file defining collision management class.
-----------------------------------------------------------------------------------*/

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "world.h"

/*-----------------------------------------------------------------------------------
Constants
-----------------------------------------------------------------------------------*/

#define NO_COLLISION				0
#define BALL_BALL_COLLISION			1
#define BALL_WALL_COLLISION			2
#define BOX_WALL_COLLISION			3
#define BOX_BOX_COLLISION			4
#define BALL_BOX_COLLISION			5

class CCollisions
{
	// ATTRIBUTES
public:

	// Structure for collision data
	struct colldata
	{
		int collID;					// ID of type of collision
        int object1;
		int object2;
		bool edge_collision;
		TVector v1, v2, v3;
		TVector edge_p1, edge_p2;
	};
	
private:

	int num_walls;					// Number of walls
	int num_balls;					// Number of balls
	int num_boxes;					// Number of boxes

	TWall *p_walls;					// Declare walls
	TBall *p_balls;					// Declare balls
	TBox *p_boxes;					// Declare boxes

	int num_sim_collisions;			// Number of simultaneous collisions
	colldata *p_cdata;				// Collision information

	float min_time;					// Time of earliest collision
	float t_left;					// Each frame has time slice which decrements to 0 (starts at 1.0)
	
	TVector tri_coords[3];			// Keep track of the coordinates of triangle collided with

	// METHODS
public:

	CCollisions(CWorld& world);
	void Test(float dt);			// Test collisions between all objects
	~CCollisions();

private:

	void TestBallBall(float dt);	// Test for collisions between balls
	void TestBallWall(float dt);	// Test for collisions between balls and walls
	void TestBoxWall(float dt);		// Test for collisions between boxes and walls
	void TestBoxBox(float dt);		// Test for collisions between boxes
	void TestBoxBall(float dt);		// Test for collisions between boxes and balls

	void BallBallResponse(int i);	// Collision response between balls
	void BallWallResponse(int i);	// Collision response between balls and walls
	void BoxWallResponse(int i);	// Collision response between boxes and walls
	void BoxBoxResponse(int i);		// Collision response between boxes
	void BallBoxResponse(int i);	// Collision response between balls and boxes
};

#endif