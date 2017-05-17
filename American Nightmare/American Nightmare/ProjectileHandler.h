#ifndef PROJECTILEHANDLER_H
#define PROJECTILEHANDLER_H

#include "Projectile.h"
#include "Accessories.h"
#include <SDL.h>

class ProjectileHandler : public Entity
{
private:
	std::vector <Projectile*> myProjtileVector;
	GLuint myShader;
	bool wasPressed;
	bool isPressed;
	int currentBullet;
	int nrOfProjectiles;
	int CAP;
	
	void initiateProjectiles(b2World *world, GLuint shader, glm::vec2 pos);

public:
	//ProjectileHandler(b2World* world, GLuint shader, int CAP = 10);
	ProjectileHandler(b2World *world, GLuint shader, glm::vec2 pos);
	ProjectileHandler();
	~ProjectileHandler();

	void Update(GLint deltaT, b2World* world, glm::vec2 position);
	void addProjectile(b2World* world);
	int getNrOffProjectiles()const;
	int getCAP()const;
	//std::vector<Projectile*> getBullets();
};

#endif
