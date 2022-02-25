//////////////////////////////////////////////////////////////////////////////
////																			
////  Car Class - has 4 wheels, physics, and bounding volumes
////  Inherites from Model to provide mechanisms for mesh storage/rendering
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#include "car.h"
#include "physics.h"

#define MAG2(Normal)(sqrt(Normal.x*Normal.x + Normal.y*Normal.y))

///////////////////////////////////// Car \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  NOTE: Car must be instantiated via this constructor, not the default one
///
///////////////////////////////////// Car \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Car::Car(Track *t) : Model() {

	myTrack = t;
	shadow.enableAlphaTest();
	
	wheelRotateAmount = 0.0f;
	wheelRotateAddition = 0.00f;

	// time based decay rates for damage/turbo etc.
	turboBoost = 100.0f;	// always 100, dont change this
	turboDecay = 0.08f;		// this changes with car

	damage = 0.0f;			// always 0, dont change this
	damageDecay = 0.002f;	// this changes with car
	damageCarScrapeDecay = 0.0005f;
	damageCarHitDecay = 0.01f;

	velocityDecay = 0.5f;

	topSpeed = 0.0f;
	fastestLap = 0.0f;
	topG = 0.0f;

	resetPhysics();

	currentLap = 0;
	vehicleIndex = 0;

	calculatePhysics = false;

	wallCollisionSfx = false;
	carDestroyedSfx = false;
	skidSfx = false;
	hasPlayedTireSfx = 0;
	setFreq = false;
}


//////////////////////////////////// setData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Sets the filename, and physics of car, MUST be called PRIOR to .load()
/////
//////////////////////////////////// setData \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Car::setData(CarSpec *ct, char *fileName, char *shadowMesh, char *wheelMesh){
	
	this->setMeshFile(fileName);
	this->shadow.setMeshFile(shadowMesh);
	
	//for (int i =0; i< 4; i++){
		this->myWheel.setWheelMesh(wheelMesh);
	//}

    cartype = ct;
	// giva jump start to the calculations
	Physics::calculatePhysics(this, (float)200/physicsDelta);
	this->velocity.x = 0;
	this->velocity.y = 0;
}

//////////////////////////////// resetPhysics \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Resets physics variables
/////
//////////////////////////////// resetPhysics \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Car::resetPhysics(void){

	wheelRotateAmount = 0.0f;
	wheelRotateAddition = 1.00f;

	turboBoost = 100.0f;
	damage = 0.0f;
	
	this->position_wc.x = 999;
	this->position_wc.y = 999;
	this->velocity_wc.x = 0;
	this->velocity_wc.y = 0;
	this->velocity.x = 0;
	this->velocity.y = 0;
	this->angle = 0;
	this->angularvelocity = 0;
	this->steerangle = 0.001f;
	this->throttle = 0;
	this->brake = 0;
	this->rearSlip = 0;
	physicsDelta = 300;	

	topSpeed = 0.0f;
	fastestLap = 0.0f;
	topG = 0.0f;
}


//////////////////////////////////// load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Loads the 4 wheels into memory, then loads the car into memory.
/////
//////////////////////////////////// load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Car::load() {
	//for (int i = 0; i < 4; i++){
		if (this->drawBB)
			myWheel.drawBB = true;
		myWheel.load();
	//}

	shadow.load();
	Model::load();

	currentLap = 0;
	return true;
}


