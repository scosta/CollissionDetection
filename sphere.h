/*-----------------------------------------------------------------------------------
File:			sphere.h
Authors:		Steve Costa
Description:	Header file defining sphere data type.
-----------------------------------------------------------------------------------*/

#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"
using namespace vec;

#include "matrix.h"
using namespace matrix;

/*-----------------------------------------------------------------------------------
Class representing geometric properties of a sphere.
-----------------------------------------------------------------------------------*/

class TSphere
{
	// ATTRIBUTES
public:

	TVector center;				// Center of sphere
	float radius;				// Radius of sphere
		
	// METHODS
public:

	// Common constructor
	TSphere() {}

	// Initialization constructor
	TSphere(const TVector& c, float r) : center(c), radius(r) {}

};

/*-----------------------------------------------------------------------------------
As a derivation of the TSphere class this contains other attributes that will
be required for collision detection and physics calculations
-----------------------------------------------------------------------------------*/
class TBall : public TSphere
{
	// ATTRIBUTES
public:
	TVector vel;				// Velocity of ball
	TVector accel;				// Acceleration of ball
	TMatrix rot;				// Rotation matrix for ball spinning
	TVector axis;				// Axis of rotation for the ball
	int texture;				// ID specifying global texture to choose
	int color;					// ID specifying global colour to choose
		
private:
	float circumference;
	float spin_angle;			// Keep track of the current rotation of the ball
			
	// METHODS
public:

	TBall() {}					// Common constructor

	// Initialization constructor
	TBall(const TVector& c, float r, const TVector& v, int col, int tex);

	// Once the player turns, the axis of rotation for the ball must
	// turn also
	void Turn(float theta);
	
	// Given the velocity of the ball, its radius and the axis of
	// rotation this method will rotate the ball accordingly
	void Rotate(float dt);
};

#endif