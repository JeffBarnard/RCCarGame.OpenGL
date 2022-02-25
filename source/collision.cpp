//////////////////////////////////////////////////////////////////////////////
////																			
////  A Series of functions to determine whether a sphere collides with
////  a polygon (defined by 3 or more verticies)
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#include "collision.h"
#include <windows.h>	

using namespace Algebra;

///////////////////////////////// AABBCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	2D Axis Aligned Bounding Box Collision Detection, returns true if intersect
/////
///////////////////////////////// AABBCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::AABBCollision(BoundingVolume::AABB2D box1, BoundingVolume::AABB2D box2, Vector2 TestPoint) {

  	/*if(blueCarBox.max.x <= TestPoint.x)
		return false;

	if(blueCarBox.min.x >= TestPoint.x)
		return false;

	if(blueCarBox.max.y <= TestPoint.y)
		return false;

	if(blueCarBox.min.y >= TestPoint.y)
		return false;*/

	if ((box1.min.x > box2.min.x && box1.min.x < box2.max.x) ||	
		(box1.max.x > box2.min.x && box1.max.x < box2.max.x)){
        
		if ((box1.min.y > box2.min.y && box1.min.y < box2.max.y) 
			||(box1.max.y > box2.min.y && box1.max.y < box2.max.y)) {
				return true;
		}
	}
	else if ((box2.min.x > box1.min.x && box2.min.x < box1.max.x) ||	
	    	(box2.max.x > box1.min.x && box2.max.x < box1.max.x)) {
		
		if ((box2.min.y > box1.min.y && box2.min.y < box1.max.y) 
			||(box2.max.y > box1.min.y && box2.max.y < box1.max.y)) {
				return true;
		}
	}
 	return false;	
}