//////////////////////////////////// Render \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////  Renders the car.  Oh yeah, wheels too!
/////
//////////////////////////////////// Render \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Car::Render(int vehicleIndex) {
	
	// position the car in world coordinates
	glTranslatef(this->position_wc.x, 0.025f, this->position_wc.y);
	glRotatef((float)(TODEGREE(this->angle)), 0.0f, 1.0f, 0.0f);

	// render shadow
	glPushMatrix();
	
		// apply physics to car body (roll, pitch etc.)
		//if (this->angularvelocity < -0.1 ||this->angularvelocity > 0.1)
		glRotatef(this->angularvelocity*3, 0.0f, 0.0f, 1.0f);
		glRotatef(-this->velocity.y/3, 0.0f, 0.0f, 1.0f); // 3

		// render the car body
		render(NULL, (float)(TODEGREE(this->angle)), this->position_wc);	
		
		// apply physics to car wheels
		glRotatef(-this->angularvelocity*3, 0.0f, 0.0f, 1.0f);

		if (vehicleIndex == 0){
			// Draw front left wheel
			glPushMatrix();
				glTranslatef(-0.50f, 0.22f, 0.75f);
				myWheel.turn((float)(this->steerangle * 180.0f/PI));		
				myWheel.rotate(wheelRotateAmount);
			glPopMatrix();

			// Draw front right wheel
			glPushMatrix();
				glTranslatef(0.51f, 0.22f, 0.75f);
				glRotatef(180, 0.0f, 0.0f, 1.0f);
				myWheel.turn((float)(-this->steerangle * 180.0f/PI));
				myWheel.rotate(-wheelRotateAmount);	
			glPopMatrix();

			// Draw rear left wheel
			glPushMatrix();
				glTranslatef(-0.5f,0.25f, -0.85f);
				myWheel.rotate(wheelRotateAmount);
			glPopMatrix();
			
			// Draw rear right wheel
			glPushMatrix();
				glTranslatef(0.5f, 0.25f, -0.85f);
				glRotatef(180, 0.0f, 0.0f, 1.0f);
				myWheel.rotate(-wheelRotateAmount);
			glPopMatrix();

		}
		else{
			// Draw front right wheel
			glPushMatrix();
				glTranslatef(-0.6f, 0.25f, 0.7f);
				myWheel.turn((float)(this->steerangle * 180.0f/PI));		
				myWheel.rotate(wheelRotateAmount);
			glPopMatrix();

			// Draw front left wheel
			glPushMatrix();
				glTranslatef(0.62f, 0.24f, 0.7f);
				glRotatef(180, 0.0f, 0.0f, 1.0f);
				myWheel.turn((float)(-this->steerangle * 180.0f/PI));
				myWheel.rotate(-wheelRotateAmount);	
			glPopMatrix();

			// Draw rear right wheel
			glPushMatrix();
				glTranslatef(-0.61f,0.23f, -0.7f);
				myWheel.rotate(wheelRotateAmount);
			glPopMatrix();
			
			// Draw rear left wheel
			glPushMatrix();
				glTranslatef(0.6f, 0.25f, -0.7f);
				glRotatef(180, 0.0f, 0.0f, 1.0f);
				myWheel.rotate(-wheelRotateAmount);
			glPopMatrix();
		}
			
	glPopMatrix();

	glTranslatef(0.0f, -0.03f, 0.0f);
	shadow.render(NULL,NULL, Vector2(0,0));	
}



///////////////////////////////// positionCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function sets the cars position on the track
/////
///////////////////////////////// positionCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Car::positionCar(float positionX, float positionY, float viewX, float viewY, float angle, int vehIndex) {

	turboBoost = 100.0f;
	damage = 0.0f;
	position_wc.x = positionX;
	position_wc.y = positionY;

	this->angle = angle;

	if (vehIndex == 0){
		// set sphere
		boundingSphere.center.x = positionX;
		boundingSphere.center.y = 0.0f;
		boundingSphere.center.z = positionY;
		boundingSphere.radius = 2.0f;

		// set bounding ellipsoid x,y,z radius
		ellipsoid.setRadius(Vector3(0.9f, 0.1f, 0.0f),
							Vector3(0.0f, 0.9f, 0.0f),
							Vector3(0.0f, 0.1f, 1.451f));

		// set bounding box properties
		boundingBox.setCenter(position_wc);
		boundingBox.setAxis(Vector3(1.0f, 0.0f, 0.0f), 
							Vector3(0.0f, 1.0f, 0.0f), 
							Vector3(0.0f, 0.0f, 1.0f));

		//                        L      H      W
		boundingBox.setHalfSizes(0.55f, 0.5f, 1.42f);
		boundingBox.setVerts();
	}
	else{
		// set sphere
		boundingSphere.center.x = positionX;
		boundingSphere.center.y = 0.0f;
		boundingSphere.center.z = positionY;
		boundingSphere.radius = 2.0f;

		// set bounding ellipsoid x,y,z radius
		ellipsoid.setRadius(Vector3(0.9f, 0.1f, 0.0f),    // side
							Vector3(0.0f, 0.9f, 0.0f),	  // up
							Vector3(0.0f, 0.1f, 1.451f)); // front

		// set bounding box properties
		boundingBox.setCenter(position_wc);
		boundingBox.setAxis(Vector3(1.0f, 0.0f, 0.0f), 
							Vector3(0.0f, 1.0f, 0.0f), 
							Vector3(0.0f, 0.0f, 1.0f));

		//                        L      H      W
		boundingBox.setHalfSizes(0.55f, 0.5f, 1.42f);
		boundingBox.setVerts();
	}
	
	TRACE("\nPositioning Car...\nBounding Volumes set. Track Position set.\n\n");
}



