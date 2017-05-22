#include "LevelManager.h"

LevelManager::LevelManager() 
{
	this->materialManager = nullptr;
	this->meshManager = nullptr;
	this->particleManager = nullptr;
	this->soundManager = nullptr;
	this->quadTree = nullptr;
	this->entityManager = nullptr;
	this->camera = nullptr;
}

LevelManager::LevelManager(const LevelManager & other) { }

LevelManager::~LevelManager() { }

bool LevelManager::Start(glm::vec2 screenSize, GLuint playerShader, GLuint mapShader, GLuint guiShader, MaterialManager* materialManager, MeshManager* meshManager, ParticleManager* particleManager, SoundManager* soundManager, Camera* camera)
{
	// Getting parameters
	this->materialManager = materialManager;
	this->meshManager = meshManager;
	this->particleManager = particleManager;
	this->soundManager = soundManager;
	this->camera = camera;
	this->playerShader = playerShader;
	this->mapShader = mapShader;
	this->guiShader = guiShader;

	// Popup Settings
	popup = new Text();
	if (popup == nullptr) return false;
	if (!popup->Start(screenSize, FONT_PATH INGAME_FONT, 60.f, glm::vec3(0, 0.5f, 0))) return false;
	popup->setShader(guiShader);
	popup->CreateText("Popup!", glm::vec4(0.f));
	popupAlpha = 0.0f;
	popupActive = false;

	// Starting contact manager
	contactManager.Start(particleManager, soundManager, camera);

	// Starting world 
	world = new b2World(b2Vec2(NULL, GRAVITY * GRAVITY_SCALE));
	if (world == nullptr) return false;
	world->SetContactListener(&contactManager);
	world->SetAllowSleeping(true);

	// Starting light Manager
	lightManager = new LightManager();
	if (lightManager == nullptr) return false;

	////////////////////////////////////////////////////////////
	// Creating the Player object
	////////////////////////////////////////////////////////////
	GLint tempNomralMapIndex = materialManager->AddTexture("playernormalmap", TEXTURE_PATH "Walk01_nor.png");

	//Test with a player who has a gun too fire with
	player = new Player();
	if (player == nullptr) return false;
	if (!player->Start(meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial"), world))
		return false;
	player->setShader(playerShader);
	player->AddAnimation(materialManager->getMaterial("playermaterial")->getTextureID(), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt");

	////////////////////////////////////////////////////////////
	// Creating the Entity Manager (Enemies/Trash/etc)
	////////////////////////////////////////////////////////////
	entityManager = new EntityManager();
	if (entityManager == nullptr) return false;
	if (!entityManager->Start(world, screenSize)) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::zombie1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial")->getTextureID(), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::zombie2, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial")->getTextureID(), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::skater1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial")->getTextureID(), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::flying1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial")->getTextureID(), materialManager->getTextureID(tempNomralMapIndex), ANIMATION_PATH "testanimationnormalmap.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::trash, mapShader, meshManager->getMesh("quad"), materialManager->getMaterial("groundmaterial"))) return false;

	////////////////////////////////////////////////////////////
	// Creating the Quad Tree Object
	////////////////////////////////////////////////////////////
	quadTree = new QuadTree();
	if (quadTree == nullptr) return false;
	if (!quadTree->Start(screenSize)) return false;



	this->myPH = new ProjectileHandler(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"), world, player->getPlayerPosAsGLM(), mapShader);
	this->wasPressed = false;
	this->isPressed = false;

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

	// Deleting quadtree
	if (quadTree != nullptr)
	{
		quadTree->Stop();
		delete quadTree;
		quadTree = nullptr;
	}

	// Unloads every entity on map
	if (entityManager != nullptr)
	{
		entityManager->Stop();
		delete entityManager;
		entityManager = nullptr;
	}

	// Unloads the map objects
	StopMap();

	// Removes the world object
	if (world != nullptr)
	{
		world->Dump();
		delete world;
		world = nullptr;
	}

	// Unloads light manager
	lightManager->Clear();
	delete lightManager;

	// These are getting removed somewhere else
	materialManager = nullptr;
	meshManager = nullptr;
	particleManager = nullptr;
	soundManager = nullptr;
	camera = nullptr;
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
	map.clear();

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
	hitboxes.clear();
	
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
	triggers.clear();

	// Unloads every projectile on the map
	for (Projectile* projectile : projectiles)
	{
		if (projectile != nullptr)
		{
			projectile->Stop();
			delete projectile;
			projectile = nullptr;
		}
	}
	projectiles.clear();
}

void LevelManager::Update(GLint deltaT)
{
	// Updating player
	player->Update(deltaT, world, player->getPlayerPosAsGLM());
	if (player->getIsDashing()) particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 1.5, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 10, glm::vec4(0.25f));
	if (player->getIsHovering()) particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 2, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 1, glm::vec4(0.25f));


	//For projectiles
	isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	if (isPressed && !wasPressed && player->getCanShoot() == true)
	{
		wasPressed = true;
		player->decreaseNrOfProjectiles();
		myPH->fireProjectiles(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"), world, player->getPlayerPosAsGLM());
	}

	//Update Projectile
	myPH->Update(deltaT, world, player->getPlayerPosAsGLM());

	//myProjectile->Update(deltaT, world, player->getPlayerPosAsGLM());

	// Updating every entity
	entityManager->Update(deltaT, player->getPosition());

	// Updating physics
	world->Step(1 / 60.f, 10, 20);

	// Updating every object on map
	//deleteProjects(world);

	for (Projectile* proj : projectiles)
		proj->Update(deltaT, world, player->getPlayerPosAsGLM());

	for (Object* object : map)
		object->Update(deltaT);

	// Updating triggers and checking for collisions
	for (Trigger* trigger : triggers)
		if (!trigger->getIsTriggered())
			trigger->CheckCollision(player->getBody());

	// Updating UI popup text
	if (popupActive)
	{
		glm::vec4 color = popup->getColor();
		float currentAlpha = color.a;
		currentAlpha += (popupAlpha - currentAlpha) * 0.035f;
		popup->setColor(glm::vec4(currentAlpha));
		popupTimer -= deltaT;
		if (popupTimer < NULL)	popupAlpha = -0.05f;
		else if (currentAlpha < 0.f) popupActive = false;
	}

	// Checking triggers
	CheckTriggers();

	//Resets variables for projectileHandler
		this->wasPressed = isPressed;
}

void LevelManager::ActivatePopup(std::string text, GLfloat timer)
{
	popup->setString(text);
	popupActive = true;
	popupAlpha = 1.f;
	popupTimer = timer;
}

bool LevelManager::LoadLevel(std::string levelPath, std::string archivePath)
{
	// Unload current level
	StopMap();

	////////////////////////////////////////////////////////////
	// Loading Archive
	////////////////////////////////////////////////////////////
	archive.readFromFile(archivePath.c_str());
	LoadArchiveTextures(archive.textures);
	LoadArchiveMaterials(archive.materials);
	LoadArchiveMeshes(archive.meshes);

	////////////////////////////////////////////////////////////
	// Loading Level
	////////////////////////////////////////////////////////////
	levelFile.readFromFile(levelPath.c_str());
	LoadLevelMeshes(levelFile.meshes);
	LoadLevelLights(levelFile.lights);
	LoadLevelHitboxes(levelFile.hitboxes);
	LoadLevelSpawners(levelFile.spawners);
	LoadLevelTriggers(levelFile.triggers);
	LoadLevelEffects(levelFile.effects);
	
	// Setting start position
	player->setPosition(glm::vec3(arrayToVec2(levelFile.levelHeader.playerSpawn), 0));

	// Music
	soundManager->playSong(SoundManager::SONG::MUSIC_BOOGIE);
	
	// Dust effect
	particleManager->EffectLightDust(glm::vec3(0, 10, 0));

	// Temp directional light for shadows
	lightManager->AddDirectionalLight(glm::vec4(5, 20, 10, 1), glm::vec4(-0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 0.3f);
	//lightManager->AddDirectionalLight(glm::vec4(-5, 20, 20, 1), glm::vec4(0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1.f);
	//lightManager->AddDirectionalLight(glm::vec4(0, 20, 20, 1), glm::vec4(0.f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1.f);

	// Loading temp level
	//LoadTempLevel(shader);

	return true;
}

void LevelManager::LoadArchiveMeshes(std::vector<AMesh> meshes)
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

void LevelManager::LoadArchiveMaterials(std::vector<AMaterial> materials)
{
	////////////////////////////////////////////////////////////
	// Loading Archive Meshes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < materials.size(); i++)
	{
		AMaterial material = materials[i];
		if (archive.getTexture(material.diffuseMap.uid))
		{
			if (material.materialType == EMaterialType::eMaterialPhong)
				materialManager->AddMaterial(material.identifier.name, arrayToVec3(material.ambient), arrayToVec3(material.diffuse), arrayToVec3(material.specular), material.shininess, material.diffuseMap.name, archive.getTexture(material.diffuseMap.uid)->texturePath /*"temp_background.jpg"*/);
			else
				materialManager->AddMaterial(material.identifier.name, arrayToVec3(material.ambient), arrayToVec3(material.diffuse), glm::vec3(0, 0, 0), 1.f, material.diffuseMap.name, archive.getTexture(material.diffuseMap.uid)->texturePath /*"temp_background.jpg"*/);
		}
		else
		{
			if (material.materialType == EMaterialType::eMaterialPhong)
				materialManager->AddMaterial(material.identifier.name, arrayToVec3(material.ambient), arrayToVec3(material.diffuse), arrayToVec3(material.specular), material.shininess, material.diffuseMap.name, TEXTURE_PATH "MissingTexture.png");
			else
				materialManager->AddMaterial(material.identifier.name, arrayToVec3(material.ambient), arrayToVec3(material.diffuse), glm::vec3(0, 0, 0), 1.f, material.diffuseMap.name, TEXTURE_PATH "MissingTexture.png");
		}
	}
}

void LevelManager::LoadArchiveTextures(std::vector<ATexture> textures)
{
	////////////////////////////////////////////////////////////
	// Loading Archive Meshes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < textures.size(); i++)
	{
		ATexture texture = textures[i];	
		materialManager->AddTexture(texture.identifier.name, texture.texturePath);
	}
}

void LevelManager::LoadLevelMeshes(std::vector<LMesh> meshes)
{
	////////////////////////////////////////////////////////////
	// Loading Meshes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < meshes.size(); i++)
	{
		Object* object = new Object();
		object->setShader(mapShader);

		LMesh* mesh = &meshes[i];

		if (meshManager->getMesh(mesh->name.data) != nullptr)
			printf("Loading Object with mesh: %s\n", mesh->name.data);
		else printf("Could not find mesh: %s\n", mesh->name.data);

		object->Start(meshManager->getMesh(mesh->name.data), materialManager->getMaterial(mesh->name.data));
		object->setScale(glm::vec3(mesh->scale[0], mesh->scale[1], mesh->scale[2]));
		object->setRotation(glm::vec3(glm::radians(mesh->rotation[0]), glm::radians(mesh->rotation[1]), glm::radians(mesh->rotation[2])));
		object->setPosition(glm::vec3(mesh->position[0], mesh->position[1], mesh->position[2]));

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
		ALight* light = archive.getLight(lights[i].name.data);
		if(light->decayType == EDecayType::eLinear)
			lightManager->AddPointLight(glm::vec4(arrayToVec3(lights[i].position), 1), glm::vec4(arrayToVec3(light->color), 1), glm::vec4(1, 1, 1, 1), light->intensity * 0.1f, 1, 10.f, 1.f);
		else if(light->decayType == EDecayType::eQuadratic)
			lightManager->AddPointLight(glm::vec4(arrayToVec3(lights[i].position), 1), glm::vec4(arrayToVec3(light->color), 1), glm::vec4(1, 1, 1, 1), light->intensity * 0.1f, 1, 1.f, 10.f);
	}
}

void LevelManager::LoadLevelSpawners(std::vector<LSpawner> spawner)
{
	////////////////////////////////////////////////////////////
	// Loading Enemies / Trash / etc
	////////////////////////////////////////////////////////////
	for (int i = 0; i < spawner.size(); i++)
	{
		LSpawner spawn = spawner[i];
		entityManager->SpawnEntity(spawn.spawnerType, arrayToVec2(spawn.position));
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
		case ETriggerType::poster:		outTriggerType = Trigger::POSTER; break;
		case ETriggerType::deathZone:	outTriggerType = Trigger::DEATH; break;
		case ETriggerType::garbageBin:	outTriggerType = Trigger::EFFECT; break;
		case ETriggerType::door:		
			outTriggerType = Trigger::POSTER; 
			Poster* poster = new Poster();
			poster->setShader(mapShader);
			poster->Start(meshManager->getMesh("quad"), materialManager->getMaterial("postermaterial_2"));
			LHitbox hitbox = triggers[i].hitbox;
			poster->setScale(glm::vec3(hitbox.scale[0], hitbox.scale[1], 1));
			poster->setPosition(glm::vec3(hitbox.position[0], hitbox.position[1] + hitbox.scale[1] / 2, 0));
			map.push_back(poster);
			outTrigger->setMapPart(poster);
			break; 
		}

		LHitbox hitbox = triggers[i].hitbox;
		outTrigger->InitializeTrigger(outTriggerType, world, glm::vec2(hitbox.position[0], hitbox.position[1]), glm::vec2(hitbox.scale[0], hitbox.scale[1]), trigger.data.data, true);
		
		// Adding trigger to vector
		this->triggers.push_back(outTrigger);

		// Adding a constant smoke on trigger for testing
		particleManager->EffectConstantSmoke(glm::vec3(outTrigger->getPosition(), 0.f), materialManager->getTextureID("smoketexture"), 10, glm::vec4(0.3f));
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
			particleManager->EffectConstantSmoke(glm::vec3(effect.position[0], effect.position[1], effect.position[2]), materialManager->getTextureID("smoketexture"));
			break;
		case EEffectType::dust:
			particleManager->EffectConstantSmoke(glm::vec3(effect.position[0], effect.position[1], effect.position[2]), materialManager->getTextureID("smoketexture"), 10, glm::vec4(0.40f, 0.3f, 0.3f, 0.7f));
			break;
		case EEffectType::steam:
			particleManager->EffectConstantSmoke(glm::vec3(effect.position[0], effect.position[1], effect.position[2]), materialManager->getTextureID("smoketexture"), 10, glm::vec4(0.3f, 0.30f, 0.5f, 0.7f));
			break;
		}
	}
}

