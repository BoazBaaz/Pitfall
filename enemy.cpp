#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "enemy.h"

Enemy::Enemy(Sprite* sprite, float2 position, float speed) :
	GameObject(sprite, position, speed) {
}

FollowEnemy::FollowEnemy(Sprite* sprite, float2 position, float speed, GameObject* target, Tilemap* tilemap) :
	Enemy(sprite, position, speed),
	target(target),
	tilemap(tilemap) {
}

void FollowEnemy::Update(float dt) {
	// move toward the target
	float dirX = target->GetPos().x - position.x;
	float distX = abs(dirX);
	if (distX < stopDist) {
		if (dirX > 0)
			velocity.x = speed;
		else if (dirX < 0)
			velocity.x = -speed;
	}

	// update position
	position.x += velocity.x * dt;

	// collide with the tilemap
	float2 nextPos = position + velocity * dt;

	int topTile = nextPos.y / tilemap->tileSize.y;
	int bottomTile = (nextPos.y + size.y) / tilemap->tileSize.y;

	for (uint y = topTile; y <= bottomTile; y++) {
		if (velocity.x < 0) {
			int leftTile = nextPos.x / tilemap->tileSize.x;
			if (tilemap->GetTile(leftTile, y).tileState == Tilemap::TileStates::collision) {
				nextPos.x = leftTile * tilemap->tileSize.x + tilemap->tileSize.x;
				position.x = nextPos.x;
				velocity.x = 0.0f;
			}
		}
		else if (velocity.x > 0) {
			int rightTile = (nextPos.x + size.x) / tilemap->tileSize.x;
			if (tilemap->GetTile(rightTile, y).tileState == Tilemap::TileStates::collision) {
				nextPos.x = rightTile * tilemap->tileSize.x - size.x;
				position.x = nextPos.x;
				velocity.x = 0.0f;
			}
		}
	}
}

PathEnemy::PathEnemy(Sprite* sprite, float2 position, float speed, bool dest, float dist) :
	Enemy(sprite, position, speed),
	dest0(position.x),
	dest1(position.x),
	curDest(position.x) {
	if (dest)
		dest1 += dist;
	else
		dest0 -= dist;
}

void PathEnemy::Update(float dt) {
	float dirX = curDest - position.x;
	float distX = abs(dirX);
	if (distX < stopDist) {
		// move to the destination
		if (dirX > 0)
			velocity.x = speed;
		else if (dirX < 0)
			velocity.x = -speed;
	}

	// update position
	position.x += velocity.x * dt;
}