/////////////////////////////////// OBBCollision  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	Checks if a OBB (Oriented BoundingBox) collides with another OBB
/////
/////   Thanks to Zygotemm (zengine.gotdns.com/zengine) for the base code.
/////
/////////////////////////////////// OBBCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::OBBCollision(BoundingVolume::OBB3D &a, BoundingVolume::OBB3D &b)    {
	
	/// Rotate one OBB into others space
	///////////////////////////////////////////////////////////
	Algebra::Matrix4 matInverse = a.rotationMatrix.GetInverse();
	Algebra::Matrix4 matB = b.rotationMatrix * matInverse;

	Vector3 vPosB = a.inverseMatrix * (b.center - a.center);
	Vector3 XAxis(matB.GetEntry(0), matB.GetEntry(1), matB.GetEntry(2));       
	Vector3 YAxis(matB.GetEntry(4), matB.GetEntry(5), matB.GetEntry(6));       
	Vector3 ZAxis(matB.GetEntry(8), matB.GetEntry(9), matB.GetEntry(10));

	/// Perform Angle Axis Theorem on the boxes (15 Tests)
	/////////////////////////////////////////////////////////// 

	//1 (Ra)x       
	if(fabs(vPosB.x) > a.halfSize.x + b.halfSize.x * fabs(XAxis.x) + b.halfSize.y * fabs(XAxis.y) + b.halfSize.z * fabs(XAxis.z))     
		return false;       
	//2 (Ra)y       
	if(fabs(vPosB.y) > a.halfSize.y + b.halfSize.x * fabs(YAxis.x) + b.halfSize.y * fabs(YAxis.y) + b.halfSize.z * fabs(YAxis.z))    
		return false;      
	//3 (Ra)z        
	if(fabs(vPosB.z) > a.halfSize.z + b.halfSize.x * fabs(ZAxis.x) + b.halfSize.y * fabs(ZAxis.y) + b.halfSize.z * fabs(ZAxis.z))      
		return false;     
	//4 (Rb)x      
	if(fabs(vPosB.x * XAxis.x+vPosB.y * YAxis.x+vPosB.z * ZAxis.x) >   
		(b.halfSize.x+a.halfSize.x*fabs(XAxis.x) + a.halfSize.y * fabs(YAxis.x) + a.halfSize.z*fabs(ZAxis.x))) 
		return false; 
	//5 (Rb)y       
	if(fabs(vPosB.x * XAxis.y+vPosB.y * YAxis.y+vPosB.z * ZAxis.y) >    
		(b.halfSize.y+a.halfSize.x*fabs(XAxis.y) + a.halfSize.y * fabs(YAxis.y) + a.halfSize.z*fabs(ZAxis.y))) 
		return false;    
	//6 (Rb)z       
	if(fabs(vPosB.x * XAxis.z+vPosB.y * YAxis.z+vPosB.z * ZAxis.z) >      
		(b.halfSize.z+a.halfSize.x*fabs(XAxis.z) + a.halfSize.y * fabs(YAxis.z) + a.halfSize.z*fabs(ZAxis.z)))     
		return false;     
	//7 (Ra)x X (Rb)x    
	if(fabs(vPosB.z * YAxis.x-vPosB.y * ZAxis.x) > a.halfSize.y * fabs(ZAxis.x) +    
		a.halfSize.z*fabs(YAxis.x) + b.halfSize.y*fabs(XAxis.z) + b.halfSize.z*fabs(XAxis.y))  
		return false;   
	//8 (Ra)x X (Rb)y   
	if(fabs(vPosB.z * YAxis.y-vPosB.y * ZAxis.y) > a.halfSize.y * fabs(ZAxis.y) +  
		a.halfSize.z*fabs(YAxis.y) + b.halfSize.x*fabs(XAxis.z) + b.halfSize.z*fabs(XAxis.x))  
		return false; 
	//9 (Ra)x X (Rb)z 
	if(fabs(vPosB.z * YAxis.z-vPosB.y * ZAxis.z) > a.halfSize.y * fabs(ZAxis.z) +    
		a.halfSize.z*fabs(YAxis.z) + b.halfSize.x*fabs(XAxis.y) + b.halfSize.y*fabs(XAxis.x))  
		return false;  
	//10 (Ra)y X (Rb)x 
	if(fabs(vPosB.x * ZAxis.x-vPosB.z * XAxis.x) > a.halfSize.x * fabs(ZAxis.x) +
		a.halfSize.z*fabs(XAxis.x) + b.halfSize.y*fabs(YAxis.z) + b.halfSize.z*fabs(YAxis.y)) 
		return false; 
	//11 (Ra)y X (Rb)y  
	if(fabs(vPosB.x * ZAxis.y-vPosB.z * XAxis.y) > a.halfSize.x * fabs(ZAxis.y) + 
		a.halfSize.z*fabs(XAxis.y) + b.halfSize.x*fabs(YAxis.z) + b.halfSize.z*fabs(YAxis.x)) 
		return false;   
	//12 (Ra)y X (Rb)z     
	if(fabs(vPosB.x * ZAxis.z-vPosB.z * XAxis.z) > a.halfSize.x * fabs(ZAxis.z) +     
		a.halfSize.z*fabs(XAxis.z) + b.halfSize.x*fabs(YAxis.y) + b.halfSize.y*fabs(YAxis.x)) 
		return false;   
	//13 (Ra)z X (Rb)x    
	if(fabs(vPosB.y * XAxis.x-vPosB.x * YAxis.x) > a.halfSize.x * fabs(YAxis.x) + 
		a.halfSize.y*fabs(XAxis.x) + b.halfSize.y*fabs(ZAxis.z) + b.halfSize.z*fabs(ZAxis.y))   
		return false; 
	//14 (Ra)z X (Rb)y  
	if(fabs(vPosB.y * XAxis.y-vPosB.x * YAxis.y) > a.halfSize.x * fabs(YAxis.y) +    
		a.halfSize.y*fabs(XAxis.y) + b.halfSize.x*fabs(ZAxis.z) + b.halfSize.z*fabs(ZAxis.x))  
		return false;   
	//15 (Ra)z X (Rb)z    
	if(fabs(vPosB.y * XAxis.z-vPosB.x * YAxis.z) > a.halfSize.x * fabs(YAxis.z) + 
		a.halfSize.y*fabs(XAxis.z) + b.halfSize.x*fabs(ZAxis.y) + b.halfSize.y*fabs(ZAxis.x))   
		return false;  
	
	return true;
}


