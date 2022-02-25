//////////////////////////////////////////////////////////////////////////////
////
////  Timer			
////
////  Jeff Barnard  2003
////																			
//////////////////////////////////////////////////////////////////////////////

#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>	
#include <mmsystem.h>
#include <stdio.h>	

class Timer {

private:
	HWND *hWnd;			// so this class can print fps to window border

	_int64 currentTime;
	
	DWORD cTime;
	DWORD oldTime;

public:

	Timer(){
		currentTime = 0;
		oldTime = 0;
	}

	Timer(HWND *hWnd){
		this->hWnd = hWnd;	
		currentTime = 0;
		oldTime = 0;
	}

	double calculateFrameRate(void);

	void startTime(void);
	void stopTime(void);

	double getTime(void);	

};

#endif  // _TIMER_H