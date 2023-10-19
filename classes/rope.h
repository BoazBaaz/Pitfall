#pragma once
namespace Tmpl8 {
	class Rope : public Entity {
	public:
		// constructor / destructor
		Rope(Surface* screen, float2 position, int width, int length, uint color);

		// special opperations
		void Tick(float dt);

	private:
		// attributes
		int width, length;
		double t = 0;
		uint color;
		Surface* screen;
	};
}