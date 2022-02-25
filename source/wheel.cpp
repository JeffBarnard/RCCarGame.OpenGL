//////////////////////////////////////////////////////////////////////////////
////																			
//// Wheel Class, inherits from model to allow loading/rendering.
//// Contains methods to turn and rotate the wheels.
////
////  Jeff Barnard  2003
////
//////////////////////////////////////////////////////////////////////////////

#include "wheel.h"

GLfloat Wheel::rotate(GLfloat amount){
	glRotatef(amount, 1.0f, 0.0f, 0.0f);

	render(NULL,NULL, Vector2(0, 0));

    return 1.0f;
}

GLfloat Wheel::turn(GLfloat amount){
	
	glRotatef(amount, 0.0f, 1.0f, 0.0f);

	return 1.0f;
}