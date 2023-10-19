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

void FollowEnemy::Tick(float dt) {
	// move toward the target
	float dirX = target->GetPosition().x - position.x;
	if (abs(dirX) > stopDist) {
		if (dirX > 0)
			velocity.x = speed;
		else if (dirX < 0)
			velocity.x = -speed;
	}

	// update position
	position.x += velocity.x * dt;

	// collide with the tilemap
	float2 nextPos = position + velocity * dt;
	uint2 tileSize = tilemap->GetTileSize();

	int topTile = nextPos.y / tileSize.y;
	int bottomTile = (nextPos.y + size.y) / tileSize.y;
	bottomTile--;
	for (uint y = topTile; y <= bottomTile; y++) {
		if (velocity.x < 0) {
			int leftTile = nextPos.x / tileSize.x;
			if (tilemap->GetTile(leftTile, y)->tileType == Tileset::TileTypes::Collision) {
				nextPos.x = leftTile * tileSize.x + tileSize.x;
				position.x = nextPos.x;
				velocity.x = 0.0f;
			}
		}
		else if (velocity.x > 0) {
			int rightTile = (nextPos.x + size.x) / tileSize.x;
			if (tilemap->GetTile(rightTile, y)->tileType == Tileset::TileTypes::Collision) {
				nextPos.x = rightTile * tileSize.x - size.x;
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

void PathEnemy::Tick(float dt) {
	float dirX = curDest - position.x;
	if (abs(dirX) > stopDist) {
		// move to the destination
		if (dirX > 0)
			velocity.x = speed;
		else if (dirX < 0)
			velocity.x = -speed;
	}
	else {
		if (curDest == dest0)
			curDest = dest1;
		else if (curDest == dest1)
			curDest = dest0;
	}

	// update position
	position.x += velocity.x * dt;
}
