//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Patrick Crawford  2004
////						
////  Note display list just so I know how, is included here for the boxes
////
//////////////////////////////////////////////////////////////////////////////

#include "MenuSystem.h"
#include "glsetup.h"
#include "FModSound.h"
#include "texture.h"

#define COST_OF_SECOND_CAR 500

extern HWND hWnd;

Track track1;
Track track1Collide;
Model trackTrees;
Model trackGround;

CarSpec slowCarSpec;
CarSpec fastCarSpec;

Car player1(&track1Collide);
Car player2(&track1Collide);

extern Sound::FModSound gbsound;

int previousRollIndex = -1;

//////////////////////////////////// INITGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
//// All setup for OpenGL goes here
//// Texture/Model loading is also done here, before anything else
////
//////////////////////////////////// INITGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

//extern int tempPlayerMode;

int MenuSystem::initGLMenu(void) {

	// Car specs, these should really be loaded from car.ini but ah well
	//
	// NOTE to Radek:
	//
	// look in gldraw.cpp at the top of the file you'll see
	// #define SLOWTHROTTLE 100
	// #define FASTTHROTTLE 150
	// These are the throttle percentages for the slow car and fast car respectively
	// These can greatly affect physics, the more throttle the twitchier the car
	// and the more the car will overcome drag and resistance.
	//
	// in other words, drag and resistance can greatly affect speed, but more throttle
	// will overpower them.
	//
	// have fun.
	//
	fastCarSpec.b			 = 1.5f;	// m	distance from CG to front axle						
	fastCarSpec.c			 = 1.5f;	// m    idem to rear axle
	fastCarSpec.wheelbase	 = fastCarSpec.b + fastCarSpec.c;
	fastCarSpec.h			 = 2.0;		// m
	fastCarSpec.mass		 = 1500;	// kg			
	fastCarSpec.inertia		 = 1500;	// kg.m			
	fastCarSpec.width		 = 1.5;		// m
	fastCarSpec.length		 = 4.0;		// m
	fastCarSpec.wheellength  = 0.8f;
	fastCarSpec.wheelwidth	 = 0.5f;
	fastCarSpec.DRAG		 = 5.0f;	// factor for air resistance (drag) 5
	fastCarSpec.RESISTANCE   = 100.0f;	// factor for rolling resistance 100
	fastCarSpec.CA_R		 = -6.20f;	// cornering stiffness
	// note, try loosening front springs
	fastCarSpec.CA_F		 = -5.0f;	// cornering stiffness 
	fastCarSpec.MAX_GRIP	 = 6.0f;	// maximum (normalized) friction force, =diameter of friction circle
	
	slowCarSpec.b			 = 1.5f;	// m	distance from CG to front axle						
	slowCarSpec.c			 = 1.5f;	// m    idem to rear axle
	slowCarSpec.wheelbase	 = slowCarSpec.b + slowCarSpec.c;
	slowCarSpec.h			 = 2.0;		// m
	slowCarSpec.mass		 = 2000;	// kg			
	slowCarSpec.inertia		 = 1500;	// kg.m			
	slowCarSpec.width		 = 1.5;		// m
	slowCarSpec.length		 = 4.0;		// m
	slowCarSpec.wheellength  = 0.8f;
	slowCarSpec.wheelwidth	 = 0.5f;
	slowCarSpec.DRAG		 = 10.0f;	// factor for air resistance (drag)
	slowCarSpec.RESISTANCE   = 100.0f;	// factor for rolling resistance
	slowCarSpec.CA_R		 = -4.5f;	// cornering stiffness //
	slowCarSpec.CA_F		 = -3.2f;	// cornering stiffness 
	slowCarSpec.MAX_GRIP	 = 5.0f;	// maximum (normalized) friction force, =diameter of friction circle

			
	// TEMP

	/*scores scoresOne;

	scoresOne.car = "Car One";
	scoresOne.player_name = "GOOD TIME";
	scoresOne.time = 20.40;
	scoresOne.track = 1;

	//scores scoresTwo;

	scoresTwo.car = "Car Two";
	scoresTwo.player_name = "PLAYER 2";
	scoresTwo.time = 70.90;
	scoresTwo.track = 2;

	save_scores(scoresOne);
	save_scores(scoresTwo);*/
	
	BuildFont();

	currentErrorState = ALL_OK; // SET ERROR STATE TO EVERYTHING COOL
	hasProfile = false;

	ShowCursor(true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// Clear the screen
	
	//resizeGLScene();

	glShadeModel(GL_SMOOTH);									// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);						// Black Background
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);									// Disables Depth Testing
	glClearStencil(0);
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Texture set up
	///////////////////////////////////////////////
	glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping 
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)H_RESOLUTION/(GLfloat)V_RESOLUTION,0.1f,10.0f);

	glMatrixMode(GL_MODELVIEW);	
	
	// Viewport set up
	//////////////////////////////////////////////
	glViewport(0,0,H_RESOLUTION,V_RESOLUTION);

	//Load & Unload all states
	
	/*MenuStates states[15];

	states[0] = TITLE_SCREEN;
	states[1] = INIT_GAMEPLAY;
	states[2] = GAMEPLAY;
	states[3] = QUIT;
	states[4] = ONE_PLAYER_TITLE;
	states[5] = ONE_PLAYER_COURSE;
	states[6] = ONE_PLAYER_DRIVER;
	states[7] = ONE_PLAYER_RACE;
	states[8] = ONE_PLAYER_VEHICLE;
	states[9] = TWO_PLAYER_TITLE;
	states[10] = TWO_PLAYER_1_VEHICLE;
	states[11] = TWO_PLAYER_2_VEHICLE;
	states[12] = TWO_PLAYER_COURSE;
	states[13] = TWO_PLAYER_RACE;
	states[14] = HIGH_SCORES;

	for(int i =0; i < 15; i++)
	{
		setState(states[i]);
	}*/
	
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

