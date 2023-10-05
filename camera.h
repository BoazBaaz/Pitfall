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
		void RenderTilemap(Tilemap* tilemap);
		void RenderGameObject(GameObject* object, uint frame);
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