#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "CourseHandler.h"
#include "Mate.h"
#include "TextRenderer.h"

#include <iostream>
#include <sstream>

// game objects settings
const glm::vec2 PLAYER_SIZE(15.0f, 90.0f);
const GLfloat PLAYER_VELOCITY(800.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -200.0f);	//todo: random
const GLfloat BALL_SIZE = 20.0f;
const GLfloat BALL_SPEED = 700.0f;
const GLfloat BALL_HIT_BOOST = 1.5f;

const GLfloat WALL_THICKNESS = 10.0f;
const glm::vec4 WALL_PADDING(100.0f, 15.0f, 15.0f, 15.0f);

// objects
SpriteRenderer *Renderer;
TextRenderer *Text;
GameObject *Player1;
GameObject *Player2;
GameObject *NWall;
GameObject *EWall;
GameObject *SWall;
GameObject *WWall;
GameObject *Ball;

Game::Game(GLuint width, GLuint height)
	: State(GAME_PAUSE), Keys(), Width(width), Height(height)
{
	this->UnitX = this->Width * 0.001f;
	this->UnitY = this->Height * 0.001f;
}

Game::~Game()
{
	delete Renderer;
	delete Text;
	delete Player1;
	delete Player2;
	delete Ball;
	delete NWall;
	delete EWall;
	delete SWall;
	delete WWall;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");

	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), 
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	// Load textures
	ResourceManager::LoadTexture("textures/paddle.png", true, "white");

	// Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// setup game objects
	glm::vec4 wallPos = CourseHandler::MakeCourse(WALL_PADDING, WALL_THICKNESS);
	glm::vec2 wallLen(
		1000 - WALL_PADDING.y - WALL_PADDING.w,	//horizontal
		1000 - WALL_PADDING.x - WALL_PADDING.z	//vertical
	);
	this->coursePos = wallPos + glm::vec4(WALL_THICKNESS, WALL_THICKNESS, 0, 0);

	NWall = new GameObject(glm::vec2(wallPos.x, wallPos.y), 
		glm::vec2(wallLen.x, WALL_THICKNESS), ResourceManager::GetTexture("white"));
	EWall = new GameObject(glm::vec2(wallPos.z, wallPos.y), 
		glm::vec2(WALL_THICKNESS, wallLen.y), ResourceManager::GetTexture("white"));
	SWall = new GameObject(glm::vec2(wallPos.x, wallPos.w), 
		glm::vec2(wallLen.x, WALL_THICKNESS), ResourceManager::GetTexture("white"));
	WWall = new GameObject(glm::vec2(wallPos.x, wallPos.y), 
		glm::vec2(WALL_THICKNESS, wallLen.y), ResourceManager::GetTexture("white"));

	glm::vec2 playerPos1 = glm::vec2(
		this->coursePos.x + PLAYER_SIZE.x * 0.5f,
		(this->coursePos.y + this->coursePos.w - PLAYER_SIZE.y) * 0.5f
	);
	Player1 = new GameObject(playerPos1, PLAYER_SIZE, ResourceManager::GetTexture("white"));
	Player1->Color = glm::vec3(0.0f, 0.2f, 0.8f);
	Player1->Velocity = glm::vec2(0.0f);

	glm::vec2 playerPos2 = glm::vec2(
		this->coursePos.z - PLAYER_SIZE.x * 1.5f,
		(this->coursePos.y + this->coursePos.w - PLAYER_SIZE.y) * 0.5f
	);
	Player2 = new GameObject(playerPos2, PLAYER_SIZE, ResourceManager::GetTexture("white"));
	Player2->Color = glm::vec3(0.8f, 0.2f, 0.0f);
	Player2->Velocity = glm::vec2(0.0f);

	glm::vec2 ballPos = glm::vec2(
		0.5f * (this->coursePos.x + this->coursePos.z - BALL_SIZE),
		0.5f * (this->coursePos.y + this->coursePos.w - BALL_SIZE)
	);
	Ball = new GameObject(ballPos, glm::vec2(BALL_SIZE), ResourceManager::GetTexture("white"));
	Ball->Velocity = glm::normalize(INITIAL_BALL_VELOCITY) * BALL_SPEED * 0.6f;

	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("fonts/ocraext.TTF", 72);

	this->pointsP1 = 0;
	this->pointsP2 = 0;

	this->ShowFPS = GL_FALSE;
}

