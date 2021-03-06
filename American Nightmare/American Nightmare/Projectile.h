#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML\Graphics.hpp>
#include "Game\Entity.h"
#include <Box2D.h>
#include "Game\Hitbox.h"
#include "Game\StructReg.h"

#define BULLET_MASS 3.0f
#define BULLET_SIZE_X 0.5f
#define BULLET_SIZE_Y 0.5f
#define BULLET_SIZE_Z 1.f

class Projectile : public Entity
{
private:
	std::string modelPath;
	std::string texturePath;

	ProjectileData projectileData;

	int damage;
	bool damageOn;
	bool marked;
	bool isProjectileFired;
	bool isZombiePart;

	glm::vec2 playerPosition;

	//Vac Variabals
	bool isInVacRange;

public:
	//Projectile(b2World *world, GLuint shader, glm::vec2 pos);
	Projectile(const MeshManager::Mesh* mesh, const MaterialManager::Material * material, b2World *world, glm::vec2 pos, bool isCircle);
	Projectile();
	~Projectile();

	void fireBullet(glm::vec2 position, glm::vec2 direction);

	b2Vec2 normalize(const b2Vec2& source);

	void setDamageOn(bool damageOn);
	bool getDamageOn()const;

	void setmarked(bool marked);
	bool getmarked()const;

	ProjectileData getProjectileData();

	void Update(GLint deltaT, b2World* world, glm::vec2 position);

	bool getIsZombiePart() const;
	void setIsFired(bool isProjectileFired);
	bool getIsFired()const;
	void setIsInVacRange(bool change);
	bool getIsInVacRange()const;
};

#endif // !PROJECTILE_H
