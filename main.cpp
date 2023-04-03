#include "Engine.h"
using namespace std; 
using namespace Batman; 

Effect* effect = NULL;	//Effect for rendering 3D
Effect* skyboxEffect = NULL; //Effect for skybox
Skybox* skybox = NULL; //skybox class
Camera* camera = NULL; //Camera class
Mesh* floorBox = NULL; //box for floor
Mesh* playerCollision = NULL; // box for players collission
Texture* terrainTex = NULL;  //texture for floor
Font* debugText = NULL; //debug text for display
Sprite* mapBg = NULL; // mini map bg
Sprite* charMap = NULL; //character map icon
Sprite* HUDBg = NULL; //hud bg bar
Sprite* ninjaFaceAnim = NULL; //ninja face animation
Mesh* mushroomObject = NULL; //mushroom scenery
Texture* mushroomTex = NULL; // mushroom texture
Mesh* treeObject = NULL; //tree scenery obj
Mesh* bushObject = NULL; //bush scenery obj
Texture* bushTex = NULL; //bush texture

//GLOBALS: Vectors for common objects
std::vector<Entity*> sceneryBoxes; //Vector for scenery boxes
std::vector<Entity*> sceneryBoxesMap; //Vector for scenery map icons
std::vector<Entity*> collectables; //vector for collectables
std::vector<Entity*> collectablesMap; //vector for collectables map icon
std::vector<Vector3> mushroomPositions; // holds all mushroom pos
std::vector<Vector3> treePositions; //holds all tree pos
std::vector<Texture*> treeTextures; // holds all tree textures
std::vector<Vector3> bushPositions; //holds all bush positions
std::vector<Weapon*> weapons; //holds all player weapons

//GLOBALS: weapons and collectables
Mesh* shurikenWeaponMesh = NULL; 
Mesh* shurikenCollectMesh = NULL;
Texture* shurikenTex = NULL;
Mesh* dartWeaponMesh = NULL;
Mesh* dartCollectMesh = NULL;
Texture* dartTex = NULL;
Mesh* bombWeaponMesh = NULL;
Mesh* bombCollectMesh = NULL;
Texture* bombTex = NULL;
Mesh* starCollectMesh = NULL; 
Texture* starTex = NULL; 

//GLOBALS weapon HUD anim
Sprite* shurikenAnim = NULL; 
Sprite* dartAnim = NULL; 
Sprite* bombAnim = NULL; 



//GLOBALS: Lighting
Vector3 lightDirection(1, 0, 0); //direction of light
float lightRed = 1.0; //red
float lightGreen = 1.0; //green
float lightBlue = 1.0; //blue
int lightState = 1; //light state (for tint)
float rotX = 0; //rotation X for light direc
#define LIGHT_RED 1 //red for red tint
#define LIGHT_GREEN 2 // green tinr
#define LIGHT_BLUE 3 //blue tint
void adjustLight(); //lighting updates prototype
Timer colourTimer; //timer for colour
Timer directionTimer; //time for direction change
bool lightDown = true; //flag - light value up or down

//GLOBALS game mechanics values
int shurikens = 0; 
int bombs = 0; 
int darts = 0; 
int pixiestars = 0; 
int health = 0; 
bool seeCollision = false; 
int selectedWeapon = 1; 
#define SELECT_SHURIKENS 1 
#define SELECT_DARTS 2
#define SELECT_BOMBS 3

//GLOBALS : Movement

Vector3 currentVelocity; //Player's current velocity
Vector3 goalVelocity; //Player's goal velocity to" linear interpolation
						//(Smoother: not just stop / start)

EAngle characterToCamera; //Euler Angles for camera target movement
Vector3 charMovement; //Vector for movement on WASD
Vector3 vecForward; //Vector for camera's forward direction
Vector3 vecRight; // Vector for camera's right direction
bool charMoving = false;

// GLOBALS: State machines
int gameState;
#define GAME_PLAY_STATE 1

//helper function prototypes
float Approach(float fGoal, float flCurrent, float dt);
Vector2 ObjectToMap(Vector3 obj); 
void Fire(); 

bool game_preload() {
//	g_engine->SetGameTitle("Bishop Basher");
//	g_engine->SetScreen(1600, 900, 32, false);
	return true;
}; 

//initialise elements
bool game_init(HWND hwnd) {
	//create skybox effect object
	skyboxEffect = new Effect();
	if (!skyboxEffect->Load("Shaders/skybox.fx")) {
		debug << "Error loading skybox effect" << std::endl;
		return false;
	}

	//create a skybox
	skybox = new Skybox();
	if (!skybox->Create("skybox.png")) {
		debug << "Error loading skybox" << std::endl;
		return false;
	}

	//set camera & perspective 
	camera = new Camera();
	//this sets Euglar angles
	characterToCamera.y = 0.0f;
	characterToCamera.p = 0.0f;
	characterToCamera.r = 0.0f;

	//set initial position and target
	//(looking at x axis) 
	camera->setPosition(400, 1, -140);
	camera->setTarget(1, 0, 0);
	camera->Update();

	//create invisible player mesh (5x5x5 around cam)
	playerCollision = new Mesh();
	playerCollision->createCube(5, 5, 5);
	playerCollision->setPosition(camera->getPosition());
	//entity types set for collision detection
	playerCollision->setEntityType(ENTITY_PLAYER_MESH);
	playerCollision->setAlive(true);
	playerCollision->setCollidable(true);
	g_engine->addEntity(playerCollision);

	//create floor mesh from file
	floorBox = new Mesh();
	if (!floorBox->Load("Scenery/mainplane.x")) {
		debug << "Error loading floorbox file" << std::endl;
		return false;
	}


	//load effect file for model shading 
	effect = new Effect();
	if (!effect->Load("Shaders/directional_textured.fx")) {
		debug << "Error loading effect file" << std::endl;
		return false;
	}
	//load terrain from file
	terrainTex = new Texture();
	if (!terrainTex->Load("Scenery/newtex.jpg"))
	{
		debug << "Error loading terrain texture!" << std::endl;
		return false;
	}
	//create Font obj for debug text
	debugText = new Font("Ariel Bold", 20);
	if (!debugText) {
		debug << "Error creating font" << std::endl;
		return false;
	}
	//create mini-map from BG from file
	mapBg = new Sprite();
	if (!mapBg->Load("HUD/Map/MapBGSprite.png")) {
		debug << "Error creating map" << std::endl;
		return false;
	}
	//set position 
	mapBg->setPosition(1300, 100);

	//create mini-character
	charMap = new Sprite();
	if (!charMap->Load("HUD/Map/ninjaMapSprite.png")) {
		debug << "error char map sprite" << std::endl;
		return false;
	}
	//set intitial position on map
	charMap->setPivot(Vector2(5, 5));
	charMap->setPosition(1302, 102);

	//create HUD background
	HUDBg = new Sprite();
	if (!HUDBg->Load("HUD/HUDbgSprite.png")) {
		debug << "Error creating HUD bg" << std::endl;
		return false;
	}
	//set position
	HUDBg->setPosition(0, 800);

	//create ninja face HUD 
	ninjaFaceAnim = new Sprite();
	if (!ninjaFaceAnim->Load("HUD/NinjaFaceAnim.png")) {
		debug << "Error creating Ninja face animation" << std::endl;
		return false;
	}
	//set position
	ninjaFaceAnim->setPosition(750, 800);
	//animation properties
	ninjaFaceAnim->animationColumns = 10;
	ninjaFaceAnim->currentFrame = 0;
	ninjaFaceAnim->setSize(100, 100);
	ninjaFaceAnim->lastFrame = 3;
	ninjaFaceAnim->animationDirection = 2;

	//WEAPONS HUD
	shurikenAnim = new Sprite();
	if (!shurikenAnim->Load("HUD/ShurikenThrowSheet.png")) {
		debug << "Error creating shuriken throw sheet" << std::endl; 
		return false; 
	}
	//set up size, position and anim settings
	shurikenAnim->setPosition(350, 450);
	shurikenAnim->animationColumns = 4; 
	shurikenAnim->currentFrame = 0; 
	shurikenAnim->setSize(900, 350); 
	shurikenAnim->lastFrame = 0; 
	shurikenAnim->animationDirection = 12; 

	dartAnim = new Sprite(); 
	if (!dartAnim->Load("HUD/DartThrowSheet.png")) {
		debug << "Error creating dart throw sheet" << std::endl;
		return false; 
	}
	//set up size, positiona nd anim settings
	dartAnim->setPosition(350, 450);
	dartAnim->animationColumns = 4;
	dartAnim->currentFrame = 0;
	dartAnim->setSize(900, 350);
	dartAnim->lastFrame = 0;
	dartAnim->animationDirection = 12;

	bombAnim = new Sprite();
	if (!bombAnim->Load("HUD/BombThrowSheet.png")) {
		debug << "Error creating Bomb throw sheet" << std::endl;
		return false;
	}
	//set up size, positiona nd anim settings
	bombAnim->setPosition(350, 450);
	bombAnim->animationColumns = 4;
	bombAnim->currentFrame = 0;
	bombAnim->setSize(900, 350);
	bombAnim->lastFrame = 0;
	bombAnim->animationDirection = 12;

	//END WEAPONS HUD

	/*//create new mesh and sprite for their secnery vectors
	sceneryBoxes.push_back(new Mesh());
	sceneryBoxesMap.push_back(new Sprite());
	//get mesh pointer and create a 10x10x10 cube and set position
	Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
	if (!meshPtr->Load("Scenery/bush.x")) {
		debug << "error loading bush rock obj" << std::endl;
		return false;
	}

	meshPtr->setPosition(350, 0, -140);
	//create sprite pointer and load map icon
	Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
	spritePtr->Load("Scenery/Map/bushMap.png");
	//set origin to center of sprite
	spritePtr->setPivot(Vector2(2, 2));
	//set position to the map based on the 3d position
	spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
	//set entity properties for collision detection
	meshPtr->setCollidable(true);
	meshPtr->setAlive(true);
	meshPtr->setEntityType(ENTITY_SCENERY_MESH);
	g_engine->addEntity(meshPtr); */

	

	gameState = GAME_PLAY_STATE;

	//collectables loading
	//array of map pos
	Vector3 a[20] = { Vector3(200, 1, -70), Vector3(300, 1, -70), Vector3(380, 1, -90),
					Vector3(430, 1, -90), Vector3(130, 1, -110), Vector3(250, 1, -140),
					Vector3(70, 1, -190), Vector3(140, 1, -190), Vector3(290, 1, -250),
					Vector3(410, 1, -250), Vector3(220, 1, -260), Vector3(160, 1, -270),
					Vector3(380, 1, -290), Vector3(280, 1, -310), Vector3(70, 1, -320),
					Vector3(200, 1, -330), Vector3(380, 1, -360), Vector3(340, 1, -380),
					Vector3(190, 1, -420), Vector3(380, 1, -430) };



	/*//BEGIN tree load
	//create new mesh for tree obj
	treeObject = new Mesh();
	if (!treeObject->Load("Scenery/tree.x")) {
		debug << "error loading treeObj!" << std::endl;
		return false;
	}
	treeObject->setPosition(250, 0, -250);

	//load textures for trees (diff colours)
	treeTextures.push_back(new Texture());
	if (!treeTextures.back()->Load("Scenery/treeMap0.png")) {
		debug << "Error loading tree tex 0!" << std::endl;
		return false;
	}
	treeTextures.push_back(new Texture());
	if (!treeTextures.back()->Load("Scenery/treeMap1.png")) {
		debug << "Error loading tree tex 1!" << std::endl;
		return false;
	}

	//array of map positions
	Vector3 t[36] = { Vector3(25,0,-25), Vector3(75,0,-25), Vector3(125,0,-25),
		Vector3(175,0,-25), Vector3(225,0,-25), Vector3(275,0,-25),
		Vector3(325,0,-25), Vector3(375,0,-25), Vector3(425,0,-25),
		Vector3(475,0,-25), //top row
							//sides
		Vector3(25,0,-75), Vector3(475,0,-75),
		Vector3(25,0,-125), Vector3(475,0,-125),
		Vector3(25,0,-175), Vector3(475,0,-175),
		Vector3(25,0,-225), Vector3(475,0,-225),
		Vector3(25,0,-275), Vector3(475,0,-275),
		Vector3(25,0,-325), Vector3(475,0,-325),
		Vector3(25,0,-375), Vector3(475,0,-375),
		Vector3(25,0,-425), Vector3(475,0,-425),
		//bottom row
		Vector3(25,0,-475),
		Vector3(75,0,-475), Vector3(125,0,-475), Vector3(175,0,-475),
		Vector3(225,0,-475), Vector3(275,0,-475), Vector3(325,0,-475),
		Vector3(375,0,-475), Vector3(425,0,-475),	Vector3(475,0,-475),
	};

	//assign array to vector
	treePositions.insert(treePositions.end(), t, t + 36);
	//END TREE LOADING*/

	//TOP BORDER
	{
		//create new Mesh and sprite for their respective scenery vectors
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite());
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
		meshPtr->createCube(500, 5, 50);
		meshPtr->setPosition(250, 1, -25);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to centre of sprite
		spritePtr->setPivot(Vector2(2, 2));
		//set position to the map based on 3D position
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
		//set entity properties for collision detection
		meshPtr->setCollidable(true);
		meshPtr->setAlive(true);
		meshPtr->setEntityType(ENTITY_SCENERY_MESH);
		g_engine->addEntity(meshPtr);
	}
	//BOTTOM BORDER
	{
		//create new Mesh and sprite for their respective scenery vectors
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite());
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
		meshPtr->createCube(500, 5, 50);
		meshPtr->setPosition(250, 1, -475);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to centre of sprite
		spritePtr->setPivot(Vector2(2, 2));
		//set position to the map based on 3D position
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
		//set entity properties for collision detection
		meshPtr->setCollidable(true);
		meshPtr->setAlive(true);
		meshPtr->setEntityType(ENTITY_SCENERY_MESH);
		g_engine->addEntity(meshPtr);
	}
	//LEFT BORDER
	{
		//create new Mesh and sprite for their respective scenery vectors
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite());
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
		meshPtr->createCube(50, 5, 500);
		meshPtr->setPosition(25, 1, -250);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to centre of sprite
		spritePtr->setPivot(Vector2(2, 2));
		//set position to the map based on 3D position
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
		//set entity properties for collision detection
		meshPtr->setCollidable(true);
		meshPtr->setAlive(true);
		meshPtr->setEntityType(ENTITY_SCENERY_MESH);
		g_engine->addEntity(meshPtr);
	}
	//RIGHT BORDER
	{
		//create new Mesh and sprite for their respective scenery vectors
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite());
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
		meshPtr->createCube(50, 5, 500);
		meshPtr->setPosition(475, 1, -250);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to centre of sprite
		spritePtr->setPivot(Vector2(2, 2));
		//set position to the map based on 3D position
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
		//set entity properties for collision detection
		meshPtr->setCollidable(true);
		meshPtr->setAlive(true);
		meshPtr->setEntityType(ENTITY_SCENERY_MESH);
		g_engine->addEntity(meshPtr);
	}

	//BEGIN: Bush loading
	//create new bush mesh
	bushObject = new Mesh();
	bushObject->Load("Scenery/updatedBush.x");

	//load tex from file for bush
	bushTex = new Texture();
	if (!bushTex->Load("Scenery/bushUVtex.png")) {
		debug << "Error loading bush tex!" << std::endl;
		return false;
	}

	//array of map positions
	Vector3 b[93] = { Vector3(405,0,-75), Vector3(405,0,-85), Vector3(145,0,-95), Vector3(405,0,-95), Vector3(145,0,-105), Vector3(405,0,-105), Vector3(145,0,-115), Vector3(405,0,-115), Vector3(375,0,-115),
		Vector3(385,0,-115), Vector3(390,0,-115), Vector3(405,0,-115), Vector3(145,0,-125), Vector3(375,0,-125), Vector3(105,0,-135), Vector3(115,0,-135), Vector3(125,0,-135), Vector3(135,0,-135),
		Vector3(145, 0, -135), Vector3(375, 0, -135), Vector3(175, 0, -145), Vector3(225, 0, -185), Vector3(235, 0, -185), Vector3(245, 0, -185), Vector3(255, 0, -185), Vector3(265, 0, -185), Vector3(275, 0, -185),
		Vector3(285, 0, -185), Vector3(285, 0, -195), Vector3(235, 0, -205), Vector3(255, 0, -205), Vector3(285, 0, -205), Vector3(295, 0, -205), Vector3(305,0,-205), Vector3(315,0,-205), Vector3(325,0,-205),
		Vector3(235,0,-215), Vector3(255,0,-215), Vector3(235,0,-225), Vector3(255,0,-225), Vector3(265,0,-225), Vector3(275,0,-225), Vector3(285,0,-225), Vector3(305,0,-225), Vector3(195,0,-235),
		Vector3(205,0,-235), Vector3(215,0,-235), Vector3(225,0,-235), Vector3(235,0,-235), Vector3(285,0,-235), Vector3(305,0,-235), Vector3(195,0,-245), Vector3(285,0,-245), Vector3(305,0,-245),
		Vector3(195,0,-255), Vector3(285,0,-255), Vector3(305,0,-255), Vector3(195,0,-265), Vector3(285,0,-265), Vector3(305,0,-265), Vector3(195,0,-275), Vector3(215,0,-275), Vector3(225,0,-275),
		Vector3(235,0,-275), Vector3(245,0,-275), Vector3(255,0,-275), Vector3(285,0,-275), Vector3(305,0,-275), Vector3(195,0,-285), Vector3(215,0,-285), Vector3(195,0,-295), Vector3(215,0,-295),
		Vector3(215,0,-305), Vector3(175,0,-315), Vector3(185,0,-315), Vector3(195,0,-315), Vector3(205,0,-315), Vector3(215,0,-315), Vector3(175,0,-325), Vector3(175,0,-335), Vector3(175,0,-345),
		Vector3(325,0,-365), Vector3(335,0,-365), Vector3(345,0,-365), Vector3(355,0,-365), Vector3(365,0,-365), Vector3(375,0,-365), Vector3(375,0,-375), Vector3(375,0,-385), Vector3(375,0,-395),
		Vector3(385,0,-395), Vector3(395,0,-395), Vector3(405,0,-395) };
		
	//assign array to vector
	bushPositions.insert(bushPositions.end(), b, b + 93); 

	for (int i = 0; i < bushPositions.size(); i++) {
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite()); 
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back(); 
		meshPtr->createCube(10, 5, 10);
		meshPtr->setPosition(bushPositions[i]);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back(); 
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to center of sprite
		spritePtr->setPivot(Vector2(2, 2)); 
		//set pos to the map based on 3D position
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition())); 
		//set entity properties for collision detection
		meshPtr->setCollidable(true); 
		meshPtr->setAlive(true); 
		meshPtr->setEntityType(ENTITY_SCENERY_MESH); 
		g_engine->addEntity(meshPtr); 
	}
	//END BUSH LOAD

	//BEGIN MUSHROOM LOAD
	//create new mushroom
	mushroomObject = new Mesh(); 
	mushroomObject->Load("Scenery/mushroom.x"); 

	//load mushroom texture
	mushroomTex = new Texture();
	if (!mushroomTex->Load("scenery/Mushroom.png")) {
		debug << "Error loading mushroom texture" << std::endl; 
		return false; 
	}

	//array of map pos
	
		
	Vector3 m[9] = { Vector3(100, 0, -100),
	Vector3(250, 0, -100), Vector3(350, 0, -100), Vector3(200, 0, -200), Vector3(150, 0, -250),
	Vector3(260, 0, -250), Vector3(380, 0, -250), Vector3(300, 0, -350), Vector3(80, 0, -420) };

	//assign array to vector
	mushroomPositions.insert(mushroomPositions.end(), m, m + 9); 

	for (int i = 0; i < mushroomPositions.size(); i++) {
		sceneryBoxes.push_back(new Mesh());
		sceneryBoxesMap.push_back(new Sprite());
		Mesh* meshPtr = (Mesh*)sceneryBoxes.back();
		meshPtr->createCube(20, 5, 20);
		meshPtr->setPosition(mushroomPositions[i]);

		//create sprite pointer and load map icon
		Sprite* spritePtr = (Sprite*)sceneryBoxesMap.back();
		spritePtr->Load("HUD/Map/bushMap.png");
		//set origin to center of sprite
		spritePtr->setPivot(Vector2(2, 2));
		//set pos to the map based on 3D pos
		spritePtr->setPosition(ObjectToMap(meshPtr->getPosition()));
		//set entity properties for collision detection
		meshPtr->setCollidable(true);
		meshPtr->setAlive(true);
		meshPtr->setEntityType(ENTITY_SCENERY_MESH);
		g_engine->addEntity(meshPtr);
	}
	//END OF MUSHROOM LOAD
	//vector for map positions
	std::vector<Vector3>collectablePositions;
	//assign array to vec
	collectablePositions.insert(collectablePositions.end(), a, a + 20);
	//shuffle randomly
	std::random_shuffle(collectablePositions.begin(), collectablePositions.end());
	int collectableType = 0;
	//loop through randomised positions
	for (int i = 0; i < collectablePositions.size(); i++) {
		//every 5 items, change type
		if (i < 5) {
			collectableType = COLLECT_SHURIKENS;
		}
		else if (i < 10) {
			collectableType = COLLECT_DARTS;
		}
		else if (i < 15) {
			collectableType = COLLECT_BOMBS;
		}
		else if (i < 20) {
			collectableType = COLLECT_STARS;
		}

		//add new collectable to vector and cast pointer 
		collectables.push_back(new Collectable(collectableType));
		Collectable* colPtr = (Collectable*)collectables.back();
		//set positions of invisible and visible mesh
		colPtr->getInvisibleMesh()->setPosition(collectablePositions[i]);
		colPtr->getVisibleMesh()->setPosition(collectablePositions[i]);

		//see collision / entity properties for invisible mesh
		colPtr->getInvisibleMesh()->setCollidable(true);
		colPtr->getInvisibleMesh()->setEntityType(ENTITY_COLLECTABLE_MESH);
		g_engine->addEntity(colPtr->getInvisibleMesh());
		//put map icon in map vector
		collectablesMap.push_back(colPtr->getMap());
	}

	//COLLECTABLES & WEAPON LOADING
	//shuriken as weapon loading
	shurikenWeaponMesh = new Mesh(); 
	shurikenWeaponMesh->Load("Weapons/shuriken.x"); 
	//shuriken collect mesh
	shurikenCollectMesh = new Mesh(); 
	shurikenCollectMesh->Load("Collectables/shuriken.x"); 
	//load texture files 
	shurikenTex = new Texture();
	if (!shurikenTex->Load("Weapons/shurikenTex.png")) {
		debug << "Error loading shuriken texture " << std::endl; 
		return false; 
	}
	//dart as weapon loading
	dartWeaponMesh = new Mesh();
	dartWeaponMesh->Load("Weapons/dart.x");
	//dart collect mesh
	dartCollectMesh = new Mesh();
	dartCollectMesh->Load("Collectables/dart.x");
	//load texture files 
	dartTex = new Texture();
	if (!dartTex->Load("Weapons/dartTex.png")) {
		debug << "Error loading dart texture " << std::endl;
		return false;
	}
	//bomb as weapon loading
	bombWeaponMesh = new Mesh();
	bombWeaponMesh->Load("Weapons/bomb.x");
	//bomb collect mesh
	bombCollectMesh = new Mesh();
	bombCollectMesh->Load("Collectables/bomb.x");
	//load texture files 
	bombTex = new Texture();
	if (!bombTex->Load("Weapons/bombTex.png")) {
		debug << "Error loading bomb texture " << std::endl;
		return false;
	}
	//star loading
	starCollectMesh = new Mesh();
	starCollectMesh->Load("Collectables/star.x");
	//load texture files 
	starTex = new Texture();
	if (!starTex->Load("Collectables/starTex.png")) {
		debug << "Error loading star texture " << std::endl;
		return false;
	}
	//END COLLECTABLES

	//set default values for mechanic stuff
	shurikens = 10; 
	bombs = 5; 
	darts = 5; 
	pixiestars = 0; 
	health = 100; 
	return true; 
}

//release memory 
void game_end() {

	if (starTex) delete starTex; 
	if (starCollectMesh) delete starCollectMesh; 
	if (bombTex) delete bombTex; 
	if (bombCollectMesh) delete bombCollectMesh; 
	if (bombWeaponMesh) delete bombWeaponMesh; 
	if (dartTex) delete dartTex; 
	if (dartCollectMesh) delete dartCollectMesh; 
	if (dartWeaponMesh) delete dartWeaponMesh; 
	if (shurikenTex) delete shurikenTex; 
	if (shurikenCollectMesh) delete shurikenCollectMesh;
	if (shurikenWeaponMesh) delete shurikenWeaponMesh; 

	for (int i = 0; i < collectables.size(); i++)//release all collectables
	{
		if (collectablesMap[i]) delete collectablesMap[i]; 
		Collectable* colPtr = (Collectable*)collectables[i];
		//colPtr->setMap(NULL); 
		if (collectables[i]) delete collectables[i];
	}

	for (int i = 0; i < sceneryBoxesMap.size(); i++) //release scenery boxes
	{
		if (sceneryBoxesMap[i]) delete sceneryBoxesMap[i];
	}
	for (int i = 0; i < sceneryBoxes.size(); i++) //release scenery boxes
	{
		if (sceneryBoxes[i]) delete sceneryBoxes[i]; 
	}
	if (charMap) delete charMap; 
	if (mapBg) delete mapBg;
	if (debugText) delete debugText; 
	if (terrainTex) delete terrainTex; 
	if (effect) delete effect; 
	if (floorBox) delete floorBox; 
	if (playerCollision) delete playerCollision; 
	if (camera) delete camera; 
	if (skybox) delete skybox; 
	if (skyboxEffect) delete skyboxEffect; 
}

void game_update(float deltaTime) {
	//PLAY STATE UPDATE BLOCK 
	if (gameState == GAME_PLAY_STATE) {

		//BEGIN WEAPONS UPDATE 
		for (int i = 0; i < weapons.size();) {
			weapons[i]->Update(deltaTime);
			//if weapon state hit, prepare removal
			if (weapons[i]->getState() == WEAPON_HIT_STATE) {
				if (weapons[i]->getWeaponType() == WEAPON_BOMB) {
				}
				weapons[i]->getInvisibleMesh()->setAlive(false);
				weapons.erase(weapons.begin() + i);
			}else{
				i++; //update it counter
			}
		}
		//END WEAPONS UPDATE
		//WEAPONS HUD UPDATE
		shurikenAnim->Update(deltaTime);
		dartAnim->Update(deltaTime);
		bombAnim->Update(deltaTime);

		//if almost finished, reset
		if (shurikenAnim->currentFrame > 3.7) { shurikenAnim->lastFrame = 0; }
		if (dartAnim->currentFrame > 3.7) { dartAnim->lastFrame = 0; }
		if (bombAnim->currentFrame > 3.7) { bombAnim->lastFrame = 0; }
		//END WEAPON HUD UPDATE

		//create a vector for the camera based on Eluar angles
		Vector3 camTarg; 
		camTarg = characterToCamera.ToVector(); 
		//take the current camera position and add target vector
		camera->setPosition(playerCollision->getPosition());
		camera->setTarget(camera->getPosition() + camTarg);

		//interpolated movement onm x and y
		//sends goal velocity, current character movement and time diff
		charMovement.setX(Approach(goalVelocity.getX(), charMovement.getX(), deltaTime * 20));
		charMovement.setZ(Approach(goalVelocity.getZ(), charMovement.getZ(), deltaTime * 20));

		//determine forward vector from cam target direction
		//set y to 0 and get x/z vector and normalise
		vecForward = characterToCamera.ToVector(); 
		vecForward.y = 0; 
		vecForward = Math::Normal(vecForward); 

		//set up as positive on the Y
		Vector3 vecUp(0, 1, 0); 

		//calculate direction to the right of where the camera is facing using cross prroduct 
		vecRight = Math::CrossProduct(vecUp, vecForward); 
		//using the current velocity calculate direction to move on x + z
		//directions are negative due to world positioning
		charMovement = goalVelocity;
		currentVelocity = vecForward * -charMovement.z + vecRight * -charMovement.x; 
		
		//currentVelocity = Vector3(0, 0, 0);

		//set up new camera position by assing the new current velocity (* the timne diff)
		camera->setPosition(camera->getPosition().getX() + currentVelocity.getX() * deltaTime,
			camera->getPosition().getY() + currentVelocity.getY() * deltaTime,
			camera->getPosition().getZ() + currentVelocity.getZ() * deltaTime);

		camera->setTarget(camera->getPosition() + camTarg); //sets cam target after pos


		//set the collision mesh to move with camera
		playerCollision->setPosition(camera->getPosition());
		playerCollision->Update(deltaTime); 

		//set flag for if character is moving
		if (currentVelocity.x == 0 && currentVelocity.z == 0) {

			charMoving = false;
			ninjaFaceAnim->firstFrame = 0;
			ninjaFaceAnim->lastFrame = 3;
		}
		else { 
			charMoving = false; 
			ninjaFaceAnim->firstFrame = 3; 
			ninjaFaceAnim->lastFrame = 6;

		}
		//set players icon position on map
		charMap->setPosition(ObjectToMap(playerCollision->getPosition())); 

		//update ninja face animation
		ninjaFaceAnim->Update(deltaTime);

		//scenery map position and box updates
		for (int i = 0; i < sceneryBoxes.size(); i++) {
			//get mesh pointer and update
			Mesh* boxPtr = (Mesh*)sceneryBoxes[i];
			boxPtr->Update(deltaTime);

			//if not a border 
			if (i > 3) {
				//adjust collission based on distance from character
				if (boxPtr->getPosition().x > playerCollision->getPosition().x + 50 ||
					boxPtr->getPosition().x < playerCollision->getPosition().x - 50 ||
					boxPtr->getPosition().z > playerCollision->getPosition().z + 50 ||
					boxPtr->getPosition().z < playerCollision->getPosition().z - 50)
				{
					boxPtr->setCollidable(false);
				}
				else {
					boxPtr->setCollidable(true); 
				}
			}

			//get sprite pointer and set map position
			Sprite* sprPtr = (Sprite*)sceneryBoxesMap[i]; 
			sprPtr->setPosition(ObjectToMap(boxPtr->getPosition()));
		}
		//End:: Scenery map position

		//collectables iterator 
		for (int i = 0; i < collectables.size();) {
			//get mesh pointer and update
			Collectable* colPtr = (Collectable*)collectables[i]; 
			colPtr->Update(deltaTime);
			//get sprite pointer and set map position
			Sprite* sprPtr = (Sprite*)collectablesMap[i];
			sprPtr->setPosition(ObjectToMap(colPtr->getVisibleMesh()->getPosition()));

			//if invisible mesh had collided
			if (colPtr->getInvisibleMesh()->isCollided()) {
				//check collectable type and update values accordingly
				switch (colPtr->getType()) {
				case COLLECT_SHURIKENS:
					shurikens += 10; 
					break;
				case COLLECT_DARTS:
					darts += 10;
					break; 
				case COLLECT_BOMBS:
					bombs = +10; 
					break;
				case COLLECT_STARS:
					pixiestars += 1;
					break; 
				}

				//remove collectables from map
				collectables.erase(collectables.begin() + i);
				collectablesMap.erase(collectablesMap.begin() + i);
			}
			else {
				//if no collision, update iteration counter
				i++; 
			}
		}
		//END: collectabler iterator

		//perform cam update and floortransform
		camera->Update(); 
		floorBox->Transform(); 
	}

	//END: PLAY STATE 
}

//method for interpolated movement
float Approach(float flGoal, float flCurrent, float dt) {
	//work out difference'goal velocity - current velocity
	float flDifference = flGoal = flCurrent; 

	//if difference is greater than the delta time
	if (flDifference > dt)
		return flCurrent + dt;	//return increased value
	if (flDifference < -dt)	//if less than negative delta time
		return flCurrent - dt; 

	return flGoal;		//return goal val
}

//convert from 3D position to 2D position
Vector2 ObjectToMap(Vector3 obj) {
	Vector2 newPos; 
	
	//convert 3DX to 2DX

	//conversion divides by 500 (map units) and multiplies by 200 (map pixels) to determine position
	newPos.x = mapBg->getPosition().x + (floor(obj.x / 500 * 200)); 

	newPos.y = mapBg->getPosition().y + (-floor(obj.z / 500 * 200));

	return newPos; 
}

void game_render3d() {

	//adjust lighting
	adjustLight(); 
	//send lighting values to shader
	effect->setParam("LightColor", D3DXVECTOR4(lightRed, lightGreen, lightBlue, 1)); 
	effect->setParam("LightVector", lightDirection.ToD3DXVECTOR3());

	//send transforms to shader
	Matrix matWorld; 
	matWorld.setIdentity(); 
	//set matrices for skybox for getting view + pojection from cam
	skyboxEffect->setWorldMatrix(matWorld); 
	skyboxEffect->setViewMatrix(camera->getViewMatrix()); 
	skyboxEffect->setProjectionMatrix(camera->getProjMatrix()); 

	//render the skybox
	skyboxEffect->Begin(); 
	skybox->Render(); 
	skyboxEffect->End(); 

	//set matrices and texture for floorbox to render
	effect->setProjectionMatrix(camera->getProjMatrix());
	effect->setViewMatrix(camera->getViewMatrix()); 
	effect->setWorldMatrix(floorBox->getMatrix());
	effect->setParam("Texture", terrainTex->getTexture());

	//calculate combined inverse transponse matric for lighting
	D3DXMATRIX inverse, wit; 
	D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*)&floorBox->getMatrix());
	D3DXMatrixTranspose(&wit, &inverse); 
	effect->setParam("WorldInverseTranspose", wit); 

	floorBox->Render(effect); 
	
	if (seeCollision) {
		//iterate through scenery boxes 
		for (int i = 0; i < sceneryBoxes.size(); i++) {
			Mesh* boxPtr = (Mesh*)sceneryBoxes[i];
			effect->setProjectionMatrix(camera->getProjMatrix()); 
			effect->setViewMatrix(camera->getViewMatrix()); 
			effect->setWorldMatrix(boxPtr->getMatrix());
				boxPtr->Render(); 
		}
	}

	for (int i = 0; i < mushroomPositions.size(); i++) {
		mushroomObject->setPosition(mushroomPositions[i]);
		mushroomObject->Transform(); 
		effect->setProjectionMatrix(camera->getProjMatrix()); 
		effect->setViewMatrix(camera->getViewMatrix()); 
		effect->setWorldMatrix(mushroomObject->getMatrix());
		effect->setParam("Texture", mushroomTex->getTexture()); 

		//calculate combined inverse transpose matrix
		D3DXMATRIX inverse, wit; 
		D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*)&mushroomObject->getMatrix());
		D3DXMatrixTranspose(&wit, &inverse); 
		effect->setParam("WorldInverseTranspose", wit);

		mushroomObject->Render(effect); 
	}

	for (int i = 0; i < bushPositions.size(); i++) {
		bushObject->setPosition(bushPositions[i]);
		bushObject->Transform();
		effect->setProjectionMatrix(camera->getProjMatrix());
		effect->setViewMatrix(camera->getViewMatrix());
		effect->setWorldMatrix(bushObject->getMatrix());
		effect->setParam("Texture", bushTex->getTexture());

		//calculate combined inverse transpose matrix
		D3DXMATRIX inverse, wit;
		D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*)&bushObject->getMatrix());
		D3DXMatrixTranspose(&wit, &inverse);
		effect->setParam("WorldInverseTranspose", wit);

		bushObject->Render(effect);
	}
	for (int i = 0; i < treePositions.size(); i++) {
		treeObject->setPosition(treePositions[i]);
		treeObject->Transform();
		effect->setProjectionMatrix(camera->getProjMatrix());
		effect->setViewMatrix(camera->getViewMatrix());
		effect->setWorldMatrix(treeObject->getMatrix());

		int modulo = i % 4; //update texture based on remainer - pattern of 4

		effect->setParam("Texture", treeTextures[modulo]->getTexture());

			//calculate combined inverse transpose mix
		D3DXMATRIX inverse, wit;
		D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*)&treeObject->getMatrix());
		D3DXMatrixTranspose(&wit, &inverse);
		effect->setParam("WorldInverseTranspose", wit);

		treeObject->Render(effect);
	}

	

	for (int i=0; i<sceneryBoxes.size(); i++){
		Mesh* boxPtr = (Mesh*)sceneryBoxes[i];
		effect->setProjectionMatrix(camera->getProjMatrix()); 
		effect->setViewMatrix(camera->getViewMatrix());
		effect->setWorldMatrix(boxPtr->getMatrix());
		boxPtr->Render(effect); 

		//iterate through collectables, set matrices and effect to render 
		for (int i = 0; i < collectables.size(); i++) {
			Collectable* colPtr = (Collectable*)collectables[i]; 
			effect->setProjectionMatrix(camera->getProjMatrix());
			effect->setViewMatrix(camera->getViewMatrix());
			effect->setWorldMatrix(colPtr->getVisibleMesh()->getMatrix());

			//calculate combined invsrse transpose matrix
			D3DXMATRIX inverse, wit; 
			D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*) &colPtr->getVisibleMesh()->getMatrix()); 
			D3DXMatrixTranspose(&wit, &inverse);
			effect->setParam("WorldInverseTranspose", wit);

			//get visible mesh properties and set to relevant weapon mesh with texture
			switch (colPtr->getType()) {
			case COLLECT_SHURIKENS:
				shurikenCollectMesh->setMatrix(colPtr->getVisibleMesh()->getMatrix());
				effect->setParam("Texture", shurikenTex->getTexture()); 
				shurikenCollectMesh->Render(effect);
				break; 
			case COLLECT_DARTS:
				dartCollectMesh->setMatrix(colPtr->getVisibleMesh()->getMatrix());
				effect->setParam("Texture", dartTex->getTexture());
				dartCollectMesh->Render(effect);
				break;
			case COLLECT_BOMBS:
				bombCollectMesh->setMatrix(colPtr->getVisibleMesh()->getMatrix());
				effect->setParam("Texture", bombTex->getTexture());
				bombCollectMesh->Render(effect);
				break;
			case COLLECT_STARS:
				starCollectMesh->setMatrix(colPtr->getVisibleMesh()->getMatrix());
				effect->setParam("Texture", starTex->getTexture());
				starCollectMesh->Render(effect);
				break;

			}
			//iterate through weapons, set matrices and effect to render 
			for (int i = 0; i < weapons.size(); i++) {
				Weapon* wPtr = (Weapon*)collectables[i];
				effect->setProjectionMatrix(camera->getProjMatrix());
				effect->setViewMatrix(camera->getViewMatrix());
				effect->setWorldMatrix(wPtr->getVisibleMesh()->getMatrix());

				//calculate combined invsrse transpose matrix
				D3DXMATRIX inverse, wit;
				D3DXMatrixInverse(&inverse, 0, (D3DXMATRIX*)&wPtr->getVisibleMesh()->getMatrix());
				D3DXMatrixTranspose(&wit, &inverse);
				effect->setParam("WorldInverseTranspose", wit);

				//get visible mesh properties and set to relevant weapon mesh with texture
				switch (wPtr->getWeaponType()) {
				case WEAPON_SHURIKEN:
					shurikenWeaponMesh->setMatrix(wPtr->getVisibleMesh()->getMatrix());
					effect->setParam("Texture", shurikenTex->getTexture());
					shurikenWeaponMesh->Render(effect);
					break;
				case WEAPON_DART:
					dartWeaponMesh->setMatrix(wPtr->getVisibleMesh()->getMatrix());
					effect->setParam("Texture", dartTex->getTexture());
					dartWeaponMesh->Render(effect);
					break;
				case WEAPON_BOMB:
					bombWeaponMesh->setMatrix(wPtr->getVisibleMesh()->getMatrix());
					effect->setParam("Texture", bombTex->getTexture());
					bombWeaponMesh->Render(effect);
					break;
				}
				//if collision viewing on, show collision mesh
				if (seeCollision) {
					effect->setWorldMatrix(wPtr->getInvisibleMesh()->getMatrix());
					wPtr->getInvisibleMesh()->Render(effect);

				}

				}
			//colPtr->getVisibleMesh()->Render(effect);
		}
	}
}

//render 2D
void game_render2d() {
	std::ostringstream debugInfo; 

	//loads of debug info
	debugInfo << "Camera Position: x " << camera->getPosition().x
		<< "| y" << camera->getPosition().y
		<< "| z" << camera->getPosition().z << std::endl; 
	debugInfo << "Forward Vector: x " << vecForward.x
		<< "| y " << vecForward.y
		<< "| z" << vecForward.z << std::endl;
	debugInfo << "Right Vector: x " << vecRight.x
		<< "| y" << vecRight.y
		<< "| z" << vecRight.z << std::endl; 
	debugInfo << "Current Velocity: x " << currentVelocity.x
		<< "| y" << currentVelocity.y
		<< "| Z" << currentVelocity.z << std::endl; 
	debugInfo << "E Angle: r " << characterToCamera.r
		<< "| p" << characterToCamera.p
		<< "| y" << characterToCamera.y << std::endl; 
	debugInfo << "Player Position: x " << playerCollision->getPosition().x
		<< "| y" << playerCollision->getPosition().y
		<< "| z" << playerCollision->getPosition().z << std::endl; 
	debugInfo << "Player Collided?" << playerCollision->isCollided(); 
	debugInfo << "Player BBOX: max x : " << playerCollision->getBoundingBox().max.x
		<< " - y " << playerCollision->getBoundingBox().max.y
		<< " - x " << playerCollision->getBoundingBox().max.x
		<< std::endl; 
	debugInfo << "Player BBOX: min x: " << playerCollision->getBoundingBox().min.x
		<< " - y: " << playerCollision->getBoundingBox().min.y
		<< " - z: " << playerCollision->getBoundingBox().min.z
		<< std::endl; 
	debugInfo << "Light r: " << lightRed
		<< " - g:" << lightGreen
		<< " - b:" << lightBlue
		<< std::endl; 
	debugInfo << "light Direction X: " << lightDirection.x
		<< " - y:" << lightDirection.y
		<< " - z:" << lightDirection.z
		<< std::endl; 

	//print debug info
	debugText->Print(10, 10, debugInfo.str());

	//render map bg and character icon
	mapBg->Render();
	charMap->Render(); 
	//iterate through scenery boxes and render map icons
	for (int i = 0; i < sceneryBoxes.size(); i++) {
		Sprite* sprPtr = (Sprite*)sceneryBoxesMap[i];
		sprPtr->Render();
	}
	//hud
		HUDBg->Render(); 
		ninjaFaceAnim->Render(); 

	//iterate through sceneryboxes and render map icons
		for (int i = 0; i < collectables.size(); i++) {
			Sprite* sprPtr = (Sprite*)collectablesMap[i];
			sprPtr->Render(); 
	}
	//HUD info
		//shrunkens
		debugInfo.clear(); debugInfo.str(""); 
		debugInfo << shurikens;
		debugText->Print(120, 865,debugInfo.str());
		//darts
		debugInfo.clear(); debugInfo.str("");
		debugInfo << darts;
		debugText->Print(340, 865, debugInfo.str());
		//bombs
		debugInfo.clear(); debugInfo.str("");
		debugInfo << bombs;
		debugText->Print(525, 865, debugInfo.str());
		//pixie stars
		debugInfo.clear(); debugInfo.str("");
		debugInfo << pixiestars;
		debugText->Print(1230, 830, debugInfo.str());

		//create colour value that matches lighting values for 3D world
		//use this to set colour of HUD animations
		Color tint = Color(lightRed, lightGreen, lightBlue, 1.0);

		//render the current selected weapon
		switch (selectedWeapon) {
		case SELECT_SHURIKENS:
			shurikenAnim->setColor(tint); 
			shurikenAnim->Render(); 
			break;
		case SELECT_DARTS: 
			dartAnim->setColor(tint);
			dartAnim->Render(); 
			break;
		case SELECT_BOMBS:
			bombAnim->setColor(tint);
			bombAnim->Render();
			break;
		}
}

