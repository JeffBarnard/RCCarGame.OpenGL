#pragma once

#define MAX_PLAYER_CHARS 15

#include <string.h>

class Player1
{
	public:
	
	//Attributes

	char name[MAX_PLAYER_CHARS + 1];
	float money;
	int vehicleIndex;
	float currentLapTime;

	float earnedMoney;
	float repairCosts;

	float grandTotal;
	
	//Operations
	
	Player1(void);
	~Player1(void);

	void reset(void){
		money = 0;
		vehicleIndex = 0;
		name[0] = '\0';
		
		earnedMoney = 0.0f;
		repairCosts = 0.0f;
		grandTotal = 0.0f;
	}
};
