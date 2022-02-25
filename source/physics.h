//////////////////////////////////////////////////////////////////////////////
////																			
//// Car Physics
////
//// Jeff Barnard  2004
////
//// Special Thanks to Marco for the Physics Formulas:
//// Marco Monster
//// monstrous@planet.nl
//// http://home.planet.nl/~monstrous
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "car.h"
#include "algebra.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

namespace Physics {

	// reference values

	// stiffness -6
	// grip 2.0
	// drag 5.0
	// resistanct 30
	// DRAG		 5.0	 // 3 
	// RESISTANCE 100.0	 // 20
	// CA_R		-6.20	 // 8.2
	// CA_F		-4.0	 // 8
	// MAX_GRIP	 6.0	 // 2

	void initCarTypes(CarSpec *);

	void calculatePhysics(Car *, float);

}

#endif