///////////////////////////////// rotateCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///
///  Turns the car left or right
///
///////////////////////////////// rotateCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Car::rotateCarNegative(double time) {
	steerangle -= (float)(PI/2048.0 * time);
}

void Car::rotateCarPositive(double time) {
	steerangle += (float)(PI/2048.0 * time);
}


///////////////////////////////// moveCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This will calculate physics and move the car, also check collision
/////
///////////////////////////////// moveCar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Car::moveCar(double time, int players, int c, Sound::FModSound *sound, int playerIndex, float &debug) {

	prevPosition_wc = position_wc;
	prevAngle = angle;
	prevEllipsoid = ellipsoid;

	// play sound effects when applicable
	//////////////////////////////////////////////
	if (this->getDamage() >= 100.0){
		if (!carDestroyedSfx){
			sound->playSound("RCexplode");
			carDestroyedSfx = true;
		}		
	}
	sound->setFrequency("RCengine", engineSfxID, (int)(8500 + (velocity.x*120)));
	if (velocity.y > 10.0f || velocity.y < -10.0f){
		if (!skidSfx){
			if (hasPlayedTireSfx == 0){
				sound->playLoopingSound("RCskid", tireSfxID);
				hasPlayedTireSfx++;
			}
			else{
				sound->pauseLoopingSound("RCskid", tireSfxID);
			}
			skidSfx = true;
		}
	}
	else{
		if (skidSfx){
			sound->pauseLoopingSound("RCskid", tireSfxID);
		}
		skidSfx = false;
	}

	
	// Calculate Physics
	////////////////////////////////////////////////
    Vector3 originPosition(0.0f, 0.1f, 0.0f);
	Vector3 newOriginPos(0.0f, 0.1f, 0.0f);
	Vector3 newView;
	prevVelocity = velocity;

	float calculatePhysicsThreshold;
	float angularVelocityThreshold;
	float velocityIncrease;

	// BEGIN physics kludge ////// haha this is due in a day! //////////////
		if (players == 1){
			calculatePhysicsThreshold = 1.2f;
			angularVelocityThreshold = 0.3f;
			velocityIncrease = 1.5;
		}
		else{
			calculatePhysicsThreshold = 1.2f;
			angularVelocityThreshold = 0.3f;
			velocityIncrease = 1.5;
		}

		if (abs(MAG2(velocity_wc)) > calculatePhysicsThreshold || abs(this->angularvelocity) > angularVelocityThreshold){			
			calculatePhysics = true;					
			// rotate the car wheels (delta_angle = speed / wheel radius)
			wheelRotateAmount += ((velocity.x - calculatePhysicsThreshold) / 3.0f) * (float)time; 
		}
		else{
			calculatePhysics = false;
		/*	

			Vector3 carview = ellipsoid.newZradius;
			Vector3 carpos;
					carpos.x = position_wc.x;
					carpos.z = position_wc.y;

			Vector3 view = Algebra::calculateVector(carpos, carview);
			//ellipsoid.ellipsoidSpace(view);
			view = Algebra::rotateVector(view, TODEGREE(this->angle), Vector3(0.0f, 1.0f, 0.0f));
			
			Vector3 velocityview;
					velocityview.x = velocity_wc.x;
					velocityview.y = 0.1f;
					velocityview.z = velocity_wc.y;

			velocity_wc.x = velocityview.x;
			velocity_wc.y = velocityview.z;
			*/
		}
		if (calculatePhysics){
			// update car physics (position, velocity etc)
			Physics::calculatePhysics(this, (float)time/physicsDelta);
		}
		else{
			if(playerIndex == 1){
				if (GetKeyState(VK_UP)& 0x80 || GetKeyState(VK_RCONTROL)& 0x80 || abs(this->angularvelocity) > angularVelocityThreshold){
                    
					velocity_wc = velocity_wc * velocityIncrease;
				}
			}
			else{
				if (GetKeyState('W')& 0x80 || GetKeyState(VK_LSHIFT)& 0x80 || abs(this->angularvelocity) > angularVelocityThreshold){

					velocity_wc = velocity_wc * velocityIncrease;
				}
			}
		}
	// END physics kludge /////////////////////////////////////////////////////////
		

	// update bounding box
	boundingBox.setCenter(position_wc);
	boundingBox.rotationMatrix.SetTranslationPart(Vector3(position_wc.x, 0.0f, position_wc.y));
	boundingBox.rotationMatrix.SetRotationPartEuler(0.0, TODEGREE(angle), 0.0);
	boundingBox.updateAxis();

	// update bounding ellipsoid
	ellipsoid.setCenter(position_wc);
	ellipsoid.transformation.SetRotationPartEuler(0.0, TODEGREE(angle), 0.0);
	ellipsoid.transformation.SetTranslationPart(Vector3(position_wc.x, 0.0f, position_wc.y));	
	ellipsoid.updateRadius();

	// update bounding sphere
	boundingSphere.setCenter(position_wc);


	// check car to car collision
	///////////////////////////////////////////////////////////////////////
	if (players == 2){
		// only if the cars are close to each other
		if (boundingSphere.sphereCollide(opponent->getBoundingSphere())){
			
			// build polygons out of opponents bounding box
			BoundingVolume::OBB3D opponentOBB = opponent->getOBB();
			opponentOBB.updateVerts();
			opponentPolys[0].setVerts(opponentOBB.newVerts[0],opponentOBB.newVerts[1],opponentOBB.newVerts[2]);
			opponentPolys[1].setVerts(opponentOBB.newVerts[1],opponentOBB.newVerts[2],opponentOBB.newVerts[3]);
			opponentPolys[2].setVerts(opponentOBB.newVerts[4],opponentOBB.newVerts[5],opponentOBB.newVerts[6]);
			opponentPolys[3].setVerts(opponentOBB.newVerts[5],opponentOBB.newVerts[6],opponentOBB.newVerts[7]);
			opponentPolys[4].setVerts(opponentOBB.newVerts[1],opponentOBB.newVerts[3],opponentOBB.newVerts[7]);
			opponentPolys[5].setVerts(opponentOBB.newVerts[1],opponentOBB.newVerts[5],opponentOBB.newVerts[7]);
			opponentPolys[6].setVerts(opponentOBB.newVerts[0],opponentOBB.newVerts[2],opponentOBB.newVerts[4]);
			opponentPolys[7].setVerts(opponentOBB.newVerts[2],opponentOBB.newVerts[4],opponentOBB.newVerts[6]);

			// loop through all eight polygons of opponents bounding box
			///////////////////////////////////////////////////////////////
			bool carHit = false; int polyCounter = 0;

			for (int i = 0; i < 8; i++){
				// orient to ellipsoid space
				ellipsoid.ellipsoidSpace(opponentPolys[i].getPoly(),3);	
				// orient to unit sphere space
				ellipsoid.sphereSpace(opponentPolys[i].getPoly(),3);
				bool nothing;
				// do simple unit sphere collision detection		
				if(Collision::spherePolygonCollision(opponentPolys[i].getPoly(), opponentPolys[i].getPoly(), originPosition, 3, true, newOriginPos, newView, 1.0f, Vector3(0.0f, 0.0f, 0.0f), nothing)){
					carHit = true;
					polyCounter = i;
					ellipsoid.inverseSphereSpace(opponentPolys[i].getPoly(),3);
					ellipsoid.inverseEllipsoidSpace(opponentPolys[i].getPoly(),3);	
				}
				originPosition = newOriginPos;				
			}

			// set current position to new position (un-orient back to ellipsoid space)
			ellipsoid.inverseSphereSpace(originPosition);
			ellipsoid.inverseEllipsoidSpace(originPosition);
			position_wc.x = originPosition.x;
			position_wc.y = originPosition.z;
			
			// Code for car to car collisin reaction
			//////////////////////////////////////////////////
			if (carHit){

				Vector3 a = Algebra::calculateVector(this->getViewVector(),originPosition);
				Vector3 b = Algebra::normal(opponentPolys[i].getPoly());
				a = Algebra::calculateVector(originPosition, getViewVector());
				b = Algebra::calculateVector(opponent->ellipsoid.center, opponent->getViewVector());
				float angleBetweenCars = (float)(TODEGREE(Algebra::angleBetweenVectors(a,b)));
				
				// check to see if the car hit the sides of the other car	
				if (polyCounter >= 4 && polyCounter <=7){
					// the car is therefore colliding straight on, so slow it down
					if ((angleBetweenCars >= 70.0f && angleBetweenCars <= 120.0f)){// ||
						//(angleBetweenCars >= 0.0f && angleBetweenCars <= 5.0f)){
						velocity_wc = velocity_wc / 1.1f;
						if (damage < 100)
							damage += float(damageCarHitDecay * abs(velocity.x) * time);
					}
					else{ // this car must be scraping	
						if (damage < 100)
							damage += float(damageCarScrapeDecay * abs(velocity.x) * time);					
					}		
				}
				// the car hit the front or the back of the other car
				else{
					velocity_wc = velocity_wc / 1.1f;
					if (damage <= 100){
						damage += float(damageCarHitDecay * velocity.x * time);
					}
				}
			} // if (carHit)
		} // if (bounding boxes are close to each other)
	}

	
	bool wallHit = false;
	Vector3 myPosition(position_wc.x, 0.1f, position_wc.y);
	Vector3 myPoly[3];
	Vector3 uPoly[3];  // un translated poly
	Vector3 overShootTestPoly[3];
	bool wallJump = false;
	bool wallJump2 = false;
	
	myTrack->frustrum.updateFrustrum();
	c3dsModel *model = myTrack->getModel();
	modelObject *trackObjects = myTrack->getObjects();

	Vector3 prevPosition(prevPosition_wc.x, 0.1f, prevPosition_wc.y);

	// translate to origin
	ellipsoid.ellipsoidSpace(prevPosition);
	//debug = Algebra::magnitude(prevPosition);

	// check wall collision for each object in the scene
	//////////////////////////////////////////////////////////
	for (int i = 0; i < model->numOfObjects; i++) {
		c3dsObject pObject = model->pObject[i];

		// if its really far away the car cant possibly hit it
		if (abs(Algebra::getDistance2D(trackObjects[i].boundingBox.center, myPosition)) < 150.0f){ //100 TODO

			// apparently its close, but is the object even visible?
			if (myTrack->frustrum.containsAABB(trackObjects[i].boundingBox) != OUTSIDEPLANE){

				// finally a potential collider, lets do it!
				for(int j = 0; j < pObject.numOfFaces; j++){

					for(int whichVertex = 0; whichVertex < 3; whichVertex++) {
						myPoly[whichVertex] = pObject.pVerts[pObject.pFaces[j].vertIndex[whichVertex]];						
					}

					// TODO: I dont think this is right, but it works for now :P
					Vector2 temp = Algebra::normalize2(getView());	
					// orient to ellipsoid space
					ellipsoid.ellipsoidSpace(myPoly,3);	
					
					for (int i =0; i< 3; i++){
						uPoly[i] = myPoly[i];
					}

					// orient to unit sphere space
					ellipsoid.sphereSpace(myPoly,3);
								
					// do unit sphere vs poly test
					if(Collision::spherePolygonCollision(myPoly, uPoly, originPosition, 
														 3, false, newOriginPos, newView, 
														 1.0f, prevPosition, wallJump))
					{
						wallHit = true;
					}
					
					if (wallJump){
						wallJump2 = true;
					}

					originPosition.x = newOriginPos.x;
					originPosition.z = newOriginPos.z;
				}
			}
		}
	}

	ellipsoid.inverseSphereSpace(originPosition);
	ellipsoid.inverseEllipsoidSpace(originPosition);
	
	if (wallJump2){        
		// we are going so amazingly fast that the car has just
		// skipped the wall entirely, stop the car!
		// note, this is very very very rare, but just in case.
		ellipsoid.inverseEllipsoidSpace(prevPosition);
		position_wc.x = prevPosition.x;
		position_wc.y = prevPosition.z;
		this->velocity_wc.x = 0.001f;
		this->velocity_wc.y = 0.001f;
		this->velocity.x = 0.001f;
		this->velocity.y = 0.001f;	
		wallHit = false;
		if (damage <=100)
			damage = 100.01f;
	}else{
        position_wc.x = originPosition.x;
		position_wc.y = originPosition.z;
	}

	// we have a collision with a wall
	////////////////////////////////////
	if (wallHit){
		if (velocity.x > 5.0){ // slow down
			//this->throttle = throttle/2;
			this->velocity_wc = velocity_wc/1.01f;//(velocityDecay * time);						
		}
		if (!wallCollisionSfx){ // play sound
			sound->playSound("hardcollision");
			wallCollisionSfx = true;
		}
		if (damage <= 100) { // apply damage
			damage += float(damageDecay * abs(velocity.x) * time);
		}
	}
	else{
		wallCollisionSfx = false;
	}

	// update bounding ellipsoid
	ellipsoid.setCenter(position_wc);
	ellipsoid.transformation.SetRotationPartEuler(0.0, TODEGREE(angle), 0.0);
	ellipsoid.transformation.SetTranslationPart(Vector3(position_wc.x, 0.0f, position_wc.y));	
	ellipsoid.updateRadius();

	// update top speeds for report screen
	if (this->velocity.x > topSpeed)
		topSpeed = velocity.x;	
	if (this->velocity.y > topG)
        topG = velocity.y;

	
	// Check lap markers (sectors and start/finish)
	/////////////////////////////////////////////////
	int currentMarker;
	currentMarker = myTrack->checkLapMarkers(&position_wc, &ellipsoid.newZradius, 1);
	return currentMarker;
}



