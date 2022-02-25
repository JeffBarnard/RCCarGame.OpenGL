//////////////////////////////////////////////////////////////////////////////
////																			
////  Camera Class
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

//  February 18 2004
//
//		Implement view rotation using Quaternion class
//		Angle can simply be car orientation (car->angle)
//		Do not use SLERP, do not pass GO.
//
//		UPDATE: done
//
//
//  February 24 2004
//
//		Implement rotational third person camera using
//		quaternions and SLERP.
//
//		UPDATE: done, requires tweaking though


#include "camera.h"

void Camera::setCamera(int view, int players, double time){
	//gluLookAt( EyePosition, ViewPosition, UpVector)
	
	float speed = abs(car->getVelocity());
	Vector2 position = car->getPosition();
	Vector3 carViewVector = car->getViewVector();

	///////////////////////////////////////////////////
	// Top down view (overhead)
	///////////////////////////////////////////////////
	if (view == TOPDOWN){
		gluLookAt( position.x, 12.0f + 0.5*speed, position.y,
				   position.x, 1.0f             , position.y,
				   0.0       , 0.0              , 1.0);
	}

	///////////////////////////////////////////////////
	// Track side view (starting line view)
	///////////////////////////////////////////////////
	else if (view == TRACKSIDE){
		gluLookAt( 11, 4, 30,
				   position.x, 1, position.y,	
				   0.0, 1.0, 0.0);
	}

	///////////////////////////////////////////////////
	// Driver view (first person)
	///////////////////////////////////////////////////
	else if (view == DRIVER){
		gluLookAt( position.x - ((position.x - carViewVector.x)/5), 
				   0.8f,  
				   position.y - ((position.y - carViewVector.z)/5),
				   
				   carViewVector.x, 
				   0.8f, 
				   carViewVector.z,

				   0.0, 1.0, 0.0);
	}

	///////////////////////////////////////////////////
	// Third person view (trailing view)
	///////////////////////////////////////////////////
	else if(view == THIRDPERSON){

		float SlerpAmount = (abs(car->velocity.x)/this->cameraSwingSpeed) * (float)time;

		// multiply view vector by rotation quaternion
		////////////////////////////////////////////////
		Quaternion::axisAngleToQuat(&testQuat, 0.0f, 1.0f, 0.0f, -car->angle);
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
		
		float distance = 0;
		float height = 0.0f;
		
		if (players == 1){
			distance = 4.8f;
			height = 1.85f;
		}
		else{
			distance = 5.5f;
			height = 1.9f;
		}

		// now set the camera position using openGL
		gluLookAt( position.x + (thirdPerson.x * distance),  height,   position.y - (thirdPerson.z* distance),
					position.x,  1.0f, position.y,
					0.0, 1.0,  0.0 );		
	}
}
