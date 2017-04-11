#include "Program.h"

Program::Program()
{
	screenManager = nullptr;
	window = nullptr;
}

Program::Program(const Program& other) { }

Program::~Program() { }

bool Program::Start()
{
	srand(time_t(NULL));
	screenSize = DEFAULT_SCREEN_SIZE;

	////////////////////////////////////////////////////////////
	// Creating a window for the program
	////////////////////////////////////////////////////////////
	StartSDLWindow();

	////////////////////////////////////////////////////////////
	// Creating Grahpics Object
	// - Updates & Renders Graphics
	////////////////////////////////////////////////////////////
	screenManager = new ScreenManager();
	if (screenManager == nullptr) return false;
	if (!screenManager->Start(hwnd))
	{
		MessageBox(hwnd, L"Could not start ScreenManager class.", L"Woops", MB_OKCANCEL);
		return false;
	}

	return true;
}

void Program::StartSDLWindow()
{
	screenSize = DEFAULT_SCREEN_SIZE;
	appName = APP_NAME;

	// Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow(appName.c_str(), 0, 0, screenSize.x, screenSize.y, SDL_WINDOW_OPENGL);

	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	// Getting Windowhandl
	SDL_SysWMinfo systemInfo;
	SDL_VERSION(&systemInfo.version);
	SDL_GetWindowWMInfo(window, &systemInfo);

	hwnd = systemInfo.info.win.window;

	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("Could not initialize GLEW");
		return;
	}

	// Setting various OpenGL settings, check header for variables
	glViewport(0, 0, screenSize.x, screenSize.y);
	glClearDepth(DEPTH_BUFFER_CLEAR);				// Threshold for depthbuffer to clear
	glFrontFace(GL_CW);								// ClockWise = CW, CounterClockWise = CCW
	glEnable(GL_CULL_FACE);							// Enable Culling
	glCullFace(GL_BACK);							// GL_BACK is default, (backculling), we can also use GL_FRONT, and GL_FRONT_AND_BACK if needed

}

void Program::Stop()
{
	// Deleting graphics
	if (screenManager != nullptr)
	{
		screenManager->Stop();
		delete screenManager;
		screenManager = nullptr;
	}

	StopWindow();
}

bool Program::Run()
{
	// Making the msg spot in memory, filling it with zeros
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	SDL_Event event;

	bool done = false;
	while (!done)
	{

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = true;
		}

		// Updating everything
		if (!Update())
			done = true;
	}

	return false;
}

void Program::StopWindow()
{
	// Showing the mouse cursor 
	if (!SHOW_CURSOR)
		ShowCursor(true);

	// Destroy window
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Program::Update()
{
	// Check if user exits
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		return false;

	// Update game
	screenManager->Update();

	// Draw game
	screenManager->Draw(window, glm::vec4(0.15f, 0.15f, 0.15f, 1.f));

	return true;
}