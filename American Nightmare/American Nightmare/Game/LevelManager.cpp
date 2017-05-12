#include "LevelManager.h"

LevelManager::LevelManager() 
{
	this->materialManager = nullptr;
	this->meshManager = nullptr;
	this->particleManager = nullptr;
	this->soundManager = nullptr;
}

LevelManager::LevelManager(const LevelManager & other) { }

LevelManager::~LevelManager() { }

bool LevelManager::Start(GLuint playerShader, MaterialManager* materialManager, MeshManager* meshManager, ParticleManager* particleManager, SoundManager* soundManager)
{
	// Getting parameters
	this->materialManager = materialManager;
	this->meshManager = meshManager;
	this->particleManager = particleManager;
	this->soundManager = soundManager;

	// Starting contact manager
	contactManager.Start(particleManager, soundManager);

	// Starting world 
	world = new b2World(b2Vec2(NULL, GRAVITY));
	if (world == nullptr) return false;
	world->SetContactListener(&contactManager);

	// Starting light Manager
	lightManager = new LightManager();
	if (lightManager == nullptr) return false;

	////////////////////////////////////////////////////////////
	// Creating the Player object
	////////////////////////////////////////////////////////////
	GLint tempNomralMapIndex = materialManager->AddTexture("playernormalmap", TEXTURE_PATH "Walk01_nor.png");
	player = new Player();
	if (player == nullptr) return false;
	if (!player->Start(meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), world))
		return false;
	player->setShader(playerShader);
	player->AddAnimation(materialManager->getMaterial("playermaterial"), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt");

	////////////////////////////////////////////////////////////
	// Creating the Enemy object
	////////////////////////////////////////////////////////////
	enemy = new Enemy();
	if (enemy == nullptr) return false;
	if (!enemy->Start(meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), world))
		return false;
	enemy->setShader(playerShader);
	enemy->AddAnimation(materialManager->getMaterial("playermaterial"), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt");

	return true;
}

void LevelManager::Stop()
{
	// Deleting player
	if (player != nullptr)
	{
		player->Stop();
		delete player;
		player = nullptr;
	}
	// Unloads the map objects
	StopMap();
	lightManager->Clear();
	delete lightManager;

	// These are getting removed in screen instead
	materialManager = nullptr;
	meshManager = nullptr;
	particleManager = nullptr;
	soundManager = nullptr;
}

void LevelManager::StopMap()
{
	// Unloads every object in map
	for (Object* object : map)
	{
		if (object != nullptr)
		{
			delete object;
			object = nullptr;
		}
	}

	// Unloads every hitbox in the map
	for (Hitbox* hitbox : hitboxes)
	{
		if (hitbox != nullptr)
		{
			hitbox->Stop();
			delete hitbox;
			hitbox = nullptr;
		}
	}

	// Unloads every trigger in the map
	for (Trigger* trigger : triggers)
	{
		if (trigger != nullptr)
		{
			trigger->Stop();
			delete trigger;
			trigger = nullptr;
		}
	}
}


