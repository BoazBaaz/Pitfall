#include "precomp.h"
#include "gameobject.h"

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
	lastPosition = position;
}