//event handling
void game_event(IEvent* e) {
	switch (e->getID()) {
		//keypress event
	case EVENT_KEYPRESS: 
	{
		KeyPressEvent* kpe = (KeyPressEvent*)e;
		int keypresscode = kpe->keycode; 
		if (keypresscode == DIK_ESCAPE)
			g_engine->Shutdown(); 
		if (gameState==GAME_PLAY_STATE) {
			//there set new goal velocities for movement
			if (keypresscode == DIK_W)
				goalVelocity.setZ(-10);
			if (keypresscode == DIK_S)
				goalVelocity.setZ(10); 
			if (keypresscode == DIK_A)
				goalVelocity.setX(10);
			if (keypresscode == DIK_D)
				goalVelocity.setX(-10);
		
			break;
		}
	}
	break;

	case EVENT_KEYRELEASE:
	{
		KeyReleaseEvent* kre = (KeyReleaseEvent*)e; 
		int keyreleasecode = kre->keycode;
		if (gameState == GAME_PLAY_STATE) {
			//these reset goal velocities for movement
			if (keyreleasecode == DIK_W)
				goalVelocity.setZ(0); 
			if (keyreleasecode == DIK_S)
				goalVelocity.setZ(0);
			if (keyreleasecode == DIK_A)
				goalVelocity.setX(0);
			if (keyreleasecode == DIK_D)
				goalVelocity.setX(0);
			if (keyreleasecode == DIK_C) {
				if (seeCollision)
					seeCollision = false;
				else
					seeCollision = true;
			}
			if (keyreleasecode == DIK_B) {
				g_engine->setGlobalCollisions(false);
			}
			if (keyreleasecode == DIK_V) {
				g_engine->setGlobalCollisions(true);
			}
			if (keyreleasecode == DIK_1) 
				selectedWeapon = SELECT_SHURIKENS;
			if (keyreleasecode == DIK_2) 
				selectedWeapon = SELECT_DARTS; 
			if (keyreleasecode == DIK_3) 
				selectedWeapon = SELECT_BOMBS; 
				if (keyreleasecode == DIK_SPACE) {
					Fire(); }
			

			break;
		}
	}
	break;

	case EVENT_MOUSEMOTION:
	{
		MouseMotionEvent* mme = (MouseMotionEvent*)e;
		float movex = mme->deltax;
		float movey = mme->deltay; 

		if (gameState == GAME_PLAY_STATE) {
			//set eu;er angles for cam direction
			characterToCamera.y -= movex*0.0075; 
			characterToCamera.p -= movey* 0.0075;

			//set restrictions on target movement
			if (characterToCamera.p >= 1.565)
				characterToCamera.p = 1.565; 
			if (characterToCamera.p <= -1.565)
				characterToCamera.p = -1.565;

			characterToCamera.Normalize(); 

		}
	}
	break;
	}
}

