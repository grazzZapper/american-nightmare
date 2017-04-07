#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>	
#include <glm/mat4x4.hpp>		// 4x4 matrices
#include <glm/gtc/matrix_transform.hpp>	// matrix stuff
#include <glm/gtx/rotate_vector.hpp>	// rotating vectors and matrices

class Camera
{
public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	////////////////////////////////////////////////////////////
	// \brief Builds a view matrix with position and rotation
	// \note Needs to be built every frame!
	////////////////////////////////////////////////////////////
	void buildViewMatrix();

	////////////////////////////////////////////////////////////
	// \brief Set a new postition of the camera
	////////////////////////////////////////////////////////////
	void setPosition(glm::vec3 position);

	////////////////////////////////////////////////////////////
	// \brief Gets the current view matrix
	// \note Only call this after you've called builtViewMatrix
	// \return The view matrix in 4x4
	////////////////////////////////////////////////////////////
	glm::mat4 getViewMatrix();

	////////////////////////////////////////////////////////////
	// \brief Gets the position of the camera
	// \return Returns a vector 3x1
	////////////////////////////////////////////////////////////
	glm::vec3 getPosition();

private:
	glm::mat4 viewMatrix;	//< The camera's view matrix
	glm::vec3 position;	//< Position of the camera
};

#endif // !CAMERA_H