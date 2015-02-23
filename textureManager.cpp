/*-----------------------------------------------------------------------------------
File:			textureManager.cpp
Authors:		Steve Costa
Description:	Define the methods for the texture manager which loads bitmap
				files and stores them as textures.
-----------------------------------------------------------------------------------*/

#include "textureManager.h"

//#include <Windows.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>
#include <cstdio>
using namespace std;

/*-----------------------------------------------------------------------------------
Load a bitmap from a filename and return the bitmap data structure.  This method
is to be used by the LoadTexture method.
-----------------------------------------------------------------------------------*/

SDL_Surface* CTextureManager::LoadBMP(char *filename)
{
	FILE *file = NULL;

	if (!filename) return NULL;

	fopen_s(&file, filename, "r");

	if (file)
	{
		fclose(file);
		return SDL_LoadBMP(filename);
	}

	// return null if failed
	return NULL;
}

/*-----------------------------------------------------------------------------------
Load a texture from a file and assign it to the specified texture_id value
-----------------------------------------------------------------------------------*/

int CTextureManager::LoadTexture(char *filename, unsigned int* texture_id, int flag)
{
	int status = -1;

	SDL_Surface *p_texture_image[1];
	ZeroMemory(p_texture_image, sizeof(void *) * 1);
	
	// load bitmap
	if (p_texture_image[0] = LoadBMP(filename))
	{
		status = 1;

		// Generate texture
		glGenTextures(1, texture_id);
		glBindTexture(GL_TEXTURE_2D, *texture_id);
		if (flag) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, p_texture_image[0]->w, 
			p_texture_image[0]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, p_texture_image[0]->pixels);
	}

	// Clean up
	if (p_texture_image[0])
	{
		SDL_FreeSurface(p_texture_image[0]);
	}

	return status;
}