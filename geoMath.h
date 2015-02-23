/*-----------------------------------------------------------------------------------
File:			geoMath.h
Authors:		Steve Costa
Description:	Header file defining geometric test functions for determining
				collisions between different 3D objects.
-----------------------------------------------------------------------------------*/

#ifndef GEOMATH_H
#define GEOMATH_H

#include <assert.h>

#include "vector.h"
using namespace vec;

#include "plane.h"
#include "sphere.h"
#include "aabb.h"

/*-----------------------------------------------------------------------------------
Encapsulate within the geomath namespace in order to prevent functions
from having global scope.
-----------------------------------------------------------------------------------*/

namespace geomath
{
	// Check for intersection between the plane and a sphere sphere
	float IntersectBallPlane(const TVector& ball_center,
							 float ball_radius,
							 const TVector& ball_vel,
							 const TVector& plane_point,
							 const TVector& plane_norm);

	// Check for intersection between two spheres
	float IntersectBallBall( const TVector& ball_center1,
							 float ball_radius1,
							 const TVector& ball_vel1,
							 const TVector& ball_center2,
							 float ball_radius2,
							 const TVector& ball_vel2);

	
	float IntersectBoxPlane(const TVector& box_min, const TVector& box_max, 
							const TVector& box_vel, const TVector& wall_point,
							const TVector& wall_normal);

	bool IsBallOnWall(	const TBall& ball, const TWall& wall);

	bool IsBoxOnWall(const TBox& box, const TWall& wall);

    float IntersectBoxBox(	const TVector& box_min1, const TVector& box_max1, 
							const TVector& box_vel1, const TVector& box_min2,
							const TVector& box_max2, const TVector& box_vel2);

	float IntersectBallTriangle(const TVector& center, const TVector& ball_vel,
								float radius, const TVector& tri_vel,
								const TVector* vertices, float t_left, 
								bool& edge_collision, TVector& edge_p1, TVector& edge_p2);

	bool IsPointInTriangle(	const TVector& point, const TVector& normal, 
							const TVector* vertices);

	float Solve2ODE(float a, float b, float c, float t_left);

	float IntersectBallVertex(	const TVector& center, const TVector& velocity,
								float radius, const TVector& vertex, float t_left);

	float IntersectBallEdge(const TVector& center, const TVector& velocity, float radius,
							const TVector& point1, const TVector& point2, bool test_vertices,
							float t_left);
}

#endif