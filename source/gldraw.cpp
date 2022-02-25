//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Jeff Barnard  2004
////																			
//////////////////////////////////////////////////////////////////////////////

#include "gldraw.h"
#include "model.h"
#include "debug.h"
#include "FModSound.h"

#define DRAWDISTANCE 450	 // rendering cutoff

#define SLOWTHROTTLE 85		 // max throttle for slow car - 80 reference
#define FASTTHROTTLE 120	 // max throttle for fast car - 120 reference

extern Track track1;         // the high poly track for rendering
extern Track track1Collide;  // the low poly track for collision testing
extern Model trackTrees;
extern Model trackGround;
extern Car player1;
extern Car player2;

// Sound
//////////////////////////////////
extern Sound::FModSound gbsound;
bool stopEngineSounds;
bool playDamageSfx;
// sound class ID's
int p1HasStartedTurboSfx = 0;
int p2HasStartedTurboSfx = 0;
int p1EngineSfxID		 = 7;
int p2EngineSfxID		 = 8;
int p1TurboLongSfxID	 = 9;
int p2TurboLongSfxID	 = 10;
int p1TireSfxID			 = 11;
int p2TireSfxID			 = 12;
int p1EngineBrakeSfxID   = 13;
int p2EngineBrakeSfxID   = 14;
// sound state flags
bool p1IsDamaged = false;
bool p2IsDamaged = false;
bool p1HornSfx;
bool p1TurboSfx;
bool p1TurboLongSfx;
bool p1EngineBrakeSfx;
bool p2HornSfx;
bool p2TurboSfx;
bool p2TurboLongSfx;
bool p2EngineBrakeSfx;
bool playedLaughSfx;

// Cameras
//////////////////////////////////
Camera player1Cam(&player1);
Camera player2Cam(&player2);
extern int p1camera;
extern int p2camera;

// Textures
//////////////////////////////////
extern GLuint texture[];
GLuint GOTexture;

// Track Sectors and Lap Timing
///////////////////////////////////
bool canControl;
bool renderStats2Player;
bool renderStats;
extern bool checkForESC;
extern bool inStats;
extern double frameInterval; // time elapsed between frames
extern Timer gameTimer;
int p1CurLap;
int p2CurLap;
static int sectorState		  = SECOND;
static int prevSectorState	  = SECOND;
static int sectorStateTwo	  = SECOND;
static int prevSectorStateTwo = SECOND;

// Global Lighting parameters
///////////////////////////////////
GLfloat matShine;
GLfloat lightAmbientDusk[]   = { 0.3f,	0.3f,	0.25f,	1.0f },
		lightAmbientLight[]  = { 0.3f,	0.3f,	0.3f,	1.0f },
		lightDiffuseDusk[]   = { 1.0f,	0.9f,	0.6f,	1.0f },
		lightDiffuseLight[]  = { 1.0f,	1.0f,	1.0f,	1.0f },
		lightPositionDusk[]  = { -120.0f, 110.0f, -0.0f,	1.0f },
		lightPositionLight[] = { -100.0f, 90.0f, 20.0f,	1.0f },
		lightSpecularDusk[]  = { 1.0f,	0.9f,	0.3f,	1.0f },
		lightSpecularLight[] = { 1.0f,	0.9f,	0.3f,	1.0f },
		noSpecular[]		 = { 0.0f,	0.0f,	0.0f,	1.0f },
		lightEmmision[]	     = { 0.0f,	0.0f,	0.0f,	1.0f };

GLuint skyList;

 // um... temporary debug values
float debugAngle = 0.0f;
float debugCollsion = 0.0f;
bool developerGraphics;

