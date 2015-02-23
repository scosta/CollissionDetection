/*-----------------------------------------------------------------------------------
File:			world.cpp
Authors:		Steve Costa
Description:	This class defines the entire world of the simulation.  It will
				load the world description from a text file and dynamically
				create all of the world objects.
-----------------------------------------------------------------------------------*/

#include "world.h"						// Common macros

/*-----------------------------------------------------------------------------------
Initialise world state variables.
-----------------------------------------------------------------------------------*/

CWorld::CWorld()
{
	// Set pointers to null
	p_sphere_obj = NULL;
	p_balls = NULL;
	p_walls = NULL;
	p_boxes = NULL;

	// Initialize the colour array

	// Red
	world_colors[0][0] = 1.0f; world_colors[0][1] = 0.0f; 
	world_colors[0][2] = 0.0f; world_colors[0][3] = 1.0f;

	// Green
	world_colors[1][0] = 0.0f; world_colors[1][1] = 1.0f; 
	world_colors[1][2] = 0.0f; world_colors[1][3] = 1.0f;

	// Blue
	world_colors[2][0] = 0.0f; world_colors[2][1] = 0.0f; 
	world_colors[2][2] = 1.0f; world_colors[2][3] = 1.0f;

	// Yellow
	world_colors[3][0] = 1.0f; world_colors[3][1] = 1.0f; 
	world_colors[3][2] = 0.0f; world_colors[3][3] = 1.0f;

	// Purple
	world_colors[4][0] = 0.68f; world_colors[4][1] = 0.0f; 
	world_colors[4][2] = 1.0f; world_colors[4][3] = 1.0f;

	// Orange
	world_colors[5][0] = 1.0f; world_colors[5][1] = 0.52f; 
	world_colors[5][2] = 0.0f; world_colors[5][3] = 1.0f;

	// Light Blue
	world_colors[6][0] = 0.21f; world_colors[6][1] = 0.6f; 
	world_colors[6][2] = 0.81f; world_colors[6][3] = 1.0f;

	// White
	world_colors[7][0] = 1.0f; world_colors[7][1] = 1.0f; 
	world_colors[7][2] = 1.0f; world_colors[7][3] = 1.0f;

	// Light Gray
	world_colors[8][0] = 0.3f; world_colors[8][1] = 0.3f; 
	world_colors[8][2] = 0.3f; world_colors[8][3] = 0.3f;

	// Dark Grey
	world_colors[9][0] = 0.55f; world_colors[9][1] = 0.55f; 
	world_colors[9][2] = 0.55f; world_colors[9][3] = 1.0f;

	// Black
	world_colors[10][0] = 0.0f; world_colors[10][1] = 0.0f; 
	world_colors[10][2] = 0.0f; world_colors[10][3] = 1.0f;

	// Glass
	world_colors[11][0] = 0.8f; world_colors[11][1] = 0.8f; 
	world_colors[11][2] = 0.8f; world_colors[11][3] = 0.3f;

	// Clear
	world_colors[12][0] = 0.0f; world_colors[12][1] = 0.0f; 
	world_colors[12][2] = 0.0f; world_colors[12][3] = 0.0f;
}

