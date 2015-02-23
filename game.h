/*-----------------------------------------------------------------------------------
File:			game.h
Author:		    Steve Costa
Description:	Header file defining the main game class.
-----------------------------------------------------------------------------------*/

#ifndef GAME_H
#define GAME_H

#include "commonUtil.h"				// Common macros and headers
#include "main.h"					// Main class variables and constants
#include "input.h"					// Direct Input class
#include "world.h"
#include "collisions.h"

class CGame
{
	// ATTRIBUTES
	private:

		CDInput input;					// Direct Input object
		CWorld world;					// World class
		CCollisions *p_collide;			// Collision detection class

		// Light information
		GLfloat spec[4];				// Specular highlight of balls
		GLfloat posl[4];				// Position of light source
		GLfloat amb[4];					// Global ambient
		GLfloat amb2[4];				// Ambient of lightsource
		float shine;					// Shininess

		int cam_view;					// Camera choice

		DWORD current_time;				// Timing variable
		
	// METHODS
private:

    void GetInput();					// Get user input
	void ApplyGravity();				// Apply gravity
	void CameraView();					// View world through correct camera

public:

	CGame();
	int Init();
	int Main();
	int Shutdown();
};


#endif GAME_H