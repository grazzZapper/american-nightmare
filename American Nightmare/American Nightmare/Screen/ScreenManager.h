#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "ScreenGame.h"
#include "ScreenStart.h"
#include "ScreenCutscene.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <Windows.h>

#define START_STATE 0 // 0 = Game, 1 = Startmeny

class ScreenManager
{
public:
	enum State { Game, StartMeny, Options, Cutscene, Posters };
	State currentState;

	ScreenManager();
	ScreenManager(const ScreenManager& other);
	~ScreenManager();

	////////////////////////////////////////////////////////////
	// \brief Saves openGL ptr and starts the start screen
	////////////////////////////////////////////////////////////
	bool Start(HWND hwnd);

	////////////////////////////////////////////////////////////
	// \brief Unloads every screen
	////////////////////////////////////////////////////////////
	void Stop();

	////////////////////////////////////////////////////////////
	// \brief Starts the screen with the current state
	// \return Returns false if the screen could not be started
	////////////////////////////////////////////////////////////
	bool StartCurrentScreen();

	////////////////////////////////////////////////////////////
	// \brief Unloads the current state
	////////////////////////////////////////////////////////////
	void StopCurrentScreen();

	////////////////////////////////////////////////////////////
	// \brief Will switch current state and it will also run the 
	//	stop function for the current state and unload everything
	//	it will do nothing if we are already in same state
	// \return Returns true if the Start function of the new state worked
	//	otherwise it returns false
	////////////////////////////////////////////////////////////
	bool goToState(State state);

	////////////////////////////////////////////////////////////
	// \brief Updates the active screen
	////////////////////////////////////////////////////////////
	void Update(GLint deltaT);

	////////////////////////////////////////////////////////////
	// \brief Draws the active screen
	////////////////////////////////////////////////////////////
	void Draw(SDL_Window* window, glm::vec4 color);

	////////////////////////////////////////////////////////////
	// \brief Returns the current screen
	////////////////////////////////////////////////////////////
	State getState();
private:
	HWND hwnd;			//< Saving the HWND for error msg boxes output
	ScreenGame* screenGame;		//< The game screen
	ScreenStart* screenStart;	//< The start meny screen
	ScreenCutscene* screenCutscene; //< The screen for cutscenes
};

#endif // !SCREENMANAGER_H
