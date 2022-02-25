//////////////////////////////////////////////////////////////////////////////
////																			
//// Track class, inherits from model to allow loading/rendering.
//// Also stores information about the track, such as textures and lap times
////
//// Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _TRACK_H
#define _TRACK_H

#include "model.h"
#include "timer.h"
#include "collision.h"

enum TrackSectors{
	START,
	FIRST,
	SECOND
};

class Track : public Model {

private:

	Timer timer;
	Timer playerTwoTimer;

	bool hasTimeStarted;
	bool hasTimeStartedPTwo;

	Vector3 startPoly[3];  // polygon defining start/finish line
	Vector3 sectorOne[3];  // polygon defining sector 1 line
	Vector3 sectorTwo[3];  // polygon defining sector 2 line

	Vector3 line[2];       // line to test for intersection against sectors

	int trackIndex;
	
public:

	Track() : Model() {
		cullingTests = true;	
		hasTimeStarted = false;
		hasTimeStartedPTwo = false;
		trackIndex = 0;
		
	}

	void setTrackPath(char *filename, int trackI) {
		this->setMeshFile(filename);
		this->trackIndex = trackI;

		TRACE("Constructing Track - Defining Track Sectors");

		// these should be loaded from tracks.ini
		if (this->trackIndex == 0){
			// define start finish line
			startPoly[0].x = 29.5f;   startPoly[1].x = 29.5f;   startPoly[2].x = 29.5f;
			startPoly[0].y = 0.0f;    startPoly[1].y = 0.0f;    startPoly[2].y = 10.0f;
			startPoly[0].z = 28.0f;   startPoly[1].z = 17.0f;   startPoly[2].z = 23.0f;
			// define sector 1 mark
			sectorOne[0].x = 235.0f;  sectorOne[1].x = 248.0f;  sectorOne[2].x = 257.0f;
			sectorOne[0].y = 0.0f;    sectorOne[1].y = 10.0f;   sectorOne[2].y = 0.0f;
			sectorOne[0].z = -24.3f;  sectorOne[1].z = -16.0f;  sectorOne[2].z = -1.5f;
			// define sector 2 mark
			sectorTwo[0].x = 95.4f;   sectorTwo[1].x = 96.77f;  sectorTwo[2].x = 97.6f;
			sectorTwo[0].y = 0.0f;    sectorTwo[1].y = 10.0f;   sectorTwo[2].y = 0.0f;
			sectorTwo[0].z = 113.6f;  sectorTwo[1].z = 123.6f;  sectorTwo[2].z = 135.5f;
		}
		else{
			// define start finish line
			startPoly[0].x = 29.5f;   startPoly[1].x = 29.5f;   startPoly[2].x = 29.5f;
			startPoly[0].y = 0.0f;    startPoly[1].y = 0.0f;    startPoly[2].y = 10.0f;
			startPoly[0].z = 28.0f;   startPoly[1].z = 17.0f;   startPoly[2].z = 23.0f;
			// define sector 1 mark
			sectorOne[0].x = -15.5f;  sectorOne[1].x = -16.0f;  sectorOne[2].x = -15.8f;
			sectorOne[0].y = 0.0f;    sectorOne[1].y = 0.0f;    sectorOne[2].y = 10.0f;
			sectorOne[0].z = -69.0f;  sectorOne[1].z = -76.0f;  sectorOne[2].z = -72.0f;
			// define sector 2 mark
			sectorTwo[0].x = -178.7f; sectorTwo[1].x = -175.0f; sectorTwo[2].x = -176.0f;
			sectorTwo[0].y = 0.0f;    sectorTwo[1].y = 0.0f;    sectorTwo[2].y = 10.0f;
			sectorTwo[0].z = 102.0f;   sectorTwo[1].z = 86.0f;   sectorTwo[2].z = 92.0f;
		}
	}

	c3dsModel * getModel(void);

	void startTime(void);

	double getTime(void);

	void startTimePTwo(void);

	double getTimePTwo(void);

	void resetTime(void);

	int checkLapMarkers(Vector2 *position, Vector3 *direction, int trackNum);
};


#endif