#include "SystemSettings.h"


SystemSettings::SystemSettings(void)
{
	numLaps = 3;
	p1 = new Player1;
	p2 = new Player2;
	trackIndex = 0;
	numPlayers = 0;
}

SystemSettings::~SystemSettings(void)
{
	delete p1;
	delete p2;
}

void SystemSettings::resetValues(void)
{
	numLaps = 3;
	p1->reset();
	p2->reset();
	trackIndex = 0;
	numPlayers = 0;
}

