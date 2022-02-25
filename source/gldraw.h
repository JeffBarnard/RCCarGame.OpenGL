//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Jeff Barnard  2003
////																			
//////////////////////////////////////////////////////////////////////////////

#include "car.h"
#include "bb.h"
#include "track.h"
#include "texture.h"
#include "camera.h"
#include "hud.h"
#include "SystemSettings.h"

#define skyBoxTexIndex_bk 4
#define skyBoxTexIndex_ft 5
#define skyBoxTexIndex_rt 6
#define skyBoxTexIndex_lt 7
#define skyBoxTexIndex_tp 8
#define skyBoxTexIndex_bt 9


//////////////////////////////////////////
////  function declarations
//////////////////////////////////////////
int InitGL(int players, int p1vehicleIndex, int p2vehicleIndex, int trackIndex);
int DrawGLScene(int players, SystemSettings *system);
void CheckForMovement(SystemSettings *system);
void displayText(int players);
void renderShutdown(void);

void compileSkyBox(void);


