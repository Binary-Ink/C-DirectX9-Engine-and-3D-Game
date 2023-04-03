#pragma once

namespace Batman
{
	class IEvent {								//event interface
	protected:
		int id;									//unique id for event
	public:
		IEvent();								//interface constructor
		virtual ~IEvent() {}					//interface destructor
		int getID() { return id; }				//get ID
	};

	enum eventtype {							//enumerator for event type
		EVENT_TIMER = 10,						//when updating event types
		EVENT_KEYPRESS = 20,					//this also needs updating
		EVENT_KEYRELEASE = 30,
		EVENT_MOUSECLICK = 40,
		EVENT_MOUSEMOTION = 50,
		EVENT_MOUSEWHEEL = 60,
		EVENT_MOUSEMOVE = 70,
		EVENT_XBUTTON = 80,
		EVENT_ENTITYUPDATE = 90, 
		EVENT_ENTITYRENDER = 100, 
	};

	//all events implement the interface

	class KeyPressEvent : public IEvent {			//Key Press
	public:
		int keycode;								//DirectInput code
		KeyPressEvent(int key);						//raise event with code
	};

	class KeyReleaseEvent : public IEvent {			//Key Release
	public:
		int keycode;								//DirectInput code
		KeyReleaseEvent(int key);					//raise event with code
	};

	class MouseClickEvent : public IEvent {			//Mouse Click
	public:
		int button;									//button pressed
		MouseClickEvent(int btn);					//raise event with data
	};

	class MouseMotionEvent : public IEvent {		//Mouse delta position
	public:
		int deltax, deltay;							//delta x, y
		MouseMotionEvent(int dx, int dy);			//raise event with data
	};

	class MouseWheelEvent : public IEvent {			//Mouse wheel event
	public:
		int wheel;									//wheel value
		MouseWheelEvent(int wheel);					//raise event with value
	};

	class MouseMoveEvent : public IEvent {			//Mouse poisition event
	public:
		int posx, posy;								//x, y position
		MouseMoveEvent(int px, int py);				//raise event with data
	};

	class XButtonEvent : public IEvent {
	public:
		XINPUT_STATE PadState;
		XboxPad* padPointer;
		XButtonEvent(XINPUT_STATE padState, XboxPad* padPtr);
	};

	class EntityRenderEvent : public IEvent {
	public:
		Batman::Entity* entity;
		EntityRenderEvent(Entity* e);
	}; 

	class EntityUpdateEvent : public IEvent {
	public:
		Batman::Entity* entity;
		EntityUpdateEvent(Entity* e);
	}; 
}