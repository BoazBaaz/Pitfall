#include "precomp.h"
#include "gameobject.h"
#include "player.h"

Player::Player(Sprite* sprite, float2 position) :
	GameObject(sprite, position) {
	spriteWidth = sprite->GetWidth();
	spriteHeight = sprite->GetHeight();
}

Player::Player(Sprite* sprite, float2 position, float speed) :
	GameObject(sprite, position),
	speed(speed) {
	spriteWidth = sprite->GetWidth();
	spriteHeight = sprite->GetHeight();
}

Player::~Player() {
	GameObject::~GameObject();
}

void Player::Update(Input* input, float dt) {
	if (input->GetKey(68)) {
		transform.position.x += speed * dt;
	}
	if (input->GetKey(65)) {
		transform.position.x -= speed * dt;
	}

	//if (transform.position.x > SCRWIDTH - spriteWidth || transform.position.x < 0) {
	//	transform.position.x = Max(0.0f, Min((float)SCRWIDTH - spriteWidth, transform.position.x));
	//	transform.velocity.x = -transform.velocity.x;
	//}

	//// if you release space and the ground collision is true, update the velocity and acceleration and reset the boost
	//if (game->input->GetKeyUp(SDL_SCANCODE_SPACE) && boostReady) {
	//	transform.velocity = (input->GetMousePos() - transform.position).normalized() * boostPower;
	//	boostReady = false;
	//}

	// update the player position
	//BouncePhysics(game, dt);

	// draw the object
	//Draw(screen);
}
