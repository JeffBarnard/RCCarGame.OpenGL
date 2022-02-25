//////////////////////////////////////////////////////////////////////////////
////																			
//// Wheel Class, inherits from model to allow loading/rendering.
//// Contains methods to turn and rotate the wheels.
////
////  Jeff Barnard  2003
////
//////////////////////////////////////////////////////////////////////////////


// TODO:  each instance of the wheel model should all point to the same vertices
//        stored in memory, 4 independent instances is wastefull on memory
//		  I'm not really sure if i'm doing this, I'm tired now. Check car constructor.

#ifndef _WHEEL_H
#define _WHEEL_H

#include "model.h"

class Wheel : public Model {

private:

	GLfloat maxTurnAngle;
	GLfloat maxRotateAmount;
	
public:

	Wheel(){
		// this is lame, c++ sucks, someone tell me how to call
		// a non-default constructor on an array of objects
		// from car.cpp.  bah.
	}

	Wheel(char *f): Model(){
		this->fileName = f;
		// We should be using this, not the default.			
	}

	void setWheelMesh(char *s){
		this->fileName = s;
	}

	GLfloat rotate(GLfloat);

	GLfloat turn(GLfloat);
};


#endif