#pragma once

namespace Tmpl8 {
	class Player : public GameObject {
	public:
		// constructor / destructor
		Player(Input* input, Sprite* sprite, int2 position, float jumpHight, float speed = 1);
		~Player() = default;

		// special operations
		void Update(float dt);

		// attributes
		bool onGround = false;

	private:
		// attributes
		float jumpHight = 0;
		Input* input;
	};
}