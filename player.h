#pragma once

namespace Tmpl8 {
	class Player : public GameObject {
	public:
		// constructor / destructor
		Player(Input* input, Sprite* sprite, float2 position, float speed);
		~Player() = default;

		// special operations
		void Update(float dt) override;
		void TileCollision(Tilemap* tilemap, float dt);
		bool AABBCollision(GameObject* object);

	private:
		// attributes
		float collisionMargin = 2.0f;
		float snapMargin = 8.0f;
		int ladderSpeed = 150;
		int jumpHight = 800;
		int health = 3;
		bool onGround = false;
		bool isClimbing = false;
		Input* input;
	};
}