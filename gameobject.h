#pragma once

namespace Tmpl8 {
	class GameObject {
	public:
		// constructor / destructor
		GameObject(Sprite* sprite, float2 position, float speed);
		GameObject(Sprite* sprite, float2 position, uint2 size, float speed);
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
		virtual void Update(float dt) {};
		virtual void Collided(GameObject* object) {};

	protected:
		// attributes
		float2 position;
		float2 velocity;
		uint2 size;
		Sprite* sprite;
		float speed = 0;
	};
}