void CWorld::Init()
{
	// Load textures
	t_manager.LoadTexture("textures\\leafs.bmp", &world_textures[0], 1);
	t_manager.LoadTexture("textures\\rinkside.bmp", &world_textures[1], 1);
	t_manager.LoadTexture("textures\\hnic.bmp", &world_textures[2], 1);
	t_manager.LoadTexture("textures\\cobblestone.bmp", &world_textures[3], 1);
	t_manager.LoadTexture("textures\\cobblestone2.bmp", &world_textures[4], 1);
	t_manager.LoadTexture("textures\\electric_big.bmp", &world_textures[5], 1);
	t_manager.LoadTexture("textures\\checker.bmp", &world_textures[6], 1);
	t_manager.LoadTexture("textures\\twirl.bmp", &world_textures[7], 1);
	t_manager.LoadTexture("textures\\marble.bmp", &world_textures[8], 1);
	t_manager.LoadTexture("textures\\electric.bmp", &world_textures[9], 1);
	t_manager.LoadTexture("textures\\green.bmp", &world_textures[10], 1);

	t_manager.LoadTexture("textures\\skybox\\front.bmp", &sky_textures[0], 1);
	t_manager.LoadTexture("textures\\skybox\\left.bmp", &sky_textures[1], 1);
	t_manager.LoadTexture("textures\\skybox\\right.bmp", &sky_textures[2], 1);
	t_manager.LoadTexture("textures\\skybox\\top.bmp", &sky_textures[3], 0);
	t_manager.LoadTexture("textures\\skybox\\back.bmp", &sky_textures[4], 1);
	t_manager.LoadTexture("textures\\skybox\\bottom.bmp", &sky_textures[5], 0);
	
	// Initialize quadratic for drawing balls
	p_sphere_obj = gluNewQuadric();
	gluQuadricNormals(p_sphere_obj, GLU_SMOOTH);
	gluQuadricTexture(p_sphere_obj, GL_TRUE);

	// Load world
	if (FAILED(Load("maps\\world_map.txt")))
		MessageBox(NULL, "Failed to load file!", "ERROR", MB_OK);

	// Normal of the clipping plane
	clip_plane[0] = -p_walls[0].normal.x; clip_plane[1] = -p_walls[0].normal.y;
	clip_plane[2] = -p_walls[0].normal.z; clip_plane[3] = 0.0f;
	

	// Render display lists
	RenderReflectiveSurface();
	RenderBoxes();
	RenderWalls();
	RenderSkyBox();
}

/*-----------------------------------------------------------------------------------
Read lines from a file ignoring empty lines and comments
-----------------------------------------------------------------------------------*/

void CWorld::ReadString(char *string, FILE *file)
{
	do
	{
		fgets(string, 512, file);
	} while ((string[0] == '#') || (string[0] == '\n'));
}

/*-----------------------------------------------------------------------------------
This method will a map configuration file and set the world object attributes
accordingly.

Errors:		-3500 = Failed to open file
			-3501 = Wrong file type
			-3502 = Data read error
			-3503 = Memory allocation error
-----------------------------------------------------------------------------------*/

