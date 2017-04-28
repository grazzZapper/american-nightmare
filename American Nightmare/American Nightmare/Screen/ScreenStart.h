#ifndef SCREENSTART_H
#define SCREENSTART_H

#include "Screen.h"
#include "../Game/GUI/GUIManager.h"
#include "../Game/MaterialManager.h"

#define CLEAR_COLOR glm::vec4(0.25f, 0.1, 0.1, 1)

class ScreenStart : public Screen
{
public:
	ScreenStart();
	ScreenStart(const ScreenStart& other);
	~ScreenStart();

	////////////////////////////////////////////////////////////
	// \brief Loads Screen specific things like buttons/logo
	////////////////////////////////////////////////////////////
	bool Start(glm::vec2 screenSize, SoundManager* soundManager);

	////////////////////////////////////////////////////////////
	// \brief Unloads Screen specific things like buttons/logo
	////////////////////////////////////////////////////////////
	void Stop();

	////////////////////////////////////////////////////////////
	// \brief Setting GUI positions 
	////////////////////////////////////////////////////////////
	void SetStartVariables();

	////////////////////////////////////////////////////////////
	// \brief Look for mouse-presses and button hovers 
	////////////////////////////////////////////////////////////
	void Update(GLint deltaT);

	////////////////////////////////////////////////////////////
	// \brief Draw full start meny
	////////////////////////////////////////////////////////////
	void Draw();

private:
	MaterialManager* materialManager;
	GUIManager* guiManager;
	ShaderManager* shaderManager;
};

#endif // !SCREENSTART_H
