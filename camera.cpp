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
	float destX = target->GetPos().x - position.x - SCRWIDTH / 2;
	float destY = target->GetPos().y - position.y - SCRHEIGHT / 2;

	// clamp the camera movement
	position.x += clamp(destX, -speed, speed);
	position.y += clamp(destY, -speed, speed);

	// clamp the camera to not make it move outside of the world
	position.x = clamp(position.x, 0.0f, static_cast<float>(worldWidth) - SCRWIDTH);
	position.y = clamp(position.y, 0.0f, static_cast<float>(worldHeight) - SCRHEIGHT);

	// clamp the target position to be inside the world
	target->SetPos(clamp(target->GetPos().x, 0.0f, static_cast<float>(worldWidth) - target->GetSize().x),
				   clamp(target->GetPos().y, 0.0f, static_cast<float>(worldHeight) - target->GetSize().y));
}

void Camera::RenderTilemap(Tilemap* tilemap, Tilesheet* tilesheet) {
	uint2 tileSize = tilesheet->tileSize;

	// go through the tilemap and check if you need to draw this tile
	for (int i = 0; i < tilemap->mapSize; i++) {
		if (tilemap->Map(i).tileID >= 0) {

			// get the tile position relative to the camera
			float2 renderPos = { (i % tilemap->columns) * tileSize.x - position.x, (i / tilemap->columns) * tileSize.y - position.y };

			// check if the tile should be draw to the screen
			if (renderPos.x + tileSize.x >= 0 && renderPos.x <= SCRWIDTH &&
				renderPos.y + tileSize.y >= 0 && renderPos.y <= SCRHEIGHT) {
				tilesheet->Sheet(tilemap->Map(i).tileID)->CopyTo(screen, renderPos.x, renderPos.y);
			}
		}
	}
}

void Camera::RenderSprite(Sprite* sprite, uint frame, float2 pos) {
	float2 renderPos = { pos.x - position.x, pos.y - position.y };
	if (renderPos.x + sprite->GetWidth() >= 0 && renderPos.x <= SCRWIDTH &&
		renderPos.y + sprite->GetHeight() >= 0 && renderPos.y <= SCRHEIGHT) {
		sprite->SetFrame(frame);
		sprite->Draw(screen, renderPos.x, renderPos.y);
	}
}

void Camera::RenderTarget() {
	target->GetSprite()->Draw(screen, target->GetPos().x - position.x, target->GetPos().y - position.y);
}
