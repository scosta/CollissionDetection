/*-----------------------------------------------------------------------------------
File:			plane.h
Authors:		Steve Costa
Description:	Header file defining plane data type that can be used for walls
				in 3D environments.
-----------------------------------------------------------------------------------*/

#ifndef PLANE_H
#define PLANE_H

#include "vector.h"
using namespace vec;

#include "matrix.h"
using namespace matrix;

/*-----------------------------------------------------------------------------------
Class representing geometric properties of a plane.
-----------------------------------------------------------------------------------*/

class TPlane
{
	// ATTRIBUTES
public:

	TVector normal;			// Normalized surface normal
	TVector point1;			// Single point on the plane

	// METHODS
public:

	// Common constructor
	TPlane() {}

	// Initialization constructor
	TPlane(const TVector& n, const TVector& p)
		: normal(n), point1(p) {}

};

/*-----------------------------------------------------------------------------------
As a derivation of the TPlane class this contains other attributes that will
be required for drawing the wall.

Using two points (point1, point2) the wall is defined on the x,y plane, where
point1 is the bottom left vertex of the wall and point2 is the top right
vertex of the wall.  The two points are vectors starting at the origin

	0,0--------> (point2)
	|
	|
	|
	v 
(point1)

The wall is further defined with translation and rotation vectors for
positioning the wall in the world correctly.
-----------------------------------------------------------------------------------*/

class TWall : public TPlane
{
	// ATRRIBUTES
public:
	
	TVector point2;
	TMatrix trans;			// Transformation matrix from local to global coords
	int texture;			// ID specifying global texture to choose
	int color;				// ID specifying global colour to choose
		
	// METHODS
public:

	// Common constructor
	TWall() {}

	// Initialization Constructor, given 2 coordinates of the wall vertices
	// as well as the translation and rotation (radians) value (axis is axis of rotation)

	TWall(	const TVector& p1, const TVector& p2, const TVector& t, float theta, 
			int axis, int col, int tex)
	{
		// Store the two extreme points for the wall
		point1 = p1;
		point2 = p2;

		// Store the rotation in the transformation matrix
		if (axis == 0) trans.LoadIdentity();
		else trans.Rotate(axis, theta);

		// Initially normal is pointing out of the screen
		normal = TVector(0.0f, 0.0f, 1.0f);
		normal = normal * trans;			// Rotate the normal
		
		// Store the translation in the transformation matrix
        trans.Translate(t);

		// Store colour and texture indices
		texture = tex;
		color = col;
	}

	// Function which returns each of the vertex coordinates of the
	// wall for drawing purposes.  The points are numbered as follows:
	// 
	//	2-----------3
	//	|           |
	//	|           |		+y
	//	|           |		 |
	//	|           |		 |
	//	0-----------1		 ----- +x
	//
	// Bit 0 selects min.x vs. max.x
	// Bit 1 selects min.y vs. max.y
	
	TVector GetVertex(int i) const
	{
		// Make sure parameter is within range
		assert (i >= 0);
		assert (i <= 3);

		return TVector (
			(i & 1) ? point2.x : point1.x,
			(i & 2) ? point2.y : point1.y,
			0.0f);
	}
};

#endif