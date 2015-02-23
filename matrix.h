/*-----------------------------------------------------------------------------------
File:			matrix.h
Authors:		Steve Costa
Description:	Header file defining 4x4 transformation matrix class for any
				affine transformations.  Includes special member/non-member
				functions and constants for matrix manipulation.
-----------------------------------------------------------------------------------*/

#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include <assert.h>

#include "vector.h"
using namespace vec;

/*-----------------------------------------------------------------------------------
Encapsulate within the matrix namespace in order to prevent non-member functions
and variables from having global scope.
-----------------------------------------------------------------------------------*/

namespace matrix
{

/*-----------------------------------------------------------------------------------
Actual matrix class definition.
-----------------------------------------------------------------------------------*/

class TMatrix
{
	// ATTRIBUTES
public:

	float	m[16];

	// METHODS
public:

	// Default constructor
	TMatrix() { }

	// Load the identity matrix
	void LoadIdentity()
	{
		m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
        m[4] = 0.0f;	m[5] = 1.0f;	m[6] = 0.0f;	m[7] = 0.0f;
		m[8] = 0.0f;	m[9] = 0.0f;	m[10] = 1.0f;	m[11] = 0.0f;
		m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
	}

	// Load the zero matrix
	void LoadZero()
	{
		m[0] = 0.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
        m[4] = 0.0f;	m[5] = 0.0f;	m[6] = 0.0f;	m[7] = 0.0f;
		m[8] = 0.0f;	m[9] = 0.0f;	m[10] = 0.0f;	m[11] = 0.0f;
		m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 0.0f;
	}
	
	// Translation operator 
	void Translate(const TVector& trans)
	{
		m[12] = trans.x;	m[13] = trans.y;	m[14] = trans.z;
	}

	// Rotation operator which only works along a cardinal axis
	// axis = 1 -> rotate about the x-axis
	// axis = 2 -> rotate about the y-axis
	// axis = 3 -> rotate about the z-axis
	// theta is the amount of rotation in radians.
	void Rotate(int axis, float theta)
	{
        float s = sin(theta);
		float c = cos(theta);

		switch(axis)
		{
		case 1:		// Rotate about x-axis
			m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
			m[4] = 0.0f;	m[5] = c;		m[6] = s;		m[7] = 0.0f;
			m[8] = 0.0f;	m[9] = -s;		m[10] = c;		m[11] = 0.0f;
			m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
            break;

		case 2: 	// Rotate about y-axis
			m[0] = c;		m[1] = 0.0f;	m[2] = -s;		m[3] = 0.0f;
			m[4] = 0.0f;	m[5] = 1.0f;	m[6] = 0.0f;	m[7] = 0.0f;
			m[8] = s;		m[9] = 0.0f;	m[10] = c;		m[11] = 0.0f;
			m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
			break;

		case 3: 	// Rotate about z-axis
			m[0] = c;		m[1] = s;		m[2] = 0.0f;	m[3] = 0.0f;
			m[4] = -s;		m[5] = c;		m[6] = 0.0f;	m[7] = 0.0f;
			m[8] = 0.0f;	m[9] = 0.0f;	m[10] = 1.0f;	m[11] = 0.0f;
			m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
			break;

		default:
			// Bad axis selection do nothing
			break;
		}
	}

	// Rotation operator which works along any specfied axis.
	// The axis of rotation is a unit vector from the origin.
	// This code is from "3D Math Primer for Graphics"
	void Rotate(const TVector& axis, float theta)
	{
		// Ensure the axis is a unit vector
		assert(fabs(axis * axis - 1.0f) < 0.01f);

		// Calculate sin and cos of rotation angle
		float s = sin(theta);
		float c = cos(theta);

        // Computer sub-calculations to speed up math
		float a = 1.0f - c;
		float ax = a * axis.x;
		float ay = a * axis.y;
		float az = a * axis.z;

		// Store matrix elements
		m[0] = ax * axis.x + c;	
		m[1] = ax * axis.y + axis.z * s;
		m[2] = ax * axis.z - axis.y * s;
		m[3] = 0.0f;

		m[4] = ay * axis.x - axis.z * s;	
		m[5] = ay * axis.y + c;
		m[6] = ay * axis.z + axis.x * s;
		m[7] = 0.0f;

		m[8] = az * axis.x + axis.y * s;	
		m[9] = az * axis.y - axis.x * s;
		m[10] = az * axis.z + c;
		m[11] = 0.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;
	}

