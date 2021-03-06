#include "ScreenOptions.h"

extern std::vector<uint16_t> unlockedPosters;

ScreenOptions::ScreenOptions() : Screen()
{
	shaderManager = nullptr;
	guiManager = nullptr;
	materialManager = nullptr;
	meshManager = nullptr;
}

ScreenOptions::ScreenOptions(const ScreenOptions& other) { }

ScreenOptions::~ScreenOptions() { }

bool ScreenOptions::Start(SDL_Window* window, glm::vec2 screenSize, glm::vec2 screenPosition, State* state, SoundManager* soundManager)
{
	Screen::Start(window, screenSize, screenPosition, state, soundManager);

	////////////////////////////////////////////////////////////
	// Creating Shader Manager
	////////////////////////////////////////////////////////////
	std::string shaderPath = SHADER_PATH;
	shaderManager = new ShaderManager();
	if (shaderManager == nullptr) return false;

	// Adding Shader Programs
	shaderManager->AddShader("texture", shaderPath + "texture_vs.glsl", shaderPath + "texture_fs.glsl");
	shaderManager->AddShader("particle_light", shaderPath + "particle_light_vs.glsl", shaderPath + "particle_light_gs.glsl", shaderPath + "particle_light_fs.glsl");
	shaderManager->AddShader("particle_texture", shaderPath + "particle_texture_vs.glsl", shaderPath + "particle_texture_gs.glsl", shaderPath + "particle_texture_fs.glsl");
	shaderManager->AddShader("particle_lines", SHADER_PATH "particle_lines_vs.glsl", SHADER_PATH "particle_lines_gs.glsl", SHADER_PATH "particle_lines_fs.glsl");

	////////////////////////////////////////////////////////////
	// Creating Particle Manager
	////////////////////////////////////////////////////////////
	particleManager = new ParticleManager();
	if (particleManager == nullptr) return false;
	particleManager->ShaderPair(shaderManager->getShader("particle_light"), ParticleType::LIGHT);
	particleManager->ShaderPair(shaderManager->getShader("particle_light"), ParticleType::BLOOD);
	particleManager->ShaderPair(shaderManager->getShader("particle_texture"), ParticleType::TEXTURE);
	particleManager->ShaderPair(shaderManager->getShader("particle_texture"), ParticleType::SMOKE);
	particleManager->ShaderPair(shaderManager->getShader("particle_light"), ParticleType::LIGHT_DUST);

	////////////////////////////////////////////////////////////
	// Creating Material Manager and loading textures/materials
	////////////////////////////////////////////////////////////
	materialManager = new MaterialManager();
	if (materialManager == nullptr) return false;

	// Loading materials
	materialManager->AddMaterial("GUI_1_mat", glm::vec3(0.1f), glm::vec3(0.75, 0.75, 0.75), glm::vec3(1.f), 1.f, "GUI_1_tex", TEXTURE_PATH "GUI_btn_1.png");
	materialManager->AddMaterial("backgroundmaterial", glm::vec3(0.1f), glm::vec3(1, 1, 1), glm::vec3(1.f), 1.f, "backgroundtexture", TEXTURE_PATH "background_2.jpg");
	materialManager->AddMaterial("smokematerial", glm::vec3(0.1f), glm::vec3(0.3f, 0.4f, 0.9f), glm::vec3(1.f), 1.f, "smoketexture", TEXTURE_PATH "smoke.png");
	if (materialManager->getMaterial("GUI_1_mat") == nullptr) printf("Button Material not found\n");
	if (materialManager->getMaterial("backgroundmaterial") == nullptr) printf("Background Material not found\n");
	if (materialManager->getMaterial("smokematerial") == nullptr) printf("Smoke Material not found\n");

	////////////////////////////////////////////////////////////
	// Creating Models
	////////////////////////////////////////////////////////////
	meshManager = new MeshManager();
	if (meshManager == nullptr) return false;

	////////////////////////////////////////////////////////////
	// Creating a GUI manager	
	////////////////////////////////////////////////////////////
	guiManager = new GUIManager();
	if (guiManager == nullptr) return false;
	if (!guiManager->Start(screenSize, screenPosition)) return false;
	
	// Options Start
	guiManager->AddButton(GUIManager::OPTION_MUTE, glm::vec3(0, 0.40f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "sound - On", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.f, 1, 0.3f, 1));
	guiManager->AddButton(GUIManager::OPTION_SHADOWS, glm::vec3(0, 0.20f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "shadows - High", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.f, 1, 0.3f, 1));
	guiManager->AddButton(GUIManager::OPTION_QUALITY, glm::vec3(0, 0.f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Quality - High", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.f, 1, 0.3f, 1));
	guiManager->AddButton(GUIManager::OPTION_REBIND, glm::vec3(0, -0.20f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Rebind keys", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OPTION_RESET, glm::vec3(0, -0.40f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Reset Data", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::STARTMENY, glm::vec3(0, -0.80f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Back", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddText(glm::vec3(0.f, 0.60f, 0.f), 80.f, "Options", FONT_PATH INGAME_FONT);

	// Options rebind keys
	guiManager->AddButton(GUIManager::OK, glm::vec3(-1.4, 0.40f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Walk Left - a", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-1.4, 0.20f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Walk Right - d", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-1.4, 0.f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Jump - w", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.0, 0.40f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Hover - Space", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.0, 0.20f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Dash - lshift", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.0, 0.00f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Shockwave - q", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.6, 0.40f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Reload - right click", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.6, 0.20f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Shoot - left click", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.6, -0.60f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Defaults", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::OK, glm::vec3(-2.0, -0.60f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Save", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddButton(GUIManager::CANCEL, glm::vec3(-1.4, -0.60f, 0), glm::vec2(0.225f, 0.05955), materialManager->getMaterial("GUI_1_mat"), meshManager->getMesh("quadbig"), "Back", FONT_PATH INGAME_FONT, 28.f, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	guiManager->AddText(glm::vec3(-2.f, 0.80f, 0.f), 80.f, "Rebind Keys", FONT_PATH INGAME_FONT);
	
	guiManager->setAlpha(1.f);
	guiManager->setShader(shaderManager->getShader("texture"));

	background = new Button();
	background->Start(screenSize, glm::vec3(0.f, 0.f, 0.1f), glm::vec2(1), materialManager->getMaterial("backgroundmaterial"), meshManager->getMesh("quadbig"));
	background->setShader(shaderManager->getShader("texture"));

	// Setting starting variables
	SetStartVariables();

	return true;
}

void ScreenOptions::SetStartVariables()
{
	// Backing the camera a little bit backwards
	camera->setPosition(glm::vec3(0, 0, 15));

	// Adding some ambient smoke on startmenu
	particleManager->EffectConstantSmoke(glm::vec3(2, 1.05, 12.5f), materialManager->getTextureID("smoketexture"), 10, glm::vec4(1.f), 0.25f);
	particleManager->EffectConstantSmoke(glm::vec3(1, 1.05, 12.5f), materialManager->getTextureID("smoketexture"), 8, glm::vec4(1.f), 0.25f);
	particleManager->EffectConstantSmoke(glm::vec3(0, 1.05, 12.5f), materialManager->getTextureID("smoketexture"), 6, glm::vec4(1.f), 0.25f);
	particleManager->EffectConstantSmoke(glm::vec3(-1, 1.05, 12.5f), materialManager->getTextureID("smoketexture"), 8, glm::vec4(1.f), 0.25f);
	particleManager->EffectConstantSmoke(glm::vec3(-2, 1.05, 12.5f), materialManager->getTextureID("smoketexture"), 10, glm::vec4(1.f), 0.25f);

	// Dust effect
	particleManager->EffectLightDust(glm::vec3(0.f, 3, 0.f), glm::vec3(10, 6, 2), 20, glm::vec4(0.90f), 0.07f);
}

void ScreenOptions::Update(GLint deltaT)
{
	// Updating Buttons
	guiManager->Update(deltaT);

	// Updating particles
	particleManager->Update(deltaT);

	std::vector<std::pair<Button*, GUIManager::Action>>* buttons = guiManager->getButtonList();
	for (int i = 0; i < buttons->size(); i++)
	{
		Button* btn = buttons[0][i].first;
		GUIManager::Action action = buttons[0][i].second;
		if (btn->getPressed())
		{
			soundManager->playSFXOverDrive(SoundManager::SFX::SFX_BTN, 50, 0.2f);
			switch (action)
			{ 
			case GUIManager::Action::STARTMENY:			
				*state = State::StartMeny;	
				break;
			case GUIManager::Action::OPTION_MUTE:		
			{
				soundManager->mute();
				{
					glm::vec4 clr = btn->getText()->getColor();
					if (clr.r == 0.f)
					{
						clr = glm::vec4(1, .3, .3, 1);
						btn->getText()->setString("sound - Off");
					}
					else
					{
						clr = glm::vec4(0, 1, .3, 1);
						btn->getText()->setString("sound - On");
					}
					btn->getText()->setColor(clr);
				}
			}
				break;
			case GUIManager::Action::OPTION_SHADOWS:
				{
					glm::vec4 clr = btn->getText()->getColor();
					if (clr.r == 0.f)
					{
						clr = glm::vec4(1, .3, .3, 1);
						btn->getText()->setString("shadows - Low");
					}
					else
					{
						clr = glm::vec4(0, 1, .3, 1);
						btn->getText()->setString("shadows - High");
					}
					btn->getText()->setColor(clr);
				}
				printf("Not fully done yet.. :D\n");
				break;
			case GUIManager::Action::OPTION_REBIND:
				guiManager->setCenter(glm::vec2(-2, 0));
				break;
			case GUIManager::Action::CANCEL:
				guiManager->setCenter(glm::vec2(0, 0));
				break;
			case GUIManager::Action::OPTION_RESET:
				remove(ARCHIVE_PATH"savedata.fu");
				for (uint16_t& i : unlockedPosters) i = 0;
				printf("Data reset\n");
				break;
			case GUIManager::Action::OPTION_QUALITY:
				{
					glm::vec4 clr = btn->getText()->getColor();
					if (clr.r == 0.f)
					{
						clr = glm::vec4(1, .3, .3, 1);
						btn->getText()->setString("Quality - Low");
					}
					else
					{
						clr = glm::vec4(0, 1, .3, 1);
						btn->getText()->setString("Quality - High");
					}
					btn->getText()->setColor(clr);
				}
				printf("Not fully done yet.. :D\n");
				break;
			}
			btn->setPressed(false);
		}
	}
}

void ScreenOptions::Draw()
{
	// Getting view matrix from camera
	camera->buildViewMatrix();
	
	// Drawing background
	DrawObjectGUI(background, shaderManager);

	// Drawing GUI
	std::vector<std::pair<Button*, GUIManager::Action>>* buttons = guiManager->getButtonList();
	for (int i = 0; i < buttons->size(); i++)
	{
		DrawObjectGUI(buttons[0][i].first, shaderManager);
		if (buttons[0][i].first->getText()) DrawObjectGUI(buttons[0][i].first->getText(), shaderManager);
	}
	std::vector<Text*>* txts = guiManager->getTextList();
	for (int i = 0; i < txts->size(); i++)
		DrawObjectGUI(txts[0][i], shaderManager);

	// Drawing particles
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	for (ParticleEmitter* emitter : *particleManager->getEmitters())
		DrawParticles(emitter, shaderManager);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void ScreenOptions::Stop()
{
	// Deleting shaders
	if (shaderManager != nullptr)
	{
		shaderManager->Stop();
		delete shaderManager;
		shaderManager = nullptr;
	}
	
	// Deleting material manager
	if (materialManager != nullptr)
	{
		materialManager->Clear();
		delete materialManager;
		materialManager = nullptr;
	}

	// Deleting gui
	if (guiManager != nullptr)
	{
		guiManager->Stop();
		delete guiManager;
		guiManager = nullptr;
	}

	if (particleManager != nullptr)
	{
		particleManager->Stop();
		delete particleManager;
		particleManager = nullptr;
	}

	if (meshManager != nullptr)
	{
		meshManager->Clear();
		delete meshManager;
		meshManager = nullptr;
	}

	if (background != nullptr)
	{
		background->Stop();
		delete background;
		background = nullptr;
	}

	// Deletes Camera & OpenGL ptr
	Screen::Stop();
}

void ScreenOptions::UpdateScreenProperties(glm::vec2 screenSize, glm::vec2 screenPos)
{
	Screen::UpdateScreenProperties(screenSize, screenPos);

	// Updating guimanager
	guiManager->setScreenPosition(screenPos);
	guiManager->setScreenSize(screenSize);
}