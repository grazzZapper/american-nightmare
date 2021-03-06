#include "LevelManager.h"

extern std::vector<uint16_t> unlockedPosters;

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

bool LevelManager::Start(glm::vec2 screenSize, glm::vec2 screenPos, GLuint playerShader, GLuint mapShader, GLuint guiShader, MaterialManager* materialManager, MeshManager* meshManager, ParticleManager* particleManager, SoundManager* soundManager, Camera* camera)
{
	// Refresh rate of current monitor for b2world->Step()
	DEVMODE displaySettings;
	ZeroMemory(&displaySettings, sizeof(DEVMODE));
	displaySettings.dmSize = sizeof(DEVMODE);
	displaySettings.dmDriverExtra = 0;
	
	/* int i = 0; 
	while (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &displaySettings))
	{
		printf("Screen Display Setting number: %d, with refresh rate: %d and size: %d, %d\n", i, displaySettings.dmDisplayFrequency, displaySettings.dmPelsWidth, displaySettings.dmPelsHeight);
		i++;
	} */
	
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &displaySettings);
	displayRefresh = displaySettings.dmDisplayFrequency;
	printf("Current display: with refresh rate: %d and size: %d, %d\n", displaySettings.dmDisplayFrequency, displaySettings.dmPelsWidth, displaySettings.dmPelsHeight);

	// Getting parameters
	this->materialManager = materialManager;
	this->meshManager = meshManager;
	this->particleManager = particleManager;
	this->soundManager = soundManager;
	this->camera = camera;
	this->playerShader = playerShader;
	this->mapShader = mapShader;
	this->guiShader = guiShader;
	this->screenSize = screenSize;
	this->screenPos = screenPos;

	this->nextArchivePath = "";
	this->nextLevelPath = "";
	this->nextLevelTrigger = false;
	this->nextLevelTimer = 0;
	this->nextLevelCameraTrigger = false;

	player = new Player();
	if (player == nullptr) return false;

	// Popup Settings
	popup = new Text();
	if (popup == nullptr) return false;
	if (!popup->Start(screenSize, FONT_PATH POPUP_FONT, 35.f, glm::vec3(0, 0.35f, 0))) return false;
	popup->setShader(guiShader);
	popup->CreateText("Popup!", glm::vec4(0.f));
	popupAlpha = 0.0f;
	popupActive = false;

	// Starting contact manager
	contactManager.Start(particleManager, soundManager, myPH, materialManager, meshManager, camera, player);

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
	GLint janeIdleL = materialManager->AddTexture("janeidlel", TEXTURE_PATH "janeIdleL.png");
	GLint janeIdleLN = materialManager->AddTexture("janeidleln", TEXTURE_PATH "janeIdleL_norm.png");
	GLint janeIdleR = materialManager->AddTexture("janeidler", TEXTURE_PATH "janeIdleR.png");
	GLint janeIdleRN = materialManager->AddTexture("janeidlern", TEXTURE_PATH "janeIdleR_norm.png");
	GLint janeRunL = materialManager->AddTexture("janerunl", TEXTURE_PATH "JaneRunL.png");
	GLint janeRunLN = materialManager->AddTexture("janerunln", TEXTURE_PATH "JaneNorm_L.png");
	GLint janeRunR = materialManager->AddTexture("janerunr", TEXTURE_PATH "JaneRunR.png");
	GLint janeRunRN = materialManager->AddTexture("janerunrn", TEXTURE_PATH "JaneNorm_R.png");
	GLint janeJumpL = materialManager->AddTexture("janejumpl", TEXTURE_PATH "janeJumpL.png");
	GLint janeJumpLN = materialManager->AddTexture("janejumpln", TEXTURE_PATH "janeJumpL_norm.png");
	GLint janeJumpR = materialManager->AddTexture("janejumpr", TEXTURE_PATH "janeJumpR.png");
	GLint janeJumpRN = materialManager->AddTexture("janejumprn", TEXTURE_PATH "janeJumpR_norm.png");
	GLint janeDashL = materialManager->AddTexture("janedashl", TEXTURE_PATH "janeDashL.png");
	GLint janeDashLN = materialManager->AddTexture("janedashln", TEXTURE_PATH "janeDashL_norm.png");
	GLint janeDashR = materialManager->AddTexture("janedashr", TEXTURE_PATH "janeDashR.png");
	GLint janeDashRN = materialManager->AddTexture("janedashrn", TEXTURE_PATH "janeDashR_norm.png");
	GLint janeHoverL = materialManager->AddTexture("janehoverl", TEXTURE_PATH "janeHoverL.png");
	GLint janeHoverLN = materialManager->AddTexture("janehoverln", TEXTURE_PATH "janeHoverL_norm.png");
	GLint janeHoverR = materialManager->AddTexture("janehoverr", TEXTURE_PATH "janeHoverR.png");
	GLint janeHoverRN = materialManager->AddTexture("janehoverrn", TEXTURE_PATH "janeHoverR_norm.png");
	GLint janeShockL = materialManager->AddTexture("janeshockl", TEXTURE_PATH "janeShockL.png");
	GLint janeShockLN = materialManager->AddTexture("janeshockln", TEXTURE_PATH "janeShockL_norm.png");
	GLint janeShockR = materialManager->AddTexture("janeshockr", TEXTURE_PATH "janeShockR.png");
	GLint janeShockRN = materialManager->AddTexture("janeshockrn", TEXTURE_PATH "janeShockR_norm.png");

	//Test with a player who has a gun too fire with
	if (!player->Start(meshManager->getMesh("quad"), materialManager->getMaterial("playermaterial"), materialManager->getMaterial("playermaterial"), world, particleManager, soundManager, meshManager, materialManager, camera, screenPos, screenSize))
		return false;
	player->setShader(playerShader);
	player->AddAnimation(materialManager->getTextureID(janeIdleL), materialManager->getTextureID(janeIdleLN), ANIMATION_PATH "janeidlel.txt");
	player->AddAnimation(materialManager->getTextureID(janeIdleR), materialManager->getTextureID(janeIdleRN), ANIMATION_PATH "janeidler.txt");
	player->AddAnimation(materialManager->getTextureID(janeRunL), materialManager->getTextureID(janeRunLN), ANIMATION_PATH "janerunl.txt");
	player->AddAnimation(materialManager->getTextureID(janeRunR), materialManager->getTextureID(janeRunRN), ANIMATION_PATH "janerunr.txt");
	player->AddAnimation(materialManager->getTextureID(janeJumpL), materialManager->getTextureID(janeJumpLN), ANIMATION_PATH "janejumpl.txt");
	player->AddAnimation(materialManager->getTextureID(janeJumpR), materialManager->getTextureID(janeJumpRN), ANIMATION_PATH "janejumpr.txt");
	player->AddAnimation(materialManager->getTextureID(janeDashL), materialManager->getTextureID(janeDashLN), ANIMATION_PATH "janedashl.txt");
	player->AddAnimation(materialManager->getTextureID(janeDashR), materialManager->getTextureID(janeDashRN), ANIMATION_PATH "janedashr.txt");
	player->AddAnimation(materialManager->getTextureID(janeHoverL), materialManager->getTextureID(janeHoverLN), ANIMATION_PATH "janehoverl.txt");
	player->AddAnimation(materialManager->getTextureID(janeHoverR), materialManager->getTextureID(janeHoverRN), ANIMATION_PATH "janehoverr.txt");
	player->AddAnimation(materialManager->getTextureID(janeShockL), materialManager->getTextureID(janeShockLN), ANIMATION_PATH "janeshockl.txt");
	player->AddAnimation(materialManager->getTextureID(janeShockR), materialManager->getTextureID(janeShockRN), ANIMATION_PATH "janeshockr.txt");
	player->changeActiveAnimation(0);

	////////////////////////////////////////////////////////////
	// Creating Projectile Handler
	////////////////////////////////////////////////////////////
	this->myPH = new ProjectileHandler(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"), world, soundManager, particleManager, player->getPlayerPosAsGLM(), mapShader);
	this->wasPressed = false;
	this->isPressed = false;

	////////////////////////////////////////////////////////////
	// Creating the Entity Manager (Enemies/Trash/etc)
	////////////////////////////////////////////////////////////
	GLint tempZombieNormal = materialManager->AddTexture("zombie1walknormalmap", TEXTURE_PATH "Zombie1WalkN.png");
	GLint tempSkaterNormal = materialManager->AddTexture("skaternormalmap", TEXTURE_PATH "skater_normal_L.png");

	entityManager = new EntityManager();
	if (entityManager == nullptr) return false;
	if (!entityManager->Start(world, soundManager, myPH, screenSize)) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::zombie1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("zombie1material"), materialManager->getMaterial("zombie1material")->getTextureID(), materialManager->getTextureID(tempZombieNormal), ANIMATION_PATH "zombie1walk.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::zombie2, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("zombie1material"), materialManager->getMaterial("zombie1material")->getTextureID(), materialManager->getTextureID(tempZombieNormal), ANIMATION_PATH "zombie1walk.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::skater1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("skatermaterial"), materialManager->getMaterial("skatermaterial")->getTextureID(), materialManager->getTextureID(tempSkaterNormal), ANIMATION_PATH "skater.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::flying1, playerShader, meshManager->getMesh("quad"), materialManager->getMaterial("zombie1material"), materialManager->getMaterial("zombie1material")->getTextureID(), materialManager->getTextureID(tempZombieNormal), ANIMATION_PATH "zombie1walk.txt")) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::trash, mapShader, meshManager->getMesh("quad"), materialManager->getMaterial("garbagematerial"))) return false;
	if (!entityManager->AddEntityBoard(ESpawnerType::box, mapShader, meshManager->getMesh("quad"), materialManager->getMaterial("boxmaterial"))) return false;

	////////////////////////////////////////////////////////////
	// Creating the Quad Tree Object
	////////////////////////////////////////////////////////////
	quadTree = new QuadTree();
	if (quadTree == nullptr) return false;
	if (!quadTree->Start(screenSize)) return false;

	return true;
}

