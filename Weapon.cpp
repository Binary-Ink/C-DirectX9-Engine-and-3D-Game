#include "Engine.h"

using namespace Batman;
//constructor 
Weapon::Weapon() {
	visibleModel = NULL; 
	weaponCollision = NULL; 
	weaponState = 0; 
	weaponType = 0; 
	forwardVector = Vector3(0, 0, 0);
	goalPos = Vector3(0, 0, 0); 
	currentVelocity = 0; 
	rotationX = 0; 
	spawnPoint = Vector3(0, 0, 0);
	effectiveDistance = Vector3(0, 0, 0);
	hVel = 0; 
	yVel = 0;
	timeElapsed = 0; 
	//initialisation init method
}

void Weapon::Init(Vector3 direction, int type, Vector3 spawn) {
	//initial states
	weaponType = type; 
	forwardVector = direction; 
	weaponState = WEAPON_INAIR_STATE; 
	spawnPoint = spawn; 

	//check weapon type
	//model and collision mesh loaded
	switch (weaponType) {

	case WEAPON_SHURIKEN: {
		//effective 50 pts from spawn
		effectiveDistance = spawnPoint + (direction * 50); 
		//small flat cube
		visibleModel = new Mesh(); 
		visibleModel->createCube(0.25, 0.1, 0.25);

		//angle calcs 
		//vector for y axis dir
		Vector3 vecY(0, 1, 0); 
		//get dot product of FV
		float scalar2 = Math::DotProduct(forwardVector, vecY); 
		//use cos ^-1 to get rot angle
		float Yangle = acos(scalar2); 

		//convert to degrees and adjust
		Yangle = D3DXToDegree(Yangle); 
		Yangle += 90; 
		Yangle *= -1; 
		//END ANGLE

		//set rot
		visibleModel->setRotation(0, Yangle, 0); 
		//spawn in front of character
		visibleModel->setPosition(spawn + (forwardVector * 2)); 
		//update matrices
		visibleModel->Transform(); 
		//create collision cube with sams props
		weaponCollision = new Mesh(); 
		weaponCollision->createCube(0.25, 0.1, 0.25); 
		weaponCollision->setPosition(spawn + (forwardVector * 2));
		weaponCollision->Transform();
		//set velocity to 30
		currentVelocity = 20;
	
	} break; 

	case WEAPON_DART:
	{
		effectiveDistance = spawnPoint + (direction * 100); 
		//crate long thin cube
		visibleModel = new Mesh(); 
		visibleModel->createCube(1, 0, 0); 
	//vector for x axis
		Vector3 vecX(1, 0, 0); 
	//don't consider Y
		Vector3 hDirection = forwardVector; 
		hDirection = Math::Normal(hDirection); 
		//get dot product
		float scalar = Math::DotProduct(hDirection, vecX); 
		//use cos^-1
		float rotationValue = acos(scalar); 

		//check which direction on Z 
		if (forwardVector.z > 0) {
			//if going positive on Z recalc rot value
			float toAdd = PI - rotationValue; 
			rotationValue = PI + toAdd; 
		}
		//convert to degrees
		rotationValue = D3DXToDegree(rotationValue);

		//angle calcs
		Vector3 vecY(0, 1, 0);
		//get dot product
		float scalar2 = Math::DotProduct(forwardVector, vecY);
		//use cos^-1 to get rot angle
		float Zangle = acos(scalar2); 

		//convert and readjust
		Zangle = D3DXToDegree(Zangle);
		Zangle += 90; 
		Zangle *= -1; 
		//END ANGLE

		//update models rot
		visibleModel->setRotation(rotationValue, 0, Zangle);
		visibleModel->Transform();

		//create collision cube with sams props
		weaponCollision = new Mesh();
		weaponCollision->createCube(0.5, 0.1, 0.1); 
		weaponCollision->setPosition(spawn + (forwardVector * 2));
		weaponCollision->Transform(); 
		//set velocity at 50
		currentVelocity = 50; 

	} break;
	case WEAPON_BOMB: {
		//create small cube
		visibleModel = new Mesh();
		visibleModel->createCube(0.4, 0.4, 0.4); 
		visibleModel->setPosition(spawn + (forwardVector * 2)); 
		visibleModel->Transform(); 
		//create collision
			weaponCollision = new Mesh();
			weaponCollision->createCube(0.4, 0.4, 0.4);
			weaponCollision->setPosition(spawn + (forwardVector * 2));
			weaponCollision->Transform();

			//angle calcs for SUVAT
			Vector3 vecY(0, 1, 0);
			//get dot product
			float scalar = Math::DotProduct(forwardVector, vecY); 
			//use cos^-1 to get rot angle
			float angle = acos(scalar); 

			//convert to degrees and adjust
			angle = D3DXToDegree(angle); 
			angle -= 90; 
			angle *= -1; 
			//end ANGLE

			//get rad value of angle
			angle = Math::ToRadians(angle); 

			//calculate Y and X velocities
			currentVelocity = 5; 
			yVel = sin(angle) * currentVelocity; 
			hVel = cos(angle)*currentVelocity; 

	}
	  break; 
	}
}

