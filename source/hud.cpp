//////////////////////////////////////////////////////////////////////////////
////																			
////  Heads Up Display
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#include "hud.h"

GLuint gaugeTexture;
GLuint postRaceTextures[4];
GLuint miniMapTexture;
GLuint miniMapTexture2;
GLuint carProfile;

//////////////////////////////////////////////////////////////////////////////
////																			
////  Loads the textures used by the hud
////
//////////////////////////////////////////////////////////////////////////////
void loadHUDTextures(void){
	
	gaugeTexture = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/gauges.tga",-1,0,NICEST,1);

	carProfile = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/carProfile.tga",-1,0,NICEST,1);
	
	miniMapTexture = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/miniTrack1.tga",-1,0,NICEST,1);
	miniMapTexture2 = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/miniTrack2.tga",-1,0,NICEST,1);

	postRaceTextures[0] = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/postRaceTL.tga",-1,0,NICEST,1);
	postRaceTextures[1] = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/postRaceTR.tga",-1,0,NICEST,1);
	postRaceTextures[2] = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/postRaceBL.tga",-1,0,NICEST,1);
	postRaceTextures[3] = 
		Texture::TextureManager::getSingleton().loadTexture("../textures/postRaceBR.tga",-1,0,NICEST,1);
}


//////////////////////////////////////////////////////////////////////////////
////																			
////  Destroys the textures used by the hud
////
//////////////////////////////////////////////////////////////////////////////
void killHUDTextures(void){
	Texture::TextureManager::getSingleton().freeTexture(gaugeTexture);	

	Texture::TextureManager::getSingleton().freeTexture(carProfile);	
	
	Texture::TextureManager::getSingleton().freeTexture(miniMapTexture);	
	Texture::TextureManager::getSingleton().freeTexture(miniMapTexture2);	
	
	for (int i=0; i < 4; i++)
		Texture::TextureManager::getSingleton().freeTexture(postRaceTextures[i]);

}


