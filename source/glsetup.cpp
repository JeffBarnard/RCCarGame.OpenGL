//////////////////////////////////////////////////////////////////////
////																	
////  Creates/shutdown and initializes an OpenGL window							
////																	
////  Initialzes FMOD for streaming a .mp3 music track					
////																	
////  Jeff Barnard  2003						
////																	
//////////////////////////////////////////////////////////////////////

#include "glsetup.h"
#include "gldraw.h"
#include "MenuSystem.h"
#include "FModSound.h"

HGLRC			hRC=NULL;				// Permanent Rendering Context
HWND			hWnd=NULL;				// Holds Window Handle
HINSTANCE		hInstance;				// Holds The Instance Of The Application
HDC				hDC=NULL;				// Private GDI Device Context
bool			active=TRUE,			// Window Active Flag Set To TRUE By Default
				fullscreen=TRUE,		// Fullscreen Flag Set To windowed Mode By Default
				keys[256];				// array of keyboard keys

char			lastKeyPressed;			// a character value storing the last key pressed
bool			keyPressed = false;
bool			keyProcessed = true;	// a boolean indicating weither or not the lastKeyPressed has been processed or not


Timer gameTimer(&hWnd);			// Game Timer
Timer menuTimer(&hWnd);

Sound::FModSound gbsound;		// Global Sound Object
GLuint	base;					// used for font

int	mouse_x, mouse_y;			// The Current Position Of The Mouse
bool hasClicked = false;		// mouseClicked flag;

bool inStats = false;

double frameInterval = 0.0f;	// stores the elapsed time between the current and last frame. 
GLuint texture[15];				// Storage for misc textures, referenced by ID's
int p1camera = 0;
int p2camera = 0;
bool checkForESC = false;		// flag whether to look for ESC key to exit program or to pop up stats screen (1 player)


