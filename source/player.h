//////////////////////////////////////////////////////////////////////////////
////																			
////  Player Class
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _PLAYER_H
#define _PLAYER_H

#include "car.h"
#include "track.h"

class Player {

public:

	Car *car;
	Track *track;

	float money;
	float time;

public:

	Player();
	
	Player(Car *c, Track *t){
		this->car = c;
		this->track = t;
	}

};

#endif // _PLAYER_H