//////////////////////////////////////////////////////////////////////////////
////																			
////  Draws a HUD for single player only
////
//////////////////////////////////////////////////////////////////////////////
void drawHUD (Car *car, Timer *gameTimer, Track *track, SystemSettings *system){

	glColor3f(1.0f,0.9f,0.7f);
	glPrint(H_RESOLUTION-80,  20, "Lap");
	glPrint(H_RESOLUTION-30,  20, "%d", car->getCurrentLap());
	glPrint(H_RESOLUTION-180, 60, "Time ");
	glPrint(H_RESOLUTION-85,  60, "%3.2lf", track->getTime()*0.001f);
	glPrint(H_RESOLUTION-180, 100, "Best ");
	glPrint(H_RESOLUTION-85,  100, "%3.2lf", car->getFastestLap()/1000.0f);

	float targetLapTime;
	if (system->trackIndex == 1){
		// if second track
		if (system->p1->vehicleIndex == 0) // slow car
			targetLapTime = 54.0f; // seconds
		else // fast car
			targetLapTime = 40.0f; // seconds
	}
	else{
		// if first track								
		if (system->p1->vehicleIndex == 0) // slow car
			targetLapTime = 42.0f; // seconds
		else // fast car
			targetLapTime = 30.0f; // seconds
	}

	glPrint(H_RESOLUTION-180, 140, "Target ");
	glPrint(H_RESOLUTION-85,  140, "%3.2lf", targetLapTime);

	// draw geometry on screen
	///////////////////////////////////////////////////////
	glMatrixMode( GL_PROJECTION );	
	glClear( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
		glLoadIdentity();
		glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 );
	
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		// Background Images
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
					
		// speedo image
		glPushMatrix();
			glTranslatef(H_RESOLUTION-520, 20, 0);			
			glBindTexture(GL_TEXTURE_2D, gaugeTexture);
			glColor3ub(240, 240,240);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2f(0, 0);
				glTexCoord2f(1,0); glVertex2f(512, 0);
				glTexCoord2f(1,1); glVertex2f(512, 512);
				glTexCoord2f(0,1); glVertex2f(0, 512);
			glEnd();
		glPopMatrix();

		// damage image
		glPushMatrix();
			glBlendFunc (GL_SRC_ALPHA, GL_ONE);
			glTranslatef(0, 0, 0);			
			glBindTexture(GL_TEXTURE_2D, carProfile);
			
			glColor3ub(0+(car->getDamage()*2.5), 255-(car->getDamage()*2.5), 0);
							
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2f(0, 0);
				glTexCoord2f(1,0); glVertex2f(256, 0);
				glTexCoord2f(1,1); glVertex2f(256, 256);
				glTexCoord2f(0,1); glVertex2f(0, 256);
			glEnd();
		glPopMatrix();
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		
		// speedo
		//////////////////////////////////////////////////
		glColor3ub(0, 0, 0);
		glPushMatrix();
			glTranslatef(H_RESOLUTION-97-8, 97+20, 0);
			// Needle
			glRotatef(140.0f, 0.0f, 0.0f, 1.0f);
			// the *3 can be adjusted 
			glRotatef(-(abs(car->velocity.x))*3,0.0f, 0.0f, 1.0f);
			glBegin(GL_QUADS);
				glVertex2f(0, 0);
				glVertex2f(3, 0);
				glVertex2f(3, 60);
				glVertex2f(0, 60);
			glEnd();
		glPopMatrix();
		
		// TurboBoost meter
		//////////////////////////////////////////////////
		glPushMatrix();
			glTranslatef(H_RESOLUTION-252-8, 57+20, 0);
			// Needle
			// smaller first number = more towards 0
			glRotatef(138.0f, 0.0f, 0.0f, 1.0f);
			glRotatef((-car->getTurbo()*2.7f),0.0f, 0.0f, 1.0f);
			glBegin(GL_QUADS);
				glVertex2f(0, 0);
				glVertex2f(2, 0);
				glVertex2f(2, 33);
				glVertex2f(0, 33);
			glEnd();  
		glPopMatrix();
		
		glColor3f(1.0f,0.9f,0.5f);
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
}


