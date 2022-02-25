//////////////////////////////////////////////////////////////////////////////
////																			
//// Camera Class - 3rd person, top down, driver, trackside
////
//// Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _CAMERA_H
#define _CAMERA_H

#include "car.h"
#include "glsetup.h"
#include "algebra.h"
#include "quaternion.h"

enum views { 
	DRIVER,
	THIRDPERSON,
	TOPDOWN,
	TRACKSIDE			
};

class Camera {

private:

	Car *car;					// pointer to this cameras car
	float cameraSwingSpeed;		// speed at which the camera swings around the car
	
	Vector3 thirdPerson;		// the third person view vector
	Quaternion::Quat from,		// prev quaternion rotation
					 to,		// next quaternion rotation
					 res;		// resulting SLERP rotation

	Quaternion::Quat testQuat;	// temp quaternion
	
public:

	Camera(Car *car){
		this->car = car;
		// NOTE: this is adjusted with car speed so
		// it should probably be different for each car
		// however it works nicely for the current two
		cameraSwingSpeed = 2300.0f;		
	}

	void setCamera(int view, int players, double time);
	
	// resets the camera to track default position
	void resetCamera(void){
		// set camera X degrees around the car (towards front) at start		
		Quaternion::axisAngleToQuat(&res, 0.0f, 1.0f, 0.0f, 180);
	}

	void doThreeSixtyCamera(double time, int players){

		float SlerpAmount = 8.0f/(this->cameraSwingSpeed) * (float)time;

		// multiply view vector by rotation quaternion
		////////////////////////////////////////////////
		Quaternion::axisAngleToQuat(&testQuat, 0.0f, 1.0f, 0.0f, (float)(TORADIAN(270)));
		thirdPerson.setValues(0.0f, 0.0f, 1.0f);

		// perform SLERP
		//////////////////////////////////////////
		this->from	= res;
		this->to	= testQuat;
		Quaternion::quaternionSlerp(&from, &to, SlerpAmount, &res);

		// create view quaternion
		Quaternion::Quat quatView(0, thirdPerson);
		Quaternion::Quat newView;

		// rotate it using equation v´ = q v (q-1) (where v = [0, v]) 
		newView = Quaternion::quatMultiply (
					  Quaternion::quatMultiply(res, quatView), 
					  Quaternion::quatConjugate(res)
				  );

		// assign rotated view quaternion back to view vector
		thirdPerson.setValues(newView.x, newView.y, newView.z);
		
		float distance = 4.8f;
		float height = 1.85f;

		if (players == 2){
			distance = 5.5f;
			height = 1.9f;
		}

		Vector2 position = car->getPosition();
		// now set the camera position using openGL
		gluLookAt( position.x + (thirdPerson.x * distance),  height,   position.y - (thirdPerson.z* distance),
					position.x,  1.0f, position.y,
					0.0, 1.0,  0.0 );
	}

};

#endif