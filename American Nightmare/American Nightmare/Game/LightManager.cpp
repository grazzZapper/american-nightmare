#include "LightManager.h"

LightManager::LightManager() { }

LightManager::LightManager(const LightManager& other) { }

LightManager::~LightManager() { Clear(); }

void LightManager::Clear()
{
	for (int i = 0; i < pointLightList.size(); i++)
	{
		delete pointLightList[i];
	}

	for (int i = 0; i < directionalLightList.size(); i++)
	{
		delete directionalLightList[i];
	}

	pointLightList.clear();
	directionalLightList.clear();
}

void LightManager::AddPointLight(glm::vec4 position, glm::vec4 diffuse, glm::vec4 specular, GLfloat constant, GLfloat linear, GLfloat quadratic)
{
	pointLightList.push_back(new PointLight(position, diffuse, specular, constant, linear, quadratic));
}

void LightManager::AddDirectionalLight(glm::vec4 direction, glm::vec4 diffuse, glm::vec4 specular)
{
	directionalLightList.push_back(new DirectionalLight(direction, diffuse, specular));
}

std::vector<LightManager::PointLight*> LightManager::getPointLightList() const { return pointLightList; }

std::vector<LightManager::DirectionalLight*> LightManager::getDirectionalLightList() const { return directionalLightList; }