//////////////////////////////////////////////////////////////////////////////
////																			
////  Seperate HUD for two player.  Much easier for viewport independent
////  drawing of text and graphics this way.
////
//////////////////////////////////////////////////////////////////////////////
void drawHUDTwoPlayer (Car *car, Car *p2car, Timer *gameTimer, Track *track, SystemSettings *system){

	glColor3f(1.0f,0.9f,0.7f);

	//////////////////////////// player 1 ///////////////////////////////////

	glMatrixMode( GL_PROJECTION );	
	glClear( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
	glLoadIdentity();
	glViewport (0, 0, H_RESOLUTION , V_RESOLUTION);
	glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Background Images player 1
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				
	glPushMatrix();
		glTranslatef(H_RESOLUTION/2-520, 20, 0);			
		glBindTexture(GL_TEXTURE_2D, gaugeTexture);
		glColor3ub(240, 240,240);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0, 0);
			glTexCoord2f(1,0); glVertex2f(512, 0);
			glTexCoord2f(1,1); glVertex2f(512, 512);
			glTexCoord2f(0,1); glVertex2f(0, 512);
		glEnd();
	glPopMatrix();
	// damage image
	glPushMatrix();
		glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		glTranslatef(H_RESOLUTION/2, 0, 0);			
		glBindTexture(GL_TEXTURE_2D, carProfile);
		
		glColor3ub(0+(car->getDamage()*2.5), 255-(car->getDamage()*2.5), 0);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0, 0);
			glTexCoord2f(1,0); glVertex2f(256, 0);
			glTexCoord2f(1,1); glVertex2f(256, 256);
			glTexCoord2f(0,1); glVertex2f(0, 256);
		glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	// speedo p1
	//////////////////////////////////////////////////
	glColor3ub(0, 0, 0);
	glPushMatrix();
		glTranslatef(H_RESOLUTION/2-97-8, 97+20, 0);
		// Needle
		glRotatef(140.0f, 0.0f, 0.0f, 1.0f);
		// the *3 can be adjusted 
		glRotatef(-(abs(p2car->velocity.x))*3,0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(3, 0);
			glVertex2f(3, 60);
			glVertex2f(0, 60);
		glEnd();
	glPopMatrix();
	
	// TurboBoost meter p1
	//////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(H_RESOLUTION/2-252-8, 57+20, 0);
		// Needle
		// smaller first number = more towards 0
		glRotatef(138.0f, 0.0f, 0.0f, 1.0f);
		glRotatef((-p2car->getTurbo()*2.7f),0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(2, 0);
			glVertex2f(2, 33);
			glVertex2f(0, 33);
		glEnd();  
	glPopMatrix();
	
	glColor3f(1.0f,0.9f,0.5f);

	//////////////////////// player 2 ///////////////////////////////////////////////

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				
	glPushMatrix();
		glTranslatef(H_RESOLUTION-520, 20, 0);			
		glBindTexture(GL_TEXTURE_2D, gaugeTexture);
		glColor3ub(240, 240,240);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0, 0);
			glTexCoord2f(1,0); glVertex2f(512, 0);
			glTexCoord2f(1,1); glVertex2f(512, 512);
			glTexCoord2f(0,1); glVertex2f(0, 512);
		glEnd();
	glPopMatrix();
	// damage image
	glPushMatrix();
		glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		glTranslatef(0, 0, 0);			
		glBindTexture(GL_TEXTURE_2D, carProfile);
		glColor3ub(0+(p2car->getDamage()*2.5), 255-(p2car->getDamage()*2.5), 0);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0, 0);
			glTexCoord2f(1,0); glVertex2f(256, 0);
			glTexCoord2f(1,1); glVertex2f(256, 256);
			glTexCoord2f(0,1); glVertex2f(0, 256);
		glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	// speedo p2
	//////////////////////////////////////////////////
	glColor3ub(0, 0, 0);
	glPushMatrix();
		glTranslatef(H_RESOLUTION-97-8, 97+20, 0);
		// Needle
		glRotatef(140.0f, 0.0f, 0.0f, 1.0f);
		// the *3 can be adjusted 
		glRotatef(-(abs(car->velocity.x))*3,0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(3, 0);
			glVertex2f(3, 60);
			glVertex2f(0, 60);
		glEnd();
	glPopMatrix();
	
	// TurboBoost meter p2
	//////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(H_RESOLUTION-252-8, 57+20, 0);
		// Needle
		// smaller first number = more towards 0
		glRotatef(138.0f, 0.0f, 0.0f, 1.0f);
		glRotatef((-car->getTurbo()*2.7f),0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(2, 0);
			glVertex2f(2, 33);
			glVertex2f(0, 33);
		glEnd();  
	glPopMatrix();
	
	glColor3f(1.0f,0.9f,0.5f);
	
	// HUD Text
	///////////////////////////////////////////////////
	glColor3f(1.0f,0.9f,0.7f);
	// player 1
	glPrint(H_RESOLUTION-100,  20, "Lap");
	glPrint(H_RESOLUTION-50,  20, "%d/%d", car->getCurrentLap(), system->numLaps);
	glPrint(H_RESOLUTION-145, 60, "Time ");
	glPrint(H_RESOLUTION-80,  60, "%3.2lf", track->getTime()* 0.001f);
	glPrint(H_RESOLUTION-145, 100, "Best ");
	glPrint(H_RESOLUTION-80,  100, "%3.2lf", car->getFastestLap()/1000.0f);
	
	// player 2
	glPrint(10, 20, "Lap");
	glPrint(60, 20, "%d/%d", p2car->getCurrentLap(), system->numLaps);
	glPrint(10, 60, "Time ");			
	glPrint(80, 60, "%3.2lf", track->getTimePTwo() * 0.001f);
	glPrint(10, 100, "Best ");
	glPrint(80,  100, "%3.2lf", p2car->getFastestLap()/1000.0f);
}



/////////////////////////// drawStatsScreen //////////////////////////////
///																			
///  After Race Statistics Screen (2 Player Only)
///
////////////////////////// drawStatsScreen ///////////////////////////////
void drawStatsScreenTwoPlayer (Car *car, Car *p2car, Timer *gameTimer, Track *track, SystemSettings *system){

	glColor3ub(100, 170, 160);

	track->resetTime();
	
	float sizeH = H_RESOLUTION/2;
	float sizeV = V_RESOLUTION/2;

	// draw geometry on screen
	/////////////////////////////////////////////////////////////////
	glDisable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );	
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, H_RESOLUTION, V_RESOLUTION,0, 0, 20 );
	
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	

	glColor3ub(255, 255, 255);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable(GL_CULL_FACE);
	glPushMatrix();
	
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -768.0f, 0.0f);
	
	glBindTexture(GL_TEXTURE_2D, postRaceTextures[0]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D, postRaceTextures[1]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D, postRaceTextures[2]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();
	glBindTexture(GL_TEXTURE_2D, postRaceTextures[3]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	glPopMatrix();
	glMatrixMode( GL_PROJECTION );	
	glPopMatrix();
		
	glColor3ub(0,0,0);

	bool p1won = false;
	bool p2won = false;

	// if one of the players has damaged 100%
	if (car->getDamage() > 100 || p2car->getDamage() > 100){

		if (car->getDamage() > 100){
			glPrint(320, 140, "Player 1 Has Been Destroyed");
			glPrint(270, 180, "Player 2 Has Won The Race. Congrats!");
		}
		else{
			glPrint(320, 140, "Player 2 Has Been Destroyed");
			glPrint(270, 180, "Player 1 Has Won The Race. Congrats!");
		}

	}
	else{ // determine winner by time
		if (car->getFastestLap() == 0.0f){
			p2won = true;
		}
		else if (p2car->getFastestLap() == 0.0f){
			p1won = true;
		}
		else{
			// check if on different laps
			if (car->getCurrentLap() > p2car->getCurrentLap()){
				p1won = true;
			}
			else if(p2car->getCurrentLap() > car->getCurrentLap()){
				p2won = true;
			}
			// same lap, compare times
			else{
				if (track->getTime() <= track->getTimePTwo())
					p1won = true;					
				else
					p2won = true;
			}
		}
	}	

	if (p1won){
		glPrint(270, 140, "Player 1 Reached The Finish Line Quicker");
		glPrint(270, 180, "Player 1 Has Won The Race. Congrats!");

	}
	if (p2won){
		glPrint(270, 140, "Player 2 Reached The Finish Line Quicker");
		glPrint(270, 180, "Player 2 Has Won The Race. Congrats!");
	}

	glPrint(270, 270, "Player 1's Fastest Lap:");
	glPrint(640, 270, "%2.2lf", car->getFastestLap()/1000.0f);
	glPrint(270, 300, "Player 1's Highest Speed:");
	glPrint(640, 300, "%2.2lf Mph", car->getTopSpeed()*0.66f);
	glPrint(270, 330, "Player 1's Max G-forces:");
	glPrint(640, 330, "%2.2lf G's", car->getTopG()/12.0f);

	glPrint(270, 420, "Player 2's Fastest Lap:");
	glPrint(640, 420, "%2.2lf", p2car->getFastestLap()/1000.0f);
	glPrint(270, 450, "Player 2's Highest Speed:");
	glPrint(640, 450, "%2.2lf Mph", p2car->getTopSpeed()*0.66f);
	glPrint(270, 480, "Player 2's Max G-forces:");
	glPrint(640, 480, "%2.2lf G's", p2car->getTopG()/12.0f);
	
	glPrint(290, 580, "Press ENTER To Return To Main Menu");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}


/////////////////////////// drawStatsScreen //////////////////////////////
///																			
///  After Race Statistics Screen (1 Player)
///
////////////////////////// drawStatsScreen ///////////////////////////////
void drawStatsScreen (Car *car, Timer *gameTimer, Track *track, SystemSettings *system){

	glColor3ub(100, 170, 160);

	track->resetTime();
	
	float sizeH = H_RESOLUTION/2;
	float sizeV = V_RESOLUTION/2;

	// draw geometry on screen
	/////////////////////////////////////////////////////////////////
	glDisable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );	
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, H_RESOLUTION, V_RESOLUTION,0, 0, 20 );
	
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	
	glColor3ub(255, 255, 255);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable(GL_CULL_FACE);
	glPushMatrix();
	
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -768.0f, 0.0f);
	
	glBindTexture(GL_TEXTURE_2D, postRaceTextures[0]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D, postRaceTextures[1]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,768);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,768);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,384);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,384);				
	glEnd();

	glBindTexture(GL_TEXTURE_2D, postRaceTextures[2]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(0,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(512,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(0,0);				
	glEnd();
	glBindTexture(GL_TEXTURE_2D, postRaceTextures[3]);	
	glBegin(GL_QUADS);							
		glTexCoord2f(0,1);glVertex2f(512,384);				
		glTexCoord2f((512.0f/512.0f),1);glVertex2f(1024,384);				
		glTexCoord2f((512.0f/512.0f),1.0f - (384.0f/512.0f));glVertex2f(1024,0);				
		glTexCoord2f(0,1.0f - (384.0f/512.0f));glVertex2f(512,0);				
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	glPopMatrix();
	glMatrixMode( GL_PROJECTION );	
	glPopMatrix();
		
	glColor3ub(0,0,0);
	
	char str[100];
	sprintf(str, "%s'S FREE RUN STATS", system->p1->name);
	glPrint((H_RESOLUTION/2)-((int)((strlen(str))*12)/2)-35, 140, "%s", str);

	float targetLapTime;
	if (system->trackIndex == 1){
		// if second track
		if (system->p1->vehicleIndex == 0) // slow car
			targetLapTime = 54.0f; // seconds
		else // fast car
			targetLapTime = 40.0f; // seconds
	}
	else{
		// if first track								
		if (system->p1->vehicleIndex == 0) // slow car
			targetLapTime = 42.0f; // seconds
		else // fast car
			targetLapTime = 30.0f; // seconds
	}

	glPrint(300, 220, "Target Lap Time:");
	glPrint(580, 220, "%2.2lf seconds", targetLapTime);
	glPrint(300, 250, "Your Fastest Lap:");
	glPrint(580, 250, "%2.2lf seconds", car->getFastestLap()/1000.0f);
	glPrint(300, 280, "Your Highest Speed:");
	glPrint(580, 280, "%2.2lf Mph", car->getTopSpeed()*0.66f);
	glPrint(300, 310, "Your Max G-forces:");
	glPrint(580, 310, "%2.2lf G's", car->getTopG()/12.0f);
	
	float prevCash = system->p1->money;	
	float cashEarned = system->p1->earnedMoney;
	float repair = system->p1->repairCosts;
	float total = prevCash + (cashEarned-repair);

	// assign the total to the value of the new total
	system->p1->grandTotal = total;
    	
	glPrint(300, 370, "Previous Cash:");
	glPrint(580, 370, "$%.2lf", prevCash);	
	glPrint(300, 400, "Cash Earned:");
	glPrint(580, 400, "$%.2lf", cashEarned);
	glPrint(300, 430, "Repair Costs:");
	glPrint(580, 430, "$%.2lf", repair);
	glPrint(300, 490, "Grand Total:");
	glPrint(580, 490, "$%.2lf", total);
    glPrint(300, 580, "Press ENTER To Return To Main Menu");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}


//////////////////////////////////////////////////////////////////////////////
///																			
///  Draws a Mini Map so players can see where they are located on track.
///
//////////////////////////////////////////////////////////////////////////////
void drawMiniMap(Car *player1, Car *player2, int players, int trackIndex){

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0, H_RESOLUTION, 0, V_RESOLUTION, 0, 1 );				
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	
	if (trackIndex == 0){

		if (players == 2)
			glTranslatef(H_RESOLUTION/2-250, V_RESOLUTION-390, 0);
		else
			glTranslatef(-190, V_RESOLUTION-390, 0);
		
		glBindTexture(GL_TEXTURE_2D, miniMapTexture);
		glColor3ub(255, 255, 255);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0,  0);
			glTexCoord2f(1,0); glVertex2f(512,0);
			glTexCoord2f(1,1); glVertex2f(512,512);
			glTexCoord2f(0,1); glVertex2f(0,  512);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glLoadIdentity();
		
		// draw players on mini map
		if (players == 2)
			glTranslatef(H_RESOLUTION/2-15, V_RESOLUTION-197, 0); // check 210
		else
			glTranslatef(45, V_RESOLUTION-200, 0);

		glRotatef(90, 0.0f, 0.0f, 1.0f);
		Vector2 player1Position = player1->getPosition();
		glPushMatrix();
			glTranslatef(player1Position.x/1.7f, -player1Position.y/1.7f, 0.0f);
			glBegin(GL_QUADS);
				glColor3ub(255, 0,0);
				glVertex2f(-5,  0);
				glVertex2f(0,0);
				glVertex2f(0,5);
				glVertex2f(-5,  5);
			glEnd();
		glPopMatrix();

		if (players == 2){
			Vector2 player2Position = player2->getPosition();
			glPushMatrix();
				glTranslatef(player2Position.x/1.7f, -player2Position.y/1.7f, 0.0f);
				glBegin(GL_QUADS);
					glColor3ub(0, 255,0);
					glVertex2f(-5,  0);
					glVertex2f(0,0);
					glVertex2f(0,5);
					glVertex2f(-5,  5);
				glEnd();
			glPopMatrix();
		}
	}
	else{
		
		if (players == 2)
			glTranslatef(H_RESOLUTION/2-250, V_RESOLUTION-390, 0);
		else
			glTranslatef(-190, V_RESOLUTION-390, 0);
		
		glBindTexture(GL_TEXTURE_2D, miniMapTexture2);
		glColor3ub(255, 255, 255);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(0,  0);
			glTexCoord2f(1,0); glVertex2f(512,0);
			glTexCoord2f(1,1); glVertex2f(512,512);
			glTexCoord2f(0,1); glVertex2f(0,  512);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glLoadIdentity();
		
		// draw players on mini map
		if (players == 2)
			glTranslatef((H_RESOLUTION/2), V_RESOLUTION-100, 0);
		else
			glTranslatef(60, V_RESOLUTION-100, 0);
		
		glRotatef(90, 0.0f, 0.0f, 1.0f);
		Vector2 player1Position = player1->getPosition();
		glPushMatrix();
			glTranslatef(player1Position.x/2, -player1Position.y/2, 0.0f);
			glBegin(GL_QUADS);
				glColor3ub(255, 0,0);
				glVertex2f(-5,  0);
				glVertex2f(0,0);
				glVertex2f(0,5);
				glVertex2f(-5,  5);
			glEnd();
		glPopMatrix();

		if (players == 2){
			Vector2 player2Position = player2->getPosition();
			glPushMatrix();
				glTranslatef(player2Position.x/2, -player2Position.y/2, 0.0f);
				glBegin(GL_QUADS);
					glColor3ub(0, 255,0);
					glVertex2f(-5,  0);
					glVertex2f(0,0);
					glVertex2f(0,5);
					glVertex2f(-5,  5);
				glEnd();
			glPopMatrix();
		}
	}
}