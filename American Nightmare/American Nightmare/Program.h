#ifndef PROGRAH
#define PROGRAH

// PRE-PROCESS
#define WIN32_LEAN_AND_MEAN // to exclude useless includes (for this project) to speed up things

// INCLUDES
#include "Game.h"
#include "OpenGL.h"
#include <SFML\Window.hpp>
#include <SFML\Window\Keyboard.hpp>

// DEFINES
#define APP_NAME		L"American Nightmare"
#define FULL_SCREEN_ON		false
#define SHOW_CURSOR		true
#define DEFAULT_SCREEN_SIZE	glm::vec2(800, 600)
#define SCREEN_DEPTH		50.f
#define SCREEN_NEAR		0.1f
#define VSYNC_ON		true

class Program
{
public:
	Program();
	Program(const Program& other);
	~Program();

	bool Start();
	void Stop();

	// Game-loop function
	bool Run();

	// Keyinput, etc
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	
	// Initializing window
	void StartSFMLWindow();
	bool StartWindow();
	void StopWindow();

	// Updating everything
	bool Update();

	sf::Window* window;
	OpenGL* openGL;
	Game* game;
	std::wstring appName;
	HINSTANCE hInstance;
	HWND hwnd;
	glm::vec2 screenSize;
};

// Window Proc Func
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
static Program* pgr = nullptr;

#endif // !PROGRAH
