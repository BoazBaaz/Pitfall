#pragma once

namespace Tmpl8 {
	class Camera;
	class Entity {
	public:
		// constructor / destructor
		Entity(float2 position, uint2 size, bool isActive = false) : position(position), size(size), isActive(isActive) {};

		// member data access
		float2 GetPosition() const { return position; }
		void SetPosition(float _x, float _y) { position.x = _x, position.y = _y; }
		void SetPosition(float2 _position) { position = _position; }
		uint2 GetSize() const { return size; }
		bool GetActive() { return isActive; }
		void SetActive(bool _activity) { isActive = _activity; }

		// member data access
		virtual void Tick(float dt) {};
		virtual void Render(Camera* camera) {};

	protected:
		// attributes
		float2 position = 0;
		uint2 size = 0;
		bool isActive = false;
	};

	class GameObject : public Entity {
	public:
		// constructor / destructor
		GameObject(Sprite* _sprite, float2 _position, float _speed);
		~GameObject();

		// member data access
		float2 GetVelocity() const { return velocity; }
		void SetVelocity(float2 _vel) { velocity = _vel; }
		void SetVelocity(float _x, float _y) { velocity.x = _x, velocity.y = _y; }
		Sprite* GetSprite() const { return sprite; }

		// special operations
		void Tick(float dt) override {};
		void Render(Camera* camera) override {};
		void AABBCollision(GameObject* _object, float dt) {};

	protected:
		// attributes
		float speed = 0;
		float2 velocity = 0;
		Sprite* sprite = nullptr;
	};
}