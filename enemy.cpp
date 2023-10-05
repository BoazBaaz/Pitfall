#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "enemy.h"

Enemy::Enemy(Sprite* sprite, float2 position, float speed) :
	GameObject(sprite, position, speed) {
}

void Enemy::Collided(GameObject* entity) {
	float2 dir = entity->GetPos().x - position.x;
	entity->SetVel(dir * knockback);
}

StaticEnemy::StaticEnemy(Sprite* sprite, float2 position) :
	Enemy(sprite, position, 0) {
}


FollowEnemy::FollowEnemy(Sprite* sprite, float2 position, float speed, GameObject* target, Tilemap* tilemap) :
	Enemy(sprite, position, speed),
	target(target),
	tilemap(tilemap) {
}

void FollowEnemy::Update(float dt) {
	// move toward the target
	float dirX = target->GetPos().x - position.x;
	if (abs(dirX) > stopDist) {
		if (dirX > 0)
			velocity.x = speed * dt;
		else if (dirX < 0)
			velocity.x = -speed * dt;
	}

	// collide with the tilemap

}

PathEnemy::PathEnemy(Sprite* sprite, float2 position, float speed, float x1, float x2) :
	Enemy(sprite, position, speed),
	leftX(x1),
	rightX(x2) {
}

PathEnemy::PathEnemy(Sprite* sprite, float2 position, float speed, PathTypes pathType, float distance) {
}

void PathEnemy::Update(float dt) {
}
