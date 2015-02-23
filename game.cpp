/*-----------------------------------------------------------------------------------
File:			game.cpp
Authors:		Steve Costa
Description:	Header file defining the main game class.
-----------------------------------------------------------------------------------*/

#include "game.h"							// Class header file
#include "main.h"

/*-----------------------------------------------------------------------------------
Set any initial values for class state variables.
-----------------------------------------------------------------------------------*/

CGame::CGame()
{
	world.Init();
	p_collide = new CCollisions(world);

	// Initialize light variables
	spec[0] = 1.0f; spec[1] = 1.0f; spec[2] = 1.0f; spec[3] = 1.0f;
	posl[0] = 15; posl[1] = 10; posl[2] = -7.5; posl[3] = 1;
	amb[0] = 0.5f; amb[1] = 0.5f; amb[2] = 0.5f; amb[3] = 1.0f;
	amb2[0] = 0.3f; amb2[1] = 0.3f; amb2[2] = 0.3f; amb2[3] = 1.0f;
	shine = 100.0f;

	// Initially set up camera to look through default view
	cam_view = 0;
}

/*-----------------------------------------------------------------------------------
Setup our OpenGL state variables for the world
-----------------------------------------------------------------------------------*/

int CGame::Init()
{
	// Set the viewport to the dimensions of the window
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);			// Select the PROJECTION matrix
	glLoadIdentity();						// Load identity

	// Calculate aspect ratio of the window
	gluPerspective(50.0f, (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 0.1f, 150.0f);

	glMatrixMode(GL_MODELVIEW);				// Select MODELVIEW matrix
	glLoadIdentity();						// Load identity

	glClearDepth(1.0f);						// Depth buffer setup
	glEnable(GL_DEPTH_TEST);				// Enable depth testing
	glDepthFunc(GL_LEQUAL);					// Depth testing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Perspective calculation

	glClearStencil(0);						// Disable clearing of stencil buffer

	glClearColor(0, 0, 0, 0);				// Clear colour is black
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, &shine);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, posl);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb2);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Start the timing clock
	current_time = GetTickCount();

	return 0;
}

/*-----------------------------------------------------------------------------------
Get user input from the keyboard and apply changes depending on what is pressed.
-----------------------------------------------------------------------------------*/

void CGame::GetInput()
{
	input.LoadKeyboardState();							// Poll the keyboard

	// Exit if the user presses escape
	if (input.IsKeyPressed(DIK_ESCAPE)) {
		SendMessage(CWin::win_handle, WM_CLOSE, 0, 0);
	}
	
	// Accelerate ball
	if (input.IsKeyPressed(DIK_UP)) {
		TVector temp_v(world.p_balls[0].vel.x, 0.0f, world.p_balls[0].vel.z);
		TVector old_y(0.0f, world.p_balls[0].vel.y, 0.0f);
		float speed = Magnitude(temp_v);
		if (speed == 0.0f) {
			speed = 0.5f;
			TVector dir(-world.p_balls[0].axis.z, 0.0f, world.p_balls[0].axis.x);
			world.p_balls[0].vel = speed * dir + old_y;
		}
		else {
			speed += 0.3f;
			temp_v.Normalize();
			world.p_balls[0].vel = speed * temp_v + old_y;
		}
	}

	// Decelerate ball
	if (input.IsKeyPressed(DIK_DOWN)) {
		TVector temp_v(world.p_balls[0].vel.x, 0.0f, world.p_balls[0].vel.z);
		TVector old_y(0.0f, world.p_balls[0].vel.y, 0.0f);
		float speed = Magnitude(temp_v);
		if (speed == 0.0f) {
			speed = -0.5f;
			TVector dir(-world.p_balls[0].axis.z, 0.0f, world.p_balls[0].axis.x);
			world.p_balls[0].vel = speed * dir + old_y;
		}
		else {
			speed -= 0.3f;
			temp_v.Normalize();
			world.p_balls[0].vel = speed * temp_v + old_y;
		}
	}

	// Turn ball
	if (input.IsKeyPressed(DIK_LEFT))
		world.p_balls[0].Turn(-0.1f);
	else if (input.IsKeyPressed(DIK_RIGHT))
		world.p_balls[0].Turn(0.1f);

	// Change camera view
	if (input.IsKeyPressed(DIK_1)) cam_view = 0;
	else if (input.IsKeyPressed(DIK_2)) cam_view = 1;
	else if (input.IsKeyPressed(DIK_3)) cam_view = 2;
	else if (input.IsKeyPressed(DIK_4)) cam_view = 3;
	else if (input.IsKeyPressed(DIK_5)) cam_view = 4;
	else if (input.IsKeyPressed(DIK_6)) cam_view = 5;
	else if (input.IsKeyPressed(DIK_7)) cam_view = 6;
}

