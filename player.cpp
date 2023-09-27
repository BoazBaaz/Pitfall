#include "precomp.h"
#include "gameobject.h"
#include "player.h"

#define GRAVITY			9.81f
#define DECELERATION	0.995f

Player::Player(Input* input, Sprite* sprite, float2 position, float jumpHight, float speed) :
	GameObject(sprite, position, speed),
	jumpHight(jumpHight),
	input(input) {
}

void Player::Update(float dt) {
	GameObject::Update(dt);

	// Input
	if (input->GetKey(68)) {
		velocity.x = speed;
	}
	if (input->GetKey(65)) {
		velocity.x = -speed;
	}
	if (input->GetKeyDown(32) && onGround) {
		velocity.y = -jumpHight;
		onGround = false;
	}

	if (onGround == false) {
		velocity.y += GRAVITY;
	}

	velocity *= DECELERATION;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

	printf("PosX: %f, PosY: %f\n", position.x, position.y);
	printf("VelX: %f, VelY: %f\n", velocity.x, velocity.y);

}