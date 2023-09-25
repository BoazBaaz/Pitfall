#include "precomp.h"
#include "tmpl8math.h"
#include "camera.h"

Camera::Camera(int worldWidth, int worldHeight, uint speed) :
    speed(speed),
	worldWidth(worldWidth),
	worldHeight(worldHeight),
	position(0),
	target(0) {
}

void Camera::SetTarget(float2 newTarget) {
	target = newTarget;
}

void Camera::Update(float dt) {
	float fspeed = speed * dt;
    float destX = target.x - position.x;
    float destY = target.y - position.y;

    position.x += clamp(destX, -fspeed, fspeed);
    position.y += clamp(destY, -fspeed, fspeed);

 	position.x = clamp(position.x, 0.0f, static_cast<float>(worldWidth));
	position.y = clamp(position.y, 0.0f, static_cast<float>(worldHeight));
}
