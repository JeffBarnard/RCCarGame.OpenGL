//////////////////////////////////////////////////////////////////////
////																
////  Creates/shutdowns and initializes an OpenGL window						
////																
////  Jeff Barnard  2003
////
//////////////////////////////////////////////////////////////////////

#ifndef _GLSETUP_H
#define _GLSETUP_H

#define H_RESOLUTION 1024
#define V_RESOLUTION 768
#define COLOR_DEPTH  32

// force the linker to include the following libraries
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "fmodvc.lib")


#define GL_CLAMP_TO_EDGE				    0x812F  
//#ifndef GL_EXT_texture_cube_map 
// EXT_texture_cube_map 
#define GL_EXT_texture_cube_map             1 
#define GL_NORMAL_MAP_EXT                   0x8511 
#define GL_REFLECTION_MAP_EXT               0x8512 
#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 
//#endif


#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <gl\gl.h>
#include <gl\glu.h>			
#include <gl\glaux.h>	
#include "timer.h"

// OpenGL startup/shutdown functions
extern int InitGL(int players, int p1vehicleIndex, int p2vehicleIndex, int trackIndex);
bool CreateGLWindow(char* , int , int , int , bool );
GLvoid ReSizeGLScene(GLsizei , GLsizei );
GLvoid KillGLWindow(GLvoid);
//void CalculateFrameRate(void);

// text functions
GLvoid BuildFont(GLvoid);
GLvoid KillFont(GLvoid);
GLvoid glPrint(int x, int y, const char *fmt, ...);

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif




