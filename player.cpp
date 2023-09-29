#include "precomp.h"
#include "gameobject.h"
#include "player.h"

Player::Player(Input* input, Sprite* sprite, float2 position, float jumpHight, float speed) :
	GameObject(sprite, position, speed),
	jumpHight(jumpHight),
	input(input) {
}

void Player::Update(float dt) {
	// Input
	if (input->GetKey(68)) {
		velocity.x = speed;
	}
	if (input->GetKey(65)) {
		velocity.x = -speed;
	}
	if (input->GetKeyDown(32) && onGround) {
		velocity.y = -jumpHight;
	}

	GameObject::Update(dt);
}