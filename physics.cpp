/*-----------------------------------------------------------------------------------
File:			physics.cpp
Authors:		Steve Costa
Description:	Physics functions for calculating effects for collisions, friction,
				etc.
-----------------------------------------------------------------------------------*/

#include "physics.h"

/*-----------------------------------------------------------------------------------
Given that a moving object collides with a stationary object calculate the
reflected vector of motion given the initial vector befor collision and the normal
of the surface the moving object collides with.
The coefficient of restitution is set at 0.5 for all wall collisions.
-----------------------------------------------------------------------------------*/

void physics::MObjSObjEffects(TVector& init_vel, const TVector& plane_normal, float e)
{
	// Apply collision effects to the ball
	// R = 2 * (-I.N) * N + I
	// R = resultant vector
	// I = initial vector before collision
	// N = normal of wall ball collided with

	assert(e >= 0.0f && e <= 1.0f);

	TVector r = (1 + e) * (-init_vel * plane_normal) * plane_normal + init_vel;

	// Now change the initial velocity vector to the new one
	init_vel = r;
}

/*-----------------------------------------------------------------------------------
Given two moving objects we calculate the resultant velocity vectors after collision.
Collisions between objects are perfectly elastic.
-----------------------------------------------------------------------------------*/

void physics::MObjMObjEffects(	TVector& init_vel1, const TVector& center1,
								TVector& init_vel2, const TVector& center2)
{
	// For first object
	// Calculate axis of collision
	TVector n_axis = center2 - center1;
	n_axis.Normalize();

	// Project velocity of first object onto this axis
	float a = n_axis * init_vel1;

	// Get projected vectors
	TVector resp1_n = n_axis * a;
	TVector resp1_t = init_vel1 - resp1_n;

	// For second object
	// Calculate axis of collision
	n_axis = center1 - center2;
	n_axis.Normalize();

	// Project velocity of first object onto this axis
	float b = n_axis * init_vel2;

	// Get projected vectors
	TVector resp2_n = n_axis * b;
	TVector resp2_t = init_vel2 - resp2_n;

    // Calculate new velocities
	TVector new_v1_n = (resp1_n + resp2_n - (resp1_n - resp2_n)) * 0.5f;
	TVector new_v2_n = (resp1_n + resp2_n - (resp2_n - resp1_n)) * 0.5f;
	TVector new_v1_t = resp1_t;
	TVector new_v2_t = resp2_t;
	init_vel1 = new_v1_n + new_v1_t;
	init_vel2 = new_v2_n + new_v2_t;
}

/*-----------------------------------------------------------------------------------
Given two moving objectswe calculate the resultant velocity vectors after collision.
Collisions between objects are perfectly elastic.
***Note:  This function takes tangential forces along the t-axis into
consideration and is used for object that would require this such as boxes.
-----------------------------------------------------------------------------------*/
void physics::MObjMObjEffects2(	TVector& init_vel1, const TVector& center1,
								TVector& init_vel2, const TVector& center2)
{
	// For first object
	// Calculate axis of collision
	TVector n_axis = center2 - center1;
	n_axis.Normalize();

	// Project velocity of first object onto this axis
	float a = n_axis * init_vel1;

	// Get projected vectors
	TVector resp1_n = n_axis * a;
	TVector resp1_t = init_vel1 - resp1_n;

	// For second object
	// Calculate axis of collision
	n_axis = center1 - center2;
	n_axis.Normalize();

	// Project velocity of first object onto this axis
	float b = n_axis * init_vel2;

	// Get projected vectors
	TVector resp2_n = n_axis * b;
	TVector resp2_t = init_vel2 - resp2_n;

    // Calculate new velocities
	TVector new_v1_n = (resp1_n + resp2_n - (resp1_n - resp2_n)) * 0.5f;
	TVector new_v2_n = (resp1_n + resp2_n - (resp2_n - resp1_n)) * 0.5f;
	TVector new_v1_t = (resp1_t + resp2_t - (resp1_t - resp2_t)) * 0.5f;
	TVector new_v2_t = (resp1_t + resp2_t - (resp2_t - resp1_t)) * 0.5f;
	init_vel1 = new_v1_n + new_v1_t;
	init_vel2 = new_v2_n + new_v2_t;		
}