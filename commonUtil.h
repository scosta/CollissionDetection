/*-----------------------------------------------------------------------------------
File:			commonUtil.h
Authors:		Steve Costa
Description:	Define any commonly used constants or macros
-----------------------------------------------------------------------------------*/

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

/*-----------------------------------------------------------------------------------
External Libraries
-----------------------------------------------------------------------------------*/

#include <windows.h>
#include <cstdio>
#include <gl\gl.h>							// OpenGL Library files
#include <gl\glu.h>
#include <SDL.h>

/*-----------------------------------------------------------------------------------
Constants
-----------------------------------------------------------------------------------*/

#define SCREEN_WIDTH			1280					// Horizontal resolution
#define SCREEN_HEIGHT			800						// Vertical resolution
#define SCREEN_BPP				32						// Color depth
#define	FRAME_INTERVAL			20						// 50 FPS

// PI
#define PI		3.14159265f
#define PI2		6.28318531f

// What will be considered a negligible time interval
#define ZERO	0.005f

// Square macro
#ifndef SQR
#define SQR(a)	((a) * (a))
#endif

// Min macro
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// Max macro
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif MAX

// Swap macro
#ifndef SWAP
#define SWAP(a, b, t) { t = a; a = b; b = t; }
#endif

// Absolute macro
#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

#endif