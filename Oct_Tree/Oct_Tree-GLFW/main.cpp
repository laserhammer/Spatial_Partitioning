/*
Oct-Tree
(c) 2015
original authors: Benjamin Robbins
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*	This example builds upon the quad-tree example by now partitioning 3-D space into an oct-tee. The majority of the code is 
*	the same as before, except now the nodes are divided into 8 children instead of only 3 and collisions are also tested on the 
*	z-axis now. Once again all tree updating is handled iteratively.
*
*	1) RenderManager
*	- This class maintains data for everything that needs to be drawn in two display lists, one for non-interactive shapes and
*	one for interactive shapes. It handels the updating and drawing of these shapes.
*
*	2) InputManager
*	- This class handles all user input from the mouse and keyboard.
*
*	3) OctTreeManager
*	- This class maintains an array of references to InteractiveShapes and every frame divides them into an oct-tree structure. It handles the
*	generation and updating of this information based on the locations of the interactive shapes. Furthermore, it maintains references and updates
*	an array of division line RenderShapes that serve to more clearly depict what the current state of the quad tree is.
*
*	RenderShape
*	- Holds the instance data for a shape that can be rendered to the screen. This includes a transform, a vao, a shader, the drawing
*	mode (eg triangles, lines), it's active state, and its color
*
*	InteractiveShape
*	- Inherits from RenderShape, possessing all the same properties. Additionally, it has a collider and can use it to check collisions against
*	world boundries, other colliders, and the cursor.
*
*	Init_Shader
*	- Contains static functions for loading, compiling and linking shaders.
*
*/

#include <GLEW\GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <ctime>

#include "RenderShape.h"
#include "Init_Shader.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "OctTreeManager.h"

GLFWwindow* window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint vbo;
GLuint vao0;
GLuint vao1;
GLuint ebo0;
GLuint ebo1;
GLint posAttrib;
GLint uTransform;
GLint uColor;

GLfloat vertices[] = {
	-1.0f, +1.0f, -1.0f,	
	+1.0f, +1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,
	-1.0f, +1.0f, +1.0f,
	+1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	+1.0f, -1.0f, +1.0f
};

GLint elements[] = {
	// Front
	0, 1, 2,
	1, 3, 2,

	// Back
	4, 5, 6,
	5, 7, 6,

	// Top
	4, 5, 0,
	5, 0, 1,

	// Bottom
	3, 2, 7,
	2, 6, 7,

	// Right
	1, 5, 3,
	1, 7, 3,

	// Left
	0, 4, 6,
	0, 2, 6,
};

GLint outlineElements[] = {
	0, 1,	1, 3,	3, 2,	2, 0, 
	0, 4,	1, 5,	3, 7,	2, 6,
	4, 5,	5, 7,	7, 6,	6, 4,
};

void initShaders()
{
	char* shaders[] = { "fshader.glsl", "vshader.glsl" };
	GLenum types[] = { GL_FRAGMENT_SHADER, GL_VERTEX_SHADER };
	int numShaders = 2;
	
	shaderProgram = initShaders(shaders, types, numShaders);
	
	// Bind buffer data to shader values
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	uTransform = glGetUniformLocation(shaderProgram, "transform");
	uColor = glGetUniformLocation(shaderProgram, "color");
}

void init()
{
	if (!glfwInit()) exit(EXIT_FAILURE);

	//Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "Oct_Tree-GLFW", NULL, NULL); // Windowed

	//Activate window
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	// Store the data for the triangles in a buffer that the gpu can use to draw
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao0);
	glBindVertexArray(vao0);

	glGenBuffers(1, &ebo0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Compile shaders
	initShaders();

	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(outlineElements), outlineElements, GL_STATIC_DRAW);

	initShaders();

	glEnable(GL_DEPTH_TEST);

	glfwSetTime(0.0f);

	Shader shader;
	shader.shaderPointer = shaderProgram;
	shader.uTransform = uTransform;
	shader.uColor = uColor;

	time_t timer;
	time(&timer);
	srand((unsigned int)timer);

	RenderManager::GenerateShapes(shader, vao0, 100, GL_TRIANGLES, 36);

	InputManager::Init(window);
	
	OctTreeManager::InitOctTree(-1.337f, 1.337f, 1.0f, -1.0f, -3.0f, -5.0f, 4, 2, RenderShape(vao1, 24, GL_LINES, shader, glm::vec4(0.0f, 1.0f, 0.3f, 1.0f)));
	unsigned int shapesSize = RenderManager::interactiveShapes().size();
	for (unsigned int i = 0; i < shapesSize; ++i)
	{
		OctTreeManager::AddShape(RenderManager::interactiveShapes()[i]);
	}
}

void step()
{
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	InputManager::Update();

	// Get delta time since the last frame
	float dt = glfwGetTime();
	glfwSetTime(0.0f);

	OctTreeManager::UpdateOctTree();

	RenderManager::Update(dt);

	RenderManager::Draw();

	// Swap buffers
	glfwSwapBuffers(window);
}

void cleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo0);
	glDeleteBuffers(1, &ebo1);
	glDeleteBuffers(1, &vao0);
	glDeleteBuffers(1, &vao1);

	RenderManager::DumpData();

	OctTreeManager::DumpData();

	glfwTerminate();
}

int main()
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		step();
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}
