//////////////////////////////////////////////////////////////////////////////
////																			
////  Car Class - has 4 wheels, physics, and bounding volumes
////  Inherites from Model to provide mechanisms for mesh storage/rendering
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _CAR_H
#define _CAR_H

#include "track.h"
#include "wheel.h"
#include "collision.h"
#include "bb.h"
#include "FModSound.h"

struct CarSpec {
public:
	float wheelbase;			// wheelbase in m
	float b;					// in m, distance from center to front axle
	float c;					// in m, distance from center to rear axle
	float h;					// in m, height of CM from ground
	float mass;					// in kg
	float inertia;				// in kg.m
	float length,width;			// in m
	float wheellength,wheelwidth;

	float DRAG;					// factor for air resistance (drag)
	float RESISTANCE;			// factor for rolling resistance
	float CA_R;					// cornering stiffness
	float CA_F;					// cornering stiffness 
	float MAX_GRIP;				// maximum (normalised) friction force, =diameter of friction circle
};


class Car : public Model {

private:
	
	BoundingVolume::OBB3D boundingBox;
	BoundingVolume::BoundingSphere boundingSphere;
	BoundingVolume::Ellipsoid ellipsoid;
	BoundingVolume::Ellipsoid prevEllipsoid;

	Model   shadow;
	Track   *myTrack;				// pointer to track object, so we can test it for collisions
	Wheel	myWheel;				// Wheel positions
									//    /|\	
	float wheelRotateAmount;		//   / | \*
	float wheelRotateAddition;		//   0 | 1
									//   2 | 3
	Car *opponent;
	Algebra::Polygon opponentPolys[8];
	
	float topSpeed;			// keep track of fastest top speed just for fun
	float fastestLap;		// keep track of fastest lap just for fun
	float topG;				// keep track of maximum corning G's just for fun
	bool calculatePhysics;  // shall we calculate physics?

	float turboBoost;
	float turboDecay;
	float damage;
	float damageDecay;
	float damageCarScrapeDecay;
	float damageCarHitDecay;
	float velocityDecay; // rate at which a wall lowers velocity
	UINT  physicsDelta;		
	UINT  currentLap;

	// sound members
	bool wallCollisionSfx;
	bool skidSfx;
	bool carDestroyedSfx;
	int engineSfxID;
	int tireSfxID;
	int hasPlayedTireSfx;
	int engineFreq;
	bool setFreq;

public:

	/////////////////////////////////////////////////////////////////////
	/// Physics variables (public because used for so many calculations)
	/////////////////////////////////////////////////////////////////////
	CarSpec	*cartype;				// pointer to car data	
	
	Vector2 carView;				// the cars view vector
	Vector2	position_wc;			// position of car center in world coordinates
	Vector2	prevPosition_wc;
	Vector2	velocity_wc;			// velocity vector of car in world coordinates
	Vector2 prevVelocity;			// the previous velocity of the car
	Vector2	velocity;				// velocity vector in local coordinates	
	
	float	angle;					// angle of car orientation (rads)
	float	prevAngle;
	float	angularvelocity;
	float	steerangle;
	float	throttle;
	float	brake;
	int		rearSlip;

public:

	//////////////////////////////////////////////////////
	/// Accessor methods
	//////////////////////////////////////////////////////
	Vector2 getPosition(void);
	Vector2 getView(void);
	Vector3 getViewVector(void);
	float getVelocity(void);
	float getDamage(void);
	float getTurbo(void);
	void setTurbo(float turbo);
	void setDamage(float damage);
	float getTurboDecay(void);
	int getCurrentLap(void);
	void setCurrentLap(int);
	float getTopSpeed();
	void getTopSpeed(float t);
	float getFastestLap();
	void setFastestLap(float l);
	float getTopG();
	void setTopSpeed(float g);

	BoundingVolume::OBB3D & getOBB(void);
	BoundingVolume::BoundingSphere * getBoundingSphere(void);
	void setOpponent(Car *opponentCar);
	
	//////////////////////////////////////////////////////
	/// Public methods
	//////////////////////////////////////////////////////	
	Car(Track *);
	void setData(CarSpec *ct, char *fileName, char *shadowMesh, char *wheelMesh);

	void positionCar(float positionX, float positionY, float viewX, float viewY, float angle, int vehicleIndex);
	
	void rotateCarNegative(double time);
	void rotateCarPositive(double time);

	void resetPhysics(void);
	int moveCar(double time, int players, int c, Sound::FModSound *sound, int playerIndex, float &debug);
    
	bool load();
	void Render(int vehicleIndex); // do not call .render(), call .Render()

	void setEngineSound(int sfx){		
		this->engineSfxID = sfx;
	}
	void setTireSound(int tsfx){
		this->hasPlayedTireSfx = 0;
		this->tireSfxID = tsfx;
	}

	friend void calculatePhysics(Car *car, float time);

	int vehicleIndex;  // which car are we? sloppy but needs to be done at this point
	void setVehicleIndex(int i){
		this->vehicleIndex = i;
	}

	BoundingVolume::Ellipsoid* getEllipsoid(void){
		return &ellipsoid;
	}

};


#endif  // _CAR_H