/////////////////////////////////// isPointInAABB  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This checks to see if a point is inside an AABB3D
/////
/////////////////////////////////// isPointInAABB \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*	
bool Collision::isPointInAABB(Vector3 point, BoundingVolume::AABB3D bb){
	
	// NOTE: this function is not 100% accurate
	if (point.x > bb.max.x || point.x < bb.min.x)
		return false;

	if (point.y > bb.max.y || point.y < bb.min.y)
		return false;

	if (point.z > bb.max.z || point.z < bb.min.z)
		return false;
		
	return true;
}



/////////////////////////////////// intersectedPlane  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This checks to see if a line intersects a plane
/////
/////////////////////////////////// intersectedPlane \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*											
bool Collision::intersectedPlane(Vector3 vPoly[], Vector3 vLine[], Vector3 &vNormal, float &originDistance){
	float distance1=0, distance2=0;		// The distances of the 2 points from the line to the plane
			
	vNormal = normal(vPoly);

	originDistance = planeDistanceFromOrigin(vNormal, vPoly[0]);

	// Get the distance of the points from the plane using: Ax + By + Cz + D =
	distance1 = ((vNormal.x * vLine[0].x)  +					
		         (vNormal.y * vLine[0].y)  +					
				 (vNormal.z * vLine[0].z)) + originDistance;	
	
	distance2 = ((vNormal.x * vLine[1].x)  +					
		         (vNormal.y * vLine[1].y)  +					
				 (vNormal.z * vLine[1].z)) + originDistance;

	if(distance1 * distance2 >= 0){
		// If it's a negative number, that means a collision
		// this is because the 2 points must be on either side of the plane (IE. -1 * 1 = -1).
		return false;
	}

	return true;
}



/////////////////////////////////// intersectionPoint \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the intersection point of the line that intersects a plane
/////
/////////////////////////////////// intersectionPoint \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Collision::intersectionPoint(Vector3 vNormal, Vector3 vLine[], double distance){
	
	Vector3 vPoint, 
			vLineDir;					
	double  Numerator = 0.0, 
			Denominator = 0.0, 
			dist = 0.0;
	
	vLineDir = vLine[1] - vLine[0];			
	vLineDir = normalize3(vLineDir);	

	// (distance = Ax + By + Cz + D) to find the distance from one points to the plane.
	Numerator = - (vNormal.x * vLine[0].x +	vNormal.y * vLine[0].y + vNormal.z * vLine[0].z + distance);

	// take the dot product between the line vector and the normal of the polygon
	Denominator = dot(vNormal, vLineDir);	
				  
	if( Denominator == 0.0)
		return vLine[0]; // return an arbitrary point on the line						

	// divide to get the multiplying (percentage) factor
	dist = Numerator / Denominator;				
	
	// multiply the dist by the vector, then add the arbitrary point.
	vPoint.x = (float)(vLine[0].x + (vLineDir.x * dist));
	vPoint.y = (float)(vLine[0].y + (vLineDir.y * dist));
	vPoint.z = (float)(vLine[0].z + (vLineDir.z * dist));

	return vPoint;
}



/////////////////////////////////// pointInsidePolygon \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This checks to see if a point is inside the ranges of a polygon
/////
/////////////////////////////////// pointInsidePolygon \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::pointInsidePolygon(Vector3 vIntersection, Vector3 Poly[], long verticeCount){
	
	const float MATCH_FACTOR = 0.99f;
	double Angle = 0.0;							
	Vector3 vA, vB;
	
	for (int i = 0; i < verticeCount; i++){		// Go in a circle to each vertex and get the angle between

		vA = Poly[i] - vIntersection;			// Subtract the intersection point from the current vertex
												// Subtract the point from the next vertex
		vB = Poly[(i + 1) % verticeCount] - vIntersection;
		Angle += angleBetweenVectors(vA, vB);	// Find the angle between the 2 vectors and add them all up as we go along
	}
											
	if(Angle >= (MATCH_FACTOR * (2.0 * PI)) )
		return true;
		
	return false;
}


