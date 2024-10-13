#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.hpp"
#include "Cylinder.h"
#include "Sphere.h"
#include "camera.h"


// Unnamed namespace to hold global variables
namespace {
	// window title
	const char* const TITLE = "Pomai Ahlo Final Project";

	// window width and height
	const int WIDTH = 1200;
	const int HEIGHT = 900;

	GLFWwindow* window;

	// camera
	Camera gCamera(glm::vec3(0.0f, 0.0f, 15.0f)); // camera with default location as param
	float gLastX = WIDTH / 2.0f;
	float gLastY = HEIGHT / 2.0f;
	bool gFirstMouse = true;
	bool gIsPerspective = true; // perspective or ortho;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;

	GLuint gTextureId0; //wood
	GLuint gTextureId1; //tissue box
	GLuint gTextureId2; //tissue box hole
	GLuint gTextureId3; //tissue
	GLuint gTextureId4; //aloe container
	GLuint gTextureId5; //yellow liquid handsanitizer
	GLuint gTextureId6; //grey cap
	GLuint gTextureId7;	//blue ball
}

int initializeWindow();
void processKeyInput(GLFWwindow* window);
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void flipImageVertically(unsigned char* image, int width, int height, int channels);
bool createTexture(const char* filename, GLuint& textureId);

int main() {
	///////////////////
	//     Setup     //
	///////////////////
	
	// initialize window, glew, and glfw
	int errorFlag = initializeWindow();
	if (errorFlag == -1) {
		std::cout << "Error.";
		return -1;
	}

	// Create and compile our GLSL program from the shaders
	GLuint programId = LoadShaders("VertexShader.vs", "FragmentShader.fs");
	
	// initialize location variables
	GLint modelLoc = glGetUniformLocation(programId, "model");
	GLint viewLoc = glGetUniformLocation(programId, "view");
	GLint projLoc = glGetUniformLocation(programId, "projection");
	GLint objectColorLoc = glGetUniformLocation(programId, "objectColor");
	GLint light0ColorLoc = glGetUniformLocation(programId, "lightColor0");
	GLint light0PositionLoc = glGetUniformLocation(programId, "lightPos0");
	GLint light1ColorLoc = glGetUniformLocation(programId, "lightColor1");
	GLint light1PositionLoc = glGetUniformLocation(programId, "lightPos1");
	GLint viewPositionLoc = glGetUniformLocation(programId, "viewPosition");
	
	///////////////////////////
	//     Load Textures     //
	///////////////////////////

	glUseProgram(programId);

	// Load table texture
	const char* texFilename0 = "../resources/textures/wood.jpg";
	if (!createTexture(texFilename0, gTextureId0)) {
		std::cout << "Failed to load texture " << texFilename0 << std::endl;
		return -1;
	}
	
	// Load tissue box texture
	const char* texFilename1 = "../resources/textures/blueSquare.jpg";
	if (!createTexture(texFilename1, gTextureId1)) {
		std::cout << "Failed to load texture " << texFilename1 << std::endl;
		return -1;
	}
	
	// Load tissue hole texture
	const char* texFilename2 = "../resources/textures/cardboard.jpg";
	if (!createTexture(texFilename2, gTextureId2)) {
		std::cout << "Failed to load texture " << texFilename2 << std::endl;
		return -1;
	}
	
	// Load tissue texture
	const char* texFilename3 = "../resources/textures/tissue.jpg";
	if (!createTexture(texFilename3, gTextureId3)) {
		std::cout << "Failed to load texture " << texFilename3 << std::endl;
		return -1;
	}

	// Load aloe texture
	const char* texFilename4 = "../resources/textures/aloe.jpg";
	if (!createTexture(texFilename4, gTextureId4)) {
		std::cout << "Failed to load texture " << texFilename4 << std::endl;
		return -1;
	}

	// Load hand sanitizer texture
	const char* texFilename5 = "../resources/textures/yellowFluid.jpg";
	if (!createTexture(texFilename5, gTextureId5)) {
		std::cout << "Failed to load texture " << texFilename5 << std::endl;
		return -1;
	}

	// Load cap texture
	const char* texFilename6 = "../resources/textures/greyPlastic.jpg";
	if (!createTexture(texFilename6, gTextureId6)) {
		std::cout << "Failed to load texture " << texFilename6 << std::endl;
		return -1;
	}

	// Load ball texture
	const char* texFilename7 = "../resources/textures/blueRubber.jpg";
	if (!createTexture(texFilename7, gTextureId7)) {
		std::cout << "Failed to load texture " << texFilename7 << std::endl;
		return -1;
	}

	///////////////////////////////////////////////////////////////////
	//     Vertex, Normal, and UV data (non-circular items only)     //
	///////////////////////////////////////////////////////////////////
	// saves triangles for the plane
	static const GLfloat vertsPlane[] = {
		1.0f, 1.0f, 0.0f,  // triangle 1
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f,  // triangle 2
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	// save normal data for plane
	static const GLfloat normalPlane[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	// saves the color data for the plane
	static const GLfloat uvPlane[] = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};
	// saves triangles for the pyramid
	static const GLfloat vertsP[] = {
		 0.5f,  0.3f,  0.0f, // face 1
		 0.5f, -0.3f,  0.0f,
		 0.0f,  0.0f,  1.0f,

		 0.5f, -0.3f,  0.0f, // face 2
		-0.5f, -0.3f,  0.0f,
		 0.0f,  0.0f,  1.0f,

		-0.5f, -0.3f,  0.0f, // face 3
		-0.5f,  0.3f,  0.0f,
		 0.0f,  0.0f,  1.0f,

		-0.5f,  0.3f,  0.0f, // face 4
		 0.5f,  0.3f,  0.0f,
		 0.0f,  0.0f,  1.0f,

		 0.5f,  0.3f,  0.0f, // base 1
		-0.5f, -0.3f,  0.0f,
		-0.5f,  0.3f,  0.0f,

		 0.5f,  0.3f,  0.0f, // base 2
		-0.5f, -0.3f,  0.0f,
		 0.5f, -0.3f,  0.0f
	};
	// saves normal data for the pyramid
	static const GLfloat normalP[] = {
		0.6f, 0.0f, 0.3f,
		0.6f, 0.0f, 0.3f,
		0.6f, 0.0f, 0.3f,

		0.0f, -1.0f, 0.3f,
		0.0f, -1.0f, 0.3f,
		0.0f, -1.0f, 0.3f,

		-0.6f, 0.0f, 0.3f,
		-0.6f, 0.0f, 0.3f,
		-0.6f, 0.0f, 0.3f,

		0.0f, 1.0f, 0.3f,
		0.0f, 1.0f, 0.3f,
		0.0f, 1.0f, 0.3f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f
	};
	// saves uv data for the pyramid
	static const GLfloat uvP[] = {
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.5f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.5f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.5f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.5f, 1.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};
	// saves triangles for the cube
	static const GLfloat vertsCube[] = {
		 1.0f,  1.0f,  1.0f, //face 1 (top)
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f,  1.0f, // face 2 (front)
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		 1.0f,  1.0f,  1.0f, //face 3 (right)
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f, 
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f, //face 4 (back)
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f, //face 5 (left)
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f,  1.0f, -1.0f, // face 6 (bottom)
		-1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f
	};
	// saves normal data for the cube
	static const GLfloat normalCube[] = {
		0.0f, 0.0f, 1.0f, //top
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, -1.0f, 0.0f, //front
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		1.0f, 0.0f, 0.0f, //right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, //back
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		-1.0f, 0.0f, 0.0f, //left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -1.0f, //bottom
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f
	};
	// saves uv data for the cube
	GLfloat repeat = 3.0f;
	static const GLfloat uvCube[] = {
		repeat, repeat,
		0.0f, repeat, 
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,

		repeat, repeat,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,

		repeat, repeat,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,

		repeat, repeat,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,

		repeat, repeat,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,

		repeat, repeat,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, repeat,
		repeat, 0.0f,
		0.0f, 0.0f,
	};

	/////////////////////////////
	//     Set Buffer Data     //
	/////////////////////////////
	
	// copy plane vertex to VBO
	GLuint vertexBufferPlane;
	glGenBuffers(1, &vertexBufferPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertsPlane), vertsPlane, GL_STATIC_DRAW);

	// copy plane normal data to VBO
	GLuint normalBufferPlane;
	glGenBuffers(1, &normalBufferPlane);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalPlane), normalPlane, GL_STATIC_DRAW);

	// copy plane uv data to VBO
	GLuint uvBufferPlane;
	glGenBuffers(1, &uvBufferPlane);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvPlane), uvPlane, GL_STATIC_DRAW);

	// copy pyramid vertex data to VBO
	GLuint vertexBufferPyramid;
	glGenBuffers(1, &vertexBufferPyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertsP), vertsP, GL_STATIC_DRAW);

	// copy pyramid normal data to VBO
	GLuint normalBufferPyramid;
	glGenBuffers(1, &normalBufferPyramid);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferPyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalP), normalP, GL_STATIC_DRAW);

	// copy pyramid uv data to VBO
	GLuint uvBufferPyramid;
	glGenBuffers(1, &uvBufferPyramid);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferPyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvP), uvP, GL_STATIC_DRAW);

	Cylinder cap;
	cap.setHeight(1.75f);

	// copy interleaved vertex data (vertex/normal/tangent) to VBO
	GLuint interleavedBufferCap;
	glGenBuffers(1, &interleavedBufferCap);
	glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferCap);             
	glBufferData(GL_ARRAY_BUFFER,                     // target
		cap.getInterleavedVertexSize(), // data size, # of bytes
		cap.getInterleavedVertices(),   // ptr to vertex data
		GL_STATIC_DRAW);                     // usage

	// copy index data to VBO
	GLuint indexBufferCap;
	glGenBuffers(1, &indexBufferCap);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferCap);     // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,             // target
		cap.getIndexSize(),             // data size, # of bytes
		cap.getIndices(),               // ptr to index data
		GL_STATIC_DRAW);                     // usage

	// copy cube vertex to VBO
	GLuint vertexBufferCube;
	glGenBuffers(1, &vertexBufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertsCube), vertsCube, GL_STATIC_DRAW);

	// copy cube normals to VBO
	GLuint normalBufferCube;
	glGenBuffers(1, &normalBufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalCube), normalCube, GL_STATIC_DRAW);

	// copy cube color data to VBO
	GLuint uvBufferCube;
	glGenBuffers(1, &uvBufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvCube), uvCube, GL_STATIC_DRAW);

	Cylinder container;
	container.setHeight(1.0f);

	// copy interleaved vertex data (vertex/normal/tangent) to VBO
	GLuint interleavedBufferContainer;
	glGenBuffers(1, &interleavedBufferContainer);
	glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferContainer);             
	glBufferData(GL_ARRAY_BUFFER, 
		container.getInterleavedVertexSize(),
		container.getInterleavedVertices(),
		GL_STATIC_DRAW);                    

	// copy index data to VBO
	GLuint indexBufferContainer;
	glGenBuffers(1, &indexBufferContainer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferContainer);  
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,            
		container.getIndexSize(),            
		container.getIndices(),          
		GL_STATIC_DRAW);                

	// create a sphere with default params
	Sphere ball;

	// copy interleaved vertex data (vertex, normals, texture) to VBO
	GLuint interleavedBufferBall;
	glGenBuffers(1, &interleavedBufferBall);
	glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferBall);       
	glBufferData(GL_ARRAY_BUFFER,                  
		ball.getInterleavedVertexSize(), 
		ball.getInterleavedVertices(),   
		GL_STATIC_DRAW);                   

	// copy index data to VBO
	GLuint indexBufferBall;
	glGenBuffers(1, &indexBufferBall);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferBall);   
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,     
		ball.getIndexSize(),             
		ball.getIndices(),              
		GL_STATIC_DRAW);

	////////////////////////////////////
	//     Create Model Matricies     //
	////////////////////////////////////

	// 1. Scales the plane
	glm::mat4 scalePlane = glm::scale(glm::vec3(10.0f, 6.0f, 5.0f));
	// 2. Rotates plane
	glm::mat4 rotationPlane = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// 3. Place plane
	glm::mat4 translationPlane = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelTable = translationPlane * rotationPlane * scalePlane;

	// 1. Scales the pyramid
	glm::mat4 scalePyramid = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	// 2. Rotates pyramid 
	glm::mat4 rotationPyramid = glm::rotate(2.5f, glm::vec3(0.0f, 0.0f, 1.0f));
	// 3. Place pyramid 
	glm::mat4 translationPyramid = glm::translate(glm::vec3(1.0f, -2.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelPyramid = translationPyramid * rotationPyramid * scalePyramid;
	
	// 1. Scales the cap
	glm::mat4 scaleCap = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	// 2. Rotates cap 
	glm::mat4 rotationCap = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.25f));
	// 3. Place cap
	glm::mat4 translationCap = glm::translate(glm::vec3(1.0f, -2.0f, 2.6f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelCap = translationCap * rotationCap * scaleCap;

	// 1. Scales the cube
	glm::mat4 scaleCube = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	// 2. Rotates cube 
	glm::mat4 rotationCube = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.25f));
	// 3. Place cube
	glm::mat4 translationCube = glm::translate(glm::vec3(-5.0f, 0.0f, 3.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelCube = translationCube * rotationCube * scaleCube;

	// 1. Scales the hole
	glm::mat4 scaleHole = glm::scale(glm::vec3(1.0f, 2.0f, 1.0f));
	// 2. Rotates hole 
	glm::mat4 rotationHole = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.25f));
	// 3. Place hole
	glm::mat4 translationHole = glm::translate(glm::vec3(-5.0f, 0.0f, 6.001f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelHole = translationHole * rotationHole * scaleHole;

	// 1. Scales the tissue
	glm::mat4 scaleTissue = glm::scale(glm::vec3(3.0f, 1.9f, 1.0f));
	// 2. Rotates tissue
	glm::mat4 rotationTissue = glm::rotate(1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
	// 3. Place tissue
	glm::mat4 translationTissue = glm::translate(glm::vec3(-5.0f, 0.0f, 6.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelTissue = translationTissue * rotationTissue * scaleTissue;

	// 1. Scales the green container
	glm::mat4 scaleContainer = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	// 2. Rotates green container 
	glm::mat4 rotationContainer = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.25f));
	// 3. Place green container
	glm::mat4 translationContainer = glm::translate(glm::vec3(3.0f, 3.0f, 1.5f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelContainer = translationContainer * rotationContainer * scaleContainer;

	// 1. Scales the ball
	glm::mat4 scaleBall = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
	// 2. Rotates ball 
	glm::mat4 rotationBall = glm::rotate(1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	// 3. Place ball
	glm::mat4 translationBall = glm::translate(glm::vec3(2.0f, 2.0f, 5.0f));
	// Model matrix: transformations are applied right-to-left order
	glm::mat4 modelBall = translationBall * rotationBall * scaleBall;
	
	/////////////////////////////////
	//     Set Light Variables     //
	/////////////////////////////////

	glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);
	glUniform3f(light0ColorLoc, 1.0f, 1.0f, 1.0f); // 100% strength, white
	glUniform3f(light0PositionLoc, 15.0f, -10.0f, 15.0f); // off to the front-right
	glUniform3f(light1ColorLoc, 0.2f, 0.5f, 0.5f); // 50% strength, light-cyan
	glUniform3f(light1PositionLoc, -1.0f, 10.0f, 15.0f); // behind the scene

	//////////////////////////////
	//     Main Render Loop     //
	//////////////////////////////
	
	// Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// black background
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programId);

		processKeyInput(window);
		const glm::vec3 cameraPosition = gCamera.Position;
		glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// per-frame timing
		// --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// camera/view transformation
		glm::mat4 View = gCamera.GetViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(View));

		// Calculate Projection
		glm::mat4 Projection;
		float scale = 75;
		if (gIsPerspective)
			Projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		else
			Projection = glm::ortho(-(GLfloat)WIDTH / scale, (GLfloat)WIDTH / scale, -(GLfloat)HEIGHT / scale, (GLfloat)HEIGHT / scale, -50.0f, 50.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(Projection));

		//enable attribute arrays
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//-----Plane 1 (Table)-----
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTable));

		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPlane);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : normals
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferPlane);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : uv
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferPlane);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId0);
		glUniform1i(gTextureId0, 0);

		// Draw the triangles to the plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Cube (Tissue Box)-----
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));

		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCube);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : normals
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferCube);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : uv
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferCube);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId1);
		glUniform1i(gTextureId1, 0);

		// Draw the triangles to the cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Plane 2 (Tissue Box Hole)------
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelHole));

		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPlane);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : normals
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferPlane);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : uv
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferPlane);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId2);
		glUniform1i(gTextureId2, 0);

		// Draw the triangles the plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Plane 2 (Tissue)------
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTissue));

		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPlane);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : normals
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferPlane);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : uv
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferPlane);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId3);
		glUniform1i(gTextureId3, 0);

		// Draw the triangles the plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Pyramid-----
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPyramid));

		// 1st attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPyramid);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : normals
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferPyramid);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : uv
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferPyramid);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId5);
		glUniform1i(gTextureId5, 0);

		// Draw the triangles to make a pyramid
		glDrawArrays(GL_TRIANGLES, 0, 18);

		// unbind VBOs and disable attribute arrays
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		//-----Cylinder 1 (Cap)------
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCap));

		// bind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferCap);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferCap);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId6);
		glUniform1i(gTextureId6, 0);

		// set attrib arrays with stride and offset
		int strideCap = cap.getInterleavedStride();   // should be 32 bytes
		glVertexAttribPointer(0, 3, GL_FLOAT, false, strideCap, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, strideCap, (void*)(sizeof(float)*3));
		glVertexAttribPointer(2, 2, GL_FLOAT, false, strideCap, (void*)(sizeof(float)*6));

		// draw a cylinder with VBO
		glDrawElements(GL_TRIANGLES, cap.getIndexCount(), GL_UNSIGNED_INT, (void*)0); 

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Cylinder 2 (Aloe Container)-----
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelContainer));

		// bind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferContainer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferContainer);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId4);
		glUniform1i(gTextureId4, 0);

		// set attrib arrays with stride and offset
		int strideContainer = container.getInterleavedStride();   // should be 32 bytes
		glVertexAttribPointer(0, 3, GL_FLOAT, false, strideCap, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, strideCap, (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, false, strideCap, (void*)(sizeof(float) * 6));

		// draw a cylinder with VBO
		glDrawElements(GL_TRIANGLES, cap.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-----Sphere (Stress Ball)-----
		// set the model
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBall));

		// bind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, interleavedBufferBall);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferBall);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTextureId7);
		glUniform1i(gTextureId7, 0);

		// set attrib arrays with stride and offset
		int stride = ball.getInterleavedStride();     // should be 32 bytes
		glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 6));

		// draw a sphere with VBO
		glDrawElements(GL_TRIANGLES,ball.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

		// unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		//disable attribute arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// Cleanup VBOs
	glDeleteBuffers(1, &vertexBufferPlane); // plane
	glDeleteBuffers(1, &normalBufferPlane);
	glDeleteBuffers(1, &uvBufferPlane);
	glDeleteBuffers(1, &vertexBufferPyramid); // pyramid
	glDeleteBuffers(1, &normalBufferPyramid);
	glDeleteBuffers(1, &uvBufferPyramid);
	glDeleteBuffers(1, &interleavedBufferCap); // cap
	glDeleteBuffers(1, &indexBufferCap);
	glDeleteBuffers(1, &vertexBufferCube); // cube
	glDeleteBuffers(1, &normalBufferCube);
	glDeleteBuffers(1, &uvBufferCube);
	glDeleteBuffers(1, &interleavedBufferContainer); //container
	glDeleteBuffers(1, &indexBufferContainer);
	glDeleteBuffers(1, &interleavedBufferBall); //ball
	glDeleteBuffers(1, &indexBufferBall);

	glDeleteProgram(programId);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Initialize GLFW, GLEW, and the window
int initializeWindow() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mousePositionCallback);

	// Ensure we can capture the keyboard and mouse
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);

	return 0;
}

// Processes key inputs continuously
void processKeyInput(GLFWwindow* window) {
	float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
}

// Callback for when the users moves the mouse
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (gFirstMouse) {
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// Callback for when the user uses the mouse scroll
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	gCamera.ProcessMouseScroll(yoffset);
}

// Callback for when the user uses the P key (applicable for single presses) 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		gIsPerspective = !gIsPerspective;
}

// Flips the Y axis, because images are loaded with Y axis going down, but OpenGL's Y axis goes up.
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}

// Generate and load the textures
bool createTexture(const char* filename, GLuint& textureId) {
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image) {
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else {
			std::cout << "Not implemented to handle image with " << channels << " channels" << std::endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}