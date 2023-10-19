#pragma once

namespace Tmpl8 {
	class Tilemap;
	class Enemy : public GameObject {
		enum EnemyType { Static = 0, Follow, Path };
	public:
		// constructor / destructor
		Enemy(Sprite* sprite, float2 position, float speed = 0);

	protected:
		float stopDist = 10.0f;
	};

	class FollowEnemy : public Enemy {
	public:
		// constructor / destructor
		FollowEnemy(Sprite* sprite, float2 position, float speed, GameObject* target, Tilemap* tilemap);

		// special operations
		void Tick(float dt) override;

	private:
		// attributes
		Tilemap* tilemap;
		GameObject* target;
	};

	class PathEnemy : public Enemy {
	public:
		// constructor / destructor
		PathEnemy(Sprite* sprite, float2 position, float speed, bool dest, float dist);

		// special operations
		void Tick(float dt) override;

	private:
		// attributes
		float dest0, dest1;
		float curDest;
	};
}