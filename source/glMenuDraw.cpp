//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Patrick Crawford  2004
////						
////  Note display list just so I know how, is included here for the boxes
////
//////////////////////////////////////////////////////////////////////////////

#include "glMenuDraw.h"
#include "glsetup.h"

//GLuint displayList;

extern HWND hWnd;

//////////////////////////////////// INITGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
//// All setup for OpenGL goes here
//// Texture/Model loading is also done here, before anything else
////
//////////////////////////////////// INITGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int initGLMenu(void) {													

	BuildFont();

	ShowCursor(true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// Clear the screen
	
	//resizeGLScene();

	glShadeModel(GL_SMOOTH);									// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);						// Black Background
	glClearDepth(1.0f);											// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
	glClearStencil(0);
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Texture set up
	///////////////////////////////////////////////
	glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping 
	
	// Viewport set up
	//////////////////////////////////////////////

	glViewport(0,0,H_RESOLUTION,V_RESOLUTION);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)H_RESOLUTION/(GLfloat)V_RESOLUTION,0.1f,10.0f);

	glMatrixMode(GL_MODELVIEW);	


	// creating a display list of the boxes... just so I know how.. used in drawBoxes

	/*displayList = glGenLists(1);
	
	glNewList(displayList, 0x1300);
		glBegin(GL_QUADS);								// Draw A Quad
			glVertex3f(-1.0f, 1.0f, 0.0f);				// Top Left
			glVertex3f( 1.0f, 1.0f, 0.0f);				// Top Right
			glVertex3f( 1.0f,-1.0f, 0.0f);				// Bottom Right
			glVertex3f(-1.0f,-1.0f, 0.0f);				// Bottom Left
		glEnd();
	glEndList();*/

	
	return true;														
}


//////////////////////////////////// DrawGLScene \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
////  This is the main rendering loop for the game.
////  This function is called by WinMain constantly, once done, the buffers
////  are swapped and the scene displayed.
////
//////////////////////////////////// DrawGLScene \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//int drawGLMenu(int mouse_x,int mouse_y)	{		

	// Clear Screen And Depth Buffer
	
	//drawBoxes();

	//return true;							
//}

int whichMouseObject(int mouse_x, int mouse_y,void (*drawFunc)(int,int))			// This Is Where Selection Is Done
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	int retVal = -1;								// Default return value (Nothing was selected) 
	
	GLuint	buffer[512];							// Set Up A Selection Buffer
	GLint	hits;									// The Number Of Objects That We Selected while in selection mode

	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);						// Tell OpenGL To Use Our Array For Selection

	
	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn. Object ID's and Extents Are Stored In The Buffer.
	(void) glRenderMode(GL_SELECT);

	glInitNames();								// Initializes The Name Stack
	glPushName(0);								// Push 0 (At Least One Entry) Onto The Stack
												// NOTE glLoadName(int) allows the naming of individual objects by a specific name (consider using ENUM)

	glMatrixMode(GL_PROJECTION);				// Selects The Projection Matrix
	glPushMatrix();								// Push The Projection Matrix
	glLoadIdentity();							// Resets The Matrix
	
	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix((GLdouble) mouse_x, (GLdouble) (viewport[3]-mouse_y), 1.0f, 1.0f, viewport);

	// Apply The Perspective Matrix
	//gluPerspective(45.0f, (GLfloat) (viewport[2]-viewport[0])/(GLfloat) (viewport[3]-viewport[1]), 0.1f, 100.0f);

	glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 ); // set that matrix to ortho mode

	// Select The Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	
	// Render The Targets To The Selection Buffer (My stupid square(s)) with names pushed on the name stack after every object's co-ord's are set
	
	drawFunc(-1,-1);							
	
	glMatrixMode(GL_PROJECTION);					// Select The Projection Matrix
	glPopMatrix();									// Pop The Projection Matrix

	hits=glRenderMode(GL_RENDER);					// Switch To Render Mode, Find Out How Many

	if (hits > 0)
	{
		retVal = buffer[0] - 1;	
	}	


	glMatrixMode(GL_PROJECTION);				// Selects The Projection Matrix
	glLoadIdentity();	
	glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 ); // set that matrix to ortho mode

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();

	return retVal;
}


