#include "SolidShader.h"

SolidShader::SolidShader() { }

SolidShader::SolidShader(const SolidShader& other) { }

SolidShader::~SolidShader() { }

void SolidShader::Start(OpenGL* openGL, std::string vs, std::string fs)
{
	// Reads the file and puts everything into buffer
	std::string vertexShaderBuffer = getSource(vs);
	std::string fragmentShaderBuffer = getSource(fs);

	// Create the shader variables
	vertexShader = openGL->glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = openGL->glCreateShader(GL_FRAGMENT_SHADER);

	// Compile the files
	vertexShader = compileShader(openGL, vertexShaderBuffer, GL_VERTEX_SHADER);
	fragmentShader = compileShader(openGL, fragmentShaderBuffer, GL_FRAGMENT_SHADER);

	// Create the program
	shaderProgram = openGL->glCreateProgram();

	// Attach shaders
	openGL->glAttachShader(shaderProgram, vertexShader);
	openGL->glAttachShader(shaderProgram, fragmentShader);

	// Attributes locations
	openGL->glBindAttribLocation(shaderProgram, 0, "inputPosition");
	openGL->glBindAttribLocation(shaderProgram, 1, "inputColor");

	// Link program
	openGL->glLinkProgram(shaderProgram);
}

void SolidShader::Stop(OpenGL* openGL)
{
	// Detach and delete!
	openGL->glDetachShader(shaderProgram, vertexShader);
	openGL->glDetachShader(shaderProgram, fragmentShader);
	openGL->glDeleteShader(vertexShader);
	openGL->glDeleteShader(fragmentShader);
	openGL->glDeleteProgram(shaderProgram);
}

void SolidShader::SetShader(OpenGL* openGL)
{
	// Setting as active
	openGL->glUseProgram(shaderProgram);
}

bool SolidShader::SetParameters(OpenGL* openGL, glm::mat4 world, glm::mat4 view, glm::mat4 projection)
{
	// Loading uniforms
	GLuint location = NULL;
	GLboolean transpose = GL_FALSE;

	// Loading world matrix (We first find the position and then put it in that position!)
	location = openGL->glGetUniformLocation(shaderProgram, "world");
	if (location == -1) return false;
	openGL->glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(world));

	// Loading view matrix
	location = openGL->glGetUniformLocation(shaderProgram, "view");
	if (location == -1) return false;
	openGL->glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(view));

	// Loading projection matrix
	location = openGL->glGetUniformLocation(shaderProgram, "projection");
	if (location == -1) return false;
	openGL->glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(projection));

	return true;
}

std::string SolidShader::getSource(std::string path)
{
	std::stringstream sstream;
	std::ifstream file(path);
	std::string source;

	if (!file.is_open()) 
		throw std::runtime_error("Could not read shader at: " + path);

	sstream << file.rdbuf();
	source = sstream.str();

	return source;
}

GLuint SolidShader::compileShader(OpenGL* openGL, std::string text, GLenum type)
{
	GLuint compiled = openGL->glCreateShader(type);

	const char* txt = text.c_str();
	openGL->glShaderSource(compiled, 1, &txt, nullptr);
	openGL->glCompileShader(compiled);

	int error;
	char msg[512];

	// Check if everything went ok
	openGL->glGetShaderiv(compiled, GL_COMPILE_STATUS, &error);
	if (!error)
	{
		openGL->glGetShaderInfoLog(compiled, 512, nullptr, msg);
		throw std::runtime_error("Could not compile shader because: " + std::string(msg));
	}

	return compiled;
}