int CWorld::Load(char *file_name)
{
	FILE *map_file;				// Input file stream variable
	char line[512];				// Holds line of text
	char *p_token;				// Point to first character of a token
	char *p_next_token;
	TVector temp[4];			// Temporary storage of geometric object vertices
	float temp_rad;				// Temporary radius variable
	int temp_rot;
	float temp_theta;			// Temporary rotation flag and rotation amount
	int temp_col, temp_tex;	// Temp color and texture indices

	// Check to see if the dynamic arrays must free memory first
	 if (p_balls != NULL)
		delete [] p_balls;

	if (p_walls != NULL)
		delete [] p_walls;

	if (p_boxes != NULL)
		delete [] p_boxes;
	
	// Open the file
	fopen_s(&map_file, file_name, "r");

	// # OF WALLS
	ReadString(line, map_file);					// Read line
	if (sscanf_s(line, "numwalls = %d", &num_walls) == EOF)
		return (-3502);							// Data read error
	
	// Knowing the number of walls we can allocate enough memory for
	// storing them
	try {
		p_walls = new TWall[num_walls];
	} catch (bad_alloc xa) {
		return (-3503);							// Memory allocation error
	}

	// The following section should contain 2 lines for each of the
	// wall variables for min, max, and rotation type with rotation angle
	for (int t = 0; t < num_walls; t++)				// Each wall
	{
	
		ReadString(line, map_file);			// Read next line
		p_token = strtok_s(line, " ,\t", &p_next_token);		// Read first value in the line

		for (int j = 0; j < 2; j++)				// Each vertex coordinate
		{
			// Store the x, y coordinates in two separate vectors
			if (sscanf_s(p_token, " %f", &temp[j][0]) == EOF)
				return (-3502);				// Data read error

			// Get the next value
			p_token = strtok_s(NULL, " ,\t", &p_next_token);

			// Store the x, y coordinates in two separate vectors
			if (sscanf_s(p_token, " %f", &temp[j][1]) == EOF)
				return (-3502);				// Data read error

			// Get the next value
			p_token = strtok_s(NULL, " ,\t", &p_next_token);
		} // End for

		// Get the colour and texture indices
		if (sscanf_s(p_token, " %d", &temp_col) == EOF)
				return (-3502);				// Data read error

		// Get the next value
		p_token = strtok_s(NULL, " ,\t", &p_next_token);

		if (sscanf_s(p_token, " %d", &temp_tex) == EOF)
				return (-3502);				// Data read error

		ReadString(line, map_file);			// Read next line
		p_token = strtok_s(line, " ,\t", &p_next_token);		// Read first value in the line

		// Read the translation vector
		for (int j = 0; j < 3; j++)
		{
			// Store the x, y, z coordinates
			if (sscanf_s(p_token, " %f", &temp[2][j]) == EOF)
				return (-3502);				// Data read error

			// Get the next value
			p_token = strtok_s(NULL, " ,\t", &p_next_token);
		}

		// Store the rotation flag coordinates
		if (sscanf_s(p_token, " %d", &temp_rot) == EOF)
			return (-3502);				// Data read error

		// Get the next value
		p_token = strtok_s(NULL, " ,\t", &p_next_token);

		// Store the rotation value
		if (sscanf_s(p_token, " %f", &temp_theta) == EOF)
			return (-3502);				// Data read error

		// We can now initialize the wall
		temp[0][2] = 0.0f;	temp[1][2] = 0.0f;	// coordinates can be referenced with . or []
		p_walls[t] = TWall(	temp[0], temp[1], temp[2], temp_theta, 
							temp_rot, temp_col, temp_tex);

	} // End for


	// # OF BALLS
	ReadString(line, map_file);					// Read line
	if (sscanf_s(line, "numballs = %d", &num_balls) == EOF)
		return (-3502);							// Data read error
	
	// Knowing the number of walls we can allocate enough memory for
	// storing them
	try {
		p_balls = new TBall[num_balls];
	} catch (bad_alloc xa) {
		return (-3503);							// Memory allocation error
	}
	
	// The following section should contain 1 line containing attributes for each ball
	for (int t = 0; t < num_balls; t++)				// Each ball
	{
		ReadString(line, map_file);			// Read next line
		p_token = strtok_s(line, " ,\t", &p_next_token);		// Read first value in the line

		for (int i = 0; i < 7; i++)					// Each attributs
		{
			if (i < 3) {						// Read centre
				if (sscanf_s(p_token, " %f", &temp[0][i]) == EOF)
					return (-3502);				// Data read error
			}
			else if (i >= 3 && i < 4) {			// Read radius
				if (sscanf_s(p_token, " %f", &temp_rad) == EOF)
					return (-3502);				// Data read error
			}
			else {								// Read velocity
				if (sscanf_s(p_token, " %f", &temp[1][i-4]) == EOF)
					return (-3502);				// Data read error
			}
			
			// Get the next value
			p_token = strtok_s(NULL, " ,\t", &p_next_token);
		}

		// Get the colour and texture indices
		if (sscanf_s(p_token, " %d", &temp_col) == EOF)
				return (-3502);				// Data read error

		// Get the next value
		p_token = strtok_s(NULL, " ,\t", &p_next_token);

		if (sscanf_s(p_token, " %d", &temp_tex) == EOF)
				return (-3502);				// Data read error
			
		// Initialize the ball
		p_balls[t] = TBall(temp[0], temp_rad, temp[1], temp_col, temp_tex);

	} // End for


	// # OF BOXES
	ReadString(line, map_file);					// Read line
	if (sscanf_s(line, "numboxes = %d", &num_boxes) == EOF)
		return (-3502);							// Data read error
	
	// Knowing the number of walls we can allocate enough memory for
	// storing them
	try {
		p_boxes = new TBox[num_boxes];
	} catch (bad_alloc xa) {
		return (-3503);							// Memory allocation error
	}
	
	// The following section should contain 1 line containing attributes for each box
	for (int t = 0; t < num_boxes; t++)				// Each ball
	{
		ReadString(line, map_file);			// Read next line
		p_token = strtok_s(line, " ,\t", &p_next_token);		// Read first value in the line

		for (int i = 0; i < 9; i++)					// Each attribute
		{
			if (i < 3) {						// Read min
				if (sscanf_s(p_token, " %f", &temp[0][i]) == EOF)
					return (-3502);				// Data read error
			}
			else if (i >= 3 && i < 6) {			// Read max
				if (sscanf_s(p_token, " %f", &temp[1][i-3]) == EOF)
					return (-3502);				// Data read error
			}
			else {								// Read velocity
				if (sscanf_s(p_token, " %f", &temp[2][i-6]) == EOF)
					return (-3502);				// Data read error
			}
			
			// Get the next value
			p_token = strtok_s(NULL, " ,\t", &p_next_token);
		}

		// Get the colour and texture indices
		if (sscanf_s(p_token, " %d", &temp_col) == EOF)
				return (-3502);				// Data read error

		// Get the next value
		p_token = strtok_s(NULL, " ,\t", &p_next_token);

		if (sscanf_s(p_token, " %d", &temp_tex) == EOF)
				return (-3502);				// Data read error
			
		// Initialize the ball
		p_boxes[t] = TBox(temp[0], temp[1], temp[2], temp_col, temp_tex);
				
	} // End for
	
	// All information has been extracted, close the file
	fclose(map_file);

	return 1;
}