void drawBoxes(int clickedIndex,int overIndex)
{	
	if (overIndex >-1) // what to do on mouseOver
	{
		switch(overIndex) // what to do on Click
		{
			case 0:
	
				glColor3ub(55,55,255);

				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(400,500);				// Top Left
					glVertex2f(500,500);				// Top Right
					glVertex2f(500,600);				// Bottom Right
					glVertex2f(400,600);				// Bottom Left
				glEnd();
				
				glPushName(0);
				
				glColor3ub(255,255,255);
				
				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(510,500);				// Top Left
					glVertex2f(610,500);				// Top Right
					glVertex2f(610,600);				// Bottom Right
					glVertex2f(510,600);				// Bottom Left
				glEnd();

				break;

			case 1:
	
				glColor3ub(255,255,255);

				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(400,500);				// Top Left
					glVertex2f(500,500);				// Top Right
					glVertex2f(500,600);				// Bottom Right
					glVertex2f(400,600);				// Bottom Left
				glEnd();
				
				glPushName(0);
				
				glColor3ub(55,55,255);
				
				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(510,500);				// Top Left
					glVertex2f(610,500);				// Top Right
					glVertex2f(610,600);				// Bottom Right
					glVertex2f(510,600);				// Bottom Left
				glEnd();

				break;
		}	
	}


	if (clickedIndex >-1) //what to do on Mouse Click
	{
		switch(clickedIndex) // what to do on Click
		{
			case 0:
	
				glColor3ub(255,55,55);

				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(400,500);				// Top Left
					glVertex2f(500,500);				// Top Right
					glVertex2f(500,600);				// Bottom Right
					glVertex2f(400,600);				// Bottom Left
				glEnd();

				glPushName(0);
				
				glColor3ub(255,255,255);
				
				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(510,500);				// Top Left
					glVertex2f(610,500);				// Top Right
					glVertex2f(610,600);				// Bottom Right
					glVertex2f(510,600);				// Bottom Left
				glEnd();

				break;

			case 1:
	
				glColor3ub(255,255,255);

				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(400,500);				// Top Left
					glVertex2f(500,500);				// Top Right
					glVertex2f(500,600);				// Bottom Right
					glVertex2f(400,600);				// Bottom Left
				glEnd();
				
				glPushName(0);
				
				glColor3ub(255,55,55);
				
				glBegin(GL_QUADS);								// Draw A Quad
					glVertex2f(510,500);				// Top Left
					glVertex2f(610,500);				// Top Right
					glVertex2f(610,600);				// Bottom Right
					glVertex2f(510,600);				// Bottom Left
				glEnd();

				break;
		}	
	}	
	
	if(overIndex == -1)
	{
		
		glColor3ub(255,255,255);

		glBegin(GL_QUADS);								// Draw A Quad
			glVertex2f(400,500);				// Top Left
			glVertex2f(500,500);				// Top Right
			glVertex2f(500,600);				// Bottom Right
			glVertex2f(400,600);				// Bottom Left
		glEnd();
		
		glPushName(0);
		
		glBegin(GL_QUADS);								// Draw A Quad
			glVertex2f(510,500);				// Top Left
			glVertex2f(610,500);				// Top Right
			glVertex2f(610,600);				// Bottom Right
			glVertex2f(510,600);				// Bottom Left
		glEnd();
	}
}


void draw3d(void){

	static float angle = 0.0f;
	static float angle2 = 0.0f;

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);				// Selects The Projection Matrix
	glLoadIdentity();	
	gluPerspective(45.0f,(GLfloat)H_RESOLUTION/(GLfloat)V_RESOLUTION,0.1f,700.0f); 
	
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();

	

	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glRotatef(angle2, 0.0f, 0.0f, 1.0f);

	float width  = 1;
	float height = 1;
	float length = 1;
	float x = 0 - width / 2;
	float y = 0 - height / 2;
	float z = 0 - length / 2;

	glColor3ub(0, 200, 0);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	glColor3ub(0, 100, 100);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	glColor3ub(0, 200, 200);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	glColor3ub(200, 200, 0);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	glColor3ub(100, 100, 0);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	glColor3ub(100, 200, 120);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();

	glPopMatrix();

	angle += 0.5f;
	angle2 += 0.05f;
	

}

