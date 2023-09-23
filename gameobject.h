#pragma once

namespace Tmpl8 {
	class GameObject {
	public:
		// constructor / destructor
		GameObject(Sprite* sprite, int2 position, float speed = 1);
		~GameObject();

		// member data access
		int2 GetPos() const { return position; }
		float2 GetVel() const { return velocity; }
		uint GetWidth() const { return width; }
		uint GetHeight() const { return height; }
		Sprite* GetSprite() const { return sprite; }
		void SetPos(int x, int y) { position.x = x, position.y = y; }
		void SetPos(int2 pos) { position = pos; }
		void SetVel(float x, float y) { velocity.x = x, velocity.y = y; }
		void SetVel(float2 vel) { velocity = vel; }

		// special operations
		void Update(float dt);
		void Render(Surface* screen);

		// attributes
		int left;
		int right;
		int top;
		int bottom;

	protected:
		// attributes
		int2 position;
		float2 velocity;
		uint width, height;
		Sprite* sprite;
		float speed = 1;
	};
}