void LevelManager::Stop()
{
	// Deleteing popup
	if (popup != nullptr)
	{
		popup->Stop();
		delete popup;
		popup = nullptr;
	}

	// Deleting quadtree
	if (quadTree != nullptr)
	{
		quadTree->Stop();
		delete quadTree;
		quadTree = nullptr;
	}

	// Unloads light manager
	if (lightManager != nullptr)
	{
		lightManager->Clear();
		delete lightManager;
		lightManager = nullptr;
	}

	// Unloads the map objects
	StopMap();

	// Deleting player
	if (player != nullptr)
	{
		if (player->getVac() != nullptr)
		{
			Vacuum* vac = player->getVac();
			vac->Stop();
			delete vac;
			vac = nullptr;
		}

		player->Stop();
		delete player;
		player = nullptr;
	}

	// Unloads every entity on map
	if (entityManager != nullptr)
	{
		entityManager->Stop();
		delete entityManager;
		entityManager = nullptr;
	}

	if (myPH != nullptr)
	{
		myPH->deleteProjects(world);
		delete myPH;
		myPH = nullptr;
	}

	if (world != nullptr)
	{
		b2Body* body = world->GetBodyList();
		while (body != nullptr)
		{
			b2Body* extra = body->GetNext();
			world->DestroyBody(body);
			body = extra;
		}
		delete world;
		world = nullptr;
	}

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
			hitbox->Stop(world);
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
			trigger->Stop(world);
			delete trigger;
			trigger = nullptr;
		}
	}
	triggers.clear();

	// Removes the world object
	std::vector<Projectile*> p = *myPH->getBullets();
	for (int i = 0; i < p.size(); i++)
		p[i]->setmarked(true);
	myPH->deleteProjects(world);
}