/////////////////////////////////// IntersectedPolygon \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This checks if a line is intersecting a polygon
/////
/////////////////////////////////// IntersectedPolygon \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::intersectedPolygon(Vector3 vPoly[], Vector3 vLine[], int verticeCount){
	Vector3 vNormal;
	float originDistance = 0;

	// First, make sure the line intersects the plane
	if(!intersectedPlane(vPoly, vLine,  vNormal,  originDistance))
		return false;

	Vector3 vIntersection = intersectionPoint(vNormal, vLine, originDistance);

	// Now that the intersection point is found, test if it's inside the polygon.
	if(pointInsidePolygon(vIntersection, vPoly, verticeCount))
		return true;

	return false;	
}


////////////////////////////// closestPointOnLine \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the point on the line vA_vB that is closest to the point vPoint
/////
////////////////////////////// closestPointOnLine \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Collision::closestPointOnLine(Vector3 vA, Vector3 vB, Vector3 point) {
	// Create the vector from end point vA to the point vPoint.
	Vector3 vVector1 = point - vA;
    Vector3 vVector2 = normalize3(vB - vA);

	// find magnitude of line segement
    float d = getDistance(vA, vB);

	// project the vVector1 onto the vector vVector2. This essentially gives the distance 
	// from projected vector to vA.
    float t = dot(vVector2, vVector1);

	// if the projected distance from vA is less than or equal to 0, it must
	// be closest to the end point vA.
    if (t <= 0) 
		return vA;

	// if the projected distance from vA is greater than or equal to the magnitude
	// or distance of the line segment, it must be closest to end point vB.
    if (t >= d) 
		return vB;
 
	Vector3 vVector3 = vVector2 * t;

	// simply add vVector3 to the original end point vA.  
    Vector3 vClosestPoint = vA + vVector3;
	return vClosestPoint;
}



////////////////////////////// unitSphereCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	A unit sphere collision test
/////
////////////////////////////// unitSphereCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::unitSphereCollision(Vector3 &c1, Vector3 c2, Vector3 &newC1){

	float distance = Algebra::getDistance(c1, c2);

	if (distance < 2.0f){

		// create vector from c2 to c1
		Vector3 direction = Algebra::calculateVector(c1, c2);
        // normalize it
		direction = Algebra::normalize3(direction);

		// now find the overlap distance
		float overlap = 2.0f - distance;

		// calculate overlap vector
		Vector3 vectorO = direction * overlap;

		// move there
		newC1 = newC1 + vectorO;
		
		return true;
	}
	return false;
}



////////////////////////////// spherePolygonCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns true if a sphere collides with the polygon passed in
/////
////////////////////////////// spherePolygonCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::spherePolygonCollision(Vector3 vPolygon[], Vector3 uPolygon[], 
									   Vector3 &vCenter, int vertexCount, bool ignoreNormal, 
									   Vector3 &myPos, Vector3 &myView, 
									   float speed, Vector3 prevPosition, bool &wallJump) 
{
	float radius = 1.0f; // always doing unit sphere testing
	Vector3 vNormal = normal(vPolygon);
	wallJump = false;
	float distance = 0.0f;
	int classification = classifySphere(vCenter, vNormal, vPolygon[0], radius, distance);

	Vector3 throughLine[2];
			throughLine[1].x = prevPosition.x;
			throughLine[1].y = 0.1f;
			throughLine[1].z = prevPosition.z;
			throughLine[0].x = myPos.x;
			throughLine[0].y = 0.1f;
			throughLine[0].z = myPos.z;

	// make sure the car is not going so fast that it skips right through the wall
	if (throughLine[0] != throughLine[1]){
		if (classification != INTERSECTS){
			// define line joining this position to previous position
			if (prevPosition.x != 0.0 && prevPosition.z != 0.0){
				if (Collision::intersectedPolygon(uPolygon, throughLine, 3)) {
					wallJump = true;
				}
			}
		}
	}

	if (classification == INTERSECTS) {
		// find the psuedo intersection point on the plane
		Vector3 vOffset = vNormal * distance;
		// subtract offset from the center of the sphere. 
		// "vPosition" now a point that lies on the plane of the polygon.
		Vector3 vPosition = vCenter - vOffset;

		// check if the intersection point is inside the polygons perimeter
		if((pointInsidePolygon(vPosition, vPolygon, vertexCount))) {// || 
			//(edgeSphereCollision(vCenter, vPolygon, vertexCount, radius/2))){
			vOffset = getSphereCollisionOffset(vNormal, radius, distance, ignoreNormal);

			myPos.x += vOffset.x;
			myPos.z += vOffset.z;
            //myView.x += vOffset.x;
			//myView.z += vOffset.z;
			return true;

		}else{

			if (edgeSphereCollision(vCenter, vPolygon, vertexCount, radius/2)) {
			   
				vOffset = getSphereCollisionOffset(vNormal, radius, distance, ignoreNormal);

				myPos.x += vOffset.x;
				myPos.z += vOffset.y;
                //myView.x += vOffset.x;
				//myView.z += vOffset.y;                
				return true;
			}
		}
	}
	return false;
}



