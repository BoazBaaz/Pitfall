#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "player.h"

#define GRAVITY			9.81f
#define DECELERATION	0.995f

Player::Player(Input* input, Sprite* sprite, float2 position, float speed) :
	GameObject(sprite, position, speed),
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
		velocity.y = -static_cast<float>(jumpHight);
	}

	if (!onGround || !isClimbing) {
		velocity.y += GRAVITY;
	}
	velocity.y *= DECELERATION;
	velocity.x *= DECELERATION / 2;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}

// I some help from this tutorial https://jonathanwhiting.com/tutorial/collision/
void Player::TileCollision(Tilemap* tilemap, float dt) {
	float2 nextPos = position + velocity * dt;
	uint2 tileSize = tilemap->tileSize;

	// get the tiles that the player corners is overlapping with
	int leftTile = (nextPos.x + collisionMargin) / tileSize.x;
	int rightTile = (nextPos.x + size.x - collisionMargin) / tileSize.x;
	int topTile = (nextPos.y + collisionMargin) / tileSize.y;
	int bottomTile = (nextPos.y + size.y - collisionMargin) / tileSize.y;

	// clamp the player position to alway be inside the grid
	if (leftTile < 0) leftTile = 0;
	if (rightTile > tilemap->columns) rightTile = tilemap->columns;
	if (topTile < 0) topTile = 0;
	if (bottomTile > tilemap->rows) bottomTile = tilemap->rows;

	bool grounded = false;
	bool climbing = false;

	// loop through all the tile that are overlapping with the player
	for (int x = leftTile; x <= rightTile; x++) {
		for (int y = topTile; y <= bottomTile; y++) {
			Tilemap::TileStates tileState = tilemap->GetTile(x + y * tilemap->columns).tileState;

			// check if the player is overlapping with a ladder
			if (tileState == Tilemap::TileStates::ladder) { 
				if (input->GetKey(87)) { /*W*/
					climbing = true;
					velocity.y = -ladderSpeed;
					position.x = static_cast<float>(((x * tileSize.x) + tileSize.x / 2) - size.x / 2);

				}
				else if (input->GetKey(83)) { /*S*/
					climbing = true;
					velocity.y = ladderSpeed;
					position.x = static_cast<float>(((x * tileSize.x) + tileSize.x / 2) - size.x / 2);
				}
			}
			// check if the player has collided with something while it was not climbing
			if (tileState == Tilemap::TileStates::collision && !climbing) {
				if (x == leftTile) {
					nextPos.x = leftTile * tileSize.x + tileSize.x;
				}
				else if (x == rightTile) {
					nextPos.x = rightTile * tileSize.x - size.x;
				}
				position.x = nextPos.x;
				velocity.x = 0.0f;
			}
		}
	}

	// re-calculate the left and right tile
	leftTile = (nextPos.x + collisionMargin) / tileSize.x;
	rightTile = (nextPos.x + size.x - collisionMargin) / tileSize.x;

	// clamp the player position to alway be inside the grid
	if (leftTile < 0) leftTile = 0;
	if (rightTile > tilemap->columns) rightTile = tilemap->columns;

	// check if the velocity on the Y axis is not 0
	if (velocity.y != 0) {

		// set the tileY using the velocity
		int tileY = (velocity.y < 0) ? (topTile - 1) : (bottomTile + 1);

		// loop through all the tiles above or below the player
		for (int x = leftTile; x <= rightTile; x++) {
			Tilemap::TileStates tileState = tilemap->GetTile(x + tileY * tilemap->columns).tileState;

			// check if the player has collided with a tile while it is not climbing
			if (tileState == Tilemap::TileStates::collision && !climbing) {

				// set the player position to the bottom the the tile hit
				if (velocity.y < 0) {
					float newY = tileY * tileSize.y + tileSize.y;
					if (position.y - snapMargin < newY) {
						position.y = newY;
						velocity.y = 0.0f;
					}
				}
				// set the player position to the top the the tile hit and set grouned to true
				else if (velocity.y > 0) {
					float newY = tileY * tileSize.y - size.y;
					if (position.y + snapMargin > newY) {
						position.y = newY;
						velocity.y = 0.0f;
						grounded = true;
					}
				}
			}
		}
	}

	onGround = grounded;
	isClimbing = climbing;
}

bool Player::AABBCollision(GameObject* object) {
	if (position.x + size.x >= object->GetPos().x && position.x <= object->GetPos().x + object->GetSize().x &&
		position.y + size.y >= object->GetPos().y && position.y <= object->GetPos().y + object->GetSize().y) {
		return true;
	}
	return false;
}
