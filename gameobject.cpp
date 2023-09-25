#include "precomp.h"
#include "gameobject.h"

Tmpl8::GameObject::GameObject(Sprite* sprite, float2 position, float speed) :
	left(position.x),
	right(position.x + sprite->GetWidth()),
	top(position.y),
	bottom(position.y + sprite->GetHeight()),
	position(position),
	velocity(0),
	width(sprite->GetWidth()),
	height(sprite->GetHeight()), 
	sprite(sprite),
	speed(speed) {
}

GameObject::~GameObject() {
	delete sprite;
}

void GameObject::Update(float dt) {
	left = position.x;
	right = position.x + width;
	top = position.y;
	bottom = position.y + height;
}

void GameObject::Render(Surface* screen) {
	sprite->Draw(screen, position.x, position.y);
}