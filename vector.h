/*-----------------------------------------------------------------------------------
File:			vector.h
Authors:		Steve Costa
Description:	Header file defining vector class including vector operators,
				special member functions, and general non-member functions
				used for vector math.
				Class similar to that found in 3D Math Primer for Games and 
				Graphics Development
-----------------------------------------------------------------------------------*/

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <assert.h>

/*-----------------------------------------------------------------------------------
Encapsulate within the vec namespace in order to prevent non-member functions
from having global scope.
-----------------------------------------------------------------------------------*/

namespace vec
{

class TVector
{
	// ATTRIBUTES
public:

	float x, y, z;				// Coordinate values

private:


	// METHODS
public:

	// Default constructor
	TVector() { }
    
	// Initializing constructor
	TVector(float x1, float y1, float z1) : x(x1), y(y1), z(z1) { }

	// Set vector to 0
	void Zero()
	{ x = 0.0f;  y = 0.0f;  z = 0.0f; }

	// Normalize the vector
	void Normalize()
	{
		float mag_sq = x * x + y * y + z * z;
		if (mag_sq > 0) {			// Check for divide by zero
			float flipped = 1.0f / sqrt(mag_sq);
			x *= flipped;
			y *= flipped;
			z *= flipped;
		}
	}

	// Overload [] operator to treat vector as 3 element array if required
	float& operator [] (int rhs)
	{
		assert(rhs >= 0 && rhs <= 2);

		if (rhs == 0) return x;
		else if (rhs == 1) return y;
		else return z;
	}

	// Overload assignment operator
	TVector& operator = (const TVector& rhs)
	{
		x = rhs.x;  y = rhs.y;  z = rhs.z;
		return *this;
	}

	// Overload equality operator
	bool operator == (const TVector& rhs) const
	{
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
	}

	// Overload inequality operator
	bool operator != (const TVector &rhs) const
	{
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
	}

	// Overload negation operator
	TVector operator - () const
	{ return TVector(-x, -y, -z); }

	// Overload addition operator
	TVector operator + (const TVector &rhs) const
	{ return TVector(x + rhs.x, y + rhs.y, z + rhs.z); }

	// Overload subtraction operator
	TVector operator - (const TVector &rhs) const
	{ return TVector(x - rhs.x, y - rhs.y, z - rhs.z); }

	// Overload multiplication by a scalar
	TVector operator * (float rhs) const
	{ return TVector(x * rhs, y * rhs, z * rhs); }

	// Overload multiplication as dot product when rhs is a vector
	float operator * (const TVector &rhs) const
	{ return x * rhs.x + y * rhs.y + z * rhs.z; }

	// Overload division by a scalar
	TVector operator / (float rhs) const
	{
		float flipped = 1.0f / rhs;
		return TVector(x * flipped, y * flipped, z * flipped);
	}

	// Overload combined addition assignment operator
	TVector& operator += (const TVector &rhs)
	{
		x += rhs.x;  y += rhs.y; z += rhs.z;
		return *this;
	}

	// Overload combined subtraction assignment operator
	TVector& operator -= (const TVector &rhs)
	{
		x -= rhs.x;  y -= rhs.y; z -= rhs.z;
		return *this;
	}

	// Overload combined multiplication assignment operator
	TVector& operator *= (float rhs)
	{
		x *= rhs;  y *= rhs; z *= rhs;
		return *this;
	}

	// Overload combined division assignment operator
	TVector& operator /= (float rhs)
	{
		float flipped = 1.0f / rhs;
		x *= flipped;  y *= flipped; z *= flipped;
		return *this;
	}

private:

};

/*-----------------------------------------------------------------------------------
Vector math functions
-----------------------------------------------------------------------------------*/

// Overload multiplier with scalar on the left and vector on the right
inline TVector operator * (float lhs, const TVector& rhs)
{ return TVector(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }

// Magnitude of a vector
inline float Magnitude(const TVector& vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

// In case we wish to get the normalized value of a vector but not change it
inline TVector Normalized(const TVector& vec)
{
	float mag_sq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	if (mag_sq > 0) {			// Check for divide by zero
		float flipped = 1.0f / sqrt(mag_sq);
		return TVector(	vec.x * flipped,
						vec.y * flipped,
                        vec.z * flipped);
	}

	return TVector(0.0f, 0.0f, 0.0f);
}

// Vector cross product
inline TVector CrossProduct(const TVector& vec1, const TVector& vec2)
{
	return TVector(	vec1.y * vec2.z - vec1.z * vec2.y,
					vec1.z * vec2.x - vec1.x * vec2.z,
					vec1.x * vec2.y - vec1.y * vec2.x);
}

// Compute the distance between two vectors
inline float Distance(const TVector& vec1, const TVector& vec2)
{
	float diff_x = vec1.x - vec2.x;
	float diff_y = vec1.y - vec2.y;
	float diff_z = vec1.z - vec2.z;
	return sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
}

// Compute the squared distance between two vectors
inline float DistanceSquared(const TVector& vec1, const TVector& vec2)
{
	float diff_x = vec1.x - vec2.x;
	float diff_y = vec1.y - vec2.y;
	float diff_z = vec1.z - vec2.z;
	return diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;
}

}

#endif