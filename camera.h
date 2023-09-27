#pragma once

namespace Tmpl8 {
	// got help making the camera class from ChatGPT
	class Camera {
	public:
		// constructor / destructor
		Camera(Surface* screen, int worldWidth, int worldHeight, uint speed);

		// member data access
		float2 GetPos() const { return position; }
		void SetTarget(GameObject* target);

		// special opperations
		void Update(float dt);
		void RenderTilemap(Tilemap* surface, Tilesheet* tilesheet);
		void RenderSprite(Sprite* sprite, uint frame, float2 pos);
		void RenderTarget();

	private:
		// attributes
		int camSpeed;
		int worldWidth;
		int worldHeight;
		float2 position;
		GameObject* target;
		Surface* screen;
	};
}