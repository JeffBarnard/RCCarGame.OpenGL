///////////////////////////////////////////////////////////////////
////
////  Algebra Functions
////
////  Jeff Barnard  2004
////
///////////////////////////////////////////////////////////////////

#include <math.h>
#include <float.h>
#include "algebra.h"

#define MAG(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))
#define MAG2(Normal)(sqrt(Normal.x*Normal.x + Normal.y*Normal.y))

///////////////////////////////// rotateVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Rotate a vector 'v' by angle theta around an arbitrary axis. Return
/////  the rotated point.  Positive angles are anticlockwise looking down
/////  the axis towards the origin.
/////
///////////////////////////////// rotateVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::rotateVector(Vector3 v, float theta, Vector3 around) {
   float costheta, 
		 sintheta;
   Vector3 result(0, 0, 0);
   Vector3 r;

   r = normalize3(around);
   costheta = cos(theta);
   sintheta = sin(theta);

   result.x += (costheta + (1 - costheta) * r.x * r.x) * v.x;
   result.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * v.y;
   result.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * v.z;

   result.y += ((1 - costheta) * r.x * r.y + r.z * sintheta) * v.x;
   result.y += (costheta + (1 - costheta) * r.y * r.y) * v.y;
   result.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * v.z;

   result.z += ((1 - costheta) * r.x * r.z - r.y * sintheta) * v.x;
   result.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * v.y;
   result.z += (costheta + (1 - costheta) * r.z * r.z) * v.z;

   return result;
}


///////////////////////////////// NormalizePlane \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This normalizes a plane from a given frustum.
/////
///////////////////////////////// NormalizePlane \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::normalizePlane(float frustum[6][4], int side) {

	float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] + 
								   frustum[side][B] * frustum[side][B] + 
								   frustum[side][C] * frustum[side][C] );

	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude; 
}

////////////////////////////////// calculateVector2D \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This calculates a vector between 2 points and returns the result
/////
////////////////////////////////// calculateVector2D \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector2 Algebra::calculateVector2D(Vector2 vPoint1, Vector2 vPoint2) {

	Vector2 vVector;		

	vVector.x = vPoint1.x - vPoint2.x;		
	vVector.y = vPoint1.y - vPoint2.y;	

	return vVector;		
}


/*if (carSpeed != 0.0f){
	Vector2 vNewView;
	Vector2 vView;	

	// Get the view Vector3 (The direciton the car is facing)
	vView.x = carView.x - carPosition.x;
	vView.y = carView.y - carPosition.y;
		
	float cosTheta = (float)cos(carRotation * 3.141592 / 180 * time);
	float sinTheta = (float)sin(carRotation * 3.141592 / 180 * time);

	// This simply rotates carView around carPosition by angle amount.
	// This simulates the direction of the car rotating
	vNewView.x = carPosition.x + (vView.x) * cosTheta - (vView.y * sinTheta);
	vNewView.y = carPosition.y + (vView.y) * cosTheta + (vView.x * sinTheta);

	carView.x =  vNewView.x; //carPosition.x + vNewView.x;
	carView.y =  vNewView.y; //carPosition.y + vNewView.y;
	
	rotateAngle += carRotation * (float)time ;
}
*/


////////////////////////////////// calculateVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This calculates a vector between 2 points and returns the result
/////
////////////////////////////////// calculateVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::calculateVector(Vector3 vPoint1, Vector3 vPoint2) {

	Vector3 vVector;		

	vVector.x = vPoint1.x - vPoint2.x;		
	vVector.y = vPoint1.y - vPoint2.y;	
	vVector.z = vPoint1.z - vPoint2.z;	

	return vVector;		
}


////////////////////////////////////// addVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This adds 2 vectors together and returns the result
/////
////////////////////////////////////// addVector \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::addVector(Vector3 vVector1, Vector3 vVector2) {

	Vector3 vResult;
	
	vResult.x = vVector2.x + vVector1.x;
	vResult.y = vVector2.y + vVector1.y;
	vResult.z = vVector2.z + vVector1.z;

	return vResult;		
}


////////////////////////////////// divideVectorByScaler \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This divides a vector by a single number (scalar) and returns the result
/////
////////////////////////////////// divideVectorByScaler \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::divideVectorByScaler(Vector3 vVector1, float Scaler) {

	Vector3 vResult;
	
	vResult.x = vVector1.x / Scaler;	
	vResult.y = vVector1.y / Scaler;
	vResult.z = vVector1.z / Scaler;

	return vResult;	
}


