#include "precomp.h"
#include "gameobject.h"

GameObject::GameObject(Sprite* sprite, float2 position) :
	sprite(sprite),
	transform{ position, float2(0, 0) } {
}

GameObject::GameObject(Sprite* sprite, float x, float y) :
	sprite(sprite),
	transform{ float2(x, y), float2(0, 0) } {

}

GameObject::~GameObject() {
	delete sprite;
}

void GameObject::Update(Input* input, float dt) {

}

void GameObject::Collision() {
}

