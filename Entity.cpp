#include "Engine.h"

namespace Batman

{
	//constructor - initialise vars

	Entity::Entity(enum RenderType type)

	{

		renderType = type;
		id = -1;
		name = "";
		visible = false;
		alive = true;
		entityType = ENTITY_UNKNOWN;
		lifetimeStart = 0;
		lifetimeCounter = 0;
		lifetimeLength = 0;
		collidable = false;
		collided = false;

	}
}; 

