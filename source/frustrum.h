//////////////////////////////////////////////////////////////////////////////
////																			
////  View Frustrum
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _FRUSTRUM_H
#define _FRUSTRUM_H

#include "algebra.h"
#include "bb.h"

enum frustumSide {
	RIGHT,
	LEFT,
	BOTTOM,
	TOP,
	BACK,
	FRONT
}; 

class Frustrum {
    
private:

	float myFrustrum[6][4];

public:

	int containsSphere(BoundingVolume::BoundingSphere& refSphere);
	
	int containsAABB(BoundingVolume::AABB3D& refBox);

	bool updateFrustrum(void);

};

#endif


