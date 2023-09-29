#include "precomp.h"
#include "gameobject.h"

#define GRAVITY			9.81f
#define DECELERATION	0.995f

GameObject::GameObject(Sprite* sprite, float2 position, float speed) :

	position(position),
	velocity(0),
	size(sprite->GetWidth(), sprite->GetHeight()),
	sprite(sprite),
	speed(speed) {
}

GameObject::~GameObject() {
	delete sprite;
}

void GameObject::Update(float dt) {
	velocity.y += GRAVITY;

	velocity.y *= DECELERATION;
	velocity.x *= DECELERATION / 2;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}