void Weapon::Update(float deltaTime) {
	//update x rot, bounded by 360
	rotationX += 0.5;
	if (rotationX == 360) {
		rotationX = 0;
	}
	//vector for current position
	Vector3 currentPos = visibleModel->getPosition(); 
	//float current velocity 
	float vel = currentVelocity; 

	//check weapon type
	//update mesh and tex
	switch (weaponType) {
	case WEAPON_SHURIKEN:
	{
		//rotate on X 
		visibleModel->Rotate(rotationX, 0, 0);

		//if in air, move forward badsed on FV, time and velocity
		if (weaponState == WEAPON_INAIR_STATE) {
			visibleModel->setPosition(currentPos.x + forwardVector.x * deltaTime * vel,
				currentPos.y + forwardVector.y * deltaTime * vel,
				currentPos.z + forwardVector.z * deltaTime * vel);

			//use length to compare distances
			if (Math::Length(spawnPoint - currentPos) >
				Math::Length(spawnPoint - effectiveDistance)) {
				weaponState = WEAPON_FALL_STATE;
			}
		}
		//if in fall state, move on X and Z as before but fall raPIDLT
		if (weaponState == WEAPON_FALL_STATE) {
			visibleModel->setPosition(currentPos.z + forwardVector.x * deltaTime * vel,
				currentPos.y - deltaTime * vel * 2,
				currentPos.z + forwardVector.z * deltaTime * vel); 
			//if it hits floor change state
			if (currentPos.y < 0) {
				weaponState = WEAPON_HIT_STATE;
			}
		}
	}
	break; 
	case WEAPON_DART: {
		//if in air move forwards in either direction
		if (weaponState == WEAPON_INAIR_STATE) {
			visibleModel->setPosition(currentPos.x + forwardVector.x * deltaTime * vel,
				currentPos.y + forwardVector.y * deltaTime * vel,
				currentPos.z + forwardVector.z * deltaTime * vel);
			if (Math::Length(spawnPoint - visibleModel->getPosition()) >
				Math::Length(spawnPoint - effectiveDistance)) {
				weaponState = WEAPON_FALL_STATE;
			}
		}

			//if in fall state, still move forwards on X and Z as before but fall rapid
		if(weaponState==WEAPON_FALL_STATE){
			visibleModel->setPosition(currentPos.x + forwardVector.x * deltaTime * vel,
				currentPos.y - deltaTime * vel * 2,
				currentPos.z + forwardVector.z * deltaTime * vel);
			//if it hits floor change states
			if (visibleModel->getPosition().y < 0) {
				weaponState = WEAPON_HIT_STATE; 
			}
		}
	}
					  break; 
	case WEAPON_BOMB: {
		if (weaponState == WEAPON_INAIR_STATE) {
			//aggregate time elapsed before firing
			timeElapsed += deltaTime; 

			//gravity setting 
			float gravity = -9.8; 
			//calculate 3D movement
			float x = hVel*timeElapsed*forwardVector.x; 
			float y = ((yVel*timeElapsed) + 0.5*(gravity*(timeElapsed))); 

			float z = hVel*timeElapsed*forwardVector.z; 

			visibleModel->setPosition(currentPos.x + x,
				currentPos.y + y,
				currentPos.z + z); 

			//if it hits floor, change state
			if (currentPos.y < 0) {
				weaponState = WEAPON_HIT_STATE; 
			}
		}
	}
					  break;

	}
	//set collision position and then update both models matrices
	weaponCollision->setPosition(currentPos); 
	visibleModel->Update(deltaTime);
	weaponCollision->Update(deltaTime);
}