//////////////////////////////////// InitGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
//// All setup for OpenGL goes here
//// Texture/Model loading is also done here, before anything else
////
//////////////////////////////////// InitGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int InitGL(int players, int p1vehicleIndex, int p2vehicleIndex, int trackIndex) {													
	
    BuildFont();
	ShowCursor(false);
	canControl = false;
	renderStats2Player = false;
	renderStats = false;

	// openGL setup
	///////////////////////////////////////////////
	glShadeModel(GL_SMOOTH);							// enable smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// black background
	glClearDepth(1.0f);									// depth buffer setup
	glEnable(GL_DEPTH_TEST);							// enables Depth Testing
	glClearStencil(0);
	glDepthFunc(GL_LEQUAL);								// the type of depth testing to do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Texture set up
	///////////////////////////////////////////////
	if (!Texture::LoadGLTextures(texture, trackIndex))									
		return false;	

	GOTexture = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/GO.tga",-1,0,NICEST,1);
	
	loadHUDTextures();    
	glEnable(GL_TEXTURE_2D);
	
	// Lighting set up
	///////////////////////////////////////////////
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE); 

	// Culling setup
	///////////////////////////////////////////////
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Blending set up
	///////////////////////////////////////////////
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	
	// fog setup
	///////////////////////
	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fogColor[4];
	if (trackIndex == 0){
		fogColor[0] = 0.6f;
		fogColor[1] = 0.5f;
		fogColor[2] = 0.2f;
		fogColor[3] = 1.0f;		
	}
	else{
		fogColor[0] = 0.8f;
		fogColor[1] = 0.8f;
		fogColor[2] = 0.8f;
		fogColor[3] = 1.0f;	
	}
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.2f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 90.0f);
	glFogf(GL_FOG_END, 170.0f);
	
	// track setup
	////////////////////////////////////////////////////
	track1.load();
	track1Collide.load();
	trackTrees.load();
	trackGround.load();
	track1.resetTime();

	sectorState = SECOND;
	prevSectorState = SECOND;
	sectorStateTwo = SECOND;
	prevSectorStateTwo = SECOND;

	p1CurLap = 0;
	p2CurLap = 0;
    
	// players setup
	/////////////////////////////////////////////////////
	if (players == 2){
		player2.enableMultiTexture(NULL);
		player2.load();		
	}	
    player1.enableMultiTexture(NULL);
	player1.load();	

	player1.positionCar(7.0f,26.0f,0.0f,0.0f,64.4f, p1vehicleIndex);
	player2.positionCar(7.0f,22.0f,0.0f,0.0f,64.4f, p2vehicleIndex);
		
	player1.setOpponent(&player2);
	player2.setOpponent(&player1);

	// Camera setup
	////////////////////////////////////////////////////////
	player1Cam.resetCamera();
	player2Cam.resetCamera();

	p1camera = THIRDPERSON;
	p2camera = THIRDPERSON;
    
	// Sound Setup
	////////////////////////////////////////////////////////
	if (trackIndex == 0){
		gbsound.loadTrack1(); // load track 1 sounds
	}
	else{
		gbsound.loadTrack2(); // load track 1 sounds
	}
	p1HasStartedTurboSfx = 0;
	p2HasStartedTurboSfx = 0;
	stopEngineSounds = false;
	playDamageSfx = false;
	player1.setEngineSound(p1EngineSfxID);
	player2.setEngineSound(p2EngineSfxID);
	player1.setTireSound(p1TireSfxID);
	player2.setTireSound(p2TireSfxID);
	p1HornSfx = false;
	p1TurboSfx = false;
	p1TurboLongSfx = false;
	p1EngineBrakeSfx = false;

	p2HornSfx = false;
	p2TurboSfx = false;
	p2TurboLongSfx = false;
	p2EngineBrakeSfx = false;
	playedLaughSfx = false;

	compileSkyBox();

	developerGraphics = false;	
	return true;														
}


