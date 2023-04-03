#pragma once

#include "Engine.h"

namespace Batman {
	//unique ID for each collectable type
	enum collectableType {
		COLLECT_SHURIKENS = 10, //weapon pickup
		COLLECT_DARTS = 20,
		COLLECT_BOMBS = 30,
		COLLECT_STARS = 40, //pixie star pickup
	};
	//inherits from Mesh
	class Collectable : public Entity {
	private: 
		Texture* texture; //texture for mesh
		Sprite* mapIcon; //sprite for map icon
		int type; //type (from collectabletype)
		Mesh* visibleModel; //visible model 9mesh)
		Mesh* collectableCollision; //invisible collision mesh

		float height;
		float xRotation; 
		float yDir; 
		Timer updateTimer; 

	public: 
		Collectable(int ctype); //constructor (takes in type)
		~Collectable(); // destructor 

		//empty methods for entity override 
		void Update(float deltaTime);
		void Render(){}

		void Init(int cType); //initialisation method
		int getType() { return type; }//returns collectable type
		Texture* getTexture() { return texture; }  //returns texture for rendering
		Sprite* getMap() { return mapIcon; }//returns icon for use
		Mesh* getVisibleMesh() { return visibleModel; }//get visible mesh pointer
		Mesh* getInvisibleMesh() { return collectableCollision;  }//gets collision mesh pointer
	};
};