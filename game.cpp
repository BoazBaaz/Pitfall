// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "player.h"
#include "enemy.h"
#include "camera.h"
#include "game.h"

#include <iostream>

#define TILE_SIZE_X	32
#define TILE_SIZE_Y	32
#define TM_COLUMNS	128
#define TM_ROWS		64


void Game::Init() {
	Tileset dungeonTileset = Tileset("assets/Incolgames/dungeon_tileset.png", 13, 6, uint2(TILE_SIZE_X, TILE_SIZE_Y));
	tilemap = new Tilemap("tiled/pitfall_Main.csv", &dungeonTileset, TM_COLUMNS, TM_ROWS);

	//Tileset entityTileset = Tileset("assets/Incolgames/entity_tileset.png", 4, 0, uint2(TILE_SIZE_X, TILE_SIZE_Y));
	//Tilemap objects = Tilemap("tiled/pitfall_Objects.csv", &entityTileset, TM_COLUMNS, TM_ROWS);


	player = new Player(input, new Sprite(new Surface("assets/test_player.png"), 1), float2(256, 128), 500);

	camera = new Camera(screen, TM_COLUMNS * TILE_SIZE_X, TM_ROWS * TILE_SIZE_Y, 1000);
	camera->SetTarget(player);

	//entities[0] = new StaticEnemy()
}

void Game::Tick(float dt) {
	dt /= 1000; // convert dt milliseconds to seconds
	screen->Clear(0);

	// UPDATE
	UpdateEntities(dt);
	player->Update(dt);
	camera->Update(dt);

	// COLLISION
	CollisionEntities();
	player->TileCollision(tilemap, dt);

	// RENDER
	camera->RenderTilemap(tilemap);
	RenderEntities();
	camera->RenderTarget(); // the player
}

void Game::Shutdown() {
	delete camera;
	delete tilemap;
	delete player;
}

void Game::UpdateEntities(float dt) {
	for (GameObject* entity : entities) {
		if (entity->GetPos().x + entity->GetSize().x >= camera->GetPos().x && entity->GetPos().x <= camera->GetPos().x + SCRWIDTH &&
			entity->GetPos().y + entity->GetSize().y >= camera->GetPos().y && entity->GetPos().y <= camera->GetPos().y +  SCRHEIGHT) {
			entity->Update(dt);
		}
	}
}

void Game::CollisionEntities() {
	for (GameObject* entity : entities) {
		if (entity->GetPos().x + entity->GetSize().x >= camera->GetPos().x && entity->GetPos().x <= camera->GetPos().x + SCRWIDTH &&
			entity->GetPos().y + entity->GetSize().y >= camera->GetPos().y && entity->GetPos().y <= camera->GetPos().y + SCRHEIGHT) {
			if (player->AABBCollision(entity)) {
				entity->Collided(player);
			}
		}
	}
}

void Game::RenderEntities() {
	for (GameObject* entity : entities) {
		if (entity->GetPos().x + entity->GetSize().x >= camera->GetPos().x && entity->GetPos().x <= camera->GetPos().x + SCRWIDTH &&
			entity->GetPos().y + entity->GetSize().y >= camera->GetPos().y && entity->GetPos().y <= camera->GetPos().y + SCRHEIGHT) {
			camera->RenderGameObject(entity, 0);
		}
	}
}