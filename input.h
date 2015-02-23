/*-----------------------------------------------------------------------------------
File:			input.h
Authors:		Steve Costa
Description:	Class for encapsulating DirectX input functionality.
-----------------------------------------------------------------------------------*/

#ifndef INPUT_H
#define INPUT_H

#include <dinput.h>

class CDInput
{
private:
	// Direct Input variables
	LPDIRECTINPUT8			lpdi;
	
	unsigned char keyboard_state[256];
	DIMOUSESTATE mouse_state;	
public:
	LPDIRECTINPUTDEVICE8	lpdikey;

	CDInput();						// Constructor
	~CDInput();						// Destructor
	void LoadKeyboardState();		// Load state of keys pressed
	bool IsKeyPressed(int index);	// Check if a certain key is pressed
};

#endif INPUT_H