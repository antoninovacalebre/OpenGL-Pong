#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// game states
enum GameState {
	GAME_ACTIVE,
	GAME_PAUSE,
	GAME_WIN
};

// game settings (eg. ball velocity, player velocity, ecc.)
class Game
{
public:
	// state, inputs, dimensions
	GameState State;
	GLboolean Keys[1024];
	GLboolean KeysProcessed[1024];
	GLboolean ShowFPS;
	GLuint Width, Height;
	GLuint FPS;

	glm::vec4 coursePos;
	GLuint pointsP1, pointsP2;

	// constructor / deconstructor
	Game(GLuint width, GLuint height);
	~Game();

	// initialize game (load shaders, textures, ...)
	void Init();

	// game loop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
		
private:
	GLfloat UnitX, UnitY;

	void ResetBall();
	void ResetPlayers();
	void Reset();
	void WinCheck();
	void DoCollisions();
};

#endif