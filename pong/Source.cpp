//debug mode
#define SHOWFPS

//includes
#define GLEW_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "ResourceManager.h"
#include "Mate.h"

#include <iostream>

// prototypes

// callback functions prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// settings
const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

// game object
Game Pong(SCREEN_WIDTH, SCREEN_HEIGHT);

int main()
{
	// initialize glfw and create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// setup callback functions
	glfwSetKeyCallback(window, key_callback);

	// configure opengl
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize game
	Pong.Init();

	// timing variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
#ifdef SHOWFPS
	GLfloat lastShowFPS = 0.0f;
#endif

	// set default game state
	Pong.State = GAME_PAUSE;

	// loop
	while (!glfwWindowShouldClose(window))
	{
		// calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
#ifdef SHOWFPS
		if (currentFrame - lastShowFPS >= 0.25f)
		{
			Pong.FPS = (GLuint)(1 / deltaTime);
			lastShowFPS = currentFrame;
		}
#endif
		deltaTime = Mate::clamp(deltaTime, 0.0f, 0.03f);

		// poll events
		glfwPollEvents();

		// manage user inputs
		Pong.ProcessInput(deltaTime);

		// update game state
		Pong.Update(deltaTime);

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Pong.Render();

		// swap buffers
		glfwSwapBuffers(window);

	}

	// terminate program and delete all resources
	ResourceManager::Clear();
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// set the key array state
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Pong.Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
		{
			Pong.Keys[key] = GL_FALSE;
			Pong.KeysProcessed[key] = GL_FALSE;
		}
			
	}
}
