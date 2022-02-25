//////////////////////////////////////////////////////////////////////////////
///																			
/// Track class, inherits from model to allow loading/rendering.
/// Also stores information about the track, such as textures and lap times
///
///  Jeff Barnard  2004
///
//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "track.h"

///////////////////////////// checkLapMarkers \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  Checks if car has passed the start/finish line (and other sectors)
///
///////////////////////////// checkLapMarkers \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Track::checkLapMarkers(Vector2 *position, Vector3 *direction, int trackNum){

	// define testing line
	line[0].x = position->x;  line[1].x = direction->x;
	line[0].y = 0.1f;         line[1].y = 0.1f;
	line[0].z = position->y;  line[1].z = direction->z;

		
	if (Collision::intersectedPolygon(startPoly, line, 3)) {
		return START;
		
	}
	else if (Collision::intersectedPolygon(sectorOne, line, 3)){
		return FIRST;
	}
	else if (Collision::intersectedPolygon(sectorTwo, line, 3)){
		return SECOND;
	}
	else{
		return -1;
	}
}

c3dsModel * Track::getModel(void){

	return get3DModel();
}

void Track::startTime(void){
	hasTimeStarted = true;
	timer.startTime();
}

double Track::getTime(void){
	if (hasTimeStarted)
		return timer.getTime();
	else
		return 0.0;
}

void Track::startTimePTwo(void){
	hasTimeStartedPTwo = true;
	playerTwoTimer.startTime();
}

double Track::getTimePTwo(void){
	if (hasTimeStartedPTwo)
		return playerTwoTimer.getTime();
	else
		return 0.0;
}

void Track::resetTime(void){
	timer.stopTime();
	playerTwoTimer.stopTime();
	hasTimeStarted = false;
	hasTimeStartedPTwo = false;
}