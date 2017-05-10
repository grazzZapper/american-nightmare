#include "Player.h"

Player::Player() : Animation() { }

Player::Player(const Player & other) { }

Player::~Player() { }

bool Player::Start(const MeshManager::Mesh* mesh, const MaterialManager::Material* material, b2World* world)
{
	// Starting entity variables (including hitbox)
	Entity::Start(mesh, material, world, glm::vec2(0, 20), glm::vec2(PLAYER_SIZE_X, PLAYER_SIZE_Y), b2_dynamicBody, b2Shape::e_polygon, true, PLAYER_DENSITY, PLAYER_FRICTION);

	// Setting starting variables
	position = glm::vec3(0, 20, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(PLAYER_SIZE_X, PLAYER_SIZE_Y, PLAYER_SIZE_Z);

	// Setting a self-pointer for collision detection
	getBody()->SetUserData(this);

	return true;
}

void Player::Update(GLint deltaT)
{
	// Getting user input
	Movement();
	
	// Updating animation texture
	updateAnimation(deltaT);

	// Correcting texture to hitbox
	Entity::Update(deltaT);
}

void Player::Movement()
{
	// Temporary movement & rotation & scaling for testing :)
	
	// Temp gravity
	/*velocity.y -= 0.02f;
	position += glm::vec3(velocity, 0);*/

	if (position.y < 0.f)
		position.y = 0.f;

	// Jumping
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && position.y == 0) velocity.y = 0.35f;
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !jumping) velocity.y = 0.35f;

	// Positioning
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		hitbox->getBody()->ApplyForceToCenter(b2Vec2(PLAYER_VEL_X, 0), true);
		directionIsRight = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		hitbox->getBody()->ApplyForceToCenter(b2Vec2(-PLAYER_VEL_X, 0), true);
		directionIsRight = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		hitbox->getBody()->ApplyForceToCenter(b2Vec2(0, -PLAYER_VEL_Y), true);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		hitbox->getBody()->ApplyForceToCenter(b2Vec2(0, PLAYER_VEL_Y), true);
	}

	// Move in Z axis
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) position.z += 0.15f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) position.z -= 0.15f;

	// Scaling
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) scale += 0.025f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) scale -= 0.025f;

	// Rotation in X
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) rotation.z += 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) rotation.z -= 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) rotation.y += 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) rotation.y -= 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y)) rotation.x += 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H)) rotation.x -= 0.1f;

	// Thresholds in velocity
	if (hitbox->getBody()->GetLinearVelocity().x > PLAYER_MAX_VEL_X) hitbox->getBody()->SetLinearVelocity(b2Vec2(PLAYER_MAX_VEL_X, hitbox->getBody()->GetLinearVelocity().y));
	if (hitbox->getBody()->GetLinearVelocity().x < -PLAYER_MAX_VEL_X) hitbox->getBody()->SetLinearVelocity(b2Vec2(-PLAYER_MAX_VEL_X, hitbox->getBody()->GetLinearVelocity().y));
	if (hitbox->getBody()->GetLinearVelocity().y > PLAYER_MAX_VEL_Y) hitbox->getBody()->SetLinearVelocity(b2Vec2(hitbox->getBody()->GetLinearVelocity().x, PLAYER_MAX_VEL_Y));
	if (hitbox->getBody()->GetLinearVelocity().y < -PLAYER_MAX_VEL_Y) hitbox->getBody()->SetLinearVelocity(b2Vec2(hitbox->getBody()->GetLinearVelocity().x, -PLAYER_MAX_VEL_Y));
}

b2Body* Player::getBody()
{
	return hitbox->getBody();
}

glm::vec2 Player::getPlayerPosAsGLM()
{
	glm::vec2 myVec;

	myVec.x = hitbox->getPosition().x;
	myVec.y = hitbox->getPosition().y;

	return myVec;
}