int MenuSystem::whichMouseObject(int mouse_x, int mouse_y)			// This Is Where Selection Is Done
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
	
	switch(currentState)
	{
		case TITLE_SCREEN:
			drawTitleScreenSelect();
			break;
		case ONE_PLAYER_TITLE:
			drawOnePlayerTitleSelect();
			break;
		case TWO_PLAYER_TITLE:
			drawTwoPlayerTitleSelect();
			break;
		case ONE_PLAYER_COURSE:
			drawOnePlayerCourseSelect();
			break;
		case ONE_PLAYER_DRIVER:
			drawOnePlayerDriverSelect();
			break;
		case ONE_PLAYER_RACE:
			drawOnePlayerRaceSelect();
			break;
		case ONE_PLAYER_VEHICLE:
			drawOnePlayerVehicleSelect();
			break;
		case TWO_PLAYER_1_VEHICLE:
			drawTwoPlayer1VehicleSelect();
			break;
		case TWO_PLAYER_2_VEHICLE:
			drawTwoPlayer2VehicleSelect();
			break;
		case TWO_PLAYER_COURSE:
			drawTwoPlayerCourseSelect();
			break;
		case TWO_PLAYER_RACE:
			drawTwoPlayerRaceSelect();
			break;
		case HIGH_SCORES:
			drawHighScoresSelect();
			break;
	}
							
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
void MenuSystem::drawTitleScreenSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

}

