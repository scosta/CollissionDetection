/*-----------------------------------------------------------------------------------
File:			textureManager.h
Authors:		Steve Costa
Description:	Description of class for loading bitmap files and storing them
				in memory as textures.
-----------------------------------------------------------------------------------*/

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "commonUtil.h"

class CTextureManager
{
	// ATTRIBUTES
private:

public:

	// METHODS
private:
	SDL_Surface* LoadBMP(char *filename);

public:
	int LoadTexture(char *filename, unsigned int* texture_id, int flag);
};

#endif