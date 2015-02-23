/*-----------------------------------------------------------------------------------
File:			physics.h
Authors:		Steve Costa
Description:	Physics functions for calculating effects for collisions, friction,
				etc.
-----------------------------------------------------------------------------------*/

#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector.h"
using namespace vec;

/*-----------------------------------------------------------------------------------
Encapsulate within the physics namespace in order to prevent functions
from having global scope.
-----------------------------------------------------------------------------------*/

namespace physics
{

	void MObjSObjEffects(TVector& init_vel, const TVector& plane_normal, float e);

	void MObjMObjEffects(	TVector& init_vel1, const TVector& center1,
							TVector& init_vel2, const TVector& center2);

	void MObjMObjEffects2(	TVector& init_vel1, const TVector& center1,
							TVector& init_vel2, const TVector& center2);
}

#endif