//////////////////////////////////// DrawGLScene \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  This is the main rendering loop for the game.
///  This function is called by WinMain constantly, once done, the buffers
///  are swapped and the scene displayed.
///
//////////////////////////////////// DrawGLScene \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int DrawGLScene(int players, SystemSettings *system)	{

	if (players == 2) {
		// check to see if we have completed the set amount of laps
		if (player1.getCurrentLap() > system->numLaps || player2.getCurrentLap() > system->numLaps){
			renderStats2Player = true;
			canControl = false;
			inStats = true;
			player1.brake = 200;
			player2.brake = 200;

			if (p1TurboLongSfx){
				gbsound.pauseLoopingSound("RCturbolong", p1TurboLongSfxID);
				p1TurboLongSfx = false;
			}

			if (p2TurboLongSfx){
				gbsound.pauseLoopingSound("RCturbolong", p2TurboLongSfxID);
				p2TurboLongSfx = false;
			}
		}

		// check for winner by damage
		if(player1.getDamage() > 100 || player2.getDamage() > 100) {

			if (!playDamageSfx){
				gbsound.playSound("RCexplode");
				playDamageSfx = true;
			}
            renderStats2Player = true;
			inStats = true;
			canControl = false;
			player1.brake = 200;
			player2.brake = 200;
		}
	}

	// start rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	for (int currentPlayer = 1; currentPlayer < players+1 ; currentPlayer++){ // two render paths

		////////////////////////////////////////////////////////////////////
		/// Set Project Modes and Camera
		///////////////////////////////////////////////////////////////////
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if (players == 1){
			gluPerspective(45.0f,(GLfloat)H_RESOLUTION/(GLfloat)V_RESOLUTION, 0.1f, DRAWDISTANCE);
		}
		else{
			gluPerspective(45.0f,(GLfloat)H_RESOLUTION/2/(GLfloat)V_RESOLUTION, 0.1f, DRAWDISTANCE); //vertical			
		}

		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();

		
		// These commands setup the viewports for both players
		if (players == 2){
			if (currentPlayer == 1){
				glViewport (H_RESOLUTION/2, 0, H_RESOLUTION/2, V_RESOLUTION);   //vertical
			}
			else{
				glViewport (0, 0, H_RESOLUTION/2 , V_RESOLUTION);				
			}
		}

		float time = (float)gameTimer.getTime()*0.001f;
		if (players == 2){
			if (currentPlayer == 1){
				if (time < 3){
					player1Cam.doThreeSixtyCamera(frameInterval, 2);			
				}
				else{
					player1Cam.setCamera(p1camera, 2, frameInterval);
				}				
			}
			else{
				if (time < 3){
					player2Cam.doThreeSixtyCamera(frameInterval, 2);			
				}
				else{
					player2Cam.setCamera(p2camera, 2, frameInterval);			
				}				
			}
		}
		else{			
			if (time < 3){
				player1Cam.doThreeSixtyCamera(frameInterval, 1);			
			}
			else{
				player1Cam.setCamera(p1camera, 1, frameInterval);
			}			
		}

		if (system->trackIndex == 0){
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientDusk);
			glLightfv(GL_LIGHT1, GL_SPECULAR,lightSpecularDusk);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseDusk);
			glLightfv(GL_LIGHT1, GL_POSITION,lightPositionDusk);
			glEnable(GL_LIGHT1);
			glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientDusk);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseDusk);
			glLightfv(GL_LIGHT2, GL_POSITION,lightPositionDusk);
		}
		else{
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientLight);
			glLightfv(GL_LIGHT1, GL_SPECULAR,lightSpecularLight);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseLight);
			glLightfv(GL_LIGHT1, GL_POSITION,lightPositionLight);
			glEnable(GL_LIGHT2);
			glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientLight);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseLight);
			glLightfv(GL_LIGHT2, GL_POSITION,lightPositionLight);
		}
		
	
		////////////////////////////////////////////////////////////////////
		/// Render Sky
		////////////////////////////////////////////////////////////////////
		glDisable(GL_FOG); // 
		glDisable(GL_DEPTH_TEST);
		
		if (currentPlayer == 1){
			glPushMatrix();
				glTranslatef(player1.getPosition().x, 0.0f, player1.getPosition().y); 
				glCallList(skyList);
			glPopMatrix();
		}
		if (currentPlayer == 2){
			glPushMatrix();
				glTranslatef(player2.getPosition().x, 0.0f, player2.getPosition().y); 
				glCallList(skyList);
			glPopMatrix();
		}

		glEnable(GL_FOG);
		
		///////////////////////////////////////////////////////////////////
		/// Render the Track
		///////////////////////////////////////////////////////////////////
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		
		// Render Trees (billboards)
		glDisable(GL_CULL_FACE);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			trackTrees.render(NULL,NULL, player1.position_wc);
		glDisable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
		
		// render track model
		glEnable(GL_DEPTH_TEST);
		if (currentPlayer == 1)
			track1.render(NULL,NULL, player1.position_wc);
		else
			track1.render(NULL,NULL, player2.position_wc);
		
		trackGround.render(NULL, NULL, Vector2(0.0f, 0.0f));
		
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT1);

		////////////////////////////////////////////////////////////////////
		/// Render the players Cars
		///////////////////////////////////////////////////////////////////
		glPushMatrix();
			matShine = 5.0f;
			if (system->trackIndex == 0)
				glMaterialfv(GL_FRONT, GL_SPECULAR, lightSpecularDusk);
			else
				glMaterialfv(GL_FRONT, GL_SPECULAR, lightSpecularLight);
			glMaterialfv(GL_FRONT, GL_SHININESS, &matShine);
			player1.Render(system->p1->vehicleIndex);
		glPopMatrix();
		if (players == 2){
			glPushMatrix();
				player2.Render(system->p2->vehicleIndex);
			glPopMatrix();
		}
		
		////////////////////////////////////////////////////////////////////
		/// Move Players (check collision and calculate physics)
		////////////////////////////////////////////////////////////////////		
		if (canControl){
			if (players == 1){ // one player

				prevSectorState = sectorState;
				int tempSector;
				tempSector = player1.moveCar(frameInterval, players, currentPlayer, &gbsound, 1, debugCollsion);
				
				if (tempSector != -1){
					sectorState = tempSector;					
				}

				if (sectorState == START){
					if (prevSectorState == SECOND){
						// the vehicle crossed the start/finish line
						/////////////////////////////////////////////////

						// up the lap counter
						player1.setCurrentLap(player1.getCurrentLap() + 1);
						
						// add laptime to SystemSettings
						if(track1.getTime() > 0.0) {
							system->addLapTime(float(track1.getTime()/1000.0));

							// calculate money earned
							float targetTime;

							// NOTE: also change in hud.cpp line 620
							if (system->trackIndex == 1){
								// if second track
								if (system->p1->vehicleIndex == 0) // slow car
									targetTime = 54.0f; // seconds
								else // fast car
									targetTime = 40.0f; // seconds
							}
							else{
								// if first track								
								if (system->p1->vehicleIndex == 0) // slow car
									targetTime = 42.0f; // seconds
								else // fast car
									targetTime = 30.0f; // seconds
							}
							
							float curTime = (float)track1.getTime()/1000.0f;
							float earned = (targetTime - curTime) * 20.0f;
							if (earned < 0){
								// give the poor guy a dolloar if hes slow
								earned = 1.0f;
							}
							system->p1->earnedMoney += earned;							
							system->p1->repairCosts += player1.getDamage();
						}

						// give player full turbo after crossing finish line
						player1.setTurbo(100.0f);
						// give i player 0 damage after crossing finish line
						player1.setDamage(0);
						
						// record fastest lap
						if ((track1.getTime()) < player1.getFastestLap() || player1.getFastestLap() == 0.0f){
							player1.setFastestLap((float)track1.getTime());
						}
						// reset and start the time
						track1.startTime();					
					}
				}
			}
			else{ // two players
				if (currentPlayer == 1){

					int tempSector;
					prevSectorState = sectorState;
					tempSector = player1.moveCar(frameInterval, players, currentPlayer, &gbsound, 1, debugCollsion);

					if (tempSector != -1)
						sectorState = tempSector;

					// crossed finish line
					if (sectorState == START){
						if (prevSectorState == SECOND){
							player1.setTurbo(100.0f);
							player1.setDamage(player1.getDamage()/2);
							player1.setCurrentLap(player1.getCurrentLap() + 1);
							
							if ((track1.getTime()) < player1.getFastestLap() || player1.getFastestLap() == 0.0f){
								player1.setFastestLap((float)track1.getTime());
							}

							track1.startTime();
						}
					}
					player2.setOpponent(&player1);

				}else{

					int tempSector2;
					prevSectorStateTwo = sectorStateTwo;
					tempSector2 = player2.moveCar(frameInterval, players, currentPlayer, &gbsound, 2, debugCollsion);

					if (tempSector2 != -1)
						sectorStateTwo = tempSector2;

					// crossed finish line
					if (sectorStateTwo == START){
						if (prevSectorStateTwo == SECOND){
							player2.setTurbo(100.0f);
							player2.setDamage(player2.getDamage()/2);
							player2.setCurrentLap(player2.getCurrentLap() + 1);

							if ((track1.getTimePTwo()) < player2.getFastestLap() || player2.getFastestLap() == 0.0f){
								player2.setFastestLap((float)track1.getTimePTwo());
							}
							track1.startTimePTwo();
						}
					}
					player1.setOpponent(&player2);
				}		
			}
		}
	}

	if (developerGraphics){
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);

		glColor3ub(0, 0, 255);
		player1.getEllipsoid()->drawEllipsoid();
		player2.getEllipsoid()->drawEllipsoid();
		
		glColor3ub(0, 255, 0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(player1.getPosition().x, 0.0f, player1.getPosition().y);
			glVertex3f(player1.getPosition().x + player1.velocity_wc.x/2, 0.0f, player1.getPosition().y + player1.velocity_wc.y/2);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glVertex3f(player2.getPosition().x, 0.0f, player2.getPosition().y);
			glVertex3f(player2.getPosition().x + player2.velocity_wc.x/2, 0.0f, player2.getPosition().y + player2.velocity_wc.y/2);
		glEnd();


		// draw track sectors
		if (system->trackIndex == 0){
			glColor3ub(255, 0, 0);
			glBegin(GL_TRIANGLES);					
				glVertex3f(29.5f, 0.0f, 28.0f);
				glVertex3f(29.5f, 0.0f, 17.0f);
				glVertex3f(29.5f, 8.0f, 23.0f);
			glEnd();
			glBegin(GL_TRIANGLES);					
				glVertex3f(235.0f, 0.0f, -24.3f);
				glVertex3f(248.0f, 8.0f, -16.0f);
				glVertex3f(257.0f, 0.0f, -1.5f);
			glEnd();
			glBegin(GL_TRIANGLES);					
				glVertex3f(95.4f, 0.0f, 113.6f);
				glVertex3f(96.77f, 8.0f, 123.6f);
				glVertex3f(97.6f, 0.0f, 135.5f);
			glEnd();
			
		}
		else{
			glColor3ub(255, 0, 0);
			glBegin(GL_TRIANGLES);					
				glVertex3f(29.5f, 0.0f, 28.0f);
				glVertex3f(29.5f, 0.0f, 17.0f);
				glVertex3f(29.5f, 8.0f, 23.0f);
			glEnd();
			glBegin(GL_TRIANGLES);					
				glVertex3f(-15.5f, 0.0f, -69.0f);
				glVertex3f(-16.0f, 0.0f, -76.0f);
				glVertex3f(-15.0f, 8.0f, -72.0f);
			glEnd();
			glBegin(GL_TRIANGLES);					
				glVertex3f(-178.7f, 0.0f, 102.0f);
				glVertex3f(-175.0f, 0.0f, 86.0f);
				glVertex3f(-176.0f, 8.0f, 92.0f);
			glEnd();
		}

		glEnable(GL_CULL_FACE);
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
		glColor3ub(255, 255, 0);
		glPrint(300, 200, "Player1 Position X: %2.2lf Z:%2.2lf", player1.getPosition().x, player1.getPosition().y);
		glPrint(305, 240, "Player1 Angular Velocity: %2.2lf", player1.angularvelocity);
		glPrint(340, 280, "Player1 Velocity: %2.2lf", player1.getVelocity()*0.66f);
		//glPrint(300, 260, "Debug: %2.2lf", debugCollsion);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_FOG);
		glEnable(GL_TEXTURE_2D);

	} // if (developerGraphics)

	////////////////////////////////////////////////////////////////////
	/// Render HUD (on top of everything else)
	///////////////////////////////////////////////////////////////////
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	
	if (players == 1){
		drawHUD(&player1, &gameTimer, &track1, system);
	}
	else{
		drawHUDTwoPlayer(&player1, &player2, &gameTimer, &track1, system);
	}

	////////////////////////////////////////////////
	// Draw MiniMap (after double render loop)
	////////////////////////////////////////////////
	drawMiniMap(&player1, &player2, players, system->trackIndex);

    // Draw Race ending staticstics screen (2 player)
	//////////////////////////////////////////////////////
	if (renderStats2Player){
		drawStatsScreenTwoPlayer(&player1, &player2, &gameTimer, &track1, system);
		if (!stopEngineSounds){
			gbsound.pauseLoopingSound("RCengine", p1EngineSfxID);
			gbsound.pauseLoopingSound("RCengine", p2EngineSfxID);
			gbsound.playSound("RCcrowd");
			stopEngineSounds = true;
		}
	}

	// Draw Race ending stats screen (1 player)
	//////////////////////////////////////////////////////
	if (renderStats){
		// the player only earned a dollar, laugh at them, haha
		if (!playedLaughSfx){
			if (system->p1->earnedMoney == 1){
				gbsound.playSound("laugh");
			}
			
			gbsound.pauseLoopingSound("RCengine", p1EngineSfxID);
			playedLaughSfx = true;
		}
		drawStatsScreen(&player1, &gameTimer, &track1, system);				
	}


	glDisable(GL_DEPTH_TEST);
	displayText(players);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);

    if (glGetError() != GL_NO_ERROR){
		return false;
	}
	return true;
}



