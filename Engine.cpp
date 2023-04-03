#include "Engine.h"
using namespace std;

namespace Batman
{
	//constructor
	Engine::Engine()
	{
		//Initialise all vars with default vals
		p_apptitle = "Batman Engine";
		p_screenwidth = 1600;
		p_screenheight = 900;
		p_colordepth = 32;
		p_fullscreen = false;
		p_device = NULL;
		p_coreFrameCount = 0;
		p_coreFrameRate = 0;
		p_screenFrameCount = 0;
		p_screenFrameRate = 0;
		//D3D Blue!
		p_backdropColor = D3DCOLOR_XRGB(0, 0, 80);
		p_windowHandle = 0;
		p_pauseMode = false;
		p_versionMajor = VERSION_MAJOR;
		p_versionMinor = VERSION_MINOR;
		p_revision = REVISION;
		p_commandLineParams = "";

		//null render target variables
		p_MainSurface = 0;
		p_MainDepthStencilSurface = 0;

		//window handle must be set later for DirectX
		p_windowHandle = 0;
	}

	//destructor
	Engine::~Engine()
	{
		//delete objects in order of creation
		delete p_input;
		if (p_device) p_device->Release();
		if (p_d3d) p_d3d->Release();
	}

	//returns string text of engine version using string stream
	std::string Engine::getVersionText()
	{
		std::ostringstream s;
		s << "Batman Engine v" << p_versionMajor << ". "
			<< p_versionMinor << "." << p_revision;
		return s.str();
	}

	//create message box from string and title
	void Engine::Message(std::string message, std::string title)
	{
		MessageBox(0, message.c_str(), title.c_str(), 0);
	}

	//set screen properties using setter methods
	void Engine::setScreen(int w, int h, int d, bool full)
	{
		setScreenWidth(w);
		setScreenHeight(h);
		setColorDepth(d);
		setFullscreen(full);
	}

	//initialise engine (window, D3D device, input, renderer)
	bool Engine::Init(HINSTANCE hInstance, int width, int height,
		int colordepth, bool fullscreen)
	{
		//get window caption string from engine
		string title;
		title = g_engine->getApptitle();

		//set window dimensions (begins with engine defaults)
		RECT windowRect;
		windowRect.left = 0;
		windowRect.right = g_engine->getScreenWidth();
		windowRect.top = 0;
		windowRect.bottom = g_engine->getScreenHeight();

		//create window class structure
		WNDCLASSEX wc;								//struct def
		memset(&wc, 0, sizeof(WNDCLASS));			//zero memory
		wc.cbSize = sizeof(WNDCLASSEX);				//get struct size
		wc.style = CS_HREDRAW | CS_VREDRAW;			//style (resize x/y)
		wc.lpfnWndProc = (WNDPROC)WndProc;			//callback function
		wc.hInstance = hInstance;					//handle instance
		wc.lpszClassName = title.c_str();			//OS class name
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);	//Cursor Style
		wc.cbClsExtra = 0;						//extra bytes for instance
		wc.cbWndExtra = 0;							//extra bytes for class
		wc.hIcon = 0;								//window icon
		wc.hIconSm = 0;								//window icon (small)
		wc.hbrBackground = 0;						//background colour
		wc.lpszMenuName = 0;						//menu

		//setup the window with the class info
		RegisterClassEx(&wc);

