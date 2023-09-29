#pragma once

namespace Tmpl8 {
	class GameObject {
		enum States {
			idle = 0,
			moving,
			jumping,
			crouching,
			swiming
		};
	public:
		// constructor / destructor
		GameObject(Sprite* sprite, float2 position, float speed = 1);
		~GameObject();

		// member data access
		float2 GetPos() const { return position; }
		float2 GetVel() const { return velocity; }
		uint2 GetSize() const { return size; }
		Sprite* GetSprite() const { return sprite; }
		void SetPos(float x, float y) { position.x = x, position.y = y; }
		void SetPos(float2 pos) { position = pos; }
		void SetVel(float x, float y) { velocity.x = x, velocity.y = y; }
		void SetVel(float2 vel) { velocity = vel; }

		// special operations
		virtual void Update(float dt);

		// attributes
		//States state = States::idle;

		bool onGround = false;

	protected:
		// attributes
		float2 position;
		float2 velocity;
		uint2 size;
		Sprite* sprite;
		float speed = 1;
	};
}