//////////////////////////////////////////////////////////////////////////////
////																			
////  Heads up Display
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _HUD_H
#define _HUD_H

#include "car.h"
#include "SystemSettings.h"

void drawHUD (Car *car, Timer *gameTimer, Track *track, SystemSettings *system);

void drawHUDTwoPlayer (Car *car, Car *p2car, Timer *gameTimer, Track *track, SystemSettings *system);

void drawStatsScreenTwoPlayer (Car *car, Car *p2car, Timer *gameTimer, Track *track, SystemSettings *system);

void drawStatsScreen (Car *car, Timer *gameTimer, Track *track, SystemSettings *system);

void drawMiniMap(Car *player1, Car *player2, int players, int trackIndex);

void loadHUDTextures(void);

void killHUDTextures(void);

#endif