#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "Button.h"
#include "Text.h"

class GUIManager
{
public:
	GUIManager();
	GUIManager(const GUIManager& other);
	~GUIManager();

	bool Start(glm::vec2 screenSize, glm::vec2 screenPosition);
	void Stop();

	void Update(GLuint deltaT);

	bool AddButton(glm::vec3 position, glm::vec2 size, const MaterialManager::Material* material);
	bool AddText(glm::vec3 position, float characterSize, std::string text, std::string fontName);

	void setShader(GLuint shader);
	void setAlpha(float alpha);

	std::vector<Button*> getButtonList();
	std::vector<Text*> getTextList();

private:

	void clearButtons();
	void clearTexts();

	glm::vec2 screenSize;
	glm::vec2 screenPosition;
	GLuint shader;
	std::vector<Button*> buttons;
	std::vector<Text*> texts;
};

#endif // !GUIMANAGER_H
