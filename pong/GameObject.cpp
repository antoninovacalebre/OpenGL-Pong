#include "GameObject.h"


GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer, glm::vec2 unit)
{
	renderer.DrawSprite(this->Sprite, glm::vec2(this->Position.x * unit.x, this->Position.y * unit.y), glm::vec2(this->Size.x * unit.x, this->Size.y * unit.y), this->Rotation, this->Color);
}
