#include "precomp.h"
#include "gameobject.h"
#include "rope.h"

Rope::Rope(Surface* screen, float2 position, int width, int length, uint color) :
	Entity(position, uint2(width, length)),
	width(width),
	length(length),
	color(color),
	screen(screen) {
}

void Rope::Tick(float dt) {
	t = (t > (PI * 2)) ? 0 : t + dt;
	float p = sin(t) * (PI / 3) + (PI / 2); 
	float2 dest = float2(position.x + cos(p) * length, position.y + sin(p) * length);
	screen->Line(position.x, position.y, dest.x, dest.y, color);
}
