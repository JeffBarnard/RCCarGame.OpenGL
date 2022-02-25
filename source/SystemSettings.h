// BE SURE TO CREATE A METHOD TO RESET ALL VALUES TO THE DEFAULTS

#pragma once

#include "track.h"
#include "Player1.h"
#include "Player2.h"
#include <vector>

class SystemSettings
{	
	public:
	
	// Attributes
	
	int numLaps;
    Track *track;
	int trackIndex;
	int numPlayers;
	Player1 *p1;
	Player2 *p2;

	vector<float> curLapTimes;

	// Operations
	
	SystemSettings(void);
	~SystemSettings(void);

	void addLapTime(float time){
		curLapTimes.push_back(time);
	}

	void resetValues(void);
};
