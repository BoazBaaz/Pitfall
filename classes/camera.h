#pragma once

namespace Tmpl8 {
	class Tilemap;
	// got help making the camera class from ChatGPT
	class Camera : public Entity {
	public:
		// constructor / destructor
		Camera(Surface* screen, uint2 camSize, uint2 worldSize, uint speed);

		// member data access
		float2 GetPosition() const { return position; }
		void SetTarget(Entity* target);

		// special opperations
		void Tick(float dt) override;
		void RenderTilemap(Tilemap* tilemap);
		void RenderTarget();
		bool OnScreen(Entity* entity);

	private:
		// attributes
		int camSpeed;
		uint2 worldSize;
		Entity* target;
		Surface* screen;
	};
}