bool LevelManager::LoadLevel(GLuint shader, std::string levelPath, std::string archivePath)
{
	// Unload current level
	StopMap();

	////////////////////////////////////////////////////////////
	// Loading Archive
	////////////////////////////////////////////////////////////
	archive.readFromFile(archivePath.c_str());
	LoadArchiveVisuals(archive.meshes);

	////////////////////////////////////////////////////////////
	// Loading Level
	////////////////////////////////////////////////////////////
	levelFile.readFromFile(levelPath.c_str());
	LoadLevelMeshes(levelFile.meshes, shader);
	LoadLevelLights(levelFile.lights);
	LoadLevelHitboxes(levelFile.hitboxes);
	LoadLevelSpawners(levelFile.spawners, shader);
	LoadLevelTriggers(levelFile.triggers);
	LoadLevelEffects(levelFile.effects);

	// Music
	soundManager->playSong(SoundManager::SONG::MUSIC_BOOGIE);
	
	// Temp directional light for shadows
	lightManager->AddDirectionalLight(glm::vec4(5, 20, 20, 1), glm::vec4(-0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 0.3f);

	// Loading temp level
//	LoadTempLevel(shader);

	return true;
}

void LevelManager::LoadArchiveVisuals(std::vector<AMesh> meshes)
{
	////////////////////////////////////////////////////////////
	// Loading Archive Meshes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < meshes.size(); i++)
	{
		AMesh mesh = meshes[i];
		if (meshManager->AddMesh(mesh.identifier.name, mesh.nrOfVerticies, mesh.vertices))
			printf("Added mesh: %s\n", mesh.identifier.name);
		else
			printf("Could not add mesh or mesh have already been added.\n");
	}
}

void LevelManager::LoadLevelMeshes(std::vector<LMesh> meshes, GLuint shader)
{
	////////////////////////////////////////////////////////////
	// Loading Meshes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < meshes.size(); i++)
	{
		Object* object = new Object();
		object->setShader(shader);

		LMesh mesh = meshes[i];
		object->Start(meshManager->getMesh(mesh.name.data), materialManager->getMaterial("groundmaterial"));
		object->setScale(glm::vec3(mesh.scale[0], mesh.scale[1], mesh.scale[2]));
		object->setRotation(glm::vec3(mesh.rotation[0], mesh.rotation[1], mesh.rotation[2]));
		object->setPosition(glm::vec3(mesh.position[0], mesh.position[1], mesh.position[2]));

		map.push_back(object);
	}
}

void LevelManager::LoadLevelHitboxes(std::vector<LHitbox> hitboxes)
{
	////////////////////////////////////////////////////////////
	// Loading Hitboxes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < hitboxes.size(); i++)
	{
		Hitbox* hitbox = new Hitbox();
		hitbox->InitializeHitbox(world, glm::vec2(levelFile.hitboxes[i].position[0], levelFile.hitboxes[i].position[1]), glm::vec2(levelFile.hitboxes[i].scale[0], levelFile.hitboxes[i].scale[1]), b2_staticBody);
		this->hitboxes.push_back(hitbox);
	}
}

void LevelManager::LoadLevelLights(std::vector<LLight> lights)
{
	////////////////////////////////////////////////////////////
	// Loading Lights
	////////////////////////////////////////////////////////////
	for (int i = 0; i < lights.size(); i++)
	{
		// temp
		lightManager->AddPointLight(glm::vec4(lights[i].position[0], lights[i].position[1], lights[i].position[2], 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1, 1, 0.01f, 0.01f);
	}
}

void LevelManager::LoadLevelSpawners(std::vector<LSpawner> spawner, GLuint shader)
{
	////////////////////////////////////////////////////////////
	// Loading Spawner
	////////////////////////////////////////////////////////////
	for (int i = 0; i < spawner.size(); i++)
	{
		LSpawner spawn = spawner[i];

		switch (spawn.spawnerType)
		{
		case ESpawnerType::flying1:		break;
		case ESpawnerType::skater1:		break;
		case ESpawnerType::trash:		break;
		case ESpawnerType::zombie1:		break;
		case ESpawnerType::zombie2:		break;

		}
	}
}

void LevelManager::LoadLevelTriggers(std::vector<LTrigger> triggers)
{
	////////////////////////////////////////////////////////////
	// Loading Triggers
	////////////////////////////////////////////////////////////
	for (int i = 0; i < triggers.size(); i++)
	{
		LTrigger trigger = triggers[i];

		Trigger* outTrigger = new Trigger();		
		Trigger::TriggerType outTriggerType;
		switch (trigger.triggerType)
		{
		case ETriggerType::door:		outTriggerType = Trigger::DOOR; break;
		case ETriggerType::deathZone:	outTriggerType = Trigger::DOOR; break;
		case ETriggerType::garbageBin:	outTriggerType = Trigger::DOOR; break;
		case ETriggerType::poster:		outTriggerType = Trigger::DOOR; break;
		}

		LHitbox hitbox = triggers[i].hitbox;
		outTrigger->InitializeTrigger(outTriggerType, world, glm::vec2(hitbox.position[0], hitbox.position[1]), glm::vec2(hitbox.scale[0], hitbox.scale[1]), trigger.data.data, true);
		
		// Adding trigger to vector
		this->triggers.push_back(outTrigger);
	}

}

void LevelManager::LoadLevelEffects(std::vector<LEffect> effects)
{
	////////////////////////////////////////////////////////////
	// Loading Effects
	////////////////////////////////////////////////////////////
	for (int i = 0; i < effects.size(); i++)
	{
		LEffect effect = effects[i];
		switch (effect.effectType)
		{
		case EEffectType::smoke: 
		//	particleManager->EffectSmoke(glm::vec3(effect.position[0], effect.position[1], effect.position[2]));
			break;
		case EEffectType::dust:
		//	particleManager->EffectDust(glm::vec3(effect.position[0], effect.position[1], effect.position[2])); 
			break;
		case EEffectType::steam:
		//	particleManager->EffectSteam(glm::vec3(effect.position[0], effect.position[1], effect.position[2]));
			break;
		}
	}
}

void LevelManager::LoadTempLevel(GLuint shader)
{
	////////////////////////////////////////////////////////////
	// Level Music
	////////////////////////////////////////////////////////////
	soundManager->playSong(SoundManager::SONG::MUSIC_BOOGIE);

	////////////////////////////////////////////////////////////
	// Map Visuals
	////////////////////////////////////////////////////////////

	// Background
	Object* background = new Object();
	background->setShader(shader);
	background->Start(nullptr, materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(0, 20, -10));
	map.push_back(background);

	// Ground
	background = new Object();
	background->setShader(shader);
	background->Start(nullptr, materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(0, 1, 0));
	background->setRotation(glm::vec3(1.5 * 3.14, 0, 0));
	map.push_back(background);

	// Right wall
	background = new Object();
	background->setShader(shader);
	background->Start(nullptr, materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(39, 20, 0));
	background->setRotation(glm::vec3(0, 1.5 * 3.14, 0));
	map.push_back(background);

	// Left wall
	background = new Object();
	background->setShader(shader);
	background->Start(nullptr, materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(-39, 20, 0));
	background->setRotation(glm::vec3(0, -1.5 * 3.14, 0));
	map.push_back(background);

	// Left platform
	background = new Object();
	background->setShader(shader);
	background->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(8, 5, 3));
	background->setPosition(glm::vec3(-10, 0, 0));
	map.push_back(background);

	// Right platform cave
	background = new Object();
	background->setShader(shader);
	background->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(10.f, 15.f, 1));
	background->setPosition(glm::vec3(20, 8.5, 0));
	background->setRotation(glm::vec3(-45, 0, 0));
	map.push_back(background);

	////////////////////////////////////////////////////////////
	// Fun boxes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < 100; i++)
	{
		Entity* moveble = new Entity();
		moveble->setShader(shader);
		moveble->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("groundmaterial"), world, glm::vec2((rand() % 40) - 20, (rand() % 40)), glm::vec3(0.5f, 0.5f, 0.5f), b2_dynamicBody, b2Shape::e_polygon, false, 1.f, 0.5f);
		map.push_back(moveble);
	}

	////////////////////////////////////////////////////////////
	// Map Collision (Invisible)
	////////////////////////////////////////////////////////////
	Hitbox* hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(0, 0), glm::vec2(40.f, 1), b2_staticBody);	 // ground
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(20, 8.5f), glm::vec2(10.f, 1), b2_staticBody);	// platform
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(-40, 0), glm::vec2(1.f, 20.f), b2_staticBody);	// left wall
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(40, 0), glm::vec2(1.f, 20.f), b2_staticBody);	// right wall
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(-10, 0), glm::vec2(8.f, 5.f), b2_staticBody);	// dammsugare
	hitboxes.push_back(hitbox);

	////////////////////////////////////////////////////////////
	// Action Triggers
	////////////////////////////////////////////////////////////
	Trigger* trigger = new Trigger();
	trigger->InitializeTrigger(Trigger::EFFECT, world, glm::vec2(10, 20), glm::vec2(1.f, 1.f));
	triggers.push_back(trigger);

	trigger = new Trigger();
	trigger->InitializeTrigger(Trigger::SPAWN, world, glm::vec2(-10, 15), glm::vec2(1.f, 1.f));
	triggers.push_back(trigger);

	// Triggers visual
	background = new Object();
	background->setShader(shader);
	background->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("lightmaterial"));
	background->setScale(glm::vec3(1, 1, 1));
	background->setPosition(glm::vec3(10, 20, 0));
	background->setRotation(glm::vec3(0, 0, 0));
	map.push_back(background);

	// Trigger visual
	background = new Object();
	background->setShader(shader);
	background->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("lightmaterial"));
	background->setScale(glm::vec3(1, 1, 1));
	background->setPosition(glm::vec3(-10, 15, 0));
	background->setRotation(glm::vec3(0, 0, 0));
	map.push_back(background);

	////////////////////////////////////////////////////////////
	// Lights
	////////////////////////////////////////////////////////////
	Object* light = new Object();
	light->setShader(shader);
	light->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("lightmaterial"));
	light->setPosition(glm::vec3(-20, 10, 15));
	map.push_back(light);

	light = new Object();
	light->setShader(shader);
	light->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("lightmaterial"));
	light->setPosition(glm::vec3(20, 10, 15));
	map.push_back(light);

	// Temp lights
	lightManager->AddPointLight(glm::vec4(-20, 10, 5, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1, 1, 0.01f, 0.01f);
	lightManager->AddPointLight(glm::vec4(20, 10, 5, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1, 1, 0.01f, 0.01f);

	lightManager->AddDirectionalLight(glm::vec4(5, 20, 20, 1), glm::vec4(-0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 0.1f);
	lightManager->AddDirectionalLight(glm::vec4(-5, 20, 20, 1), glm::vec4(0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 0.1f);
	lightManager->AddDirectionalLight(glm::vec4(0, 20, 20, 1), glm::vec4(0.f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 0.1f);
}

void LevelManager::Update(GLint deltaT)
{
	// Updating player
	player->Update(deltaT);

	//Update Projectile
	//myPH->Update(deltaT, world);

	//myProjectile->Update(deltaT, world, player->getPlayerPosAsGLM());
	
	//moveble->Update(deltaT);

	// Updating enemies
	enemy->Update(deltaT, player->getBody()->GetPosition());

	// Updating physics
	world->Step(1 / 60.f, 3, 3);

	// Updating every object on map
	/*for (Object* object : map)
		object->Update(deltaT);*/

	for (Object* object : map)
		object->Update(deltaT);

	// Updating triggers and checking for collisions
	for (Trigger* trigger : triggers)
		if (!trigger->getIsTriggered())	
			trigger->CheckCollision(player->getBody());
	
	// Checking triggers
	CheckTriggers();
}

void LevelManager::CheckTriggers()
{
	for (Trigger* trigger : triggers)
	{
		if (trigger->getIsTriggered())
		{
			switch (trigger->triggerType)
			{

			////////////////////////////////////////////////////////////
			// Door - The player switches level
			////////////////////////////////////////////////////////////
			case Trigger::DOOR:	

				// Checks if the door have a level file
				if (!trigger->getData().empty())
				{
					// Loads new level with the current player's shader
					LoadLevel(player->getShader(), trigger->getData(), trigger->getData());
				} 
				break;

			////////////////////////////////////////////////////////////
			// Popup - For popups with text/pictures, anything
			////////////////////////////////////////////////////////////
			case Trigger::POPUP:				
				break;

			////////////////////////////////////////////////////////////
			// Push - Move an entity with a force
			////////////////////////////////////////////////////////////
			case Trigger::PUSH:		
				break;

			////////////////////////////////////////////////////////////
			// Effect - Starts an particle effect
			////////////////////////////////////////////////////////////
			case Trigger::EFFECT:

				// Effects
				soundManager->playModifiedSFX(SoundManager::SFX::SFX_BIP, 50, 0.5f);
				particleManager->EffectExplosionLights(glm::vec3(trigger->getPosition(), 0), 1, glm::vec4(1, 1, 1, 1));

				// Temporary effect, clear all lights and a new light
				lightManager->Clear(); 	
				lightManager->AddPointLight(glm::vec4(20, 10, 5, 1), glm::vec4(1, 1, 0.25f, 1), glm::vec4(1, 1, 1, 1), 1, 1, 0.01f, 0.001f);
				break;

			////////////////////////////////////////////////////////////
			// SFX - Plays a sound effect
			////////////////////////////////////////////////////////////
			case Trigger::SFX:					
				break;

			////////////////////////////////////////////////////////////
			// Spawn Trigger - Spawns anything, anywhere, (currently boxes)
			////////////////////////////////////////////////////////////
			case Trigger::SPAWN:

				// Temporary sound effect
				soundManager->playSFX(SoundManager::SFX::SFX_POWERUP);
			{
				Entity* moveble = new Entity();
				moveble->setShader(map[0]->getShader());
				moveble->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("backgroundmaterial"), world, glm::vec2((rand() % 40) - 20, (rand() % 40)), glm::vec3(randBetweenF(0.30f, 0.70f), randBetweenF(0.30f, 0.70f), randBetweenF(0.30f, 0.70f)), b2_dynamicBody, b2Shape::e_polygon, false, 1.f, 1.f);
				map.push_back(moveble);
			}

				break;


			////////////////////////////////////////////////////////////
			// Save - Save station for the player, saves the game
			////////////////////////////////////////////////////////////
			case Trigger::SAVE:
				break;

			////////////////////////////////////////////////////////////
			// Cutscene - Switches screen to cutscene and plays a cutscene
			////////////////////////////////////////////////////////////
			case Trigger::CUTSCENE:				
				break;
			}

			// Trigger is now reactivated
			trigger->setIsTriggered(false);
		}
	}
}

std::vector<Object*> LevelManager::getMap()
{
	return map;
}


//void LevelManager::shoot(GLuint shader, std::string modelPath)
//{
//	moveble = new Projectile(world, shader);
//	//moveble->setShader(shader);
//	//moveble->Start(meshManager->getMesh("quad"), materialManager.getMaterial("lightmaterial"), world, glm::vec2(0, 0), glm::vec2(0.5f, 0.5f), b2_dynamicBody, b2Shape::e_polygon, 1.f, 0.5f);
//	//moveble->setScale(glm::vec3(0.5f, 0.5f, 1));
//	map.push_back(moveble);
//}

const LightManager* LevelManager::getLightManager() const {	return lightManager; }
Player* LevelManager::getPlayer() { return player; }
Enemy* LevelManager::getEnemy() { return enemy; }
//ProjectileHandler* LevelManager::getProjectiles() { return myPH; }
Projectile* LevelManager::getProjectile() { return myProjectile; }
