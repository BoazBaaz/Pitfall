#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "player.h"

#define GRAVITY			9.81f
#define DECELERATION	0.995f

Player::Player(Input* input, Sprite* sprite, int2 position, float jumpHight, float speed) :
	GameObject(sprite, position, speed),
	jumpHight(jumpHight),
	input(input) {
}

void Player::Update(float dt) {

	// Input
	if (input->GetKey(68)) {
		position.x += speed * dt;
	}
	if (input->GetKey(65)) {
		position.x -= speed * dt;
	}
	if (input->GetKeyDown(32)) {
		velocity.y = -jumpHight * 10;
		onGround = false;
	}

	if (onGround == false) {
		velocity.y += GRAVITY;
	}

	// add gravity and deceleration to the velocity
	velocity *= DECELERATION;

	// update the position using the velocity
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

	printf("PosX: %i, PosY: %i\n", position.x, position.y);
	printf("VelX: %f, VelY: %f\n", velocity.x, velocity.y);

	GameObject::Update(dt);
}