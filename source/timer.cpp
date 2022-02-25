//////////////////////////////////////////////////////////////////////////////
////
////  Timer	Class		
////
////  Jeff Barnard  2003
////																			
//////////////////////////////////////////////////////////////////////////////

#include "timer.h"

//////////////////////////////// startTime \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Starts and Stops Time
///
//////////////////////////////// startTime \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Timer::startTime(void){
	oldTime = GetTickCount();
}

void Timer::stopTime(void){
	currentTime = 0;
	oldTime = 0;
}

//////////////////////////////// getTime \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	This function calculates the frame rate and time intervals between frames
///
//////////////////////////////// getTime \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
double Timer::getTime(void){

	cTime = GetTickCount();
	return cTime - oldTime;
}

////////////////////////////// calculateFrameRate \\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	This function calculates the frame rate and time intervals between frames
///
////////////////////////////// calculateFrameRate \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
double Timer::calculateFrameRate(void) {

	static float   fps = 0.0f;		
    static double  lastTime;
	static char    winText[40];	
	static double  lt;
	double ct    = GetTickCount() * 0.001f;

	
	// retrieves the current value of the high-resolution performance counter. (milliseconds)
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	double timerdiff = (currentTime) - (lastTime);

	// Here I store the elapsed time between the current and last frame,
	// then keep the current frame in the static variable for the next frame.
 	double frameInterval = timerdiff / 10000.0f ;
	
	lastTime = (double)currentTime;

	++fps;
	// print fps to the window title
	if( (ct - lt) > 1.0f ) {

		lt = ct;

		sprintf(winText, "RC Car Game    FPS: %d", (int)fps);
		SetWindowText(*hWnd, winText);		
 
		fps = 0;
    }

	return frameInterval;
}