/*-----------------------------------------------------------------------------------
Treat the first wall object as a reflective surface.  The wall itself
must be rendered in its own display list to be used in the rendering
of the reflective surface.
-----------------------------------------------------------------------------------*/

void CWorld::RenderReflectiveSurface()
{
	l_reflective_surface = glGenLists(1);

	// Save colour settings
	glPushAttrib(GL_CURRENT_BIT);

	glNewList(l_reflective_surface, GL_COMPILE);

		// Apply texture
		int t = p_walls[0].texture;
		if (t >= 0 && t < MAX_TEXTURES) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, world_textures[t]);
		}

		// Apply colour
		int c = p_walls[0].color;
		if (c >= 0 && c < MAX_COLORS) {
			glColor4f(	world_colors[c][0], world_colors[c][1],
						world_colors[c][2], world_colors[c][3]);
		}

		// Begin Drawing
		// Get the four coordinates for the wall and apply the transformation
		// matrix to them
		glBegin(GL_QUADS);
			TVector temp_point;

			glNormal3f(p_walls[0].normal.x, p_walls[0].normal.y, p_walls[0].normal.z);

			temp_point = p_walls[0].GetVertex(0) * p_walls[0].trans;
			glTexCoord2f(0.0f, 0.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = p_walls[0].GetVertex(1) * p_walls[0].trans;
			glTexCoord2f(1.0f, 0.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = p_walls[0].GetVertex(3) * p_walls[0].trans;
			glTexCoord2f(1.0f, 1.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = p_walls[0].GetVertex(2) * p_walls[0].trans;
			glTexCoord2f(0.0f, 1.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		if (t >= 0 && t < MAX_TEXTURES) glDisable(GL_TEXTURE_2D);

	glEndList();

	glPopAttrib();
}

/*-----------------------------------------------------------------------------------
Using the stencil buffer reflection effects are applied to the reflective
surface object.
-----------------------------------------------------------------------------------*/

void CWorld::DrawReflectiveSurface(float *posl, float dt)
{
	glColorMask(0, 0, 0, 0);						// Prevent any drawing to appear

	glEnable(GL_STENCIL_TEST);						// Enable stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 1);					// Type of test on each pixel
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);		// What to do depending on test outcomes

	glDisable(GL_DEPTH_TEST);
	
	glCallList(l_reflective_surface);

	glEnable(GL_DEPTH_TEST);
	glColorMask(1, 1, 1, 1);
	glStencilFunc(GL_EQUAL, 1, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, clip_plane);		// Plane for reflected objects

	// Draw world reflected in surface
	glPushMatrix();
		glScalef(1.0f, -1.0f, 1.0f);
		glLightfv(GL_LIGHT0, GL_POSITION, posl);
		DrawWorld(dt);
	glPopMatrix();

	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_STENCIL_TEST);

	// Now draw the real reflective floor
	glLightfv(GL_LIGHT0, GL_POSITION, posl);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCallList(l_reflective_surface);

	// Set up lighting for drawing of the objects in world
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

/*-----------------------------------------------------------------------------------
Render all the boxes to display lists
-----------------------------------------------------------------------------------*/

void CWorld::RenderBoxes()
{
	TVector temp_point0, temp_point1, temp_point2, 
			temp_point3, vector1, vector2, norm;

	l_boxes = glGenLists(num_boxes);

	// Save colour settings
	glPushAttrib(GL_CURRENT_BIT);

	for (int i = 0; i < num_boxes; i++)
	{

		glNewList(l_boxes + i, GL_COMPILE);

			// Apply texture
			int t = p_boxes[i].texture;
			if (t >= 0 && t < MAX_TEXTURES) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, world_textures[t]);
			}

			// Apply colour
			int c = p_boxes[i].color;
			if (c >= 0 && c < MAX_COLORS) {
				if (c >= 11) { // Disable lights to allow transparency
					glEnable(GL_BLEND);
					glDisable(GL_LIGHTING);
				}

				glColor4f(	world_colors[c][0], world_colors[c][1],
							world_colors[c][2], world_colors[c][3]);
			}

			// Begin Drawing
			glBegin(GL_QUADS);
				// Front Face
				temp_point0 = p_boxes[i].GetVertex(4) - p_boxes[i].GetVertex(0);
				temp_point1 = p_boxes[i].GetVertex(5) - p_boxes[i].GetVertex(0);
				temp_point2 = p_boxes[i].GetVertex(7) - p_boxes[i].GetVertex(0);
				temp_point3 = p_boxes[i].GetVertex(6) - p_boxes[i].GetVertex(0);

				vector1 = temp_point1 - temp_point0;
				vector2 = temp_point2 - temp_point1;

				norm = CrossProduct(vector1, vector2);
				norm.Normalize();

				glNormal3f(norm.x, norm.y, norm.z);
				glVertex3f(temp_point0.x, temp_point0.y, temp_point0.z);
				glVertex3f(temp_point1.x, temp_point1.y, temp_point1.z);
				glVertex3f(temp_point2.x, temp_point2.y, temp_point2.z);
				glVertex3f(temp_point3.x, temp_point3.y, temp_point3.z);
					
				// Left Face
				temp_point0 = p_boxes[i].GetVertex(0) - p_boxes[i].GetVertex(0);
				temp_point1 = p_boxes[i].GetVertex(4) - p_boxes[i].GetVertex(0);
				temp_point2 = p_boxes[i].GetVertex(6) - p_boxes[i].GetVertex(0);
				temp_point3 = p_boxes[i].GetVertex(2) - p_boxes[i].GetVertex(0);

				vector1 = temp_point1 - temp_point0;
				vector2 = temp_point2 - temp_point1;

				norm = CrossProduct(vector1, vector2);
				norm.Normalize();

				glNormal3f(norm.x, norm.y, norm.z);
				glVertex3f(temp_point0.x, temp_point0.y, temp_point0.z);	
				glVertex3f(temp_point1.x, temp_point1.y, temp_point1.z);
				glVertex3f(temp_point2.x, temp_point2.y, temp_point2.z);
				glVertex3f(temp_point3.x, temp_point3.y, temp_point3.z);

				// Right Face
				temp_point0 = p_boxes[i].GetVertex(5) - p_boxes[i].GetVertex(0);
				temp_point1 = p_boxes[i].GetVertex(1) - p_boxes[i].GetVertex(0);
				temp_point2 = p_boxes[i].GetVertex(3) - p_boxes[i].GetVertex(0);
				temp_point3 = p_boxes[i].GetVertex(7) - p_boxes[i].GetVertex(0);

				vector1 = temp_point1 - temp_point0;
				vector2 = temp_point2 - temp_point1;

				norm = CrossProduct(vector1, vector2);
				norm.Normalize();

				glNormal3f(norm.x, norm.y, norm.z);
				glVertex3f(temp_point0.x, temp_point0.y, temp_point0.z);
				glVertex3f(temp_point1.x, temp_point1.y, temp_point1.z);
				glVertex3f(temp_point2.x, temp_point2.y, temp_point2.z);
				glVertex3f(temp_point3.x, temp_point3.y, temp_point3.z);

				// Back Face
				temp_point0 = p_boxes[i].GetVertex(2) - p_boxes[i].GetVertex(0);
				temp_point1 = p_boxes[i].GetVertex(3) - p_boxes[i].GetVertex(0);
				temp_point2 = p_boxes[i].GetVertex(1) - p_boxes[i].GetVertex(0);
				temp_point3 = p_boxes[i].GetVertex(0) - p_boxes[i].GetVertex(0);

				vector1 = temp_point1 - temp_point0;
				vector2 = temp_point2 - temp_point1;

				norm = CrossProduct(vector1, vector2);
				norm.Normalize();

				glNormal3f(norm.x, norm.y, norm.z);
				glVertex3f(temp_point0.x, temp_point0.y, temp_point0.z);
				glVertex3f(temp_point1.x, temp_point1.y, temp_point1.z);
				glVertex3f(temp_point2.x, temp_point2.y, temp_point2.z);
				glVertex3f(temp_point3.x, temp_point3.y, temp_point3.z);

				// Top Face
				temp_point0 = p_boxes[i].GetVertex(6) - p_boxes[i].GetVertex(0);
				temp_point1 = p_boxes[i].GetVertex(7) - p_boxes[i].GetVertex(0);
				temp_point2 = p_boxes[i].GetVertex(3) - p_boxes[i].GetVertex(0);
				temp_point3 = p_boxes[i].GetVertex(2) - p_boxes[i].GetVertex(0);

				vector1 = temp_point1 - temp_point0;
				vector2 = temp_point2 - temp_point1;

				norm = CrossProduct(vector1, vector2);
				norm.Normalize();

				glNormal3f(norm.x, norm.y, norm.z);
				glVertex3f(temp_point0.x, temp_point0.y, temp_point0.z);
				glVertex3f(temp_point1.x, temp_point1.y, temp_point1.z);
				glVertex3f(temp_point2.x, temp_point2.y, temp_point2.z);
				glVertex3f(temp_point3.x, temp_point3.y, temp_point3.z);
			glEnd();

			if (t >= 0 && t < MAX_TEXTURES) glDisable(GL_TEXTURE_2D);
			if (c >= 11) {
				glEnable(GL_LIGHTING);
				glDisable(GL_BLEND);
			}

		glEndList();
	
	} // End for

	glPopAttrib();
}

/*-----------------------------------------------------------------------------------
Render the walls to display lists.
-----------------------------------------------------------------------------------*/

void CWorld::RenderWalls()
{
	l_walls = glGenLists(num_walls - 1);

	// Save colour settings
	glPushAttrib(GL_CURRENT_BIT);

	for (int i = 1; i < num_walls; i++)
	{
		glNewList(l_walls + i - 1, GL_COMPILE);

			// Apply texture
			int t = p_walls[i].texture;
			if (t >= 0 && t < MAX_TEXTURES) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, world_textures[t]);
			}

			// Apply colour
			int c = p_walls[i].color;
			if (c >= 0 && c < MAX_COLORS) {
				if (c >= 11) { // Disable lights to allow transparency
					glEnable(GL_BLEND);
					glDisable(GL_LIGHTING);
				}
				glColor4f(	world_colors[c][0], world_colors[c][1],
							world_colors[c][2], world_colors[c][3]);
			}

			// Begin Drawing
			// Get the four coordinates for the wall and apply the transformation
			// matrix to them
			glBegin(GL_QUADS);
				TVector temp_point;

				glNormal3f(p_walls[i].normal.x, p_walls[i].normal.y, p_walls[i].normal.z);

				temp_point = p_walls[i].GetVertex(0) * p_walls[i].trans;
				glTexCoord2f(0.0f, 0.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
				temp_point = p_walls[i].GetVertex(1) * p_walls[i].trans;
				glTexCoord2f(1.0f, 0.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
				temp_point = p_walls[i].GetVertex(3) * p_walls[i].trans;
				glTexCoord2f(1.0f, 1.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
				temp_point = p_walls[i].GetVertex(2) * p_walls[i].trans;
				glTexCoord2f(0.0f, 1.0f);  glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			glEnd();

			if (t >= 0 && t < MAX_TEXTURES) glDisable(GL_TEXTURE_2D);
			if (c >= 11) {
				glEnable(GL_LIGHTING);
				glDisable(GL_BLEND);
			}

		glEndList();

	} // End for

	glPopAttrib();
}

/*-----------------------------------------------------------------------------------
The sky box encapsulates the whole world in a city landscape.  It is an AABB which
is initially empty.  This method will look at all the objects in the world
and expand the box so that it is large enough to encapsulate the whole world.
-----------------------------------------------------------------------------------*/

void CWorld::RenderSkyBox()
{
	TVector temp_point;

	l_sky = glGenLists(1);

	// We must create a sky box that is big enough
	// to contain the world that was created
	TAABB skybox;
	skybox.Empty();

	// Encompass all the walls
	for (int i = 0; i < num_walls; i++)
	{
		temp_point = p_walls[i].GetVertex(0) * p_walls[i].trans;
		skybox.Add(temp_point);
	}

	//Encompass all the boxes
	for (int i = 0; i < num_boxes; i++)
	{
		for (int t = 0; t < 8; t++)
		{
			temp_point = p_boxes[i].GetVertex(t);
			skybox.Add(temp_point);
		}
	}

	// Encompass all the balls
	for (int i = 0; i < num_balls; i++)
	{
		temp_point = p_balls[i].center + TVector(p_balls[i].radius, 0.0f, 0.0f);
		skybox.Add(temp_point);
		temp_point = p_balls[i].center + TVector(-p_balls[i].radius, 0.0f, 0.0f);
		skybox.Add(temp_point);
		temp_point = p_balls[i].center + TVector(0.0f, p_balls[i].radius, 0.0f);
		skybox.Add(temp_point);
		temp_point = p_balls[i].center + TVector(0.0f, -p_balls[i].radius, 0.0f);
		skybox.Add(temp_point);
		temp_point = p_balls[i].center + TVector(0.0f, 0.0f, p_balls[i].radius);
		skybox.Add(temp_point);
		temp_point = p_balls[i].center + TVector(0.0f, 0.0f, -p_balls[i].radius);
	}

	// Now we'll stretch it out a little more
	skybox.maxv += TVector(50.0f, 40.0f, 50.0f);
	skybox.minv -= TVector(50.0f, 40.0f, 50.0f);

	// Save colour settings
	glPushAttrib(GL_CURRENT_BIT);

	glNewList(l_sky, GL_COMPILE);
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
				
		// Front Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[0]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, -1.0f);
			temp_point = skybox.GetVertex(5);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(4);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(6);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(7);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		// Left Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[1]);
		glBegin(GL_QUADS);
			glNormal3f(1.0f, 0.0f, 0.0f);
			temp_point = skybox.GetVertex(2);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(6);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(4);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

        // Right Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[2]);
		glBegin(GL_QUADS);
			glNormal3f(-1.0f, 0.0f, 0.0f);
			temp_point = skybox.GetVertex(7);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(3);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(1);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(5);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		// Top Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[3]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, -1.0f, 0.0f);
			temp_point = skybox.GetVertex(7);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(6);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(2);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(3);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		// Back Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[4]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			temp_point = skybox.GetVertex(3);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(2);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(1);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		// Bottom Face
		glBindTexture(GL_TEXTURE_2D, sky_textures[5]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			temp_point = skybox.GetVertex(1);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(4);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
			temp_point = skybox.GetVertex(5);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(temp_point.x, temp_point.y, temp_point.z);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	glEndList();
	
	glPopAttrib();
}

/*-----------------------------------------------------------------------------------
Draw all the components of the world.
-----------------------------------------------------------------------------------*/

void CWorld::DrawWorld(float dt)
{
	// Draw the boxes
	glPushAttrib(GL_CURRENT_BIT);
	for (int i = 0; i < num_boxes; i++)
	{		
		glPushMatrix();
			glTranslatef(p_boxes[i].minv.x, p_boxes[i].minv.y, p_boxes[i].minv.z);
			glCallList(l_boxes + i);
		glPopMatrix();
	}
	glPopAttrib();

	// Draw the balls
	glPushAttrib(GL_CURRENT_BIT);
	for (int i = 0; i < num_balls; i++)
	{
		glPushMatrix();

			// Apply texture
			int t = p_balls[i].texture;
			if (t >= 0 && t < MAX_TEXTURES) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, world_textures[t]);
			}

			// Apply colour
			int c = p_balls[i].color;
			if (c >= 0 && c < MAX_COLORS) {

				if (c >= 11) { // Disable lights to allow transparency
					glEnable(GL_BLEND);
					glDisable(GL_LIGHTING);
				}
				glColor4f(	world_colors[c][0], world_colors[c][1],
							world_colors[c][2], world_colors[c][3]);
			}
		
			// Rotate the balls proportional to their speed * time
			p_balls[i].Rotate(dt);
			glTranslatef(p_balls[i].center.x, p_balls[i].center.y, p_balls[i].center.z);
			glMultMatrixf(p_balls[i].rot.m);
			gluSphere(p_sphere_obj, p_balls[i].radius, 20, 20);

			if (t >= 0 && t < MAX_TEXTURES) glDisable(GL_TEXTURE_2D);
			if (c >= 11) {
				glEnable(GL_LIGHTING);
				glDisable(GL_BLEND);
			}

		glPopMatrix();
	}
	glPopAttrib();

	glPushAttrib(GL_CURRENT_BIT);
		glCallList(l_sky);
	glPopAttrib();

	glPushAttrib(GL_CURRENT_BIT);
	// Draw the walls
	for (int i = 1; i < num_walls; i++)
		glCallList(l_walls + i - 1);
	glPopAttrib();
}

void CWorld::ShutDown()
{
	if (p_balls != NULL)
		delete [] p_balls;

	if (p_walls != NULL)
		delete [] p_walls;

	if (p_boxes != NULL)
		delete [] p_boxes;

	gluDeleteQuadric(p_sphere_obj);
}