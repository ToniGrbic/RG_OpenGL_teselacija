#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cerrno>
#include <string>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window variables
GLFWwindow* window;
const std::string windowName = "Tessellation";
const unsigned int SCREEN_WIDTH = 720;
const unsigned int SCREEN_HEIGHT = 720;

// Rendering settings
bool vSync = true;
bool wireframe = true;

// Shader Programs
GLuint shaderProgram;

// Textures
GLuint screen;

std::vector<GLfloat> genNonSymPlaneUniform(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int div)
{
	std::vector<GLfloat> plane;
	// Calculate directional vectors
	glm::vec3 dir03 = (v3 - v0) / float(div);
	glm::vec3 dir12 = (v2 - v1) / float(div);

	// Loop over divisions in the first direction
	for (int i = 0; i < div + 1; i++)
	{
		glm::vec3 dir03i = dir03 * float(i);
		// Loop over divisions in the second direction
		for (int j = 0; j < div + 1; j++)
		{
			glm::vec3 acrossj = ((v1 + dir03i) - (v0 + dir03i)) / float(div);
			glm::vec3 dir12j = acrossj * float(j);
			glm::vec3 crntVec = v0 + dir03i + dir12j;
			// Position
			plane.push_back(crntVec.x);
			plane.push_back(crntVec.y);
			plane.push_back(crntVec.z);
			// Texture UV coodrs
			plane.push_back(float(j) / div);
			plane.push_back(float(i) / div);
		}
	}
	return plane;
}

// Plane indices for triangle patches
std::vector<GLuint> genPlaneInd(int div)
{
	std::vector<GLuint> indices;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;
			// Top triangle								   ____
			indices.push_back(index);					// |  /
			indices.push_back(index + (div + 1) + 1);	// | /
			indices.push_back(index + (div + 1));		// |/
			// Bot triangle
			indices.push_back(index);					//   /|
			indices.push_back(index + 1);				//	/ |
			indices.push_back(index + (div + 1) + 1);	// /__|
		}
	}

	return indices;
}

// Plane indices for quad patches
std::vector<GLuint> genPlaneIndTes(int div)
{
	std::vector<GLuint> indices;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;			// 3___2
			indices.push_back(index);					//     |
			indices.push_back(index + 1);				//     |
			indices.push_back(index + (div + 1) + 1);	//  ___|
			indices.push_back(index + (div + 1));		// 0   1
		}
	}

	return indices;
}

// Screen mesh
GLuint VAO, VBO, EBO;

// Quads
int divisions = 4;
std::vector<GLfloat> vertices = genNonSymPlaneUniform(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f), divisions);
std::vector<GLuint> indices = genPlaneIndTes(divisions);

// Triangle
//std::vector<GLfloat> vertices = 
//{
//	-0.6f, -0.3f, 0.0f, 0.0f, 0.0f,
//	 0.6f, -0.3f, 0.0f, 1.0f, 0.0f,
//	 0.0f,  0.6f, 0.0f, 0.5f, 1.0f,
//};
//std::vector<GLuint> indices = { 0, 1, 2 };

void init()
{
	glfwInit();
	

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void createContext()
{
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowName.c_str(), NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context\n";
	}
}

void glEnableConfig()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

std::string readText(const char* textFile)
{
	std::FILE* fp = std::fopen(textFile, "rb");
	if (fp)
	{
		std::string contents;
		std::fseek(fp, 0, SEEK_END);
		contents.resize(std::ftell(fp));
		std::rewind(fp);
		std::fread(&contents[0], 1, contents.size(), fp);
		std::fclose(fp);
		return(contents);
	}
	throw(errno);
}

void initRender()
{
	std::string vertexCode = readText("vertex.vert");
	std::string fragmentCode = readText("fragment.frag");

	//comment this block to disable teselation
	std::string tesControlCode = readText("tessellation.tesc");
	std::string tesEvalCode = readText("tessellation.tese");
	//----------------------------------------------------

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//comment this block to disable teselation
	const char* tesControlSource = tesControlCode.c_str();
	const char* tesEvalSource = tesEvalCode.c_str();
	//----------------------------------------------------

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//comment this block to disable teselation
	GLuint tesControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tesControlShader, 1, &tesControlSource, NULL);
	glCompileShader(tesControlShader);
	GLuint tesEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tesEvalShader, 1, &tesEvalSource, NULL);
	glCompileShader(tesEvalShader);
	//----------------------------------------------------

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//comment this to disable teselation
	glAttachShader(shaderProgram, tesControlShader);
	glAttachShader(shaderProgram, tesEvalShader);
	//----------------------------------------------------

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//comment this block to disable teselation
	glDeleteShader(tesControlShader);
	glDeleteShader(tesEvalShader);
	//---------------------------------

	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(EBO, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);


	glCreateTextures(GL_TEXTURE_2D, 1, &screen);
	glTextureParameteri(screen, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screen, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screen, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screen, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screen, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindImageTexture(0, screen, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