//int tempPlayerMode; // just a temp to be able to play from clicking the 1 player button
int WINAPI WinMain(	HINSTANCE hInstance,		// Instance
					HINSTANCE hPrevInstance,	// Previous Instance
					LPSTR     lpCmdLine,		// Command Line Parameters
					int		  nCmdShow) {		// Window Show State

	// Main entry point for the windows application
	// Calls all necessary setup and shutdown functions
	// And resolves key presses
	//////////////////////////////////////////////////////////////////
	MSG		msg;				// Windows Message Structure
	bool	done=FALSE;			

	bool    wireframe = false,
		    vp,
			endp;
	
	//fullscreen=!fullscreen;
	
	// Create OpenGL Window
	if (!CreateGLWindow("RC Car Game",1024,768,COLOR_DEPTH,fullscreen))
		return 0;		

	// Main loop
	// SET TEMP STATES (without object)
	bool isMenu = true;
	
	// declare indexes of polygons that were clicked, or moused on
	int clickedIndex = -1;
	int onIndex = -1;

	MenuSystem gameMenuSystem;	// declare the MenuSystem object	
	gameMenuSystem.initGLMenu(); // Initalize the menu
	
	ShowCursor(false);

	gameMenuSystem.setState(CREDITS_SCREEN); // the first thing done is always the Credits

	while(!done){	
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){			// Is There A Message Waiting?
			if (msg.message==WM_QUIT)						// Received A Quit Message?
				done=TRUE;										
			else{											// If Not, Deal With Window Messages
				TranslateMessage(&msg);						// Translate The Message
				DispatchMessage(&msg);						// Dispatch The Message
			}
		}
		else{
			if (active)	
			{										
				if(gameMenuSystem.getState() == QUIT)
				{
					done=TRUE;
				}

				// Menu State	
		
				else if (gameMenuSystem.getState() == CREDITS_SCREEN)
				{	
					if(keyPressed)
					{
						menuTimer.startTime();
						gameMenuSystem.setState(TITLE_SCREEN);
						ShowCursor(true);
					}

					gameMenuSystem.drawCredits();
					SwapBuffers(hDC); // swap the buffers
					
				}

				else if (gameMenuSystem.getState() == TITLE_SCREEN)
				{	
					
					if (menuTimer.getTime()/1000.0f > 300.0f){
						menuTimer.stopTime();
						keyPressed = false;
						gameMenuSystem.setState(CREDITS_SCREEN);
					}
					
					
					// clear out the settings...
					gameMenuSystem.currentErrorState = ALL_OK;
					gameMenuSystem.hasProfile = false; // reset the "hasProfile" flag
					lastKeyPressed = '\0';
					gameMenuSystem.gameSettings.resetValues();	
									
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTitleScreen(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers

					
					if(hasClicked)
					{			

						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				
				// ONE_PLAYER_TITLE
				
				else if (gameMenuSystem.getState() == ONE_PLAYER_TITLE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawOnePlayerTitle(clickedIndex,onIndex);
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == TWO_PLAYER_TITLE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTwoPlayerTitle(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers

					
					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == ONE_PLAYER_COURSE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawOnePlayerCourse(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers
					
					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == ONE_PLAYER_DRIVER)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawOnePlayerDriver(clickedIndex,onIndex);
					
					if (!keyProcessed) // If the key press hasn't been processed, process it
					{
						gameMenuSystem.processKey(lastKeyPressed);
						keyProcessed = true;
					}
					
					// deal with printKeys (working) code latah
					
					gameMenuSystem.printKeys(); // print the entered keys
					
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == ONE_PLAYER_RACE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawOnePlayerRace(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == ONE_PLAYER_VEHICLE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawOnePlayerVehicle(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == TWO_PLAYER_1_VEHICLE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTwoPlayer1Vehicle(clickedIndex,onIndex);
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == TWO_PLAYER_2_VEHICLE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTwoPlayer2Vehicle(clickedIndex,onIndex);
					
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == TWO_PLAYER_COURSE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTwoPlayerCourse(clickedIndex,onIndex);
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == TWO_PLAYER_RACE)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawTwoPlayerRace(clickedIndex,onIndex);
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}
				else if (gameMenuSystem.getState() == HIGH_SCORES)
				{	
					// figure out if the mouse is over one of the TITLE_SCREEN Select Quads
					onIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);

					// reset the clicked index if the mouse isn't on anything
					if(onIndex == -1) clickedIndex = -1;

					//draw current state (happens to be silly boxes)
					gameMenuSystem.drawHighScores(clickedIndex,onIndex);
	
					
					SwapBuffers(hDC); // swap the buffers

					if(hasClicked)
					{
						clickedIndex = gameMenuSystem.whichMouseObject(mouse_x,mouse_y);
						hasClicked = false;
						
						gameMenuSystem.processClicked(clickedIndex);
					}
				}

				
				// Init Gameplay
				else if(gameMenuSystem.getState() == INIT_GAMEPLAY)
				{

					gameMenuSystem.gameSettings.curLapTimes.clear();
					
					
					ShowCursor(false);
		
					gameMenuSystem.initGLMenu();
					gameMenuSystem.whichMouseObject(NULL, NULL);
					gameMenuSystem.drawLoadingScreen();	
					SwapBuffers(hDC); // swap the buffers
					
					gameMenuSystem.setState(GAMEPLAY);

					if (!InitGL(gameMenuSystem.gameSettings.numPlayers,
								gameMenuSystem.gameSettings.p1->vehicleIndex,
								gameMenuSystem.gameSettings.p2->vehicleIndex,
								gameMenuSystem.gameSettings.trackIndex))	{
						KillGLWindow();							
						return FALSE;								
					}

					gameTimer.startTime();
				}
				// Gameplay State				
				else if (gameMenuSystem.getState() == GAMEPLAY) {	

					frameInterval = gameTimer.calculateFrameRate();
					gameMenuSystem.executeCheckForMovement();
					gameMenuSystem.executeRenderLoop();		// Draw The Scene
					SwapBuffers(hDC);		// Swap Buffers (Double Buffering)

					float time = (float)gameTimer.getTime()*0.001f;
					if (time > 6){ // cant quit until race has started
						if (gameMenuSystem.gameSettings.numPlayers == 2){
							if ((GetKeyState(VK_ESCAPE) & 0x80) && !inStats){

								scores curScore;
								curScore.car = "Car One";
								curScore.track = 1;
								curScore.player_name = gameMenuSystem.gameSettings.p1->name;
								
								for(UINT i = 0; i < gameMenuSystem.gameSettings.curLapTimes.size();i++)							{
									curScore.time = gameMenuSystem.gameSettings.curLapTimes[i];
									gameMenuSystem.save_scores(curScore);
								}
														
								// delete player and track objects
								gameMenuSystem.deleteObjects();
								renderShutdown();
								// kill GL window
								KillGLWindow();
								// recreate GL window
								if (!CreateGLWindow("RC Car Game",1024,768,COLOR_DEPTH,fullscreen))
									return 0;
								
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
								glColor3ub(255,255,255);
								// init menu sounds
								if (gameMenuSystem.gameSettings.trackIndex == 0){
									gbsound.unloadTrack1();
								}
								else{
									gbsound.unloadTrack2();
								}
								
								// init menu system
								gameMenuSystem.initGLMenu();
								gameMenuSystem.setState(TITLE_SCREEN);
								//inStats = true;
							}
						}
					}

					if (inStats){
						if (GetKeyState(VK_RETURN) & 0x80){
							
							// WRITE THE PLAYER TO THE FILE:

							if(gameMenuSystem.gameSettings.numPlayers == 1){
								player currentPlayer;
					
								currentPlayer.name = gameMenuSystem.gameSettings.p1->name;
								currentPlayer.money = gameMenuSystem.gameSettings.p1->grandTotal;
								currentPlayer.cars = 1;

								gameMenuSystem.save_player(currentPlayer);
							
							
								// TEMP.. FOR NOW.. ALWAYS CAR ONE, AND ALWAYS TRACK 1, BECAUSE
								// THE OTHERS HAVE NOT BEEN IMPLEMENTED YET
								scores curScore;
								
								if(gameMenuSystem.gameSettings.p1->vehicleIndex == 0)
								{
									curScore.car = "RC Buggy";
								}
								else if(gameMenuSystem.gameSettings.p1->vehicleIndex == 1)
								{
									curScore.car = "RC Ferrari";
								}
								
								curScore.track = gameMenuSystem.gameSettings.trackIndex + 1;

								curScore.player_name = gameMenuSystem.gameSettings.p1->name;
								
								for(UINT i = 0; i < gameMenuSystem.gameSettings.curLapTimes.size();i++)							{
									curScore.time = gameMenuSystem.gameSettings.curLapTimes[i];
									gameMenuSystem.save_scores(curScore);
								}
							}
													
							// delete player and track objects
							gameMenuSystem.deleteObjects();
							renderShutdown();
							// kill GL window
							KillGLWindow();
							// recreate GL window
							if (!CreateGLWindow("RC Car Game",1024,768,COLOR_DEPTH,fullscreen))
								return 0;
							
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
							glColor3ub(255,255,255);
							// init menu sounds
							if (gameMenuSystem.gameSettings.trackIndex == 0){
								gbsound.unloadTrack1();
							}
							else{
								gbsound.unloadTrack2();
							}
							// init menu system
							gameMenuSystem.initGLMenu();
							gameMenuSystem.setState(TITLE_SCREEN);
							inStats = false;
							gameMenuSystem.gameSettings.p1->reset();
						}
					}
					
					if (keys['V'] && !vp){					
						// if V is pressed, change view for player2!
						vp=TRUE;
						if (p2camera < 2)
							p2camera++;
						else
							p2camera = 0;
					}

					if (keys[VK_END] && !endp){					
						// if END is pressed, change view for player1!
						endp=TRUE;
						if (p1camera < 2)
							p1camera++;
						else
							p1camera = 0;
					}

					if (!keys[VK_END])							
						endp=FALSE;	

					if (!keys['V'])							
						vp=FALSE;

				}
			}
		}
	}
	
	//MessageBox(NULL,"GAME OVER","Game Over!",MB_OK|MB_ICONEXCLAMATION);
	
	// Shutdown
	Texture::TextureManager::getSingleton().freeAll();
	Texture::TextureManager::getSingleton().destroy();
	//gbsound.unloadCommon();

	return ((int)msg.wParam);							
}



GLvoid ReSizeGLScene(GLsizei width, GLsizei height){		
	// Resize And Initialize The GL Window
	////////////////////////////////////////////////////
	if (height==0)													// Prevent A Divide By Zero By
		height=1;													// Making Height Equal One
	
/*	glViewport(0,0,width,height);									// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);									// Select The Projection Matrix
	glLoadIdentity();												// Reset The Projection Matrix

	glOrtho(0.0f,H_RESOLUTION,V_RESOLUTION,0.0f,-1.0f,1.0f);

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix
	glLoadIdentity();										// Reset The Modelview Matrix
*/
	
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)H_RESOLUTION/(GLfloat)V_RESOLUTION,0.1f,300.0f);

	glMatrixMode(GL_MODELVIEW);	

}


 
//////////////////////////////////// CREATEGLWINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Uses Windows SDK, its basically generated code and very ugly
/////
//////////////////////////////////// CREATEGLWINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag) {

	//	This Code Creates the OpenGL Window.  Parameters Are:					
	//	title				- Title To Appear At The Top Of The Window				
	//	width				- Width Of The GL Window Or Fullscreen Mode				
	//	height		     	- Height Of The GL Window Or Fullscreen Mode			
	//	bits				- Number Of Bits To Use For Color (8/16/24/32)			
	//	fullscreenflag	    - Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	
	///////////////////////////////////////////////////////////////////////////////////
	
	GLuint		PixelFormat;				// Holds The Results After Searching For A Match
	WNDCLASS	wc;							// Windows Class Structure
	DWORD		dwExStyle;					// Window Extended Style
	DWORD		dwStyle;					// Window Style
	RECT		WindowRect;					// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;				// Set Left Value To 0
	WindowRect.right=(long)width;			// Set Right Value To Requested Width
	WindowRect.top=(long)0;					// Set Top Value To 0
	WindowRect.bottom=(long)height;			// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;				// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// No Menu
	wc.lpszClassName	= "Car Game";							// Set The Class Name

	if (!RegisterClass(&wc)){									// Attempt To Register The Window Class
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									
	}
	
	if (fullscreen){											// Attempt Fullscreen Mode?
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results. CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Windowed Mode?"
								,"OpenGL Video Error",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreen=FALSE;	
			else{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Your Video Card is not supported.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									
			}
		}
	}
	
	if (fullscreen){											// Still In Fullscreen Mode?
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			
		dwStyle=WS_OVERLAPPEDWINDOW;						
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"Car Game",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,							// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL))){							// Dont Pass Anything To WM_CREATE
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;										
	}

	static	PIXELFORMATDESCRIPTOR pfd={			// pfd Tells Windows How We Want Things To Be
		sizeof(PIXELFORMATDESCRIPTOR),			// Size Of This Pixel Format Descriptor
		1,										// Version Number
		PFD_DRAW_TO_WINDOW |					// Format Must Support Window
		PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,							// Request An RGBA Format
		bits,									// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		0,										// No Alpha Buffer
		0,										// Shift Bit Ignored
		0,										// No Accumulation Buffer
		0, 0, 0, 0,								// Accumulation Bits Ignored
		16,										// 16Bit Z-Buffer (Depth Buffer)  
		0,										// No Stencil Buffer
		0,										// No Auxiliary Buffer
		PFD_MAIN_PLANE,							// Main Drawing Layer
		0,										// Reserved
		0, 0, 0									// Layer Masks Ignored
	};
	// Did Windows assign A Device Context?
	if (!(hDC=GetDC(hWnd))){					
		KillGLWindow();						
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							
	}
	// Did Windows Find A Matching Pixel Format?
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))){
		KillGLWindow();										
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							
	}
	// Is Windows able to set the Pixel Format?
	if(!SetPixelFormat(hDC,PixelFormat,&pfd)){
		KillGLWindow();							
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							
	}
	// Are We Able To Get A Rendering Context?
	if (!(hRC=wglCreateContext(hDC))){			
		KillGLWindow();							
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}
	// Try To Activate The Rendering Context
	if(!wglMakeCurrent(hDC,hRC)){					
		KillGLWindow();							
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen
	
	/*if (!InitGL())	{
		KillGLWindow();							
		MessageBox(NULL,"Initialization Failed.  Your Video Card Does not support OpenGL 1.3","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}*/
	
	return TRUE;									
}

