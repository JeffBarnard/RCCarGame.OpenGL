//////////////////////////////////////////////////////////////////////////////
////																			
////  A Series of 3D collision functions
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _COLLISION_H
#define _COLLISION_H

#include "algebra.h"
#include "bb.h"

#define BEHIND		0
#define INTERSECTS	1
#define FRONT		2

namespace Collision {

	/////////////////////////////////////////////////////////////////
	///
	/// Bounding Box Collision
	///
	/////////////////////////////////////////////////////////////////
	bool AABBCollision(BoundingVolume::AABB2D box1, BoundingVolume::AABB2D box2, Vector2 TestPoint);

	bool OBBCollision(BoundingVolume::OBB3D &a, BoundingVolume::OBB3D &b);

	bool isPointInAABB(Vector3 point, BoundingVolume::AABB3D bb);


	///////////
	// not sure where to put this right now
	//////////////////////////////////////////
	

	/////////////////////////////////////////////////////////////////
	///
	/// Sphere to poly collision
	///
	/////////////////////////////////////////////////////////////////
	bool edgeSphereCollision(Vector3 &, Vector3 vPolygon[], int, float);

	int classifySphere(Vector3 &, Vector3 &, Vector3 &, float, float &);

	bool spherePolygonCollision(Vector3 vPolygon[],Vector3 uPolygon[], Vector3 &, int, bool ignoreNormal, 
								Vector3 &myPos, Vector3 &myView, float, Vector3 prevPosition, bool &wallJump);

	Vector3 getSphereCollisionOffset(Vector3 &vNormal, float radius, float distance, bool ignoreNormal);

	/////////////////////////////////////////////////////////////////
	///
	/// Sphere to Sphere collision
	///
	/////////////////////////////////////////////////////////////////
	bool unitSphereCollision(Vector3 &c1, Vector3 c2, Vector3 &newC1);

	Vector3 intersectionPoint(Vector3, Vector3 vLine[], double);

	bool intersectedPlane(Vector3 vPoly[], Vector3 vLine[], Vector3 &, float &);


	// support functions
	///////////////////////////////////////////////////////////////
	bool pointInsidePolygon(Vector3, Vector3 Poly[], long);

	Vector3 closestPointOnLine(Vector3, Vector3, Vector3) ;

	bool intersectedPolygon(Vector3 vPoly[], Vector3 vLine[], int);

}

#endif // _COLLISION_H