//////////////////////////////////// displayText \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  Prints data to the screen
///
//////////////////////////////////// displayText \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void displayText(int players){

	static bool timeStart = false;	
	static bool RCStartupSfx = false;

	float time = (float)gameTimer.getTime()*0.001f;

	if (time < 4) {
		glColor3f(1.0f,0.9f,0.5f);
		glPrint(H_RESOLUTION/2-80, V_RESOLUTION/2-10, "GET READY");

		// rc car startup sound (start it up at 0 second)
		if (time > 0 && time < 3){
			if (!RCStartupSfx){
				gbsound.playSound("RCstartup");
				RCStartupSfx = true;
			}
		}

		// start the engine sound at 3 seconds in
		if (time > 3 && time < 4){
			if (RCStartupSfx){
				RCStartupSfx = false;
				gbsound.playLoopingSound("RCengine", p1EngineSfxID);
				gbsound.setFrequency("RCengine", p1EngineSfxID, 8500);

				if (players == 2){
					gbsound.playLoopingSound("RCengine", p2EngineSfxID);
					gbsound.setFrequency("RCengine", p2EngineSfxID, 8500);
				}
			}
		}
	}

	if (time > 4 && time < 6) {
		glColor3f(1.0f,1.0f,1.0f);
		
		// display GO!
		glMatrixMode( GL_PROJECTION );	
		glClear( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
		glLoadIdentity();
		glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 );	
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glPushMatrix();
			glTranslatef(H_RESOLUTION/2-128, V_RESOLUTION/2-128, 0);			
			glBindTexture(GL_TEXTURE_2D, GOTexture);
			glColor3ub(255, 255, 255);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2f(0, 0);
				glTexCoord2f(1,0); glVertex2f(256, 0);
				glTexCoord2f(1,1); glVertex2f(256, 256);
				glTexCoord2f(0,1); glVertex2f(0, 256);
			glEnd();
		glPopMatrix();
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);

		canControl = true;		
	}
}