	// Return the translation vector stored in the matrix
	TVector GetTranslation() const
	{
		TVector translation(m[12], m[13], m[14]);
		return translation;
	}
	
};

/*-----------------------------------------------------------------------------------
Matrix non-member functions
-----------------------------------------------------------------------------------*/

// Overload * operator for vector * matrix multiplication
// TVector is of 1x3 dimensions and TMatrix is of 4x4 dimensions
// Mathematically this is undefined, but we implicitly assume TVector is
// of 1x4 dimensions with a 1 as the last element
inline TVector operator * (const TVector& lhs, const TMatrix& rhs)
{
	return TVector(	lhs.x * rhs.m[0] + lhs.y * rhs.m[4] + lhs.z * rhs.m[8] + 1.0f * rhs.m[12],
					lhs.x * rhs.m[1] + lhs.y * rhs.m[5] + lhs.z * rhs.m[9] + 1.0f * rhs.m[13],
					lhs.x * rhs.m[2] + lhs.y * rhs.m[6] + lhs.z * rhs.m[10] + 1.0f * rhs.m[14]);
}

// Similarly overload the *= operator
inline TVector& operator *= (TVector& lhs, const TMatrix& rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

// Overload * operator for matrix * matrix multiplication
// This allows for concatenation of multiple transformation matrices
inline TMatrix operator * (const TMatrix& lhs, const TMatrix& rhs)
{
	TMatrix temp;

	temp.m[0] = lhs.m[0] * rhs.m[0] + lhs.m[1] * rhs.m[4] + lhs.m[2] * rhs.m[8] + lhs.m[3] * rhs.m[12];
	temp.m[1] = lhs.m[0] * rhs.m[1] + lhs.m[1] * rhs.m[5] + lhs.m[2] * rhs.m[9] + lhs.m[3] * rhs.m[13];
	temp.m[2] = lhs.m[0] * rhs.m[2] + lhs.m[1] * rhs.m[6] + lhs.m[2] * rhs.m[10] + lhs.m[3] * rhs.m[14];
	temp.m[3] = lhs.m[0] * rhs.m[3] + lhs.m[1] * rhs.m[7] + lhs.m[2] * rhs.m[11] + lhs.m[3] * rhs.m[15];

	temp.m[4] = lhs.m[4] * rhs.m[0] + lhs.m[5] * rhs.m[4] + lhs.m[6] * rhs.m[8] + lhs.m[7] * rhs.m[12];
	temp.m[5] = lhs.m[4] * rhs.m[1] + lhs.m[5] * rhs.m[5] + lhs.m[6] * rhs.m[9] + lhs.m[7] * rhs.m[13];
	temp.m[6] = lhs.m[4] * rhs.m[2] + lhs.m[5] * rhs.m[6] + lhs.m[6] * rhs.m[10] + lhs.m[7] * rhs.m[14];
	temp.m[7] = lhs.m[4] * rhs.m[3] + lhs.m[5] * rhs.m[7] + lhs.m[6] * rhs.m[11] + lhs.m[7] * rhs.m[15];

	temp.m[8] = lhs.m[8] * rhs.m[0] + lhs.m[9] * rhs.m[4] + lhs.m[10] * rhs.m[8] + lhs.m[11] * rhs.m[12];
	temp.m[9] = lhs.m[8] * rhs.m[1] + lhs.m[9] * rhs.m[5] + lhs.m[10] * rhs.m[9] + lhs.m[11] * rhs.m[13];
	temp.m[10] = lhs.m[8] * rhs.m[2] + lhs.m[9] * rhs.m[6] + lhs.m[10] * rhs.m[10] + lhs.m[11] * rhs.m[14];
	temp.m[11] = lhs.m[8] * rhs.m[3] + lhs.m[9] * rhs.m[7] + lhs.m[10] * rhs.m[11] + lhs.m[11] * rhs.m[15];

	temp.m[12] = lhs.m[12] * rhs.m[0] + lhs.m[13] * rhs.m[4] + lhs.m[14] * rhs.m[8] + lhs.m[15] * rhs.m[12];
	temp.m[13] = lhs.m[12] * rhs.m[1] + lhs.m[13] * rhs.m[5] + lhs.m[14] * rhs.m[9] + lhs.m[15] * rhs.m[13];
	temp.m[14] = lhs.m[12] * rhs.m[2] + lhs.m[13] * rhs.m[6] + lhs.m[14] * rhs.m[10] + lhs.m[15] * rhs.m[14];
	temp.m[15] = lhs.m[12] * rhs.m[3] + lhs.m[13] * rhs.m[7] + lhs.m[14] * rhs.m[11] + lhs.m[15] * rhs.m[15];

	return temp;
}

// Similarly overload the *= operator
inline TMatrix& operator *= (TMatrix& lhs, const TMatrix& rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

}

#endif