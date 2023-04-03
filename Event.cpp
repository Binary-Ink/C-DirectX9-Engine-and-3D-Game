#include "Engine.h"

namespace Batman {

	//Event constructor - resets id 
	IEvent::IEvent() {
		id = 0;
	}

	KeyPressEvent::KeyPressEvent(int key)
	{
		id = EVENT_KEYPRESS;		//set enum
		keycode = key;				//set key
	}

	KeyReleaseEvent::KeyReleaseEvent(int key)
	{
		id = EVENT_KEYRELEASE;		//set enum
		keycode = key;				//set key
	}

	MouseClickEvent::MouseClickEvent(int btn)
	{
		id = EVENT_MOUSECLICK;		// set enum
		button = btn;				//set button
	}

	MouseMotionEvent::MouseMotionEvent(int dx, int dy)
	{
		id = EVENT_MOUSEMOTION;		//set enum
		deltax = dx;				//set delta x
		deltay = dy;				//set delta y
	}

	MouseWheelEvent::MouseWheelEvent(int whl)
	{
		id = EVENT_MOUSEWHEEL;		//set enum
		wheel = whl;				//set wheel val
	}

	MouseMoveEvent::MouseMoveEvent(int px, int py)
	{
		id = EVENT_MOUSEMOVE;		//set enum
		posx = px;					//set x pos
		posy = py;					//set y pos
	}

	XButtonEvent::XButtonEvent(XINPUT_STATE padState, XboxPad* padPtr)
	{
		id = EVENT_XBUTTON; 
		PadState = padState; 
		padPointer = padPtr; 
	}

	EntityRenderEvent::EntityRenderEvent(Entity* e)
	{
		id = EVENT_ENTITYRENDER; 
		entity = e; 
	}

	EntityUpdateEvent::EntityUpdateEvent(Entity* e)
	{
		id = EVENT_ENTITYUPDATE; 
		entity = e; 
	}
};