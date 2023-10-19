#include "precomp.h"
#include "gameobject.h"
#include "camera.h"
#include "tmloader.h"

GameObject::GameObject(Sprite* sprite, float2 position, float speed) :
	Entity(position, uint2(sprite->GetWidth(), sprite->GetHeight())),
	velocity(0),
	sprite(sprite),
	speed(speed) {
}

GameObject::~GameObject() {
	delete sprite;
}