///////////////////////////////// getPosition \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	Accessor functions
/////
///////////////////////////////// getPosition \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Vector2 Car::getPosition(void){
	return position_wc;
}

Vector2 Car::getView(void){
	return velocity_wc;
}

Vector3 Car::getViewVector(void){
	return ellipsoid.newZradius;
}

float Car::getVelocity(void){
	return velocity.x;
}

void Car::setOpponent(Car *opponentCar){
    opponent = opponentCar;
}

float Car::getDamage(void){
    return damage;
}

int Car::getCurrentLap(void){
	return currentLap;
}

void Car::setCurrentLap(int cl){
	currentLap = cl;
}

float Car::getTurbo(void){
    return turboBoost;
}

void Car::setTurbo(float turbo){
	turboBoost = turbo;
}

void Car::setDamage(float damage){
	this->damage = damage;
}

float Car::getTurboDecay(void){
	return turboDecay;
}

BoundingVolume::OBB3D & Car::getOBB(void){
	return boundingBox;
}

BoundingVolume::BoundingSphere * Car::getBoundingSphere(void){
	return &boundingSphere;
}

float Car::getTopSpeed(){
	return topSpeed;	
}

void Car::getTopSpeed(float t){
	topSpeed = t;
}

float Car::getFastestLap(){
	return fastestLap;	
}

void Car::setFastestLap(float l){
	fastestLap = l;
}

float Car::getTopG(){
	return topG;	
}

void Car::setTopSpeed(float g){
	topG = g;
}