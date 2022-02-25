#pragma once

class Player2
{
public:

	//Attributes

	float currentLapTime;
	int vehicleIndex;
	
	//Operations

	Player2(void);
	~Player2(void);

	void reset(void){
		vehicleIndex = 0;
		currentLapTime = 0.0;
	}
};
