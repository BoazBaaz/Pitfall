#pragma once

namespace Tmpl8 {
	class Player : public GameObject {
	public:
		// constructor / destructor
		Player(Input* input, Sprite* sprite, float2 position, float jumpHight, float speed);
		Player(Input* input, Sprite* sprite, float2 position, uint2 size, float jumpHight, float speed);
		~Player() = default;

		// special operations
		void Update(float dt) override;

	private:
		// attributes
		float jumpHight = 0;
		Input* input;
	};
}