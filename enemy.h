#pragma once

namespace Tmpl8 {
	class Enemy : public GameObject {
	public:
		// constructor / destructor
		Enemy(Sprite* sprite, float2 position, float speed);

		// special operations
		void Collided(GameObject* entity) override;

	private:
		// attributes
		float knockback = 100.0f;
	};

	class StaticEnemy : public Enemy {
	public:
		// constructor / destructor
		StaticEnemy(Sprite* sprite, float2 position);
	};

	class FollowEnemy : public Enemy {
	public:
		// constructor / destructor
		FollowEnemy(Sprite* sprite, float2 position, float speed, GameObject* target, Tilemap* tilemap);

		// special operations
		void Update(float dt) override;

	private:
		// attributes
		float stopDist = 10.0f;
		GameObject* target;
		Tilemap* tilemap;
	};

	class PathEnemy : public Enemy {
	public:
		// constructor / destructor
		PathEnemy(Sprite* sprite, float2 position, float speed, float x1, float x2);

		// special operations
		void Update(float dt) override;

	private:
		// attributes
		float stopDist = 10.0f;
		float leftX, rightX;
		GameObject* target;
		Tilemap* tilemap;
	};
}