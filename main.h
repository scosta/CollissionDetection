/*-----------------------------------------------------------------------------------
File:			main.h
Author:			Steve Costa
Description:	Define the windows class and the necessary constants
-----------------------------------------------------------------------------------*/

#ifndef MAIN_H
#define MAIN_H

#include "commonUtil.h"				// Common macros and headers
//#include <windows.h>					// Windows header file
//#include <gl\gl.h>						// OpenGL Library files
//#include <gl\glu.h>
//#include <SDL.h>
#include "game.h"

/*-----------------------------------------------------------------------------------
Constants
-----------------------------------------------------------------------------------*/

#define WINDOW_CLASS_NAME		"Advance Graphics 1"	// Class name

/*-----------------------------------------------------------------------------------
Devine the main window class for the game.
-----------------------------------------------------------------------------------*/

class CWin
{
	// ATTRIBUTES
private:
	WNDCLASSEX						win_class;			// Window class
	DEVMODE							screen_settings;	// Screen settings
public:
	static HWND						win_handle;			// Window handle
	static HINSTANCE				main_instance;		// Program instance
	static HGLRC					render_context;		// Rendering context
	static HDC						device_context;		// GDI device context
	static PIXELFORMATDESCRIPTOR	pfd;				// Format desired for pixels
	
	// METHODS
public:
	int Init(WNDPROC window_proc, HINSTANCE hinstance);		// Class constructor
	int Shutdown();											// Class destructor
};


#endif MAIN_H