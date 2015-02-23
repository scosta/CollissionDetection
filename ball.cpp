/*-----------------------------------------------------------------------------------
File:			ball.cpp
Authors:		Steve Costa
Description:	Define methods for a ball datatype.
-----------------------------------------------------------------------------------*/

#include "sphere.h"

#include "commonUtil.h"

/*-----------------------------------------------------------------------------------
Initialize state variables
-----------------------------------------------------------------------------------*/

TBall::TBall(const TVector& c, float r, const TVector& v, int col, int tex)
{ 
	vel = v;								// Store the velocity
	accel = TVector(0.0f, -0.49f, 0.0f);	// Simulate gravity
	center = c;								// Store the center coordinate
	radius = r;								// Store the radius
	color = col;
	texture = tex;

	circumference = PI2 * radius;			// Calculate the circumference

	rot.LoadIdentity();						// Initialize rotation matrix

	// The axis of rotation for the ball is perpindicular to
	// the direction it is pointing in
	axis.x = vel.z;
	axis.y = 0.0f;
	axis.z = -vel.x;
	if (Magnitude(axis) > 0.0f) {
		axis.Normalize();
	}
	else {
		axis.x = -1.0f;
		axis.y = 0.0f;
		axis.z = 0.0f;
	}
	
	rot.LoadIdentity();				// Set rotation matrix to identity matrix

	spin_angle = 0.0f;				// Ball is not initially rotating
}

/*-----------------------------------------------------------------------------------
Once the player turns, the axis of rotation for the ball must turn also
-----------------------------------------------------------------------------------*/

void TBall::Turn(float theta)
{
	float old_velx = vel.x;
	float old_velz = vel.z;

    // Orient the velocity and the axis as per the direction the ball points in
	// rotation:	xr = x * cos(theta) - y * sin(theta)
	//				yr = x * sin(theta) + y * cos(theta)
	vel.x = old_velx * cos(theta) - old_velz * sin(theta);
	vel.z = old_velx * sin(theta) + old_velz * cos(theta);

	if (Magnitude(vel) > 0.0f) {
		axis.x = vel.z;
		axis.z = -vel.x;
		axis.Normalize();
	}
	else {
		float old_axisx = axis.x;
		float old_axisz = axis.z;
		axis.x = old_axisx * cos(theta) - old_axisz * sin(theta);
		axis.z = old_axisx * sin(theta) + old_axisz * cos(theta);
	}
	
}

/*-----------------------------------------------------------------------------------
Given the velocity of the ball, its radius and the axis of rotation
this method will rotate the ball accordingly
-----------------------------------------------------------------------------------*/

void TBall::Rotate(float dt)
{
	// Calculate the arc given the displacement (speed * time) and circumference
	// (ball should only roll if moving along x-z axis)
	TVector xz_vel(vel.x, 0.0f, vel.z);
	float arc = Magnitude(xz_vel) * dt / circumference;

	// Calculate the amount of rotation the ball must turn by
	spin_angle += (arc / radius);

	// Now we can rotate the ball by this ammount
	rot.Rotate(axis, spin_angle);
}