////////////////////////////////////// classifySphere \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This tells if a sphere is BEHIND, in FRONT, or INTERSECTS a plane, also it's distance
/////
////////////////////////////////////// classifySphere \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Collision::classifySphere(Vector3 &vCenter, Vector3 &vNormal, Vector3 &vPoint, float radius, float &distance) {
	// First, find the distance the polygon plane is from the origin.
	float d = (float)planeDistanceFromOrigin(vNormal, vPoint);

	// find the distance the center point of the sphere is from the polygon's plane.  
	// Ax + By + Cz + d = 0 with ABC = Normal, XYZ = Point
	distance = (vNormal.x * vCenter.x + vNormal.y * vCenter.y + vNormal.z * vCenter.z + d);

	// note: the side with the normal pointing out from it is the front side
	// If the distance found is less than the radius, the sphere intersected the plane.

	//abs(distance)
	if(absolute(distance) < radius)
		return INTERSECTS;

	//else if(distance
	else if(distance >= radius)
		return FRONT;
	
	return BEHIND;
}


///////////////////////////////// EdgeSphereCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns true if the sphere is intersecting any of the edges of the polygon
/////
///////////////////////////////// EdgeSphereCollision \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Collision::edgeSphereCollision(Vector3 &vCenter, Vector3 vPolygon[], int vertexCount, float radius) {
	Vector3 vPoint;

	// every edge line segment finds the closest point on that line to the center of the sphere.
	// If the distance from that closest point is less than the radius of the sphere, there was
	// a collision.
	for(int i = 0; i < vertexCount; i++) {

		vPoint = closestPointOnLine(vPolygon[i], vPolygon[(i + 1) % vertexCount], vCenter);
		float d = getDistance(vPoint, vCenter);
		if(d < radius)
			return true;
	}

	return false;
}


///////////////////////////////// getSphereCollisionOffset \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  How far is the sphere overlapping a plane?  Returns a Vector3 (with proper size)
/////
///////////////////////////////// getSphereCollisionOffset \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Collision::getSphereCollisionOffset(Vector3 &vNormal, float radius, float distance, bool ignoreNormal) {

	Vector3 vOffset = Vector3(0, 0, 0);

	int j = 0;

	if (ignoreNormal){
		// if greater than zero, its in front of the polygon
		if(distance > 0) {		
			// find the distance that the sphere is overlapping the plane, then
			// find the direction vector to move the sphere.
			float distanceOver = radius - distance;
			vOffset = vNormal * distanceOver;
		}
		else { // else colliding from behind the polygon		
			float distanceOver = radius + distance;
			vOffset = vNormal * -distanceOver;
		}
	}
	else{
		float distanceOver = radius + distance;
		vOffset = vNormal * -distanceOver;		
	}	
	return vOffset;
}