void LevelManager::Update(GLint deltaT)
{
	// Updating physics
	//world->Step(deltaT * 0.001f, 10, 10);
	world->Step(1.f / displayRefresh, 10, 10);

	// Updating player
	player->Update(deltaT, world);
	if (player->getIsDashing()) 
	{
		camera->screenShake(300.f, 0.5f);
	//	particleManager->EffectSmokeInDirection(player->getPosition() - glm::vec3(0, player->getScale().y / 1.5, 0), materialManager->getMaterial("smokematerial")->getTextureID(), (player->getIsFacingRight()) ? 180 : 0);
		particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 1.5, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 15, glm::vec4(0.85f));
	}
	if (player->getIsHovering()) 
	{
		particleManager->EffectSmokeCloud(player->getPosition() - glm::vec3(0, player->getScale().y / 2, 0), materialManager->getMaterial("smokematerial")->getTextureID(), 1, glm::vec4(0.85f));
	}

	//For projectiles
	if (!player->getIsDead())
	{
		if (sf::Joystick::isConnected(0))
		{
			isPressed = sf::Joystick::isButtonPressed(0, BTN_RB);
			if (!player->getIsHovering() && !player->getCanShoot() && isPressed && !wasPressed) soundManager->playSFXOverDrive(SoundManager::SFX_EMPTY, 25, 0.15f);
			if (!player->getIsHovering() && isPressed && !wasPressed && player->getCanShoot() == true)
			{
				soundManager->playSFXOverDrive(SoundManager::SFX::SFX_FIRE, 30, 0.1f);
				wasPressed = true;
				player->decreaseNrOfProjectiles();
				camera->screenShake(150.0f, 0.05f);

				myPH->fireProjectiles(player->popProjectile(), world, player->getPlayerPosAsGLM(), player->getHasJumped(), player->getFireDirection());
			}
		}
		else
		{
			isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
			if (!player->getIsHovering() && !player->getCanShoot() && isPressed && !wasPressed) soundManager->playSFXOverDrive(SoundManager::SFX_EMPTY, 25, 0.15f);
			if (!player->getIsHovering() && isPressed && !wasPressed && player->getCanShoot() == true)
			{
				soundManager->playSFXOverDrive(SoundManager::SFX::SFX_FIRE, 30, 0.1f);
				wasPressed = true;
				player->decreaseNrOfProjectiles();
				camera->screenShake(250.0f, 0.5f);
				myPH->fireProjectiles(player->popProjectile(), world, player->getPlayerPosAsGLM(), player->getHasJumped(), player->getFireDirection());
			}
		}
	}

	//Update Projectile
	myPH->Update(deltaT, world, player->getPlayerPosAsGLM(), player->getAmmoFull());

		// Updating every entity
		entityManager->Update(deltaT, player->getPosition(), player->getIsDead(), world);


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
		currentAlpha += (popupAlpha - currentAlpha) * 0.1f;
		popup->setColor(glm::vec4(currentAlpha));
		popupTimer -= deltaT;
		if (currentAlpha > 0.1f && (rand() % 5) == 1) particleManager->EffectMusicLines(player->getPosition(), player->getIsFacingRight() ? glm::pi<float>() : 0, 0.013, 1);
		if (popupTimer < NULL)	popupAlpha = -0.05f;
		else if (currentAlpha <= 0.0f) popupActive = false;
	}

	if (nextLevelCameraTrigger)
	{
		nextLevelTimer -= deltaT;
		if (nextLevelTimer < 0.f)
		{
			nextLevelTrigger = true;
			nextLevelCameraTrigger = false;
		}
	}

	// Checking triggers
	CheckTriggers();

	//Resets variables for projectileHandler
	this->wasPressed = isPressed;
}