//////////////////////////////////////// cross \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the cross product between 2 vectors
/////
//////////////////////////////////////// cross \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::cross(Vector3 vVector1, Vector3 vVector2) {

	Vector3 vCross;							// The vector to hold the cross product
												
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));												
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));											
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;
}


/////////////////////////////////////// Normalize3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns a normalized vector (A vector of length 1)
/////
/////////////////////////////////////// Normalize3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector3 Algebra::normalize3(Vector3 vNormal) {
	double Magnitude;				

	Magnitude = MAG(vNormal);

	vNormal.x /= (float)Magnitude;	
	vNormal.y /= (float)Magnitude;
	vNormal.z /= (float)Magnitude;	

	return vNormal;
}

/////////////////////////////////////// normalize2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns a normalized vector (A vector of length 1)
/////
/////////////////////////////////////// normalize2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector2 Algebra::normalize2(Vector2 vNormal) {
	double Magnitude;				

	Magnitude = MAG2(vNormal);

	vNormal.x /= (float)Magnitude;	
	vNormal.y /= (float)Magnitude;
	
	return vNormal;
}

/////////////////////////////////////// magnitude \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the magnitude of a normal (or any other vector)
/////
/////////////////////////////////////// magnitude \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
float Algebra::magnitude(Vector3 vNormal) {
	return (float)sqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
}



/////////////////////////////////////// absolute \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the absolute value of the number passed in
/////
/////////////////////////////////////// absolute \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
float Algebra::absolute(float num) {
	if(num < 0)
		return (0 - num);

	return num;
}


/////////////////////////////////////// normal \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the normal of a polygon (The direction the polygon is facing)
/////
/////////////////////////////////////// normal \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\**
Vector3 Algebra::normal(Vector3 vPolygon[]) {
	// Get 2 vectors from the polygon (2 sides)
	Vector3 vVector1 = vPolygon[2] - vPolygon[0];
	Vector3 vVector2 = vPolygon[1] - vPolygon[0];

	Vector3 vNormal = Algebra::cross(vVector1, vVector2);
	vNormal = Algebra::normalize3(vNormal);

	return vNormal;
}


/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the distance between a plane and the origin
/////
/////////////////////////////////// PLANE DISTANCE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*									
float Algebra::planeDistanceFromOrigin(Vector3 Normal, Vector3 Point) {	
	float distance = 0;		// the distance from the plane to the origin

	// Use the plane equation to find the distance )
	// (Ax + By + Cz + D = 0     thus D = -(Ax + By + Cz)
	distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

	return distance;	
}



/////////////////////////////////// dot \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This computers the dot product of 2 vectors
/////
/////////////////////////////////// dot \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
float Algebra::dot(Vector3 vVector1, Vector3 vVector2) {
	return ((vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z));
}


//////////////////////////////// angleBetweenVectors \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	Calculates the radian angle between two vectors (origin based)
/////
//////////////////////////////// angleBetweenVectors \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
double Algebra::angleBetweenVectors(Vector3 Vector1, Vector3 Vector2) {							
	
	double dotProduct = dot(Vector1, Vector2);				
	// Get the product of both of the vectors magnitudes
	double vectorsMagnitude = magnitude(Vector1) * magnitude(Vector2) ;

	// Get the angle in radians between the 2 vectors
	double angle = acos( dotProduct / vectorsMagnitude );

	// This makes sure that the angle is not an indefinate number
	if(_isnan(angle))
		return 0;
	
	return( angle );
}


/////////////////////////////////// getDistance \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This returns the distance between 2 3D points
/////
/////////////////////////////////// getDistance \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
float Algebra::getDistance(Vector3 vPoint1, Vector3 vPoint2) {
	
	float distance = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
						   (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
						   (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );
	return distance;
}
float Algebra::getDistance2D(Vector3 vPoint1, Vector3 vPoint2) {
	
	float distance = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
						    0.0f +
						   (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );
	return distance;
}

float Algebra::getDistance2D2(Vector2 vPoint1, Vector2 vPoint2) {
	
	float distance = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
						    
						   (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) );
	return distance;
}


