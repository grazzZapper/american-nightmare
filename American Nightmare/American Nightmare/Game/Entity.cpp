#include "Entity.h"

bool Entity::Start(std::string modelName, const MaterialManager::Material * material, b2World* world)
{
	Object::Start(modelName, material);

	hitbox = new Hitbox();
	if (hitbox == nullptr) return false;
	if (!hitbox->InitializeHitbox(world))
		return false;

	return false;
}

bool Entity::Start(std::string modelName, const MaterialManager::Material * material, b2World * world, glm::vec2 position, glm::vec2 size, b2BodyType type, b2Shape::Type shapeType, float density, float friction)
{
	Object::Start(modelName, material);

	hitbox = new Hitbox();
	if (hitbox == nullptr) return false;
	if (!hitbox->InitializeHitbox(world, position, size, type, shapeType, density, friction)) 
		return false;

	return false;
}

void Entity::Stop()
{
	if (hitbox != nullptr)
	{
		hitbox->Stop();
		delete hitbox;
		hitbox = nullptr;
	}
	
	Object::Stop();
}

void Entity::Update(GLint deltaT)
{
	// Update the texture position
	position.x = hitbox->getPosition().x;
	position.y = -hitbox->getPosition().y;
	rotation.z = hitbox->getBody()->GetAngle();

	Object::Update(deltaT);
}

void Entity::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

Hitbox * Entity::getHitbox() { return hitbox; }