		//set up in windowed or fullscreen?
		DWORD dwStyle, dwExStyle;
		if (g_engine->getFullscreen())
		{
			//struct for screen setup
			DEVMODE dm;
			memset(&dm, 0, sizeof(dm));				//zero memory
			dm.dmSize = sizeof(dm);					//struct size
			dm.dmPelsWidth =						//pixel width of visable screen
				g_engine->getScreenWidth();
			dm.dmPelsHeight =						//pixel height of visable scren
				g_engine->getScreenHeight();
			dm.dmBitsPerPel =						//colour resolution
				g_engine->getColorDepth();
			dm.dmFields =							//initalise properties
				DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			//attempt to change to fullscreen
			if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				//if failed, debg message and set to false
				debug << "Diplay mode change failed" << std::endl;
				g_engine->setFullscreen(false);
			}
			//constants for window setup
			dwStyle = WS_POPUP;
			dwExStyle = WS_EX_APPWINDOW;
		}
		else
		{
			//constants for window setup
			dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		}

		//adjust window to true request size
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		//create program window
		int wwidth = windowRect.right - windowRect.left;
		int wheight = windowRect.bottom - windowRect.top;

		//some output for log
		debug << "Screen Size: " << width << ", " << height << endl;
		debug << "Creating program window" << endl;

		//create window functions
		HWND hWnd = CreateWindowEx(
			0,
			title.c_str(),			//window class
			title.c_str(),			//title bar
			dwStyle |
			WS_CLIPCHILDREN |
			WS_CLIPSIBLINGS,			//window styles
			0, 0,					//x,y coordinate
			wwidth,					//width of window
			wheight,				//height of window
			0,						//parent window
			0,						//menu
			hInstance,				//application instance
			0						//window parameters
		);

		//was there an error creating the window
		if (!hWnd)
		{
			//output for log
			debug << "Error creating program window" << endl;
			return 0;
		}

		//display the window
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);

		//save window handle in engine
		g_engine->setWindowHandle(hWnd);

		//output log
		debug << "Creating Direct3D Object" << endl;

		//initialise Direct3D
		p_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (p_d3d == NULL) {
			return 0;
		}

		//get system desktop color depth
		D3DDISPLAYMODE dm;
		//display mode saved at dm location
		p_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

		//set configuration options for Direct3D
		D3DPRESENT_PARAMETERS d3dpp;			//presentation params structs
		ZeroMemory(&d3dpp, sizeof(d3dpp));		//zero meory
		d3dpp.Windowed = (!fullscreen);			//windowed or not
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//swap chain (discard last frame)
												//(back buffer)
		d3dpp.EnableAutoDepthStencil = 1;		//auto depth stencil (near camera?)
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//format for depth stencil
		d3dpp.Flags =
			D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;//discard depth stencil each time
		d3dpp.PresentationInterval =
			D3DPRESENT_INTERVAL_IMMEDIATE;		//no interval between screen refresh
		d3dpp.BackBufferFormat = dm.Format;		//display mode format
		d3dpp.BackBufferCount = 1;				//how many back buffers?
		d3dpp.BackBufferWidth = width;			//width of back buffer
		d3dpp.BackBufferHeight = height;		//height of back buffer
		d3dpp.hDeviceWindow = p_windowHandle;	//window handle
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;//multi-sample type (can be used for AA)

		//output log
		debug << "Creating Direct3D Device" << endl;

		//create Direct 3D Device (hardware t&l)
		p_d3d->CreateDevice(
			D3DADAPTER_DEFAULT,			//default adapter
			D3DDEVTYPE_HAL,				//Hardware Rasterisation
			p_windowHandle,				//window handle
			D3DCREATE_HARDWARE_VERTEXPROCESSING,//Hardware Vertex processing
			&d3dpp,						//D3D presentation struct
			&p_device					//D3D device memory location
		);

		//if hardware T&L fails, try software
		if (p_device == NULL)
		{
			//output log
			debug << "Hardware vertex option failed! Trying software..." << endl;

			p_d3d->CreateDevice(
				D3DADAPTER_DEFAULT,			//default adapter
				D3DDEVTYPE_HAL,				//Hardware Rasterisation
				p_windowHandle,				//window handle
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,//Software Vertex processing
				&d3dpp,						//D3D presentation struct
				&p_device					//D3D Device Memory location
			);
			//has it worked? Tell outlog log the result
			if (p_device == NULL)
			{
				debug << "Software vertex option failed; shutting down." << endl;
				return 0;
			}
			else {
				debug << "Software vertex processing" << endl;
			}

		}
		else {
			//output log
			debug << "Hardware vertext processing" << endl;
		}

		//output log
		debug << "Creating 2D Renderer" << endl;

		//initialise 2D Renderer / error handling
		HRESULT result = D3DXCreateSprite(
			p_device,			//D3D device
			&p_spriteObj		//sprite object memory location
		);
		if (result != D3D_OK)
		{
			debug << "D3DXCreateSprite failed" << endl;
			return 0;
		}

		//initialise directinput
		debug << "Init input system" << endl;
		p_input = new Input(getWindowHandle());
		p_padinput = new XboxPad(1);

		//output log
		debug << "Calling game_init(" << getWindowHandle() << ")" << endl;

		//call game initialisation extern function
		if (!game_init(getWindowHandle())) return 0;

		//output log
		debug << "Engine init succeeded" << endl;

		return 1;
	}

	/**
	Resets transforms by setting the identity matrix
	**/
	void Engine::setIdentity()
	{
		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);
		g_engine->getDevice()->SetTransform(D3DTS_WORLD, &identity);
	}

	/**
	Resets sprite trandforms by setting the identity matrix
	**/
	void Engine::setSpriteIdentity()
	{
		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);
		g_engine->getSpriteObj()->SetTransform(&identity);
	}

	/**
	Savaing and restoring the render target is used when rendering to a texture
	**/
	void Engine::savePrimaryRenderTarget()
	{
		//save primary rendering & depty stencil surfaces
		p_device->GetRenderTarget(0, &p_MainSurface);
		p_device->GetDepthStencilSurface(&p_MainDepthStencilSurface);
	}
	void Engine::restorePrimaryRenderTarget()
	{
		//restore normal render target
		p_device->SetRenderTarget(0, p_MainSurface);
		p_device->GetDepthStencilSurface(&p_MainDepthStencilSurface);
	}

	void Engine::clearScene(D3DCOLOR color)
	{
		//clears the scene with bg colour
		p_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			color, 1.0f, 0);
	}

	void Engine::Update(float elapsedTime)
	{
		static float accumTime = 0;

		//calculate core framerate
		//increment core frame count
		p_coreFrameCount++;
		//if second passed, update rate value
		if (p_coreTimer.Stopwatch(1000))
		{
			p_coreFrameRate = p_coreFrameCount;
			p_coreFrameCount = 0;
		}

		//fast update (logic)
		game_update(elapsedTime);

		//ENTITY MANAGEMENT 
		//update entities

		if (!p_pauseMode) {
			updateEntities(elapsedTime);
			testForCollisions(); //collision testing
		}

		//60 fps ~16ms per frame
		if (!timedUpdate.Stopwatch(16))
		{
			//free the CPU for 1ms
			timedUpdate.Rest(1);
		}
		else
		{
			//calculate real framerate
			p_screenFrameCount++;
			if (p_screenTimer.Stopwatch(100))
			{
				p_screenFrameRate = p_screenFrameCount;
				p_screenFrameCount = 0;
			}

			//update input devices
			p_input->Update();
			updateKeyboard();
			updateMouse();

			//begin rendering
			if (p_device->BeginScene() == D3D_OK)
			{
				//clear scene
				g_engine->clearScene(p_backdropColor);

				//ENTITY MNANAGEMENT
				entitiesRender3D();

				//3D rendering
				game_render3d();

				//2D rendering (using sprite objects)
				p_spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

				//ENTITY MANAGEMENT
				entitiesRender2D();

				game_render2d();

				p_spriteObj->End();

				//finish scene and refresh
				p_device->EndScene();
				p_device->Present(0, 0, 0, 0);

			}

		}

		//ENTITY MANAGEMENT
		buryEntities();
	}

	void Engine::updateMouse()
	{
		static int oldPosX = 0;
		static int oldPosY = 0;

		//check mouse buttons
		for (int n = 0; n < 4; n++)
		{
			if (p_input->GetMouseButton(n))
			{
				//launch event
				raiseEvent(new MouseClickEvent(n));
			}

			XINPUT_STATE PadState = p_padinput->GetState();
			XboxPad* padPtr = p_padinput;
			raiseEvent(new XButtonEvent(PadState, padPtr));
		}

		//check mouse position
		int posx = p_input->GetMousePosX();
		int posy = p_input->GetMousePosY();
		if (posx != oldPosX || posy != oldPosY)
		{
			int oldPosX = p_input->GetMousePosX();
			int oldPosY = p_input->GetMousePosY();
			//launch event
			raiseEvent(new MouseMoveEvent(posx, posy));
		}

		//check mouse motion
		int deltax = p_input->GetMouseDeltaX();
		int deltay = p_input->GetMouseDeltaY();
		if (deltax != 0 || deltay != 0)
		{
			//launch event
			raiseEvent(new MouseMotionEvent(deltax, deltay));
		}

		//check mouse wheel
		int wheel = p_input->GetMouseDeltaWheel();
		if (wheel != 0)
		{
			//launch event
			raiseEvent(new MouseWheelEvent(wheel));
		}
	}

	void Engine::updateKeyboard()
	{
		static char old_keys[256];

		for (int n = 0; n < 255; n++)
		{
			//check for key press
			if (p_input->GetKeyState(n) & 0x80)
			{
				old_keys[n] = p_input->GetKeyState(n);
				//launch event
				raiseEvent(new KeyPressEvent(n));
			}
			//check for release
			else if (old_keys[n] & 0x80)
			{
				old_keys[n] = p_input->GetKeyState(n);
				//launch event
				raiseEvent(new KeyReleaseEvent(n));
			}
		}
	}

	void Engine::Shutdown()
	{
		PostQuitMessage(0);
	}

	void Engine::raiseEvent(IEvent* e)
	{
		//call game event function
		game_event(e);
		delete e;
	}

	//Adds entity to entity vector
	void Engine::addEntity(Entity* entity)
	{
		//log text
		debug << "Engine::addEntity: received new entity\n";
		//counter for unique ids
		static int id = 0;
		//set entity ID
		entity->setID(id);
		//add entity to vector
		p_entities.push_back(entity);
		//increment id
		id++;
	}

	//update entities
	void Engine::updateEntities(float deltaTime)
	{
		//loop through each entity
		BOOST_FOREACH(Entity* entity, p_entities)
		{

			//is this entity alive?
			if (entity->getAlive())
			{

				//move/animate entity
				entity->Update(deltaTime);

				//te11 game that this entity has been updated
				raiseEvent(new EntityUpdateEvent(entity));

				//see if this entity with auto expire
				if (entity->getLifetime() > 0)
				{
					//update entities life counter
					entity->addToLifetimeCounter(deltaTime * 1000);

					//if life expired, set alive to false
					if (entity->lifetimeExpired())
					{
						entity->setAlive(false);
					}
				}
			}
		}
	}

	//remove dead entities
	void Engine::buryEntities()

	{
		//an iterator will work better than BOOST_FOREACH in this cas
		std::vector<Entity*>::iterator iter = p_entities.begin();
		while (iter != p_entities.end())
		{
			//if entity not alive, erase from vector
			if ((*iter)->getAlive() == false)
			{
				iter = p_entities.erase(iter);
			}
			else iter++;
		}
	}

	//render 30 entities
	void Engine::entitiesRender3D()

	{
		BOOST_FOREACH(Entity* entity, p_entities)

		{
			//is this a 3D entity?
			if (entity->getRenderType() == RENDER_3D)
			{
				//is this entity in use?
				if (entity->getAlive() && entity->getVisible())
				{
					//for global rendering
					//entity->Render();
					//raiseEvent( new EntityRenderEvent( entity ) );
				}
			}
		}
	}

	//render ZD entities
	void Engine::entitiesRender2D()

	{
		BOOST_FOREACH(Entity* entity, p_entities)
		{
			//is this a 20 entity?
			if (entity->getRenderType() == RENDER_2D)
			{
				//is this entity in use?
				if (entity->getAlive() && entity->getVisible())
				{
					//for global rendering
					//entity->Render();
					//raiseEvent( new EntityRenderEvent( entity ) );
				}
			}
		}
	}

	//find entity by name
	Entity *Engine::findEntity(std::string name)
	{
		//loop through and return alive entity where name matches
		BOOST_FOREACH(Entity* entity, p_entities)
		{
			if (entity->getAlive() && entity->getName() == name)
				return entity;
		}
		return NULL;
	}

	//find entity by name
	Entity *Engine::findEntity(int id)

	{
		//loop through and return alive entity where id matches
		BOOST_FOREACH(Entity* entity, p_entities)
		{
			if (entity->getAlive() && entity->getID() == id)
				return entity;
		}
		return NULL;
	}

	//count of LIVING entities by type
	int Engine::getEntityCount(enum EntityType entityType)
	{
		//tota1 var
		int total = 0;
		//loop through and increment total of alive entity by type passed in
		BOOST_FOREACH(Entity* entity, p_entities)
		{
			if (entity->getAlive() && entity->getEntityType() == entityType)
				total++;
		}
		return total;
	}

	//main collision test loops
	void Engine::testForCollisions()
	{
		//reset all collided properties
		BOOST_FOREACH(Entity* entity, p_entities)
		{
			entity->setCollided(false);
			entity->setCollideBuddy(NULL);

		}

		//escape if global collisions are disabled
		if (!p_globalCollision) return;

		//loop through all entities (first)
		BOOST_FOREACH(Entity* first, p_entities)
		{
			if (first->getAlive() && first->isCollidable())
			{

				//test all other entities for collision (second)
				BOOST_FOREACH(Entity* second, p_entities)
				{

					//do not test object with itself
					if (second->getID() != first->getID())
					{

						if (second->getAlive() && second->isCollidable())
						{

							//test for collision

							if (Collision(first, second))
							{
								//set collision flags and buddies
								first->setCollided(true);
								first->setCollideBuddy(second);
								second->setCollided(true);
								second->setCollideBuddy(first);
							}
						}//if
					}//for each
				}//if
			}//foreach
		}
	}

	//main collision check between entity types
	//BASED ON SNITCH / CASE CHECKS, CALL OVERLOADED COLLISION
	//METHOO THAT MATCHES THE ENTITY TYPES
	bool Engine::Collision(Entity* entity1, Entity* entity2)
	{
		switch (entity1->getEntityType())
		{
			//THIS CHECK ALLOWS US TO DETERMINE THAT THE FIRST ENTITY IS A PLAYER
		case ENTITY_PLAYER_MESH:
			switch (entity2->getEntityType())

			{
				//if second one a scenery mesh
			case ENTITY_SCENERY_MESH:
				return PlayerToSceneryCollision((Mesh*)entity1, (Mesh*)entity2);
				break;
			}
			//if second one is a collectable mesh 
		case ENTITY_COLLECTABLE_MESH:
			return Collision((Mesh*)entity1, (Mesh*)entity2);
			break;

			//THIS CHECK ALLOWS US TO DETERMINE THAT THE FIRST ENTITY IS A SPRITE
		case ENTITY_SPRITE:
			switch (entity2->getEntityType())

			{
				//if second one is a sprite
			case ENTITY_SPRITE:
				return Collision((Sprite*)entity1, (Sprite*)entity2);
				break;
			}

		case ENTITY_MESH:
			switch (entity2->getEntityType())
			{
				//if second one is a mesh
			case ENTITY_MESH:
				return Collision((Mesh*)entity1, (Mesh*)entity2);
				break;
				//if second on is scenery mesh
			case ENTITY_SCENERY_MESH:
				return Collision((Mesh*)entity1, (Mesh*)entity2);
				break;
			}
			//THIS ALLOWS US TO DETERMINE THAT THE FIRST ENTITY IS A RAY
		case ENTITY_RAY:
			switch (entity2->getEntityType())
			{
				//if second one is scenery
			case ENTITY_SCENERY_MESH:
				return RayAABBCollision((Mesh*)entity2, (Ray*)entity1);
				break;
				//if second one is player
			case ENTITY_PLAYER_MESH:
				return RayAABBCollision((Mesh*)entity2, (Ray*)entity1);
				break;
			}
		}
		return false;
	}

	//Sprite to Sprite collisions
	bool Engine::Collision(Sprite* sprite1, Sprite* sprite2)

	{
		//Use 20 AABB based on previously created Sprite methods
		Rect r1 = sprite1->getBounds();
		Rect r2 = sprite2->getBounds();
		if (r1.Intersects(r2)) return true;
		else return false;
	}

	//Mesh to Mesh collisions
	bool Engine::Collision(Mesh* mesh1, Mesh* mesh2)

	{

		//If Mesh 1's min X position is greater than Mesh 2's max X then false
		if (mesh1->getPosition().x + mesh1->getBoundingBox().min.x >
			mesh2->getPosition().x + mesh2->getBoundingBox().max.x) return false;

		//If Mesh 1's min Y position is greater than Mesh 2's max Y then false
		if (mesh1->getPosition().y + mesh1->getBoundingBox().min.y >
			mesh2->getPosition().y + mesh2->getBoundingBox().max.y) return false;

		//If Mesh 1's min 2 position is greater than Mesh 2's max 2 then false
		if (mesh1->getPosition().z + mesh1->getBoundingBox().min.z >
			mesh2->getPosition().z + mesh2->getBoundingBox().max.z) return false;

		//If Mesh 1's max X position is less than Mesh 2's min X then false
		if (mesh1->getPosition().x + mesh1->getBoundingBox().max.x <
			mesh2->getPosition().x + mesh2->getBoundingBox().min.x) return false;

		//If Mesh 1's max Y position is less than Mesh 2's min Y then false
		if (mesh1->getPosition().y + mesh1->getBoundingBox().max.y <
			mesh2->getPosition().y + mesh2->getBoundingBox().min.y) return false;

		//If Mesh 1's max 2 position is less than Mesh 2's min Z then false
		if (mesh1->getPosition().z + mesh1->getBoundingBox().max.z <
			mesh2->getPosition().z + mesh2->getBoundingBox().min.z) return false;

		//if none of the tests above are valid, there's a collision
		return true;
	}

	//AABB Collision and Response
	bool Engine::PlayerToSceneryCollision(Mesh* player, Mesh* scenery) {

		//Check AABB collision first - if no collision return false
		if (!Collision(player, scenery))
			return false;
		//if collision, update player pos to previous pos
		player->restorePreviousPosition();

		//return collision
		return true;
	}
	/*

		//This will get the direction vector from the scenery centre position

		//to the player position
		Vector3 dirVec = Math::Normal(scenery->getPosition() - player->getPosition());

		//floats for new X,Y,Z positions as necessary
		float newXpos;
		float newZpos;
		float newYpos;

		if (dirVec.x < -0.5) {
			//if nearest negative X edge
			//Use the max values of the bounding boxes to calculate new X Pos
			newXpos = scenery->getPosition().x +
				scenery->getBoundingBox().max.x +
				player->getBoundingBox().max.x;
			//Set new position
			player->setPosition(newXpos,
				player->getPosition().y,
				player->getPosition().z);
		}

		else if (dirVec.x > 0.5) {
			//if nearest positive X edge
			//Use the min values of the bounding boxes to calculate new X Pos
			newXpos = scenery->getPosition().x +
				scenery->getBoundingBox().min.x +
				player->getBoundingBox().min.x;
			//Set new position
			player->setPosition(newXpos,
				player->getPosition().y,
				player->getPosition().z);
		}
		else if (dirVec.z < -0.5) {
			//if nearest negative Z edge
			//Use the max values of the bounding boxes to calculate new 2 Pos
			newZpos - scenery->getPosition().z +
				scenery->getBoundingBox().max.z +
				player->getBoundingBox().max.z;
			//Set new position
			player->setPosition(player->getPosition().x,
				player->getPosition().y,
				newZpos);
		}
		else if (dirVec.z > 0.5) {
			//if nearest positive 2 edge
			//Use the min values of the bounding boxes to calculate new Z Pos
			newZpos = scenery->getPosition().z +
				scenery->getBoundingBox().min.z +
				player->getBoundingBox().min.z;
			//Set new position
			player->setPosition(player->getPosition().x,
				player->getPosition().y,
				newZpos);
		}
		else if (dirVec.y < -0.5) {
			//if nearest negative Y edge
			//Use the max values of the bounding boxes to calculate new Y Pos
			newYpos - scenery->getPosition().y +
				scenery->getBoundingBox().max.y +
				player->getBoundingBox().max.y;
			//Set new position
			player->setPosition(player->getPosition().x,
				newYpos,
				player->getPosition().z);
		}
		else if (dirVec.y > 0.5) {
			//if nearest positive Y edge
			//Use the min values of the bounding boxes to calculate new Y Pos
			newYpos = scenery->getPosition().y +
				scenery->getBoundingBox().min.y +
				player->getBoundingBox().min.y;
			//Set new position
			player->setPosition(player->getPosition().x,
				newYpos,
				player->getPosition().z);
		}

		//if none of the tests above are valid, there's a collision
		return true;
	}*/

	bool Engine::RayAABBCollision(Mesh* player, Ray* shot) {

		//ray-mesh intersection test
		int hasHit;
		float distanceToCollision;

		//CREATE VECTOR FRom RAY ORIGIN FOR LENGTH OF 100

		Vector3 rayTestVec0 = shot->RayBegin;

		Vector3 rayTestVec1 = shot->RayBegin + shot->direction * 100;

		//GET AABB DETAILS

		Vector3 BoxMin = player->getPosition() + player->getBoundingBox().min;
		Vector3 BoxMax = player->getPosition() + player->getBoundingBox().max;
		// Set 0 as beginning and 1 as end value

		float fractionLow = 0;
		float fractionHigh = 1;

		//CLIP x
		//x distance from box min/max to ray begin / full vector x distance

		float fractionLowX = (BoxMin.x - rayTestVec0.x) / (rayTestVec1.x - rayTestVec0.x);
		float fractionHighX = (BoxMax.x - rayTestVec0.x) / (rayTestVec1.x - rayTestVec0.x);

		//ensure the high / low values are correct
		if (fractionHighX < fractionLowX)
			swap(fractionLowX, fractionHighX);

		//if the max x clip is less than 0, no intersection
		if (fractionHighX < fractionLow)
			return false;

		//if the min x clip is greater than 1, no intersection
		if (fractionLowX > fractionHigh)
			return false;

		//reset the low and high fractions of the line to
		//the min and max x clip (clip the line)
		fractionLow = max(fractionLowX, fractionLow);
		fractionHigh = min(fractionHighX, fractionHigh);

		//if the low clip is greater than the high clip, no intersection
		if (fractionLow > fractionHigh)
			return false;

		//CLIP Y
		//y distance from box min/max to ray begin / full vector y distance

		float fractionLowY = (BoxMin.y - rayTestVec0.y) / (rayTestVec1.y - rayTestVec0.y);
		float fractionHighY = (BoxMax.y - rayTestVec0.y) / (rayTestVec1.y - rayTestVec0.y);

		//ensure the high / low values are correct
		if (fractionHighY < fractionLowY)
			swap(fractionLowY, fractionHighY);

		//if the max y clip is less than 0, no intersection
		if (fractionHighY < fractionLow)
			return false;

		//if the min y clip is greater than 1, no intersection
		if (fractionLowY > fractionHigh)
			return false;

		//reset the low and high fractions of the line to
		//the min and max y clip (clip the line)
		fractionLow = max(fractionLowY, fractionLow);
		fractionHigh = min(fractionHighY, fractionHigh);

		//if the low clip is greater than the high clip, no intersection
		if (fractionLow > fractionHigh)
			return false;

		//CLIP z
		//z distance from box min/max to ray begin / full vector 2 distance

		float fractionLowZ = (BoxMin.z - rayTestVec0.z) / (rayTestVec1.z - rayTestVec0.z);
		float fractionHighZ = (BoxMax.z - rayTestVec0.z) / (rayTestVec1.z - rayTestVec0.z);

		//ensure the high / low values are correct
		if (fractionHigh < fractionLowZ)
			swap(fractionLowZ, fractionHigh);

		//if the max 2 clip is less than 0, no intersection
		if (fractionHigh < fractionLow)
			return false;

		//if the min 2 clip is greater than 1, no intersection
		if (fractionLow > fractionHigh)
			return false;
	
	//*************************************
	//intersection point
	//get full line length
	Vector3 raySection = rayTestVec1 - rayTestVec0;

	//calculate distance from origin using line portion
	Vector3 intersection = rayTestVec0 + raySection *fractionLow;

	//update the ray class with the intersection point
	shot->intersectionPoint = intersection;

	//collision true
	return true;

}


}//namespace
				
				