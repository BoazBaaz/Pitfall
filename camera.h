#pragma once

namespace Tmpl8 {
	// got help making the camera class from ChatGPT
	class Camera {
	public:
		// constructor / destructor
		Camera(int worldWidth, int worldHeight, uint speed);

		// member data access
		float2 GetPos() const { return position; }
		void SetTarget(float2 target);

		// special opperations
		void Update(float dt);

	private:
		// attributes
		int speed;
		int worldWidth;
		int worldHeight;
		float2 position;
		float2 target;
	};
}