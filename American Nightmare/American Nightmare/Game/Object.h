#ifndef OBJECT_H
#define OBJECT_H

// Classes
// #include "Texture.h"
#include "Model.h"

class Object
{
public:
	Object();
	Object(const Object& other);
	~Object();

	////////////////////////////////////////////////////////////
	// \brief Loads both a model & texture
	// \return Returns false if one or both fails
	////////////////////////////////////////////////////////////
	virtual bool Start(OpenGL* openGL, std::string modelName, std::string textureName);

	////////////////////////////////////////////////////////////
	// \brief Unloads both model & texture
	////////////////////////////////////////////////////////////
	virtual void Stop();

	////////////////////////////////////////////////////////////
	// \brief Virtual Update function
	////////////////////////////////////////////////////////////
	virtual void Update() = 0;

	////////////////////////////////////////////////////////////
	// \brief Draws the model
	////////////////////////////////////////////////////////////
	virtual void Draw();

protected:	// Note: Make this private when loadModel is implemented!
	glm::vec3 position;
	OpenGL* openGL;		//< OpenGL functions ptr
	Model* model;		//< This object's model
	// Texture* texture;	//< This object's texture 
};


#endif // !OBJECT_H