void Game::Update(GLfloat dt)
{
	if(this->State == GAME_ACTIVE)
	{
		// move ball and check for collisions
		glm::vec2 nextPos = Ball->Position + Ball->Velocity * dt;
		GLfloat angle;

		//collisions with walls
		if (nextPos.x < this->coursePos.x)
		{
			Ball->Velocity.x *= -1;
			this->ResetBall();
			nextPos = Ball->Position;
			this->pointsP2++;

			//std::cout << this->pointsP1 << " - " << this->pointsP2 << std::endl;
		}
		if (nextPos.x > this->coursePos.z - Ball->Size.x)
		{
			Ball->Velocity.x *= -1;
			this->ResetBall();
			nextPos = Ball->Position;
			this->pointsP1++;

			//std::cout << this->pointsP1 << " - " << this->pointsP2 << std::endl;
		}
		if (nextPos.y < this->coursePos.y)
		{
			nextPos.y = this->coursePos.y;
			Ball->Velocity.y *= -1;
		}
		if (nextPos.y > this->coursePos.w - Ball->Size.y)
		{
			nextPos.y = this->coursePos.w - Ball->Size.y;
			Ball->Velocity.y *= -1;
		}

		//collisions with player 1
		GLfloat normNextYPos;
		GLfloat boost;
		normNextYPos = (Player1->Position.y + Player1->Size.y * 0.5f) - (nextPos.y + Ball->Size.y * 0.5);
		normNextYPos /= (Player1->Size.y + Ball->Size.y) * 0.5f;

		if (Mate::abs(normNextYPos) < 1.0f && nextPos.x < Player1->Position.x + Player1->Size.x)
		{
			nextPos.x = Player1->Position.x + Player1->Size.x;
			if (Player1->Velocity.y != 0)
			{
				angle = Mate::clamp(asin(normNextYPos), glm::radians(-60.0f), glm::radians(60.0f));
				angle = - Player1->Velocity.y * Mate::abs(angle);
				boost = glm::dot(Player1->Velocity, Ball->Velocity) > 0.0f ? BALL_HIT_BOOST : 1.0f;
				Ball->Velocity = glm::vec2(cos(angle), sin(angle)) * BALL_SPEED * boost;
			}
			else
			{
				Ball->Velocity = glm::normalize(Ball->Velocity) * BALL_SPEED;
				Ball->Velocity.x *= -1;
			}
		}
	
		//collisions with player 2
		normNextYPos = (Player2->Position.y + Player2->Size.y * 0.5f) - (nextPos.y + Ball->Size.y * 0.5);
		normNextYPos /= (Player2->Size.y + Ball->Size.y) * 0.5f;
	
		if (Mate::abs(normNextYPos) < 1.0f && nextPos.x + Ball->Size.x > Player2->Position.x)
		{
			nextPos.x = Player2->Position.x - Ball->Size.x;
			if (Player2->Velocity.y != 0)
			{
				angle = Mate::clamp(asin(normNextYPos), glm::radians(-60.0f), glm::radians(60.0f));
				angle = -Player2->Velocity.y * Mate::abs(angle);
				boost = glm::dot(Player2->Velocity, Ball->Velocity) > 0.0f ? BALL_HIT_BOOST : 1.0f;
				Ball->Velocity = glm::vec2(-cos(angle), sin(angle)) * BALL_SPEED * boost;
			}
			else
			{
				Ball->Velocity = glm::normalize(Ball->Velocity) * BALL_SPEED;
				Ball->Velocity.x *= -1;
			}
		}
	
		//apply movement
		Ball->Position = nextPos;

		//check win state
		this->WinCheck();
	}
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		// pause
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->State = GAME_PAUSE;
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}

		// Move player 1 and 2
		GLfloat velocity = PLAYER_VELOCITY * dt;

		GLfloat nextYPos = Player1->Position.y;
		Player1->Velocity.y = 0.0f;
		if (this->Keys[GLFW_KEY_W])
		{
			nextYPos -= velocity;
			if (nextYPos < NWall->Position.y + WALL_THICKNESS)	
				nextYPos = NWall->Position.y + WALL_THICKNESS;
			else	Player1->Velocity.y = 1.0f;
		}
		if (this->Keys[GLFW_KEY_S])
		{
			nextYPos += velocity;
			if (nextYPos > SWall->Position.y - Player1->Size.y)	
				nextYPos = SWall->Position.y - Player1->Size.y;
			else	Player1->Velocity.y = -1.0f;
		}
		Player1->Position.y = nextYPos;

		nextYPos = Player2->Position.y;
		Player2->Velocity.y = 0.0f;
		if (this->Keys[GLFW_KEY_UP])
		{
			nextYPos -= velocity;
			if (nextYPos < NWall->Position.y + WALL_THICKNESS)	
				nextYPos = NWall->Position.y + WALL_THICKNESS;
			else	Player2->Velocity.y = 1.0f;
		}
		if (this->Keys[GLFW_KEY_DOWN])
		{
			nextYPos += velocity;
			if (nextYPos > SWall->Position.y - Player2->Size.y)	
				nextYPos = SWall->Position.y - Player2->Size.y;
			else	Player2->Velocity.y = -1.0f;
		}
		Player2->Position.y = nextYPos;

		// Reset Game with 'R'
		if (this->Keys[GLFW_KEY_R] && !this->KeysProcessed[GLFW_KEY_R])
		{
			this->Reset();
		}
	}
	else if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->Reset();
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}
	}
	else if (this->State == GAME_PAUSE)
	{
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}
	}
	if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
	{
		this->ShowFPS = !(this->ShowFPS);
		this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
	}

}