LRESULT CALLBACK WndProc(	HWND	hWnd,					// Handle For This Window
									UINT	uMsg,			// Message For This Window
									WPARAM	wParam,			// Additional Message Information
									LPARAM	lParam){		// Additional Message Information
	// WndProc handles all Windows messages
	// wParam hold additional message info such as what key
	// was pressed (ex VK_UP, or 'L')
	///////////////////////////////////////////////////////////
	
	switch (uMsg){									
		case WM_ACTIVATE: 									// Watch For Window Activate Message
			if (!HIWORD(wParam))							// Check Minimization State
				active=TRUE;								// Program Is Active
			else
				active=FALSE;								// Program Is No Longer Active
			return 0;								
		case WM_SYSCOMMAND:									// Intercept System Commands
			switch (wParam){								// Check System Calls
				case SC_SCREENSAVE:							// Screensaver Trying To Start?
				case SC_MONITORPOWER:						// Monitor Trying To Enter Powersave?
				return 0;									// Prevent From Happening
			}
			break;									
		case WM_CLOSE: 										// Close message recieved?
			PostQuitMessage(0);								// Send A Quit Message
			return 0;								
		case WM_KEYDOWN: 									// Is A Key Being Held Down?
			keys[wParam] = TRUE;							// If So, Mark It As TRUE
			if((wParam >=65 && wParam <=90) ||				// set global lastKeyPressed if it's alphabetic, backspace, or spacebar
				(wParam >=97 && wParam <=122) ||
				(wParam == 8) || (wParam == 32))
			{
				lastKeyPressed = char(wParam);
				keyProcessed = false;						// this new key hasn't been processed yet
			}
			keyPressed = true;
			return 0;							
		case WM_KEYUP: 										// Has A Key Been Released?
			keys[wParam] = FALSE;							// If So, Mark It As FALSE
			return 0;								
		case WM_SIZE: 										// Resize The OpenGL Window
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));   // LoWord=Width, HiWord=Height
			return 0;										// Jump Back
		case WM_MOUSEMOVE:
			mouse_x = LOWORD(lParam);          
			mouse_y = HIWORD(lParam);
			return 0;
		case WM_LBUTTONDOWN:
			hasClicked = true;
			return 0;
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//////////////////////////////////// BuildFont \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Builds a font list from windows
/////
//////////////////////////////////// BuildFont \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
GLvoid BuildFont(GLvoid){

	HFONT	font;
	HFONT	oldfont;
	base = glGenLists(96);

	font = CreateFont(	-24,							// Height
						0,								// Width
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Comic Sans MS");				// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // select the font
	wglUseFontBitmaps(hDC, 32, 96, base);				// builds 96 characters starting at 32
	SelectObject(hDC, oldfont);							// selects the font
	DeleteObject(font);
}

//////////////////////////////////// KillFont \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Deletes the font list
/////
//////////////////////////////////// KillFont \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
GLvoid KillFont(GLvoid) {
	glDeleteLists(base, 96);
}


//////////////////////////////////// glPrint \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Prints using the current font, saves current screen info before printing
/////
//////////////////////////////////// glPrint \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
GLvoid glPrint(int x, int y, const char *fmt, ...){	

	char text[256];	
	va_list	ap;										// Pointer To List Of Arguments

	if (fmt == NULL)
		return;	

	va_start(ap, fmt);								// Parses The String For Variables
	    vsprintf(text, fmt, ap);					// And Converts Symbols To Actual Numbers
	va_end(ap);										// Results Are Stored In Text

	// save current viewport
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
		// use a new projection and modelview matrix to work with.
		glMatrixMode( GL_PROJECTION );					
		glPushMatrix();	
			glLoadIdentity();
			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
				glLoadIdentity();

				y = V_RESOLUTION - 10 - y;

				glViewport( x - 1, y - 1, 0, 0 );	// create a new viewport to draw into
				glRasterPos4f( 0, 0, 0, 1 );

			glPopMatrix();							// pop the current modelview matrix off the stack
		glMatrixMode( GL_PROJECTION );				// go back into projection mode
		glPopMatrix();								// pop the projection matrix off the stack
	glPopAttrib();									// this restores the TRANSFORM and VIEWPORT attributes

	glPushAttrib(GL_LIST_BIT);
		glListBase(base - 32);
		glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);	
	glPopAttrib();
}



/////////////////////////////////// KILLGLWINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Properly Kills the Window
/////
/////////////////////////////////// KILLGLWINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
GLvoid KillGLWindow(GLvoid) {			
	
	if (fullscreen) {								
		ChangeDisplaySettings(NULL,0);				// Switch back to desktop if in fullscreen mode
		ShowCursor(TRUE);								
	}

	if (hRC) {										// Do We Have A Rendering Context?
		if (!wglMakeCurrent(NULL,NULL))				// Are We Able To Release The DC And RC Contexts?
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		
		if (!wglDeleteContext(hRC))					// Are We Able To Delete The RC?
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hRC=NULL;
	}

	if (hDC && !ReleaseDC(hWnd,hDC)) {				// Are We Able To Release The DC
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;
	}

	if (hWnd && !DestroyWindow(hWnd)) {				// Are We Able To Destroy The Window?
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;
	}

	if (!UnregisterClass("Car Game",hInstance)) {	// Are We Able To Unregister Class
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;
	}
}
