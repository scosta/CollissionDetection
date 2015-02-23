/*-----------------------------------------------------------------------------------
File:			main.cpp
Author:			Steve Costa
Description:	This is a generic windows shell allowing for OpenGL programming
				in full-screen mode.
-----------------------------------------------------------------------------------*/

#include "main.h"							// Header file for this class
#include "game.h"							// Game header file

/*-----------------------------------------------------------------------------------
Declare static variables of the CWindow class.
-----------------------------------------------------------------------------------*/

HINSTANCE				CWin::main_instance;			// Program instance
HWND					CWin::win_handle = NULL;		// Window handle
HGLRC					CWin::render_context = NULL;	// Rendering context
HDC						CWin::device_context = NULL;	// GDI device context
PIXELFORMATDESCRIPTOR	CWin::pfd;						// Format desired for pixels

/*-----------------------------------------------------------------------------------
Window class method which initialises the window and the OpenGL graphics
rendering context.
Return values:		0 = Success
					1 = Failed to register class
					2 = Failed to create window
					3 = Failed to change display settings
					4 = Failed to get device context
					5 = Failed to get compatible pixel format
					6 = Failed to set chosen pixel format
					7 = Failed to get a rendering context
					8 = Failed to activate the rendering context
-----------------------------------------------------------------------------------*/

int CWin::Init(WNDPROC window_proc, HINSTANCE hinstance)
{
	GLuint	pixel_format;								// Pixel format id
	
	// Assign the instance variable to our static variable
	main_instance = hinstance;

	// Fill the class structure
	win_class.cbSize		= sizeof(WNDCLASSEX);
	win_class.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	win_class.lpfnWndProc	= window_proc;
	win_class.cbClsExtra	= 0;
	win_class.cbWndExtra	= 0;
	win_class.hInstance		= main_instance;
	win_class.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	win_class.hCursor		= LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = NULL;
	win_class.lpszMenuName	= NULL;
	win_class.lpszClassName = WINDOW_CLASS_NAME;
	win_class.hIconSm		= LoadIcon(NULL, IDI_WINLOGO);

	// Register the window class
	if (!RegisterClassEx(&win_class))
		return 1;
	
	// Set up the OpenGL context
	ZeroMemory(&screen_settings, sizeof(DEVMODE));		// Clear device memory
	screen_settings.dmSize = sizeof(DEVMODE);			// Set size of structure
	screen_settings.dmPelsWidth = SCREEN_WIDTH;			// Set screen width
	screen_settings.dmPelsHeight = SCREEN_HEIGHT;		// Set screen hight
	screen_settings.dmBitsPerPel = SCREEN_BPP;			// Set bits per pixel
	screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	// Create the window
	if (!(win_handle = CreateWindowEx(WS_EX_APPWINDOW, WINDOW_CLASS_NAME, "Advance Graphics 1",
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, main_instance, NULL)))
			return 2;

	// Change the display settings
	if (ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		return 3;

	// Get the device context
	if (!(device_context = GetDC(win_handle)))
		return 4;

	// Set up the descriptor according to how we want the pixels
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));		// Clear descriptor memory
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);				// Set size
	pfd.nVersion = 1;										// Version num
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
		| PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;							// RGBA setting
	pfd.cColorBits = SCREEN_BPP;							// Set the bit depth
	pfd.cDepthBits = 16;									// z-buffer	
	pfd.cStencilBits = 1;									// Use stencil buffer
	pfd.iLayerType = PFD_MAIN_PLANE;						// Main canvas
	
	// Query for closest matching pixel format
	if (!(pixel_format = ChoosePixelFormat(device_context, &pfd)))
		return 5;
	
	// Set the pixel format
	if (!(SetPixelFormat(device_context, pixel_format, &pfd)))
		return 6;

	// Now we can get a rendering context from our device context
	if (!(render_context = wglCreateContext(device_context)))
		return 7;
	
	// Activate the rendering context
	if (!(wglMakeCurrent(device_context, render_context)))
		return 8;

	return 0;
} // End CWin::CWin

/*-----------------------------------------------------------------------------------
Shutdown method which frees all resources.
-----------------------------------------------------------------------------------*/

int CWin::Shutdown()
{
	// Change back to original display settings
	ChangeDisplaySettings(NULL, 0);

	// Release the rendering context
	if (!(wglMakeCurrent(NULL, NULL)))
		MessageBox(NULL, "Failed to release rendering context", "ERROR", MB_OK | MB_ICONINFORMATION);

	// Free the memory allocated
	if (!(wglDeleteContext(render_context)))
		MessageBox(NULL, "Failed to delete rendering context", "ERROR", MB_OK | MB_ICONINFORMATION);
	render_context = NULL;

	// Unregister this window class
	if (!(UnregisterClass(WINDOW_CLASS_NAME, main_instance)))
		MessageBox(NULL, "Failed to unregister the windows class", "ERROR", MB_OK | MB_ICONINFORMATION);
	main_instance = NULL;

	return 0;
} // End CWin::~CWin

/*-----------------------------------------------------------------------------------
Windows message handling procedure
-----------------------------------------------------------------------------------*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Proceess messages
	switch(msg)
	{
	case WM_SETCURSOR:					// Invisible mouse cursor
		SetCursor(NULL);
		return 0;
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
} // End WindowProc

/*-----------------------------------------------------------------------------------
Program entry point
-----------------------------------------------------------------------------------*/

//int WINAPI WinMain (HINSTANCE hinstance, HINSTANCE hprevinstance,
//					LPSTR lpcmdline, int ncmdshow)
//{
//	MSG			msg;
//	CWin		*p_window;					// Window object pointer
//	CGame		*p_game;					// Game object pointer
//	
//	p_window = new CWin();					// Allocate memory for new window
//	p_window->Init(WindowProc, hinstance);	// Initialise window
//
//	p_game = new CGame();					// Allocate memory for game object
//	p_game->Init();							// Initialise game
//		
//	// Program loop
//	while(true)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
//		{
//			if (!GetMessage(&msg, NULL, 0, 0))
//				break;
//			DispatchMessage(&msg);
//		}
//		else
//		{
//			p_game->Main();
//			SwapBuffers(CWin::device_context);
//		}
//	}
//
//	// Clean up when done
//	p_game->Shutdown();
//	delete(p_game);
//	p_game = NULL;
//
//	return 0;
//} // End WinMain

int main(int argc, char* argv[])
{
	MSG			msg;
	CWin		*p_window;					// Window object pointer
	CGame		*p_game;					// Game object pointer
	HINSTANCE hinstance = GetModuleHandle(NULL);

	p_window = new CWin();					// Allocate memory for new window
	p_window->Init(WindowProc, hinstance);	// Initialise window

	p_game = new CGame();					// Allocate memory for game object
	p_game->Init();							// Initialise game

	// Program loop
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			DispatchMessage(&msg);
		}
		else
		{
			p_game->Main();
			SwapBuffers(CWin::device_context);
		}
	}

	// Clean up when done
	p_game->Shutdown();
	delete(p_game);
	p_game = NULL;

	return 0;
}