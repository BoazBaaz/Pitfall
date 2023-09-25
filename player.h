#pragma once

namespace Tmpl8 {
	class Player : public GameObject {
	public:
		// constructor / destructor
		Player(Input* input, Sprite* sprite, float2 position, float jumpHight, float speed = 1);
		~Player() = default;

		// special operations
		void Update(float dt) override;
		void Render(Surface* screen) override;

		// attributes
		bool onGround = false;
		bool canJump = false;

	private:
		// attributes
		float jumpHight = 0;
		int2 renderPos;
		Input* input;
	};
}