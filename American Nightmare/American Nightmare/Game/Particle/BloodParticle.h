#ifndef BLOODPARTICLE_H
#define BLOODPARTICLE_H

#include "Particle.h"

#define BLOOD_VELOCITY			randBetweenF(-0.05f, 0.05f)
#define BLOOD_VELOCITY_FALL_OFF	randBetweenF(0.15f, 0.225f)
#define BLOOD_LIFETIME			randBetweenF(100.f, 700.f)

struct BloodParticle : public Particle
{
public:

	////////////////////////////////////////////////////////////
	// \brief Initializes the particle with a position and color
	// \param position The starting position of the particle
	// \param color The starting color of the particle
	// \param size The size in w/h of the particle quad
	// \param angle The angle of the impact in radians
	// \param strength Impact strength of the hit (amount of dmg done)
	////////////////////////////////////////////////////////////
	virtual void Start(glm::vec3 position, glm::vec4 color, glm::vec2 size, float angle, float strength);

	////////////////////////////////////////////////////////////
	// \brief Updates various variables in the pixel
	//	Removes lifetime from particle and makes it dead if not time left
	//	Also adds velocity fall-off for a more realistic effect
	// \param deltaT The time between each frame in milliseconds
	////////////////////////////////////////////////////////////
	virtual void Update(GLfloat deltaT);

protected:
	float angle;
};

#endif // !BLOODPARTICLE_H