/*-----------------------------------------------------------------------------------
Simply applies gravity acceleration to all objects.
-----------------------------------------------------------------------------------*/

void CGame::ApplyGravity()
{
	for (int i = 0; i < world.num_balls; i++)
	{
		world.p_balls[i].vel += world.p_balls[i].accel;
	}
	for (int i = 0; i < world.num_boxes; i++)
	{
		world.p_boxes[i].vel += world.p_boxes[i].accel;
	}
}

/*-----------------------------------------------------------------------------------
Depending on cam_view, different cameras are used to view the scene.
-----------------------------------------------------------------------------------*/

void CGame::CameraView()
{
	switch(cam_view)
	{
	case 0:		// Over the shoulder 1
		{
			TVector c = world.p_balls[0].center;
			gluLookAt(	c.x + 3.0f, c.y + 3.0f, c.z + 5.0f,
						c.x, c.y, c.z, 0.0f, 1.0f, 0.0f);
			break;
		}
	case 1:		// Over the shoulder 2
		{
			TVector c = world.p_balls[0].center;
			gluLookAt(	c.x + 5.0f, c.y + 5.0f, c.z + 10.0f,
						c.x, c.y, c.z, 0.0f, 1.0f, 0.0f);
			break;
		}
	case 2:		// Room 1
		{
			gluLookAt(	-5.0f, 7.0f, -15.0f, 
						10.0f, 2.0f, -7.0f, 0.0f, 1.0f, 0.0f);
			break;
		}
	case 3:		// Room 2
		{
			gluLookAt(	30.0f, 7.0f, -10.0f, 
						10.0f, 5.0f, -25.0f, 0.0f, 1.0f, 0.0f);
			break;
		}
	case 4:		// 2-D View
		{
			gluLookAt(	15.0f, 50.0f, -17.5f, 
						15.0f, 2.0f, -17.5f, 0.0f, 0.0f, -1.0f);
			break;
		}
	case 5:		// 2-D View 2
		{
			TVector c = world.p_balls[0].center;
			gluLookAt(	c.x, 30.0f, c.z, 
						c.x, 2.0f, c.z, 0.0f, 0.0f, -1.0f);
			break;
		}
	default:		// Panoramic View
		{
			static TVector view(15.0f, 0.0f, 0.0f);
			TVector old_view = view;
			view.x = old_view.x * cos(0.01f) - old_view.z * sin(0.01f);
			view.z = old_view.x * sin(0.01f) + old_view.z * cos(0.01f);
			gluLookAt(	15.0f, 3.0f, -17.5f,
						15.0f + view.x, 3.0f, -17.5f + view.z,
						0.0f, 1.0f, 0.0f);
			break;
		}
	}
}

/*-----------------------------------------------------------------------------------
Main Game Loop. This is where the magic happens!
-----------------------------------------------------------------------------------*/

int CGame::Main()
{
	// Calculate the elapsed time
	DWORD elapsed_time = GetTickCount() - current_time;
	current_time += elapsed_time;
	float elapsed_secs = (float)elapsed_time * 0.001f;

	// Clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	GetInput();						// Get user input
	ApplyGravity();					// Apply gravity to objects
	p_collide->Test(elapsed_secs);	// Test for collisions

	CameraView();					// View the scene from current camera

	world.DrawReflectiveSurface(posl, elapsed_secs);	
	world.DrawWorld(elapsed_secs);
	glFlush();
	
	// Ensure that we keep a constant frame rate
	DWORD sleep_time = GetTickCount() - current_time;
	sleep_time = (sleep_time > FRAME_INTERVAL) ? 0 : FRAME_INTERVAL - sleep_time;
	Sleep(sleep_time);

	return 0;
}

int CGame::Shutdown()
{
	return 0;
}