void LevelManager::LoadTempLevel()
{
	////////////////////////////////////////////////////////////
	// Level Music
	////////////////////////////////////////////////////////////
	soundManager->playSong(SoundManager::SONG::MUSIC_BOOGIE);

	////////////////////////////////////////////////////////////
	// Map Visuals
	////////////////////////////////////////////////////////////
	// Dammsugare in the middle of the screen
	Entity* box = new Entity();
	box->setShader(mapShader);
	box->Start(meshManager->getMesh("pCube"), materialManager->getMaterial("lightmaterial"), world, glm::vec2(-10, 0), glm::vec3(8.f, 5.f, 0.5f), b2_staticBody);
	box->setScale(glm::vec3(8, 5, 3));
	map.push_back(box);

	// Background
	Object* background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(0, 10, -5));
	map.push_back(background);

	// Ground
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(0, 0.5f, 0));
	background->setRotation(glm::vec3(1.5 * 3.14, 0, 0));
	map.push_back(background);

	// Right wall
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(19, 10, 0));
	background->setRotation(glm::vec3(0, 1.5 * 3.14, 0));
	map.push_back(background);

	// Left wall
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(40, 20, 1));
	background->setPosition(glm::vec3(-19, 10, 0));
	background->setRotation(glm::vec3(0, -1.5 * 3.14, 0));
	map.push_back(background);

	// Left platform
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(8, 5, 3));
	background->setPosition(glm::vec3(-5, 0, 0));
	map.push_back(background);

	// Right platform cave
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("groundmaterial"));
	background->setScale(glm::vec3(10.f, 15.f, 1));
	background->setPosition(glm::vec3(10, 4.25, 0));
	background->setRotation(glm::vec3(-45, 0, 0));
	map.push_back(background);

	////////////////////////////////////////////////////////////
	// Fun boxes
	////////////////////////////////////////////////////////////
	for (int i = 0; i < 25; i++)
	{
		Entity* moveble = new Entity();
		moveble->setShader(mapShader);
		moveble->Start(meshManager->getMesh("quad"), materialManager->getMaterial("groundmaterial"), world, glm::vec2((rand() % 20) - 10, (rand() % 20)), glm::vec3(0.5f, 0.5f, 0.5f), b2_dynamicBody, b2Shape::e_polygon, false, 1.5f, 0.4f);
		map.push_back(moveble);
	}

	////////////////////////////////////////////////////////////
	// Map Collision (Invisible)
	////////////////////////////////////////////////////////////
	Hitbox* hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(0, 0), glm::vec2(40.f, 1), b2_staticBody);	 // ground
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(10, 4.25f), glm::vec2(10.f, 1), b2_staticBody);	// platform
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(-19, 10), glm::vec2(1.f, 20.f), b2_staticBody);	// left wall
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(19, 10), glm::vec2(1.f, 20.f), b2_staticBody);	// right wall
	hitboxes.push_back(hitbox);
	hitbox = new Hitbox();
	hitbox->InitializeHitbox(world, glm::vec2(-5, 0), glm::vec2(8.f, 5.f), b2_staticBody);	// dammsugare
	hitboxes.push_back(hitbox);

	////////////////////////////////////////////////////////////
	// Action Triggers
	////////////////////////////////////////////////////////////
	Trigger* trigger = new Trigger();
	trigger->InitializeTrigger(Trigger::EFFECT, world, glm::vec2(5, 10), glm::vec2(1.f, 1.f));
	triggers.push_back(trigger);

	trigger = new Trigger();
	trigger->InitializeTrigger(Trigger::SPAWN, world, glm::vec2(-5, 7.5), glm::vec2(1.f, 1.f));
	triggers.push_back(trigger);

	// Triggers visual
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	background->setScale(glm::vec3(1, 1, 1));
	background->setPosition(glm::vec3(5, 10, 0));
	background->setRotation(glm::vec3(0, 0, 0));
	map.push_back(background);

	// Trigger visual
	background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	background->setScale(glm::vec3(1, 1, 1));
	background->setPosition(glm::vec3(-5, 7.5, 0));
	background->setRotation(glm::vec3(0, 0, 0));
	map.push_back(background);

	////////////////////////////////////////////////////////////
	// Lights
	////////////////////////////////////////////////////////////
	Object* light = new Object();
	light->setShader(mapShader);
	light->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	light->setPosition(glm::vec3(-15, 1.5, -2.5));
	map.push_back(light);

	light = new Object();
	light->setShader(mapShader);
	light->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	light->setPosition(glm::vec3(5, 7, 0));
	map.push_back(light);

	// Temp lights
	lightManager->AddPointLight(glm::vec4(-15, 1.5, -2.5, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 5, 1, 0.5f, 0.5f);
	lightManager->AddPointLight(glm::vec4(5, 7, 0, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1, 1, 0.01f, 0.01f);

}

//void LevelManager::Update(GLint deltaT)
//{
//	// Updating player
//	player->Update(deltaT, world, player->getPlayerPosAsGLM());
//	if (player->getIsDashing()) particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 1.5, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 10, glm::vec4(0.25f));
//	if (player->getIsHovering()) particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 2, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 1, glm::vec4(0.25f));
//
//
//	
//
//	//Update Projectile
//	myPH->Update(deltaT, world, player->getPlayerPosAsGLM());
//	
//
//	//meshManager->getMesh("quad");
//
//	// Updating enemies
////	enemy->Update(deltaT, player->getBody()->GetPosition());
//
//	// Updating physics
//	world->Step(1 / 60.f, 10, 20);
//
//
//	 //for (Projectile* proj : projectiles)
//		 //proj->Update(deltaT, world, player->getPlayerPosAsGLM());
//
//	/* for (Projectile* proj : projectiles)
//		 proj->Update(deltaT, world, player->getPlayerPosAsGLM());*/
//
//	for (Object* object : map)
//		object->Update(deltaT);
//
//	// Updating triggers and checking for collisions
//	for (Trigger* trigger : triggers)
//		if (!trigger->getIsTriggered())	
//			trigger->CheckCollision(player->getBody());
//	
//	// Checking triggers
//	CheckTriggers();
//
//	//Resets variables for projectileHandler
//	this->wasPressed = isPressed;
//}

void LevelManager::CheckTriggers()
{
	for (int i = 0; i < triggers.size(); i++)
	{
		bool remove = false;
		Trigger* trigger = triggers[i];
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
					LoadLevel(trigger->getData(), trigger->getData());
				} 
				break;

			////////////////////////////////////////////////////////////
			// Popup - For popups with text/pictures, anything
			////////////////////////////////////////////////////////////
			case Trigger::POPUP:				
				break;

			////////////////////////////////////////////////////////////
			// Poster - Unlockables
			////////////////////////////////////////////////////////////
			case Trigger::POSTER:
				remove = true;
				particleManager->EffectExplosionLights(glm::vec3(trigger->getPosition(), 0), 50, glm::vec4(0.25, 1, 0.85, 1));
				UnlockPoster(2);
				ActivatePopup("You unlocked a poster!", 2000.f);
				break;

			////////////////////////////////////////////////////////////
			// Push - Move an entity with a force
			////////////////////////////////////////////////////////////
			case Trigger::PUSH:		
				break;

			////////////////////////////////////////////////////////////
			// Death - Zone in which the player instantly dies
			////////////////////////////////////////////////////////////
			case Trigger::DEATH:
				player->TakeDamage(player->getHP());
				break;

			////////////////////////////////////////////////////////////
			// Effect - Starts an particle effect
			////////////////////////////////////////////////////////////
			case Trigger::EFFECT:
				soundManager->playModifiedSFX(SoundManager::SFX::SFX_BIP, 50, 0.5f);
				particleManager->EffectExplosionLights(glm::vec3(trigger->getPosition(), 0), 1, glm::vec4(0.6f, 0.6f, 0.6f, 1));
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
				moveble->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"), world, glm::vec2((rand() % 40) - 20, (rand() % 40)), glm::vec3(randBetweenF(0.30f, 0.70f), randBetweenF(0.30f, 0.70f), randBetweenF(0.30f, 0.70f)), b2_dynamicBody, b2Shape::e_polygon, false, 2.5f, 1.f);
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

		if (remove)
		{
			Poster* poster = static_cast<Poster*>(trigger->getMapPart());
			if (poster)
			{
				for (int i = 0; i < map.size(); i++)
					if (map[i] == poster)
					{
						delete map[i];
						map.erase(map.begin() + i);
					}
			}
			trigger->Stop();
			delete trigger;
			triggers.erase(triggers.begin() + i);
		}
	}
}

void LevelManager::UnlockPoster(int index)
{
/*	std::ifstream file(ARCHIVE_PATH "game.ini");

	if (file.is_open())
	{
		std::string str;
		std::vector <std::string> vec;

		while (std::getline(file, str))
		{
			vec.push_back(str);
		}

		for (std::string& s : vec)
			printf("%s\n", s);
	}

	file.close(); */
}

std::vector<Object*> LevelManager::getMap()
{
	return map;
}

std::vector<Projectile*> LevelManager::getProjectiles()
{
	return this->myPH->getBullets();
}

const LightManager* LevelManager::getLightManager() const {	return lightManager; }
EntityManager* LevelManager::getEntityManager() { return entityManager; }
Player* LevelManager::getPlayer() { return player; }

Text* LevelManager::getPopup()
{
	return popup;
}

//ProjectileHandler* LevelManager::getProjectiles() { return myPH; }
//Projectile* LevelManager::getProjectile() { return moveble; }
