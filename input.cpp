/*-----------------------------------------------------------------------------------
File:			input.cpp
Authors:		Steve Costa
Description:	Methods for the CDInput class to simplify DirectInput access.
-----------------------------------------------------------------------------------*/

#include "input.h"					// Class header file
#include "main.h"					// Access the window handle

/*-----------------------------------------------------------------------------------
Initialise Direct Input through the CDInput constructor
-----------------------------------------------------------------------------------*/

CDInput::CDInput()
{
	lpdi		= NULL;
	lpdikey		= NULL;

	// Create the direct input object
	if (DirectInput8Create(CWin::main_instance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&lpdi, NULL) != DI_OK)
		return;

	// Create the keyboard device
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL) != DI_OK)
		return;

	// Set cooperation level
	if (lpdikey->SetCooperativeLevel(CWin::win_handle, 
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return;

	// Set data format
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return;

	// Acquire the keyboard
	if (lpdikey->Acquire() != DI_OK)
		return;
} // End CDInput::CDInput

/*-----------------------------------------------------------------------------------
Return a pointer to the array of key states
-----------------------------------------------------------------------------------*/

void CDInput::LoadKeyboardState()
{
	lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);
} // End CDInput::LoadKeyboardState()

/*-----------------------------------------------------------------------------------
By passing the key index this method will tell you if that key has been pressed.
-----------------------------------------------------------------------------------*/

bool CDInput::IsKeyPressed(int index)
{
	if (keyboard_state[index])
		return TRUE;
	else
		return FALSE;
} // End CDInput::IsKeyPressed

/*-----------------------------------------------------------------------------------
Clean up after using direct input
-----------------------------------------------------------------------------------*/

CDInput::~CDInput()
{
	if (lpdikey)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
		lpdikey = NULL;
	}
	if (lpdi)
	{
		lpdi->Release();
		lpdi = NULL;
	}
} // End CDInput::~CDInput