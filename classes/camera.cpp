#include "precomp.h"
#include "gameobject.h"
#include "tmloader.h"
#include "camera.h"

Camera::Camera(Surface* screen, uint2 camSize, uint2 worldSize, uint speed) :
	Entity(0, camSize, true),
	screen(screen),
	worldSize(camSize),
	camSpeed(speed),
	target(0) {
}

void Camera::SetTarget(Entity* newTarget) {
	target = newTarget;
}

void Camera::Tick(float dt) {
	// calculate the next position of the camera
	float speed = camSpeed * dt;
	float2 destX = float2(target->GetPosition().x - position.x - SCRWIDTH / 2, target->GetPosition().y - position.y - SCRHEIGHT / 2);

	// clamp the camera movement
	position += clamp(destX, -speed, speed);

	// clamp the camera to not make it move outside of the world
	position = float2(clamp(position.x, 0.0f, static_cast<float>(worldSize.x) - SCRWIDTH),
					  clamp(position.y, 0.0f, static_cast<float>(worldSize.y) - SCRHEIGHT));

	// clamp the target position to be inside the world
	target->SetPosition(clamp(target->GetPosition().x, 0.0f, static_cast<float>(worldSize.x) - target->GetSize().x),
						clamp(target->GetPosition().y, 0.0f, static_cast<float>(worldSize.y) - target->GetSize().y));
}

void Camera::RenderTilemap(Tilemap* tilemap) {
	uint mapSize = tilemap->mapSize;
	uint2 tileSize = tilemap->GetTileSize();
	for (uint i = 0; i < mapSize; i++) {
		if (tilemap->GetTileID(i) < 0)
			continue;
		int x = (i % tilemap->columns) * tileSize.x;
		int y = (i / tilemap->columns) * tileSize.y;
		float2 renderPos = { x - position.x , y - position.y };
		if (x + tileSize.x >= position.x && x <= position.x + size.x &&
			y + tileSize.y >= position.y && y <= position.y + size.y) {
			tilemap->GetTileSurface(i)->CopyTo(screen, renderPos.x, renderPos.x);
		}
	}
}

void Camera::RenderTarget() {
	target->Render(this);
}

bool Camera::OnScreen(Entity* entity) {
	if (entity->GetPosition().x + entity->GetSize().x >= position.x && entity->GetPosition().x <= position.x + size.x &&
		entity->GetPosition().y + entity->GetSize().y >= position.y && entity->GetPosition().y <= position.y + size.y) {
		entity->SetActive(true);
		return true;
	}
	entity->SetActive(false);
	return false;
}
