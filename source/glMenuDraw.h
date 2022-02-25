//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Patrick Crawford  2003
////																			
//////////////////////////////////////////////////////////////////////////////

#include "texture.h"

//////////////////////////////////////////
////  function declarations
//////////////////////////////////////////

int initGLMenu(void);
//int drawGLMenu(int,int);
int whichMouseObject(int,int,void (*drawFunc)(int,int));
void drawBoxes(int,int);
void draw3d(void);