void LevelManager::ActivatePopup(std::string text, GLfloat timer)
{
	soundManager->playModifiedSFX(SoundManager::SFX_STATIC, 100, 0.f);
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
	// Loading Level
	////////////////////////////////////////////////////////////
	levelFile.readFromFile(levelPath.c_str());

	// Loading archive from level
	archive.readFromFile((ARCHIVE_PATH + std::string(levelFile.archives[0].data)).c_str()); // Remove under row and uncomment this when updated level file is uploaded
//	archive.readFromFile(archivePath.c_str());
	LoadArchiveTextures(archive.textures);
	LoadArchiveMaterials(archive.materials);
	LoadArchiveMeshes(archive.meshes);

	// Loading current level stuff
	LoadLevelMeshes(levelFile.meshes);
	LoadLevelLights(levelFile.lights);
	LoadLevelHitboxes(levelFile.hitboxes);
	LoadLevelSpawners(levelFile.spawners);
	LoadLevelTriggers(levelFile.triggers);
	LoadLevelEffects(levelFile.effects);

	// Setting start position
	glm::vec3 start = glm::vec3(levelFile.levelHeader.playerSpawn[0], levelFile.levelHeader.playerSpawn[1] - 5, 0);
	player->setPosition(start);
	player->setStartingPosition(start);

	// Music
//	soundManager->playSong(SoundManager::SONG::JAZZY_INTERLUDE);
	soundManager->playSong(SoundManager::SONG::SING_SING_SING);
	soundManager->playSFXOverDrive(SoundManager::SFX::SFX_BIRDS, 90.f, 0);
	
	// Dust effect
	particleManager->EffectLightDust(glm::vec3(50, 7.5, 0), glm::vec3(100, 15, 0), 125, glm::vec4(0.67f), 0.07f);

	// Temp directional light for shadows
	//lightManager->AddDirectionalLight(glm::vec4(5, 20, 10, 1), glm::vec4(-0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1.f);
	//lightManager->AddDirectionalLight(glm::vec4(-5, 20, 20, 1), glm::vec4(0.5f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1.f);
	//lightManager->AddDirectionalLight(glm::vec4(0, 20, 20, 1), glm::vec4(0.f, -0.5f, -1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), 1.f);



	Object* background = new Object();
	background->setShader(mapShader);
	background->Start(meshManager->getMesh("quad"), materialManager->getMaterial("backgroundmaterial"));
	background->setScale(glm::vec3(1024, 128, 1));
	background->setPosition(glm::vec3(254, 64, -130));
	map.push_back(background);

	/*Object* test = new Object();
	test->setShader(mapShader);
	test->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	test->setScale(glm::vec3(1, 1, 1));
	test->setPosition(glm::vec3(-5, 3, 2));
	map.push_back(test);

	Object* test2 = new Object();
	test2->setShader(mapShader);
	test2->Start(meshManager->getMesh("quad"), materialManager->getMaterial("lightmaterial"));
	test2->setScale(glm::vec3(1, 1, 1));
	test2->setPosition(glm::vec3(5, 3, 2));
	map.push_back(test2);

	lightManager->Clear();
	lightManager->AddPointLight(glm::vec4(-5, 3, 2, 1), glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 1, 1), 100, 1, 10.f, 1.f);
	lightManager->AddPointLight(glm::vec4(5, 3, 2, 1), glm::vec4(0, 0, 1, 1), glm::vec4(1, 1, 1, 1), 100, 1, 10.f, 1.f);*/

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
		const MaterialManager::Material* material = materialManager->getMaterial(archive.getMesh(mesh->name.data)->material.name);
		if (material == nullptr) material = materialManager->getMaterial("missingmaterial");

		object->Start(meshManager->getMesh(mesh->name.data), material);
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
		hitbox->getBody()->SetTransform(b2Vec2(levelFile.hitboxes[i].position[0], -levelFile.hitboxes[i].position[1]), -glm::radians(levelFile.hitboxes[i].rotation));
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
		LLight l = lights[i];
		if (light->lightType == ELightType::eDirectional)
		{
			glm::vec3 dir = { 0, 0, -1 };
			dir = glm::rotateX(dir, glm::radians(l.rotation[0]));
			dir = glm::rotateY(dir, glm::radians(l.rotation[1]));
			dir = glm::rotateZ(dir, glm::radians(l.rotation[2]));

			lightManager->AddDirectionalLight(glm::vec4(1.f), glm::vec4(dir, 1), glm::vec4(arrayToVec3(light->color), 1), glm::vec4(1, 1, 1, 1), light->intensity * 0.01f);
		}
		else
		{
			if (light->decayType == EDecayType::eQuadratic)
				lightManager->AddPointLight(glm::vec4(arrayToVec3(lights[i].position), 1), glm::vec4(arrayToVec3(light->color), 1), glm::vec4(1, 1, 1, 1), light->intensity, 1, 1.f, 10.f);
			else
				lightManager->AddPointLight(glm::vec4(arrayToVec3(lights[i].position), 1), glm::vec4(arrayToVec3(light->color), 1), glm::vec4(1, 1, 1, 1), light->intensity, 1, 10.f, 1.f);
		}
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
		case ETriggerType::poster:		
		{	
			if (trigger.data.data.empty())
			{
				printf("No number on the poster data\n");
				outTriggerType = Trigger::NOTHING;
			}
			else if (unlockedPosters[atoi(trigger.data.data.c_str()) - 1] == 0)
			{
				outTriggerType = Trigger::POSTER;
				Poster* poster = new Poster();
				poster->setShader(mapShader);
				poster->Start(meshManager->getMesh("quad"), materialManager->getMaterial("postermaterial_" + std::to_string(atoi(trigger.data.data.c_str()))));
				LHitbox hitbox = triggers[i].hitbox;
				poster->setScale(glm::vec3(hitbox.scale[0], hitbox.scale[1], 1));
				poster->setPosition(glm::vec3(hitbox.position[0], hitbox.position[1] + hitbox.scale[1] / 2, 0));
				map.push_back(poster);
				outTrigger->setMapPart(poster);
			}
			else
			{
				outTriggerType = Trigger::NOTHING;
			}
		}
			break;
		case ETriggerType::deathZone:	
			outTriggerType = Trigger::DEATH; 
			break;
		case ETriggerType::garbageBin:	
			outTriggerType = Trigger::EFFECT; 
			break;
		case ETriggerType::message:		
			outTriggerType = Trigger::POPUP; 
			if (trigger.data.data.empty())
			{
				printf("No text in message data\n");
				outTriggerType = Trigger::NOTHING;
			}
			else
			{
				outTrigger->setData(trigger.data.data);
			}
			break;
		case ETriggerType::door:	
			outTriggerType = Trigger::DOOR;
			break; 
		}

		if (outTriggerType != Trigger::NOTHING)
		{
			LHitbox hitbox = triggers[i].hitbox;
			outTrigger->InitializeTrigger(outTriggerType, world, glm::vec2(hitbox.position[0], hitbox.position[1]), glm::vec2(hitbox.scale[0], hitbox.scale[1]), trigger.data.data, true);

			// Adding trigger to vector
			this->triggers.push_back(outTrigger);
		}
		else
			delete outTrigger;
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
			particleManager->EffectConstantSmoke(glm::vec3(effect.position[0], effect.position[1], 0), materialManager->getTextureID("smoketexture"), 30);
			break;
		case EEffectType::dust:
			particleManager->EffectSmokeSignal(glm::vec3(effect.position[0], effect.position[1], 0), materialManager->getTextureID("firetexture"), glm::pi<float>() * 0.5f, 25);
			particleManager->EffectSmokeSignal(glm::vec3(effect.position[0], effect.position[1], 0), materialManager->getTextureID("smoketexture"), glm::pi<float>() * 0.5f, 15);
			break;
		case EEffectType::steam:
			particleManager->EffectConstantSmoke(glm::vec3(effect.position[0], effect.position[1], 0), materialManager->getTextureID("smoketexture"), 30, glm::vec4(0.3f, 0.30f, 0.5f, 0.7f));
			break;
		}
	}
}

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
			//		camera->setFinishPosition(glm::vec3(trigger->getPosition(), 0));
			//		camera->activateFinishAnimation();
			//		nextLevelCameraTrigger = true;	
			//		nextLevelTimer = 2000.f;
					nextLevelTrigger = true;
					nextLevelPath = trigger->getData();
					nextArchivePath = trigger->getData();
				} 
				break;

			////////////////////////////////////////////////////////////
			// Popup - For popups with text/pictures, anything
			////////////////////////////////////////////////////////////
			case Trigger::POPUP:
				remove = true;
				ActivatePopup(trigger->getData(), 4250.f);
				break;

			////////////////////////////////////////////////////////////
			// Poster - Unlockables
			////////////////////////////////////////////////////////////
			case Trigger::POSTER:
				remove = true;
				particleManager->EffectExplosionLights(glm::vec3(trigger->getPosition(), 0), 40, glm::vec4(0.8f));
				soundManager->playModifiedSFX(SoundManager::SFX::SFX_UNLOCK, 50, 0.05f);
				UnlockPoster(atoi(trigger->getData().c_str()) - 1);
				ActivatePopup("You unlocked a poster!", 4250.f);
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
				soundManager->playSFX(SoundManager::SFX::SFX_UNLOCK);
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
				ActivatePopup("Saved.", 4250.f);
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
			world->DestroyBody(trigger->getBody());
			// trigger->Stop(world);
			delete trigger;
			triggers.erase(triggers.begin() + i);
		}
	}
}

void LevelManager::UnlockPoster(int index)
{
	//Create file at path
	ofstream out("Data/savedata.fu", ios::binary);

	unlockedPosters[index] = 1;

	//Write header
	if (out.is_open())
		out.write(reinterpret_cast<const char*>(unlockedPosters.data()), sizeof(uint16_t) * 10);

	out.close();
}

std::vector<Object*> LevelManager::getMap()
{
	return map;
}

std::vector<Projectile*>* LevelManager::getProjectiles()
{
	return (myPH->getBullets());
}

const LightManager* LevelManager::getLightManager() const {	return lightManager; }
EntityManager* LevelManager::getEntityManager() { return entityManager; }
Player* LevelManager::getPlayer() { return player; }

Text* LevelManager::getPopup()
{
	return popup;
}

ProjectileHandler * LevelManager::getPH()
{
	return myPH;
}

bool LevelManager::getNextLevelTrigger()
{
	return nextLevelTrigger;
}

std::string LevelManager::getNextLevelPath()
{
	return nextLevelPath;
}

std::string LevelManager::getNextArchivePath()
{
	return nextArchivePath;
}