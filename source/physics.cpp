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

#include "physics.h"

// this shouldnt be here
void Physics::initCarTypes( CarSpec *cartype ) {
/*
	cartype->b			 = 1.5f;	// m	distance from CG to front axle						
	cartype->c			 = 1.5f;	// m    idem to rear axle
	cartype->wheelbase	 = cartype->b + cartype->c;
	cartype->h			 = 2.0;		// m
	cartype->mass		 = 1500;	// kg			
	cartype->inertia	 = 1500;	// kg.m			
	cartype->width		 = 1.5;		// m
	cartype->length		 = 4.0;		// m
	cartype->wheellength = 0.8f;
	cartype->wheelwidth	 = 0.5f;

	cartype->DRAG		 = 5.0f;	// factor for air resistance (drag)
	cartype->RESISTANCE  = 100.0f;	// factor for rolling resistance
	cartype->CA_R		 = -6.20f;	// cornering stiffness
	cartype->CA_F		 = -4.0f;	// cornering stiffness 
	cartype->MAX_GRIP	 = 6.0f;	// maximum (normalised) friction force, =diameter of friction circle
*/

}

void Physics::calculatePhysics(Car *car, float time ) {

	Vector2d acceleration_wc;
	double	 rot_angle;
	double	 sideslip;
	double	 slipanglefront;
	double	 slipanglerear;
	Vector2d force;
	Vector2d resistance;
	Vector2d acceleration;
	double	 torque;
	double	 angular_acceleration;
	double	 sn, cs;
	double	 yawspeed;
	double	 weight;
	Vector2d ftraction;
	Vector2d flatf, flatr;

	sn = (double)sin(car->angle);
	cs = (double)cos(car->angle);
	// SAE convention: x is to the front of the car, y is to the right, z is down
	
	// transform velocity in world reference frame to velocity in car reference frame
	car->velocity.x = (float)( cs * car->velocity_wc.y + sn * car->velocity_wc.x );
	car->velocity.y = (float)(-sn * car->velocity_wc.y + cs * car->velocity_wc.x );

	// Lateral force on wheels
	///////////////////////////////////////////

	// Resulting velocity of the wheels as result of the yaw rate of the car body
	// v = yawrate * r where r is distance of wheel to CG (approx. half wheel base)
	// yawrate (ang.velocity) must be in rad/s
	yawspeed = car->cartype->wheelbase * 0.5 * car->angularvelocity;	

	if( car->velocity.x == 0 )	
		rot_angle = 0;
	else
		rot_angle = atan2( yawspeed, (double)car->velocity.x);

	// Calculate the side slip angle of the car (a.k.a. beta)
	if( car->velocity.x == 0 )
		sideslip = 0;
	else
		sideslip = atan2( car->velocity.y, car->velocity.x);		

	// Calculate slip angles for front and rear wheels (a.k.a. alpha)
	slipanglefront = sideslip + rot_angle - car->steerangle;
	slipanglerear  = sideslip - rot_angle;

	// weight per axle = half car mass times 1G (=9.8m/s^2) 
	weight = car->cartype->mass * 9.8 * 0.5;	
	
	// lateral force on front wheels = (Ca * slip angle) capped to friction circle * load
	flatf.x = 0;
	flatf.y = car->cartype->CA_F * slipanglefront;
	flatf.y = min(car->cartype->MAX_GRIP, flatf.y);
	flatf.y = max(-car->cartype->MAX_GRIP, flatf.y);
	flatf.y *= weight;
	//if(front_slip)
	//	flatf.y *= 0.5;

	// lateral force on rear wheels
	flatr.x = 0;
	flatr.y = car->cartype->CA_R * slipanglerear;
	flatr.y = min(car->cartype->MAX_GRIP, flatr.y);
	flatr.y = max(-car->cartype->MAX_GRIP, flatr.y);
	flatr.y *= weight;
	if(car->rearSlip)
		flatr.y *= 0.5;

	// longtitudinal force on rear wheels - very simple traction model
	ftraction.x = 100 * (car->throttle - car->brake * SGN(car->velocity.x));	  ///////ahhhh not sure
	ftraction.y = 0;
	if(car->rearSlip)
		ftraction.x *= 0.5;

	// Forces and torque on body
	///////////////////////////////////////////
	
	// drag and rolling resistance
	resistance.x = -(car->cartype->RESISTANCE*car->velocity.x + car->cartype->DRAG*car->velocity.x*abs(car->velocity.x) );
	resistance.y = -(car->cartype->RESISTANCE*car->velocity.y + car->cartype->DRAG*car->velocity.y*abs(car->velocity.y) );

	// sum forces
	force.x = ftraction.x + sin(car->steerangle) * flatf.x + flatr.x + resistance.x;
	force.y = ftraction.y + cos(car->steerangle) * flatf.y + flatr.y + resistance.y;	

	// torque on body from lateral forces
	torque = car->cartype->b * flatf.y - car->cartype->c * flatr.y;

	// Acceleration
	///////////////////////////////////////////
	
	// Newton F = m.a, therefore a = F/m
	acceleration.x = force.x/car->cartype->mass;
	acceleration.y = force.y/car->cartype->mass;
	angular_acceleration = torque / car->cartype->inertia;

	// Velocity and position
	///////////////////////////////////////////
	
	// transform acceleration from car reference frame to world reference frame
	acceleration_wc.x =  cs * acceleration.y + sn * acceleration.x;
	acceleration_wc.y = -sn * acceleration.y + cs * acceleration.x;

	// velocity is integrated acceleration
	car->velocity_wc.x += (float)(time * acceleration_wc.x);
	car->velocity_wc.y += (float)(time * acceleration_wc.y);

	// position is integrated velocity
	car->position_wc.x += time * car->velocity_wc.x;
	car->position_wc.y += time * car->velocity_wc.y;


	// Angular velocity and heading
	///////////////////////////////////////////

	// integrate angular acceleration to get angular velocity
	car->angularvelocity += (float)(time * angular_acceleration);

	// integrate angular velocity to get angular orientation
	car->angle += time * car->angularvelocity ;

	car->rearSlip = 0;

}