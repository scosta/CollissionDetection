/*-----------------------------------------------------------------------------------
File:			world.h
Authors:		Steve Costa
Description:	Header file defining the world and all its objects
-----------------------------------------------------------------------------------*/

#ifndef WORLD_H
#define WORLD_H

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cstdio>					// Header for data streaming
#include <cstring>					// Header for string handling
#include <new>						// Header for dynamic memory allocation
using namespace std;

#include "plane.h"					// Plane type class
#include "sphere.h"					// Sphere type class
#include "aabb.h"					// Bounding Box type class
#include "textureManager.h"			// Load textures

/*-----------------------------------------------------------------------------------
Constants
-----------------------------------------------------------------------------------*/

#define MAX_TEXTURES		11
#define MAX_COLORS			13

class CWorld
{
	// ATTRIBUTES
private:

	GLUquadricObj *p_sphere_obj;	// Quadratic object
	CTextureManager t_manager;		// Used for loading textures

	// Display Lists
	unsigned int l_reflective_surface;
	unsigned int l_boxes;
	unsigned int l_walls;
	unsigned int l_sky;
	
	// World textures and colours that can be used
	unsigned int world_textures[MAX_TEXTURES];
	unsigned int sky_textures[6];
	float world_colors[MAX_COLORS][4];

	// Clipping plane for reflective surface
	double clip_plane[4];

public:

	int num_walls;					// Number of walls
	int num_balls;					// Number of balls
	int num_boxes;					// Number of boxes
	
	TWall *p_walls;					// Declare walls
	TBall *p_balls;					// Declare balls
	TBox *p_boxes;					// Declare boxes

	// METHODS
public:

	CWorld();
	void Init();					// Init objects in the world
	void Draw();					// Draw all the world components
	void ShutDown();				// Release all alocated memory

	void DrawReflectiveSurface(float *posl, float dt);
	void DrawWorld(float dt);

private:
	void ReadString(char *string, FILE *file);	// Read a string, ignore empty lines and comments
	int Load(char *file_name);					// Load world configuration from file

	void RenderReflectiveSurface();
	void RenderBoxes();
	void RenderWalls();
	void RenderSkyBox();
};

#endif