void adjustLight() {
	//every half second
	if (colourTimer.Stopwatch(500)) {
		//check light state
		switch (lightState) {
		case LIGHT_RED:
			//adjust colour values up or down
			if (lightDown) {
				lightBlue -= 0.005;
				lightGreen -= 0.005;
			}
			else {
				lightBlue += 0.005;
				lightGreen += 0.005;
			}
			//if down so far, reset flag
			if (lightGreen < 0.5) {
				lightDown = false; 
			}
			//if up so far, set flag and change state
			if (lightGreen == 1.0) {
				lightState = LIGHT_GREEN; 
				lightDown = true; 
			}
			break;
		case LIGHT_GREEN:
			//adjust colour values up or down 
		
			if (lightDown) {
				lightBlue -= 0.005;
				lightRed -= 0.005;
			}
			else {
				lightBlue += 0.005;
				lightRed += 0.005;
			}
			//if down so far, reset flag
			if (lightRed < 0.5) {
				lightDown = false;
			}
			//if up so far, set flag and change state
			if (lightRed == 1.0) {
				lightState = LIGHT_BLUE;
				lightDown = true;
			}
			break;
		case LIGHT_BLUE:
			//adjust colour values up or down
			if (lightDown) {
				lightRed -= 0.005;
				lightGreen -= 0.005;
			}
			else {
				lightRed += 0.005;
				lightGreen += 0.005;
			}
			//if down so far, reset flag
			if (lightGreen < 0.5) {
				lightDown = false;
			}
			//if up so far, set flag and change state
			if (lightGreen == 1.0) {
				lightState = LIGHT_RED;
				lightDown = true;
			}
			break;
		}
	}

	//every second
	if (directionTimer.Stopwatch(1000)) {
		//increase rotation value (wrap 360)
		rotX += 0.05; 
		if (rotX == 360)
			rotX = 0; 

		//set rotation matrix
		double x = D3DXToRadian(rotX); 
		double y = 0; 
		double z = 0; 
		D3DXMATRIX matRotate; 

		//calculate rotation matrix
		D3DXMatrixRotationYawPitchRoll(&matRotate, (float)x, (float)y, (float)z);

		//multiply light direction by rotation matrix to update direction
		D3DXVECTOR3 newLight = lightDirection.ToD3DXVECTOR3(); 
		D3DXVec3TransformCoord(&newLight, &newLight, &matRotate); 

		//set new direction
		lightDirection = newLight;
	}
}

//Fire 
void Fire() {
	//to hold weapon type
	int weaponType = 0; 
	bool hasFired = false; 

	//check weapon type and see if ammo in supply
	if (selectedWeapon == SELECT_SHURIKENS && shurikens > 0) {
		//update animation
		shurikenAnim->lastFrame = 3;
		shurikens--;
		weaponType = WEAPON_SHURIKEN;
		hasFired = true;
	}
	if (selectedWeapon == SELECT_DARTS && darts > 0) {
		//update animation
		dartAnim->lastFrame = 3;
		darts--;
		weaponType = WEAPON_DART; 
		hasFired = true;
	}
	if (selectedWeapon == SELECT_BOMBS && bombs > 0) {
		//update animation
		bombAnim->lastFrame = 3;
		bombs--;
		weaponType = WEAPON_BOMB;
		hasFired = true;
	}
	//if hasFired flag set
	if (hasFired) {
		//add new weapon to weapons vec
		weapons.push_back(new Weapon());
		//send forward directopmn, weapon type and current pos
		weapons.back()->Init(Math::Normal(characterToCamera.ToVector()),
			weaponType, camera->getPosition());
		//get invisible mesh properties

		weapons.back()->getInvisibleMesh()->setCollidable(true); 
		weapons.back()->getInvisibleMesh()->setAlive(true); 
		weapons.back()->getInvisibleMesh()->setEntityType(ENTITY_WEAPON_MESH);
		g_engine->addEntity(weapons.back()->getInvisibleMesh());
	}
}