///////////////////////////////// CheckForMovement \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  User input via keyboard  (high resolution input) 
///  The lower precision input function is located in glsetup.cpp
///
///////////////////////////////// CheckForMovement \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CheckForMovement(SystemSettings *system) {

	if (canControl){

		// player 1 controls
		///////////////////////////////////////////////////////		
		if(GetKeyState(VK_UP)& 0x80 ){
			if (p1IsDamaged){
				if (player1.throttle < 6.0){
					player1.throttle += 0.5;
				}
			}
			else{
				
				if (system->p1->vehicleIndex == 0){
					if (player1.throttle < SLOWTHROTTLE){
						player1.throttle += 1.0;
					}
				}
				else{ // if fast car more throttle
					if (player1.throttle < FASTTHROTTLE){
						player1.throttle += 1.0;
					}
				}
			}		
		}
		else{
			if (player1.throttle > 0.0){
				player1.throttle -= 1.0;
			}
		}

		if (player1.getDamage() < 100.0f){
			// turbo boost key maps
			if(GetKeyState(VK_RCONTROL)& 0x80 ){
				if (!p1TurboSfx){
					gbsound.playSound("RCbrake2");
					p1TurboSfx = true;
				}
				if (!p1TurboLongSfx){
					if (player1.getTurbo() > 0){
						//TRACE("\n\nPlaying RCturbolong Sound Effect");
						if (p1HasStartedTurboSfx == 0){
							gbsound.playLoopingSound("RCturbolong", p1TurboLongSfxID);
							p1HasStartedTurboSfx++;
						}
						else{
							gbsound.pauseLoopingSound("RCturbolong", p1TurboLongSfxID);						
						}
						p1TurboLongSfx = true;
					}
				}
				if (player1.getTurbo() > 0){
					if (player1.throttle < 350.0){
						player1.throttle = 350.0;
					}
					// drain the turbo gauge
					player1.setTurbo(player1.getTurbo() - float(player1.getTurboDecay() * frameInterval));
				}
				else{

					if (system->p1->vehicleIndex == 0){
						if (player1.throttle > SLOWTHROTTLE)
							player1.throttle = SLOWTHROTTLE;
					}
					else{ // if fast car more throttle
						if (player1.throttle > FASTTHROTTLE)
							player1.throttle = FASTTHROTTLE;
					}
				}
			}
			else{ // if not pressing turbo
				
				p1TurboSfx = false;
				if (p1TurboLongSfx){
					//TRACE("\n\nStopping RCturbolong Sound Effect");
					gbsound.pauseLoopingSound("RCturbolong", p1TurboLongSfxID);
				}
				p1TurboLongSfx = false;
				
				if (system->p1->vehicleIndex == 0){
					if (player1.throttle > SLOWTHROTTLE){
						if (p1IsDamaged){ // if damaged
							player1.throttle = 6.0f;
						}
						else{ // if not damaged
							player1.throttle = SLOWTHROTTLE;			
						}
					}
					
				}
				else{ // if fast car more throttle
					if (player1.throttle > FASTTHROTTLE){
						if (p1IsDamaged){ // if damaged
							player1.throttle = 6.0f;
						}
						else{ // if not damaged
							player1.throttle = FASTTHROTTLE;			
						}
					}									
				}				
			}
		}
		else{
			if (p1TurboLongSfx){
				gbsound.pauseLoopingSound("RCturbolong", p1TurboLongSfxID);
				p1TurboLongSfx = false;
			}
		}
		
		if((GetKeyState(VK_DOWN) & 0x80)){// && player1.velocity.x > 0)	{	
			player1.brake = 150;
			player1.throttle = 0;
			if (!p1EngineBrakeSfx){
				gbsound.playSound("RCbrake");
				p1EngineBrakeSfx = true;
			}			
		}
		else{
			player1.brake = 0;
			p1EngineBrakeSfx = false;
		}

		if(GetKeyState(VK_RIGHT) & 0x80) 	{		
			if( player1.steerangle > - PI/4.0 )
				player1.rotateCarNegative(frameInterval);
		}
		else{
			if (player1.steerangle < 0)
				player1.steerangle += (float)(PI/2048 * frameInterval);
		}

		if(GetKeyState(VK_LEFT) & 0x80)		{
			if( player1.steerangle <  PI/4.0 )
				player1.rotateCarPositive(frameInterval);
				
		}
		else{
			if (player1.steerangle > 0)
				player1.steerangle -= (float)(PI/2048 * frameInterval);
		}

		// rounding correction in steering :P
		if(!(GetKeyState(VK_LEFT) & 0x80) && !(GetKeyState(VK_RIGHT) & 0x80)){
			if ((player1.steerangle > -0.05 && player1.steerangle < 0) || player1.steerangle < 0.05 && player1.steerangle > 0)
				player1.steerangle = 0;

		}

		// Horn sound effect
		if (GetKeyState(VK_DELETE) & 0x80){
			if (!p1HornSfx){
				gbsound.playSound("RChorn");
				p1HornSfx = true;
			}
		}
		else{
			p1HornSfx = false;
		}

		// player 2 controls
		///////////////////////////////////////////////////////	
		if(GetKeyState('W') & 0x80){
			if (p2IsDamaged){
				if (player2.throttle < 6.0){
					player2.throttle += 0.5;
				}
			}
			else{
				if (system->p2->vehicleIndex == 0){
					if (player2.throttle < SLOWTHROTTLE){
						player2.throttle += 1.0;
					}
				}
				else{ // if fast car more throttle
					if (player2.throttle < FASTTHROTTLE){
						player2.throttle += 1.0;
					}
				}
			}	
		}
		else{
			if (player2.throttle > 0.0)
				player2.throttle -= 1.0;
		}

		if (player2.getDamage() < 100.0f){
			// turbo boost key maps
			if(GetKeyState(VK_LSHIFT)& 0x80 ){

				if (!p2TurboSfx){
					gbsound.playSound("RCbrake2");
					p2TurboSfx = true;
				}
				if (!p2TurboLongSfx){
						if (player2.getTurbo() > 0){
							if (p2HasStartedTurboSfx == 0){
								gbsound.playLoopingSound("RCturbolong", p2TurboLongSfxID);
								p2HasStartedTurboSfx++;
							}
							else{
								gbsound.pauseLoopingSound("RCturbolong", p2TurboLongSfxID);						
							}
							p2TurboLongSfx = true;
						}
					}

				if (player2.getTurbo() > 0){
					if (player2.throttle < 350.0){
						player2.throttle = 350.0;
					}
					player2.setTurbo(player2.getTurbo() - float(player2.getTurboDecay() * frameInterval));
				}
				else{ // if pressing, but no turbo left
					if (system->p2->vehicleIndex == 0){
						if (player2.throttle > SLOWTHROTTLE)
							player2.throttle = SLOWTHROTTLE;
					}
					else{ // if fast car more throttle
						if (player2.throttle > FASTTHROTTLE)
							player2.throttle = FASTTHROTTLE;
					}
				}
			}
			else{ // if not pressing turbo
				p2TurboSfx = false;
				if (p2TurboLongSfx){
					gbsound.pauseLoopingSound("RCturbolong", p2TurboLongSfxID);
				}
				p2TurboLongSfx = false;

				if (system->p2->vehicleIndex == 0){
					if (player2.throttle > SLOWTHROTTLE){
						if (p2IsDamaged){ // if damaged
							player2.throttle = 6.0f;
						}
						else{ // if not damaged
							player2.throttle = SLOWTHROTTLE;			
						}
					}				
				}
				else{ // if fast car more throttle
					if (player2.throttle > FASTTHROTTLE){
						if (p2IsDamaged){ // if damaged
							player2.throttle = 6.0f;
						}
						else{ // if not damaged
							player2.throttle = FASTTHROTTLE;			
						}
					}				
				}
			}
		}else{
			if (p2TurboLongSfx){
				gbsound.pauseLoopingSound("RCturbolong", p2TurboLongSfxID);
				p2TurboLongSfx = false;
			}
		}

		if(GetKeyState('S') & 0x80)	{	
			player2.brake = 150;
			player2.throttle = 0;
			if (!p2EngineBrakeSfx){
				gbsound.playSound("RCbrake");
				p2EngineBrakeSfx = true;
			}
		}else{
			player2.brake = 0;
			p2EngineBrakeSfx = false;
		}

		if(GetKeyState('A') & 0x80)		{
			if( player2.steerangle <  PI/4.0 )
				player2.rotateCarPositive(frameInterval);			
		}
		else{
			if (player2.steerangle > 0)
				player2.steerangle -= (float)(PI/2048 * frameInterval);
		}

		if(GetKeyState('D') & 0x80) 	{		
			if( player2.steerangle > - PI/4.0 )
				player2.rotateCarNegative(frameInterval);
				
		}
		else{
			if (player2.steerangle < 0)
				player2.steerangle += (float)(PI/2048 * frameInterval);
		}

		// Horn sound effect
		if (GetKeyState('F') & 0x80){
			if (!p2HornSfx){
				gbsound.playSound("RChorn");
				p2HornSfx = true;
			}
		}
		else{
			p2HornSfx = false;
		}

		// rounding correction in steering :P
		if(!(GetKeyState('A') & 0x80) && !(GetKeyState('D') & 0x80)){
			if ((player2.steerangle > -0.05 && player2.steerangle < 0) || player2.steerangle < 0.05 && player2.steerangle > 0)
				player2.steerangle = 0;
		}
		
		// if Y is pressed, reset car positions 
		// (debugging purposes)
		//if(GetKeyState('Y') & 0x80){
		//	player1.positionCar(7.0f,26.0f,0.0f,0.0f,64.4f, player1.vehicleIndex);
		//	player2.positionCar(7.0f,22.0f,0.0f,0.0f,64.4f, player2.vehicleIndex);
		//}

    } // if (canControl)

	float time = (float)gameTimer.getTime()*0.001f;
	if (time > 6){ // cant quit until race has started
		if (system->numPlayers == 1){
			if (GetKeyState(VK_ESCAPE) & 0x80){
				inStats = true;
				renderStats = true;
				canControl = false;
			}		
		}

		if (system->numPlayers == 2){
			if (GetKeyState(VK_ESCAPE) & 0x80){
				inStats = false;			
			}		
		}


		// developor keypresses
		if (GetKeyState(VK_F1) & 0x80){
			player1.drawBB = true;
			player2.drawBB = true;
			track1.drawBB = false;
			developerGraphics = true;
		}
		if (GetKeyState(VK_F2) & 0x80){
			player1.drawBB = true;
			player2.drawBB = true;
			track1.drawBB = true;
			developerGraphics = true;
		}
		if (GetKeyState(VK_F3) & 0x80){
			player1.drawBB = false;
			player2.drawBB = false;
			track1.drawBB = false;
			developerGraphics = false;
		}
		
	}
}


/////////////////////////////////// drawSkyBox \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
////  Simply draws a sky box (rectangle) around the world
////
/////////////////////////////////// drawSkyBox \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void compileSkyBox(void){

	float width  = 500;
	float height = 300;
	float length = 500;
	float x = 0 - width / 2;
	float y = 0 - height / 2;
	float z = 0 - length / 2;

	skyList = glGenLists(1);
	glNewList(skyList, 0x1300);

	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 255);

	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_bk]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// FRONT
	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_ft]);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// BOTTOM
	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_bt]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// TOP 
	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_tp]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// LEFT 
	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_lt]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// RIGHT 
	glBindTexture(GL_TEXTURE_2D, texture[skyBoxTexIndex_rt]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();

	glEnable(GL_LIGHTING);

	glEndList();
}


////////////////////////////////// modelShutdown \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
////  This function is called when the program ends, all model delete[] should
////  be called here.
////
////////////////////////////////// modelShutdown \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void renderShutdown(void){	
	killHUDTextures();
	Texture::TextureManager::getSingleton().freeTexture(GOTexture);	
}
