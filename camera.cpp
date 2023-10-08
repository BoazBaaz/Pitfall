#include "precomp.h"
#include "gameobject.h"
#include "tmloader.h"
#include "camera.h"

Camera::Camera(Surface* screen, int worldWidth, int worldHeight, uint speed) :
	screen(screen),
	worldWidth(worldWidth),
	worldHeight(worldHeight),
	camSpeed(speed),
	position(0),
	target(0) {
}

void Camera::SetTarget(GameObject* newTarget) {
	target = newTarget;
}

void Camera::Update(float dt) {
	// calculate the next position of the camera
	float speed = camSpeed * dt;
	float2 destX = float2(target->GetPos().x - position.x - SCRWIDTH / 2, target->GetPos().y - position.y - SCRHEIGHT / 2);

	// clamp the camera movement
	position += clamp(destX, -speed, speed);

	// clamp the camera to not make it move outside of the world
	position = float2(clamp(position.x, 0.0f, static_cast<float>(worldWidth) - SCRWIDTH), 
					  clamp(position.y, 0.0f, static_cast<float>(worldHeight) - SCRHEIGHT));

	// clamp the target position to be inside the world
	target->SetPos(clamp(target->GetPos().x, 0.0f, static_cast<float>(worldWidth) - target->GetSize().x),
				   clamp(target->GetPos().y, 0.0f, static_cast<float>(worldHeight) - target->GetSize().y));
}

void Camera::RenderTilemap(Tilemap* tilemap) {
	float2 renderPos = { -position.x, -position.y };
	tilemap->GetSurface()->CopyTo(screen, renderPos.x, renderPos.y);
}

void Camera::RenderGameObject(GameObject* object, uint frame) {
	float2 renderPos = object->GetPos() - position;
	object->GetSprite()->SetFrame(frame);
	object->GetSprite()->Draw(screen, renderPos.x, renderPos.y);
}

void Camera::RenderTarget() {
	target->GetSprite()->Draw(screen, target->GetPos().x - position.x, target->GetPos().y - position.y);
}