void MenuSystem::drawTitleScreen(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 5.tga

	glBindTexture(GL_TEXTURE_2D,textures[4]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[8]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{	
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawLoadingScreen()
{	
	glEnable(GL_TEXTURE_2D);
	glColor3ub(255,255,255);
	//GLuint tempTexture;

	vector<unsigned int> currentTextures;

	currentTextures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/1.tga",-1,0,NICEST,1));
	currentTextures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/2.tga",-1,0,NICEST,1));
	currentTextures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/3.tga",-1,0,NICEST,1));
	currentTextures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/4.tga",-1,0,NICEST,1));

	//tempTexture = Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/1.tga");
	glBindTexture(GL_TEXTURE_2D,currentTextures[0]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();
	
	//Texture::TextureManager::getSingleton().freeTexture(tempTexture);

	
	//tempTexture = Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/2.tga");
	glBindTexture(GL_TEXTURE_2D,currentTextures[1]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	
	//tempTexture = Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/3.tga");
	glBindTexture(GL_TEXTURE_2D,currentTextures[2]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	
	//tempTexture = Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/LOADING_SCREEN/4.tga");
	glBindTexture(GL_TEXTURE_2D,currentTextures[3]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

}

void MenuSystem::drawOnePlayerTitleSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

}
void MenuSystem::drawOnePlayerTitle(int clickedIndex,int overIndex)
{	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawTwoPlayerTitleSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

}
void MenuSystem::drawTwoPlayerTitle(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawOnePlayerCourseSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(628,225);
		glVertex2f(690,225);
		glVertex2f(690,181);
		glVertex2f(628,181);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(690,225);
		glVertex2f(749,225);
		glVertex2f(749,181);
		glVertex2f(690,181);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawOnePlayerCourse(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(gameSettings.trackIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else if(gameSettings.trackIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(550,448);
		glTexCoord2f((283.0f/512.0f),1);glVertex2f(833,448);
		glTexCoord2f((283.0f/512.0f),1.0f - (208.0f/256.0f));glVertex2f(833,240);
		glTexCoord2f(0,1.0f - (208.0f/256.0f));glVertex2f(550,240);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawOnePlayerDriverSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(613,238);				
		glVertex2f(776,238);				
		glVertex2f(776,203);				
		glVertex2f(613,203);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(590,272);
		glVertex2f(810,272);
		glVertex2f(810,238);
		glVertex2f(590,238);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawOnePlayerDriver(int clickedIndex,int overIndex)
{
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(613,238);				
		glTexCoord2f((163.0f/256.0f),1);glVertex2f(776,238);				
		glTexCoord2f((163.0f/256.0f),1.0f - (35.0f/64.0f));glVertex2f(776,203);				
		glTexCoord2f(0,1.0f - (35.0f/64.0f));glVertex2f(613,203);				
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(590,272);
		glTexCoord2f((220.0f/256.0f),1);glVertex2f(810,272);
		glTexCoord2f((220.0f/256.0f),1.0f - (34.0f/64.0f));glVertex2f(810,238);
		glTexCoord2f(0,1.0f - (34.0f/64.0f));glVertex2f(590,238);
	glEnd();

	if( currentErrorState == DRIVER_DOES_NOT_EXIST)
	{
		glBindTexture(GL_TEXTURE_2D,textures[20]);
	}
	else if( currentErrorState == DRIVER_NAME_TAKEN )
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}
	else if( currentErrorState == PROFILE_REGISTERED)
	{
		glBindTexture(GL_TEXTURE_2D,textures[21]);
	}
	else if( currentErrorState == PROFILE_LOADED)
	{
		glBindTexture(GL_TEXTURE_2D,textures[22]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(582,346);
		glTexCoord2f((244.0f/256.0f),1);glVertex2f(806,346);
		glTexCoord2f((244.0f/256.0f),1.0f - (31.0f/32.0f));glVertex2f(806,315);
		glTexCoord2f(0,1.0f - (31.0f/32.0f));glVertex2f(582,315);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
	
}
void MenuSystem::drawOnePlayerRaceSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(632,283);
		glVertex2f(756,283);
		glVertex2f(756,192);
		glVertex2f(632,192);
	glEnd();

}
void MenuSystem::drawOnePlayerRace(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(632,283);
		glTexCoord2f((124.0f/128.0f),1);glVertex2f(756,283);
		glTexCoord2f((124.0f/128.0f),1.0f - (91.0f/128.0f));glVertex2f(756,192);
		glTexCoord2f(0,1.0f - (91.0f/128.0f));glVertex2f(632,192);
	glEnd();

	// ERROR MESSAGE QUAD

	if (hasProfile == false)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(582,346);
		glTexCoord2f((244.0f/256.0f),1);glVertex2f(806,346);
		glTexCoord2f((244.0f/256.0f),1.0f - (31.0f/32.0f));glVertex2f(806,315);
		glTexCoord2f(0,1.0f - (31.0f/32.0f));glVertex2f(582,315);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawOnePlayerVehicleSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(628,225);
		glVertex2f(690,225);
		glVertex2f(690,181);
		glVertex2f(628,181);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(690,225);
		glVertex2f(749,225);
		glVertex2f(749,181);
		glVertex2f(690,181);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawOnePlayerVehicle(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(gameSettings.p1->vehicleIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else if(gameSettings.p1->vehicleIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}
	else if(gameSettings.p1->vehicleIndex == -1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[20]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(550,448);
		glTexCoord2f((283.0f/512.0f),1);glVertex2f(833,448);
		glTexCoord2f((283.0f/512.0f),1.0f - (208.0f/256.0f));glVertex2f(833,240);
		glTexCoord2f(0,1.0f - (208.0f/256.0f));glVertex2f(550,240);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawTwoPlayer1VehicleSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(628,225);
		glVertex2f(690,225);
		glVertex2f(690,181);
		glVertex2f(628,181);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(690,225);
		glVertex2f(749,225);
		glVertex2f(749,181);
		glVertex2f(690,181);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawTwoPlayer1Vehicle(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(gameSettings.p1->vehicleIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else if(gameSettings.p1->vehicleIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(550,448);
		glTexCoord2f((283.0f/512.0f),1);glVertex2f(833,448);
		glTexCoord2f((283.0f/512.0f),1.0f - (208.0f/256.0f));glVertex2f(833,240);
		glTexCoord2f(0,1.0f - (208.0f/256.0f));glVertex2f(550,240);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawTwoPlayer2VehicleSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(628,225);
		glVertex2f(690,225);
		glVertex2f(690,181);
		glVertex2f(628,181);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(690,225);
		glVertex2f(749,225);
		glVertex2f(749,181);
		glVertex2f(690,181);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawTwoPlayer2Vehicle(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(gameSettings.p2->vehicleIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else if(gameSettings.p2->vehicleIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(550,448);
		glTexCoord2f((283.0f/512.0f),1);glVertex2f(833,448);
		glTexCoord2f((283.0f/512.0f),1.0f - (208.0f/256.0f));glVertex2f(833,240);
		glTexCoord2f(0,1.0f - (208.0f/256.0f));glVertex2f(550,240);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawTwoPlayerCourseSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(628,225);
		glVertex2f(690,225);
		glVertex2f(690,181);
		glVertex2f(628,181);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(690,225);
		glVertex2f(749,225);
		glVertex2f(749,181);
		glVertex2f(690,181);
	glEnd();

	glPushName(0);

}
void MenuSystem::drawTwoPlayerCourse(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(overIndex == 5)
	{
		glBindTexture(GL_TEXTURE_2D,textures[15]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[14]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(628,225);
		glTexCoord2f((62.0f/64.0f),1);glVertex2f(690,225);
		glTexCoord2f((62.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(690,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(628,181);
	glEnd();

	if(overIndex == 6)
	{
		glBindTexture(GL_TEXTURE_2D,textures[17]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[16]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(690,225);
		glTexCoord2f((59.0f/64.0f),1);glVertex2f(749,225);
		glTexCoord2f((59.0f/64.0f),1.0f - (44.0f/64.0f));glVertex2f(749,181);
		glTexCoord2f(0,1.0f - (44.0f/64.0f));glVertex2f(690,181);
	glEnd();

	if(gameSettings.trackIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[18]);
	}
	else if(gameSettings.trackIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[19]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(550,448);
		glTexCoord2f((283.0f/512.0f),1);glVertex2f(833,448);
		glTexCoord2f((283.0f/512.0f),1.0f - (208.0f/256.0f));glVertex2f(833,240);
		glTexCoord2f(0,1.0f - (208.0f/256.0f));glVertex2f(550,240);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawTwoPlayerRaceSelect()
{
	// 1.tga
	
	glBegin(GL_QUADS);							
		glVertex2f(297,742);				
		glVertex2f(461,742);				
		glVertex2f(461,578);				
		glVertex2f(297,578);				
	glEnd();

	glPushName(0);

	// 2.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,578);				
		glVertex2f(297,578);				
		glVertex2f(297,409);				
		glVertex2f(130,409);				
	glEnd();

	glPushName(0);

	// 3.tga

	glBegin(GL_QUADS);							
		glVertex2f(130,358);				
		glVertex2f(297,358);				
		glVertex2f(297,192);				
		glVertex2f(130,192);				
	glEnd();
	
	glPushName(0);

	// 4.tga

	glBegin(GL_QUADS);							
		glVertex2f(297,192);				
		glVertex2f(461,192);				
		glVertex2f(461,27);				
		glVertex2f(297,27);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);							
		glVertex2f(775,192);				
		glVertex2f(906,192);				
		glVertex2f(906,72);				
		glVertex2f(775,72);				
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(477,441);
		glVertex2f(542,441);
		glVertex2f(542,355);
		glVertex2f(477,355);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(542,441);
		glVertex2f(627,441);
		glVertex2f(627,355);
		glVertex2f(542,355);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(627,441);
		glVertex2f(734,441);
		glVertex2f(734,355);
		glVertex2f(627,355);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(734,441);
		glVertex2f(823,441);
		glVertex2f(823,355);
		glVertex2f(734,355);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(823,441);
		glVertex2f(913,441);
		glVertex2f(913,355);
		glVertex2f(823,355);
	glEnd();

	glPushName(0);

	glBegin(GL_QUADS);
		glVertex2f(632,283);
		glVertex2f(756,283);
		glVertex2f(756,192);
		glVertex2f(632,192);
	glEnd();

}
void MenuSystem::drawTwoPlayerRace(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	// 6.tga

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	// 7.tga

	glBindTexture(GL_TEXTURE_2D,textures[6]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	// 8.tga

	glBindTexture(GL_TEXTURE_2D,textures[7]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	// 9.tga

	glBindTexture(GL_TEXTURE_2D,textures[8]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	// begin button rendering

	// 1.tga

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[9]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,742);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,742);				
		glTexCoord2f((164.0f/256.0f),1.0f - (164.0f/256.0f));glVertex2f(461,578);				
		glTexCoord2f(0,1.0f - (164.0f/256.0f));glVertex2f(297,578);				
	glEnd();

	// 2.tga

	if(overIndex == 1)
	{
		glBindTexture(GL_TEXTURE_2D,textures[10]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,578);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,578);				
		glTexCoord2f((167.0f/256.0f),1.0f - (169.0f/256.0f));glVertex2f(297,409);				
		glTexCoord2f(0,1.0f - (169.0f/256.0f));glVertex2f(130,409);				
	glEnd();

	// 3.tga

	if(overIndex == 2)
	{
		glBindTexture(GL_TEXTURE_2D,textures[11]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[2]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(130,358);				
		glTexCoord2f((167.0f/256.0f),1);glVertex2f(297,358);				
		glTexCoord2f((167.0f/256.0f),1.0f - (166.0f/256.0f));glVertex2f(297,192);				
		glTexCoord2f(0,1.0f - (166.0f/256.0f));glVertex2f(130,192);				
	glEnd();

	// 4.tga

	if(overIndex == 3)
	{
		glBindTexture(GL_TEXTURE_2D,textures[12]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[3]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(297,192);				
		glTexCoord2f((164.0f/256.0f),1);glVertex2f(461,192);				
		glTexCoord2f((164.0f/256.0f),1.0f - (165.0f/256.0f));glVertex2f(461,27);				
		glTexCoord2f(0,1.0f - (165.0f/256.0f));glVertex2f(297,27);				
	glEnd();

	if(overIndex == 4)
	{
		glBindTexture(GL_TEXTURE_2D,textures[13]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[4]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(775,192);				
		glTexCoord2f((131.0f/256.0f),1);glVertex2f(906,192);				
		glTexCoord2f((131.0f/256.0f),1.0f - (120.0f/128.0f));glVertex2f(906,72);				
		glTexCoord2f(0,1.0f - (120.0f/128.0f));glVertex2f(775,72);				
	glEnd();

	if(gameSettings.numLaps == 1)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[26]);
	}
	else
	{
		if(overIndex == 5)
		{
			glBindTexture(GL_TEXTURE_2D,textures[15]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,textures[14]);
		}
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(477,441);
		glTexCoord2f((65.0f/128.0f),1);glVertex2f(542,441);
		glTexCoord2f((65.0f/128.0f),1.0f - (86.0f/128.0f));glVertex2f(542,355);
		glTexCoord2f(0,1.0f - (86.0f/128.0f));glVertex2f(477,355);
	glEnd();

	if(gameSettings.numLaps == 2)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[27]);
	}
	else
	{
		if(overIndex == 6)
		{
			glBindTexture(GL_TEXTURE_2D,textures[17]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,textures[16]);
		}
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(542,441);
		glTexCoord2f((85.0f/128.0f),1);glVertex2f(627,441);
		glTexCoord2f((85.0f/128.0f),1.0f - (86.0f/128.0f));glVertex2f(627,355);
		glTexCoord2f(0,1.0f - (86.0f/128.0f));glVertex2f(542,355);
	glEnd();

	if(gameSettings.numLaps == 3)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[28]);
	}
	else
	{
		if(overIndex == 7)
		{
			glBindTexture(GL_TEXTURE_2D,textures[19]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,textures[18]);
		}
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(627,441);
		glTexCoord2f((107.0f/128.0f),1);glVertex2f(734,441);
		glTexCoord2f((107.0f/128.0f),1.0f - (86.0f/128.0f));glVertex2f(734,355);
		glTexCoord2f(0,1.0f - (86.0f/128.0f));glVertex2f(627,355);
	glEnd();

	if(gameSettings.numLaps == 4)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[29]);
	}
	else
	{
		if(overIndex == 8)
		{
			glBindTexture(GL_TEXTURE_2D,textures[21]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,textures[20]);
		}
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(734,441);
		glTexCoord2f((89.0f/128.0f),1);glVertex2f(823,441);
		glTexCoord2f((89.0f/128.0f),1.0f - (86.0f/128.0f));glVertex2f(823,355);
		glTexCoord2f(0,1.0f - (86.0f/128.0f));glVertex2f(734,355);
	glEnd();

	if(gameSettings.numLaps == 5)
	{	
		glBindTexture(GL_TEXTURE_2D,textures[30]);
	}
	else
	{
		if(overIndex == 9)
		{
			glBindTexture(GL_TEXTURE_2D,textures[23]);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,textures[22]);
		}
	}
	
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(823,441);
		glTexCoord2f((90.0f/128.0f),1);glVertex2f(913,441);
		glTexCoord2f((90.0f/128.0f),1.0f - (86.0f/128.0f));glVertex2f(913,355);
		glTexCoord2f(0,1.0f - (86.0f/128.0f));glVertex2f(823,355);
	glEnd();

	if(overIndex == 10)
	{
		glBindTexture(GL_TEXTURE_2D,textures[25]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[24]);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);glVertex2f(632,283);
		glTexCoord2f((124.0f/128.0f),1);glVertex2f(756,283);
		glTexCoord2f((124.0f/128.0f),1.0f - (91.0f/128.0f));glVertex2f(756,192);
		glTexCoord2f(0,1.0f - (91.0f/128.0f));glVertex2f(632,192);
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;
}

void MenuSystem::drawHighScoresSelect()
{
	glBegin(GL_QUADS);							
		glVertex2f(833,183);				
		glVertex2f(979,183);				
		glVertex2f(979,40);				
		glVertex2f(833,40);				
	glEnd();

	glPushName(0);
}
void MenuSystem::drawHighScores(int clickedIndex,int overIndex)
{	
	
	// enable texturing (can be done in init)
	
	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	glBindTexture(GL_TEXTURE_2D,textures[2]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[3]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[4]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[5]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();

	if(overIndex == 0)
	{
		glBindTexture(GL_TEXTURE_2D,textures[1]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,textures[0]);
	}

	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(833,183);				
		glTexCoord2f((146.0f/256.0f),1);glVertex2f(979,183);				
		glTexCoord2f((146.0f/256.0f),1.0f - (143.0f/256.0f));glVertex2f(979,40);				
		glTexCoord2f(0,1.0f - (143.0f/256.0f));glVertex2f(833,40);				
	glEnd();

	if(overIndex != previousRollIndex) 
		gbsound.playSound("MENUbuttonhover");

	previousRollIndex = overIndex;

	load_scores();
}

void MenuSystem::drawCredits()
{
	// enable texturing (can be done in init)
	
	glMatrixMode(GL_PROJECTION);				// Selects The Projection Matrix
	glLoadIdentity();	
	glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 ); // set that matrix to ortho mode

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	
	glColor3ub(255,255,255);
	
	// begin background rendering

	glBindTexture(GL_TEXTURE_2D,textures[0]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[1]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[2]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D,textures[3]);
	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();
}

void MenuSystem::processKey(int asciiValue)
{
	if(gameSettings.p1->name[0] == '\0') 
		nameCharCtr = 0;
	
	if (asciiValue != 8) // not backspace
	{	
		if(nameCharCtr < MAX_PLAYER_CHARS)
		{
			gameSettings.p1->name[nameCharCtr] = asciiValue;
			nameCharCtr++;
			gameSettings.p1->name[nameCharCtr] = '\0';
		}
	}
	else // backspace was pressed
	{
		if (nameCharCtr > 0)
		{
			nameCharCtr--;
			gameSettings.p1->name[nameCharCtr] = '\0';
		}
	}
}
void MenuSystem::processClicked(int whichClicked)
{
	switch(currentState)
	{
		case TITLE_SCREEN:

			if (whichClicked == 0)
			{	
				gameSettings.numPlayers = 1;
				setState(ONE_PLAYER_TITLE);
			}
			else if (whichClicked == 1)
			{	
				gameSettings.numPlayers = 2;
				setState(TWO_PLAYER_TITLE);
			}
			else if (whichClicked == 2)
			{	
				setState(HIGH_SCORES);
			}
			else if (whichClicked == 3)
			{	
				setState(QUIT);
			}
			break;

		case ONE_PLAYER_TITLE:

			if(whichClicked == 0)
			{
				//for (int i = 0; i < MAX_PLAYER_CHARS; i++){
				//	this->gameSettings.p1->name[i] = '\0';
				//}
				//flush(
				setState(ONE_PLAYER_DRIVER);
			}
			else if(whichClicked == 1)
			{
				setState(ONE_PLAYER_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(ONE_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(ONE_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}

			break;

		case ONE_PLAYER_COURSE:

			if(whichClicked == 0)
			{
				setState(ONE_PLAYER_DRIVER);
			}
			else if(whichClicked == 1)
			{
				setState(ONE_PLAYER_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(ONE_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(ONE_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.trackIndex = 0;
			}
			else if (whichClicked == 6)
			{	
				gameSettings.trackIndex = 1;
			}

			break;

		case ONE_PLAYER_DRIVER:

			if(whichClicked == 0)
			{
				setState(ONE_PLAYER_DRIVER);
			}
			else if(whichClicked == 1)
			{
				setState(ONE_PLAYER_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(ONE_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(ONE_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5) // Load Profile
			{
				if(gameSettings.p1->name[0] != '\0')
				{
					if(load_player())
					{
						currentErrorState = PROFILE_LOADED;
						hasProfile = true;
						// reset the car selection
						gameSettings.p1->vehicleIndex = 0;
					}
					else
					{
						currentErrorState = DRIVER_DOES_NOT_EXIST;
						hasProfile = false;
					}
				}
				else
				{
					currentErrorState = DRIVER_DOES_NOT_EXIST;
					hasProfile = false;
				}

			}
			else if (whichClicked == 6) // Register Profile
			{
				player currentPlayer;
				
				// name
				currentPlayer.name = gameSettings.p1->name;

				// money
				currentPlayer.money = 100.00;

				currentPlayer.cars = 1;
				
				if(gameSettings.p1->name[0] != '\0')
				{
					if(check_save_file_exists(currentPlayer))
					{
						currentErrorState = DRIVER_NAME_TAKEN;
						hasProfile = false;
					}
					else
					{
						save_player(currentPlayer);
						currentErrorState = PROFILE_REGISTERED;
						hasProfile = true;
					}
				}
				else
				{
					currentErrorState = DRIVER_NAME_TAKEN;
					hasProfile = false;
				}
			}

			break;

		case ONE_PLAYER_RACE:

			if(whichClicked == 0)
			{
				setState(ONE_PLAYER_DRIVER);
			}
			else if(whichClicked == 1)
			{
				setState(ONE_PLAYER_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(ONE_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(ONE_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				if(hasProfile)
				{

					// if he doesn't have enough money, give him the first car
					if(gameSettings.p1->vehicleIndex == -1) gameSettings.p1->vehicleIndex = 0;


					// set track objects	
					if (gameSettings.trackIndex == 0){
						track1.setMeshFile("../models/track1.3ds");
						track1Collide.setTrackPath("../models/track1Collision.3ds", gameSettings.trackIndex);
						trackTrees.setMeshFile("../models/track1trees.3ds");
						trackGround.setMeshFile("../models/track1Ground.3ds");
					}
					else if (gameSettings.trackIndex == 1){
						track1.setMeshFile("../models/track2.3ds");
						track1Collide.setTrackPath("../models/track2Collision.3ds", gameSettings.trackIndex);
						trackTrees.setMeshFile("../models/track2Trees.3ds");
						trackGround.setMeshFile("../models/track2Ground.3ds");
					}
					
					// set car objects
					player1.resetPhysics();
					player2.resetPhysics();
					
					if (gameSettings.p1->vehicleIndex == 0){
						player1.setData(&slowCarSpec, "../models/buggy.3ds", 
										"../models/shadowBuggy.3ds", "../models/wheel.3ds");

						// i realize we should not have to set player 2 in 1 player, but we do
						player2.setData(&slowCarSpec, "../models/buggy.3ds", 
										"../models/shadowBuggy.3ds", "../models/wheel.3ds");
					}
					else if (gameSettings.p1->vehicleIndex == 1){
						player1.setData(&fastCarSpec, "../models/ferrari.3ds", 
										"../models/shadowFerrari.3ds", "../models/wheel2.3ds");

						// i realize we should not have to set player 2 in 1 player, but we do
						player2.setData(&fastCarSpec, "../models/ferrari.3ds", 
										"../models/shadowFerrari.3ds", "../models/wheel2.3ds");

					}

					bool everythingSupported = true;
					everythingSupported = player1.checkSupport();
					
					// init gameplay
					if (everythingSupported){
						setState(INIT_GAMEPLAY);
					}
					else{

					}
				}
			}

			break;

		case ONE_PLAYER_VEHICLE:

			if(whichClicked == 0)
			{
				setState(ONE_PLAYER_DRIVER);
			}
			else if(whichClicked == 1)
			{
				setState(ONE_PLAYER_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(ONE_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(ONE_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.p1->vehicleIndex = 0;
			}
			else if (whichClicked == 6)
			{	
				if(gameSettings.p1->money >= COST_OF_SECOND_CAR)
				{
					gameSettings.p1->vehicleIndex = 1;
				}
				else
				{
					gameSettings.p1->vehicleIndex = -1;
				}
			}

			break;

		case TWO_PLAYER_TITLE:

			if(whichClicked == 0)
			{
				setState(TWO_PLAYER_1_VEHICLE);
			}
			else if(whichClicked == 1)
			{
				setState(TWO_PLAYER_2_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(TWO_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(TWO_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}

			break;

		case TWO_PLAYER_1_VEHICLE:

			if(whichClicked == 0)
			{
				setState(TWO_PLAYER_1_VEHICLE);
			}
			else if(whichClicked == 1)
			{
				setState(TWO_PLAYER_2_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(TWO_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(TWO_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.p1->vehicleIndex = 0;
			}
			else if (whichClicked == 6)
			{	
				gameSettings.p1->vehicleIndex = 1;
			}

			break;

		case TWO_PLAYER_2_VEHICLE:

			if(whichClicked == 0)
			{
				setState(TWO_PLAYER_1_VEHICLE);
			}
			else if(whichClicked == 1)
			{
				setState(TWO_PLAYER_2_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(TWO_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(TWO_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.p2->vehicleIndex = 0;
			}
			else if (whichClicked == 6)
			{	
				gameSettings.p2->vehicleIndex = 1;
			}

			break;

		case TWO_PLAYER_COURSE:

			if(whichClicked == 0)
			{
				setState(TWO_PLAYER_1_VEHICLE);
			}
			else if(whichClicked == 1)
			{
				setState(TWO_PLAYER_2_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(TWO_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(TWO_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.trackIndex = 0;
			}
			else if (whichClicked == 6)
			{	
				gameSettings.trackIndex = 1;
			}

			break;

		case TWO_PLAYER_RACE:

			if(whichClicked == 0)
			{
				setState(TWO_PLAYER_1_VEHICLE);
			}
			else if(whichClicked == 1)
			{
				setState(TWO_PLAYER_2_VEHICLE);
			}
			else if(whichClicked == 2)
			{
				setState(TWO_PLAYER_COURSE);
			}
			else if(whichClicked == 3)
			{
				setState(TWO_PLAYER_RACE);
			}
			else if (whichClicked == 4)
			{	
				setState(TITLE_SCREEN);
			}
			else if (whichClicked == 5)
			{	
				gameSettings.numLaps = 1;
			}
			else if (whichClicked == 6)
			{	
				gameSettings.numLaps = 2;
			}
			else if (whichClicked == 7)
			{	
				gameSettings.numLaps = 3;
			}
			else if (whichClicked == 8)
			{	
				gameSettings.numLaps = 4;
			}
			else if (whichClicked == 9)
			{	
				gameSettings.numLaps = 5;
			}
			else if (whichClicked == 10)
			{	

				// set track objects	
				if (gameSettings.trackIndex == 0){
					track1.setMeshFile("../models/track1.3ds");
					track1Collide.setTrackPath("../models/track1Collision.3ds", gameSettings.trackIndex);
					trackTrees.setMeshFile("../models/track1trees.3ds");
					trackGround.setMeshFile("../models/track1Ground.3ds");
				}
				else if (gameSettings.trackIndex == 1){
					track1.setMeshFile("../models/track2.3ds");
					track1Collide.setTrackPath("../models/track2Collision.3ds", gameSettings.trackIndex);
					trackTrees.setMeshFile("../models/track2Trees.3ds");
					trackGround.setMeshFile("../models/track2Ground.3ds");
				}

				// set car objects
				player1.resetPhysics();
				player2.resetPhysics();

				if (gameSettings.p1->vehicleIndex == 0){
					player1.setData(&slowCarSpec, "../models/buggy.3ds", 
									"../models/shadowBuggy.3ds", "../models/wheel.3ds");

				}
				else if (gameSettings.p1->vehicleIndex == 1){
					player1.setData(&fastCarSpec, "../models/ferrari.3ds", 
									"../models/shadowFerrari.3ds", "../models/wheel2.3ds");
				}

				if (gameSettings.p2->vehicleIndex == 0){
					player2.setData(&slowCarSpec, "../models/buggy.3ds", 
									"../models/shadowBuggy.3ds", "../models/wheel.3ds");
				}
				else if (gameSettings.p2->vehicleIndex == 1){
					player2.setData(&fastCarSpec, "../models/ferrari.3ds", 
									"../models/shadowFerrari.3ds", "../models/wheel2.3ds");
				}
				
				bool everythingSupported = true;
				everythingSupported = player1.checkSupport();
				player2.checkSupport();
				// init gameplay
				if (everythingSupported){
					setState(INIT_GAMEPLAY);
				}
				else{

				}
			}

			break;

		case HIGH_SCORES:

			if(whichClicked == 0)
			{
				setState(TITLE_SCREEN);
			}

			break;
	}
}

void MenuSystem::deleteObjects(void){
	track1.destroy();
	track1Collide.destroy();
	trackTrees.destroy();

	player1.destroy();
	if (this->gameSettings.numPlayers == 2){
		player2.destroy();
	}

	Texture::TextureManager::getSingleton().freeAll();
}


void MenuSystem::printKeys(void)
{
	glDisable(GL_TEXTURE_2D);
	glColor3ub(102,125,196);

	glPrint(615,346,"%s",gameSettings.p1->name);
	glPrint(615,390,"$ %.2f",gameSettings.p1->money);
}

void MenuSystem::setState(MenuStates state)
{
	switch(state)
	{
		case CREDITS_SCREEN:

			Texture::TextureManager::getSingleton().freeAll();
			
			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/CREDITS_SCREEN/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/CREDITS_SCREEN/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/CREDITS_SCREEN/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/CREDITS_SCREEN/4.tga",-1,0,NICEST,1));

			break;
	
		case TITLE_SCREEN:
			
			Texture::TextureManager::getSingleton().freeAll();
			
			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TITLE_SCREEN/12.tga",-1,0,NICEST,1));
		
			break;
		
		case ONE_PLAYER_TITLE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_TITLE/14.tga",-1,0,NICEST,1));
		
			break;

		case TWO_PLAYER_TITLE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_TITLE/14.tga",-1,0,NICEST,1));
		
			break;
		
		case ONE_PLAYER_COURSE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_COURSE/20.tga",-1,0,NICEST,1));
		
			break;

		case ONE_PLAYER_DRIVER:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/20.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/21.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/22.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_DRIVER/23.tga",-1,0,NICEST,1));
		
			break;

		case ONE_PLAYER_RACE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/25.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/26.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/27.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_RACE/28.tga",-1,0,NICEST,1));
		
			break;

		case ONE_PLAYER_VEHICLE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/20.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/ONE_PLAYER_VEHICLE/21.tga",-1,0,NICEST,1));
		
			break;

		case TWO_PLAYER_1_VEHICLE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_1_VEHICLE/20.tga",-1,0,NICEST,1));

			break;

		case TWO_PLAYER_2_VEHICLE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_2_VEHICLE/20.tga",-1,0,NICEST,1));
		
			break;

		case TWO_PLAYER_COURSE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_COURSE/20.tga",-1,0,NICEST,1));
		
			break;

		case TWO_PLAYER_RACE:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/6.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/7.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/8.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/9.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/10.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/11.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/12.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/13.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/14.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/15.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/16.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/17.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/18.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/19.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/20.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/21.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/22.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/23.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/24.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/25.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/26.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/27.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/28.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/29.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/30.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/TWO_PLAYER_RACE/31.tga",-1,0,NICEST,1));
			break;

		case HIGH_SCORES:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();

			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/1.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/2.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/3.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/4.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/5.tga",-1,0,NICEST,1));
			textures.push_back(Texture::TextureManager::getSingleton().loadTexture("../textures/MenuTextures/HIGH_SCORES/6.tga",-1,0,NICEST,1));
		
			break;

		case GAMEPLAY:

			Texture::TextureManager::getSingleton().freeAll();

			textures.clear();
		
			break;
	}
	
	currentState = state;
}
// save player data (takes a player object defined in the FileStreams.h file)
bool MenuSystem::save_player(player &plr)
{
   // variable declarations
   string path = "";
   bool retVal = true;
   
   // if player's name is valid, write it to a file that's titled after 
   // the player's name
   if(plr.name.length() > 0)
   {
      // create a path to file
      path += "saves\\";
	  path += plr.name;
	  path += ".sav";  
   
      // create stream
      fstream f_player(path.c_str(), ios::out | ios::binary | ios::trunc);
      
      // write to file
      if(!f_player.fail())
      {
         f_player.write((char *)&plr, sizeof plr);
      }
      else
      {
         //cout << "Couldn't save player data!" << endl;
		  MessageBox(NULL, "Error while saving (first)","ALERT",MB_OK);
		  retVal = false;
      }
      f_player.close();
   }
   // if name is invalid
   else
   {
		MessageBox(NULL, "Error while saving (second)","ALERT",MB_OK);
	   retVal = false;
   }
   return retVal;
}

// load player data (takes a string that represent the name of the player)
bool MenuSystem::load_player()
{
   // variable declarations
   string path = "";
   bool retVal = true;
   
   // if player's name is valid, try to open the file
   if(strcmp("","") == 0)
   {
      // create a path to file
      path += "saves\\";
	  path += gameSettings.p1->name;
	  path += ".sav"; 
      
      // create stream
      fstream f_player(path.c_str(), ios::in | ios::binary);
      
      // read from file to a temporary player object
      if(!f_player.fail())
      {
         player tmp;
         f_player.read((char *)&tmp, sizeof tmp);
         
         // these are simple printouts that are used for testing purposes--uncomment
         // and add anything to this code block that you want if you want to do your 
         // own testing
         //cout << "Name: " << tmp.name << endl;
		 gameSettings.p1->money = tmp.money;
         //cout << "Money: " << tmp.money << endl;
         //cout << "Cars: " << tmp.cars << endl;
      }
      // if file can't be found
      else
      {
			//MessageBox(NULL, "Error while loading (first)","ALERT",MB_OK);
		  retVal = false;
      }
      f_player.close();
   }
   // if name is invalid
   else
   {
		//MessageBox(NULL, "Error while loading (second)","ALERT",MB_OK);
	   retVal = false;
   }
   return retVal;
}

// save high scores (takes a scores object defined in the FileStreams.h file)
void MenuSystem::save_scores(scores &scr)
{
   // variable delcarations
   scores table[10];
   int i = 0, spot = 10, multiplyer = 1;
   bool inserted = false;

	fstream f_scores,f_scores2;
   
   // create stream

   if(scr.track == 1)
   {
		f_scores.open("scores.dat", ios::in | ios::out | ios::binary);
   }
   else if(scr.track == 2)
   {
		f_scores.open("scores2.dat", ios::in | ios::out | ios::binary);
   }

   // if no high scores file currently exists, create it and fill it with 
   // blank values
   if(f_scores.fail())
   {
	   f_scores.clear();
	    
	   if(scr.track == 1)
		{
			f_scores.open("scores.dat", ios::in | ios::out | ios::trunc | ios::binary);
		}
		else if(scr.track == 2)
		{
			f_scores.open("scores2.dat", ios::in | ios::out | ios::trunc | ios::binary);
		}
 
      for(i = 0; i < 10; i++)
      {
         table[i].car = "N/A";
         table[i].player_name = "N/A";
         table[i].time = 0;
         table[i].track = 0;
      }
   }
   // if scores exist
   else
   {  
      // get current high scores and copy them to an array
      while(f_scores.read((char *)&table[i], sizeof table[i]))
      {  
         i++;
         f_scores.seekg(i * (multiplyer * sizeof (scr)));
      }

      // if there were less than ten records in the high scores table, 
      // fill the rest of the array with "blanks"
      for(i; i < 10; i++)
      {
         table[i].car = "N/A";
         table[i].player_name = "N/A";
         table[i].time = 0;
         table[i].track = 0;
      }
   }
   f_scores.close();
      
   // now that you got all the high scores, cycle through them to find out
   // which spot the passed high score (scr) should take. this should be 
   // sorted by the score id (money and time can't do it 'cause those might
   // be totally different on different tracks, so a combination of the two
   // perhaps? for now I'm presuming that the higher the id, the better the score)
   for(i = 9; i >= 0; i--)
   {
      if(scr.time <= table[i].time || table[i].time == 0)
         spot = i;
   }
      
   // if the passed score made it to the top ten, write it back to the file

   if(scr.track == 1)
   {
		f_scores2.open("scores.dat", ios::in | ios::out | ios::binary);
   }
   else if(scr.track == 2)
   {
		f_scores2.open("scores2.dat", ios::in | ios::out | ios::binary);
   }
   
   if(spot < 10)
   {
      for(i = 0; i < 10; i++)
      {
         // find proper location in file
         //if(i == 0 && scr.track != 1)
            //f_scores2.seekp(multiplyer * sizeof (scr));
         //else 
		  if(i > 0)
            f_scores2.seekp(i * (multiplyer * sizeof (scr)));
         
         if(i == spot)
         {
            f_scores2.write((char *)&scr, sizeof scr);
            inserted = true;
         }
         else
         {
            if(inserted)
               f_scores2.write((char *)&table[i - 1], sizeof table[i - 1]);
            else
               f_scores2.write((char *)&table[i], sizeof table[i]);
         }
      }
   }
   f_scores2.close();
}

bool MenuSystem::check_save_file_exists(player &plr)
{
   // variable declarations
   string path = "";
   bool retVal = false;
   
   // if player's name is valid, write it to a file that's titled after 
   // the player's name
   if(plr.name.length() > 0)
   {
      // create a path to file
      path += "saves\\";
	  path += plr.name;
	  path += ".sav";  
   

      // create stream
	  fstream file(path.c_str(),ios::in);

	  if(file.fail()) // if you can't open it, it's not there.. so return false
	  {
		  retVal = false;
	  }
	  else
	  {
		  retVal = true;
		  file.close();
	  }
   }
   // if name is invalid
   else
   {
	   retVal = true;
   }
   return retVal;
}

// load player data
void MenuSystem::load_scores(){

   	glColor3ub(102,125,196);
	//int track;
	
	// variable delcarations
   scores scr[10];
   int i = 0, multiplyer = 1;
   
   fstream f_scores,f_scores2;
   
	// TRACK 1
    f_scores.open("scores.dat", ios::in | ios::binary);
   
   // if scores exist
   if(!f_scores.fail())
   {
	   
	  // find proper location in file
         
      while(f_scores.read((char *)&scr[i], sizeof scr[i]))
      {
         i++;
         f_scores.seekg(i * (multiplyer * sizeof (scr[i])));
      }
      
      // print out of the contents of the array--used for testing purposes

	// RANK;NAME;CAR;TIME
	  glPrint(205,245,"1");glPrint(265,245,"%s",scr[0].player_name.c_str());glPrint(585,245,"%s",scr[0].car.c_str());glPrint(743,245,"%.2f",scr[0].time);
	  glPrint(205,274,"2");glPrint(265,274,"%s",scr[1].player_name.c_str());glPrint(585,274,"%s",scr[1].car.c_str());glPrint(743,274,"%.2f",scr[1].time);
	  glPrint(205,303,"3");glPrint(265,303,"%s",scr[2].player_name.c_str());glPrint(585,303,"%s",scr[2].car.c_str());glPrint(743,303,"%.2f",scr[2].time);
	  glPrint(205,332,"4");glPrint(265,332,"%s",scr[3].player_name.c_str());glPrint(585,332,"%s",scr[3].car.c_str());glPrint(743,332,"%.2f",scr[3].time);
	  glPrint(205,361,"5");glPrint(265,361,"%s",scr[4].player_name.c_str());glPrint(585,361,"%s",scr[4].car.c_str());glPrint(743,361,"%.2f",scr[4].time);
	  
   }
   // if no high scores file was found
   else
   {
      glPrint(440,130,"( NO SCORES FILE FOUND )");
   }
   f_scores.close();

   // TRACK 2
	f_scores2.open("scores2.dat", ios::in | ios::binary);
   
   // if scores exist
   if(!f_scores2.fail())
   {
	   
	   // find proper location in file

	   i = 0;
         
      while(f_scores2.read((char *)&scr[i], sizeof scr[i]))
      {
         i++;
         f_scores2.seekg(i * (multiplyer * sizeof (scr[i])));
      }
      
      // print out of the contents of the array--used for testing purposes

	// TRACK 2 SCORES

	// RANK;NAME;CAR;TIME
	  glPrint(205,452,"1");glPrint(265,452,"%s",scr[0].player_name.c_str());glPrint(585,452,"%s",scr[0].car.c_str());glPrint(743,452,"%.2f",scr[0].time);
	  glPrint(205,481,"2");glPrint(265,481,"%s",scr[1].player_name.c_str());glPrint(585,481,"%s",scr[1].car.c_str());glPrint(743,481,"%.2f",scr[1].time);
	  glPrint(205,510,"3");glPrint(265,510,"%s",scr[2].player_name.c_str());glPrint(585,510,"%s",scr[2].car.c_str());glPrint(743,510,"%.2f",scr[2].time);
	  glPrint(205,539,"4");glPrint(265,539,"%s",scr[3].player_name.c_str());glPrint(585,539,"%s",scr[3].car.c_str());glPrint(743,539,"%.2f",scr[3].time);
	  glPrint(205,568,"5");glPrint(265,568,"%s",scr[4].player_name.c_str());glPrint(585,568,"%s",scr[4].car.c_str());glPrint(743,568,"%.2f",scr[4].time);
	  
   }
   // if no high scores file was found
   else
   {
      glPrint(440,130,"( NO SCORES2 FILE FOUND )");
   }
   
   f_scores.close();
}
