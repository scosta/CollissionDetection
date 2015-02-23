/*-----------------------------------------------------------------------------------
File:			vector.h
Authors:		Steve Costa
Description:	Header file defining Axially Aligned Bounding Box data type.
-----------------------------------------------------------------------------------*/

#ifndef AABB_H
#define AABB_H

#include <assert.h>

#include "vector.h"
using namespace vec;

class TAABB
{
	// ATTRIBUTES
public:

	TVector minv;			// Min point of bounding box
	TVector maxv;			// Max point of bounding box

	// METHODS
public:

	// Common constructor
	TAABB() {}

	// Initialization constructor
	TAABB(const TVector& mn, const TVector& mx) : minv(mn), maxv(mx) {}

	// Empty the box so that it contains no points
	void Empty()
	{
		minv = TVector(100000.0f, 100000.0f, 100000.0f);
		maxv = -minv;
	}

	// Expand the box to contain the point passed as a parameter
	void Add(const TVector& point)
	{
		if (point.x < minv.x) minv.x = point.x;
		if (point.x > maxv.x) maxv.x = point.x;
		if (point.y < minv.y) minv.y = point.y;
		if (point.y > maxv.y) maxv.y = point.y;
		if (point.z < minv.z) minv.z = point.z;
		if (point.z > maxv.z) maxv.z = point.z;
	}

	// Function which returns all the vertices of the bounding
	// as found in AABB3.cpp from 3D Math Primer for Games and Graphics Development
	// Return one of the 8 corner points.  The points are numbered as follows:
	//
	//            2                                3
	//              ------------------------------
	//             /|                           /|
	//            / |                          / |
	//           /  |                         /  |
	//          /   |                        /   |
	//         /    |                       /    |
	//        /     |                      /     |
	//       /      |                     /      |
	//      /       |                    /       |
	//     /        |                   /        |
	//  6 /         |                7 /         |
	//   /----------------------------/          |
	//   |          |                 |          |
	//   |          |                 |          |      +Y
	//   |        0 |                 |          | 
	//   |          |-----------------|----------|      |
	//   |         /                  |         /  1    |
	//   |        /                   |        /        |       -Z
	//   |       /                    |       /         |
	//   |      /                     |      /          |     /
	//   |     /                      |     /           |    /
	//   |    /                       |    /            |   /
	//   |   /                        |   /             |  /
	//   |  /                         |  /              | /
	//   | /                          | /               |/
	//   |/                           |/                ----------------- +X
	//   ------------------------------
	//  4                              5
	//
	// Bit 0 selects min.x vs. max.x
	// Bit 1 selects min.y vs. max.y
	// Bit 2 selects min.z vs. max.z

	TVector GetVertex(int i) const
	{
		// Make sure parameter is within range
		assert (i >= 0);
		assert (i <= 7);

		return TVector (
			(i & 1) ? maxv.x : minv.x,
			(i & 2) ? maxv.y : minv.y,
			(i & 4) ? maxv.z : minv.z );
	}

};

class TBox : public TAABB
{
	// ATTRIBUTES
public:

	TVector vel;						// Velocity of box
	TVector accel;						// Acceleration of box
	int color;							// ID specifying global colour to choose
	int texture;						// ID specifying global texture to choose
	
	// METHODS
public:

	// Common constructor
	TBox() {}

	// Initialization constructor
	TBox(const TVector& mn, const TVector& mx, const TVector& v, int c, int t)
	{
		 minv = mn;
		 maxv = mx;
		 vel = v;
		 accel = TVector(0.0f, -0.49f, 0.0f);	// Simulate gravity
		 color = c;
		 texture = t;
	}
};

#endif