void Game::Render()
{
	// GUI
	std::stringstream P1; P1 << this->pointsP1;
	std::stringstream P2; P2 << this->pointsP2;
	std::stringstream fps; fps << this->FPS;
	
	Text->RenderText(P1.str(), 200.0f, 10.0f, 1.0f, glm::vec2(this->UnitX, this->UnitY), glm::vec3(0.0f, 0.2f, 0.8f));
	Text->RenderText(P2.str(), 750.0f, 10.0f, 1.0f, glm::vec2(this->UnitX, this->UnitY), glm::vec3(0.8f, 0.2f, 0.0f));
	if(this->ShowFPS)
		Text->RenderText(fps.str(), 10.0f, 15.0f, 0.25f, glm::vec2(this->UnitX, this->UnitY), glm::vec3(0.0f, 1.0f, 0.0f));
	
	if (this->State == GAME_WIN)
	{
		std::string winText;
		glm::vec3 textColor;

		if (this->pointsP1 == 7)
		{
			winText = "P1 wins!";
			textColor = glm::vec3(0.0f, 0.2f, 0.8f);
		}
		else
		{
			winText = "P2 wins!";
			textColor = glm::vec3(0.8f, 0.2f, 0.1f);
		}

		Text->RenderText(winText, 300.0f, 300.0f, 1.0f, glm::vec2(this->UnitX, this->UnitY), textColor);
	}

	if (this->State == GAME_PAUSE)
	{
		Text->RenderText("PAUSE", 400.0f, 15.0f, 0.75f, glm::vec2(this->UnitX, this->UnitY), glm::vec3(1.0f));
	}

	// course
	NWall->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));
	EWall->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));
	SWall->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));
	WWall->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));

	// player paddles
	Player1->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));
	Player2->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));

	// ball
	Ball->Draw(*Renderer, glm::vec2(this->UnitX, this->UnitY));
}

void Game::DoCollisions()
{

}

void Game::ResetBall()
{
	Ball->Position = glm::vec2(
		0.5f * (this->coursePos.x + this->coursePos.z - BALL_SIZE),
		0.5f * (this->coursePos.y + this->coursePos.w - BALL_SIZE)
	);
	Ball->Velocity = glm::normalize(Ball->Velocity) * BALL_SPEED * 0.6f;
}

void Game::ResetPlayers()
{
	Player1->Position = glm::vec2(
		this->coursePos.x + PLAYER_SIZE.x * 0.5f,
		(this->coursePos.y + this->coursePos.w - PLAYER_SIZE.y) * 0.5f
	);
	Player2->Position = glm::vec2(
		this->coursePos.z - PLAYER_SIZE.x * 1.5f,
		(this->coursePos.y + this->coursePos.w - PLAYER_SIZE.y) * 0.5f
	);
}

void Game::Reset()
{
	ResetBall();
	ResetPlayers();

	this->pointsP1 = 0;
	this->pointsP2 = 0;

	this->State = GAME_PAUSE;
}

void Game::WinCheck()
{
	if (this->pointsP1 >= 7)
	{
		//std::cout << "P1 wins!" << std::endl;
		this->State = GAME_WIN;
	}
	else if (this->pointsP2 >= 7)
	{
		//std::cout << "P2 wins!" << std::endl;
		this->State = GAME_WIN;
	}
}