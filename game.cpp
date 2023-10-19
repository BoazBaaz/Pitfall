// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "classes/tmloader.h"
#include "classes/gameobject.h"
#include "classes/player.h"
#include "classes/enemy.h"
#include "classes/rope.h"
#include "classes/camera.h"
#include "game.h"

#include <iostream>

#define TILE_SIZE_X	uint(32)
#define TILE_SIZE_Y	uint(32)
#define TM_COLUMNS	uint(128)
#define TM_ROWS		uint(64)

void Game::Init() {
	mainTileset = new Tileset("assets/dungeon_tileset.png", 13, 6, uint2(TILE_SIZE_X, TILE_SIZE_Y));
	entityTileset = new Tileset("assets/entity_tileset.png", 6, 1, uint2(TILE_SIZE_X, TILE_SIZE_Y));
	mainTilemap = new Tilemap("tiled/pitfall_Main.csv", mainTileset, TM_COLUMNS, TM_ROWS);
	entityTilemap = new Tilemap("tiled/pitfall_Entities.csv", entityTileset, TM_COLUMNS, TM_ROWS);

	player = new Player(input, new Sprite(new Surface("assets/test_player.png"), 1), float2(256, 512), 500);

	camera = new Camera(screen, uint2(SCRWIDTH, SCRHEIGHT), uint2(TM_COLUMNS * TILE_SIZE_X, TM_ROWS * TILE_SIZE_Y), 1000);
	camera->SetTarget(player);

	Entity* entityTypes[4] = {
				new Enemy(new Sprite(new Surface("assets/snake.png"), 4), 0),
				new FollowEnemy(new Sprite(new Surface("assets/entities/bat_sheet.png"), 4), 0, 100, player, mainTilemap),
				new PathEnemy(new Sprite(new Surface("assets/entities/slime_sheet.png"), 1), 0, 100, true, 500),
				new Rope(screen, float2(200, 10), 5, 200, 0xFF0000)
	};

	entityMap = new EntityMap(entityTilemap, entityTypes);
}

void Game::Tick(float dt) {
	dt /= 1000; // convert dt milliseconds to seconds
	screen->Clear(0);

	// UPDATE
	player->Tick(dt);
	camera->Tick(dt);
	UpdateEntities(dt);

	// COLLISION
	player->TileCollision(mainTilemap, dt);

	// RENDER
	camera->RenderTilemap(mainTilemap);
	RenderEntities();
	camera->RenderTarget(); // the player

	rope->Tick(dt);
}

void Game::Shutdown() {
	delete camera;
	delete mainTileset;
	delete mainTilemap;
	delete entityTileset;
	delete entityTilemap;
	delete entityMap;
	delete player;
	delete rope;
}

void Game::UpdateEntities(float dt) {
	for (uint i = 0; i < entityMap->entityCount; i++) {
		if (entityMap->entities[i] != nullptr) {
			if (entityMap->entities[i]->GetPosition().x + entityMap->entities[i]->GetSize().x >= camera->GetPosition().x && entityMap->entities[i]->GetPosition().x <= camera->GetPosition().x + SCRWIDTH &&
				entityMap->entities[i]->GetPosition().y + entityMap->entities[i]->GetSize().y >= camera->GetPosition().y && entityMap->entities[i]->GetPosition().y <= camera->GetPosition().y + SCRHEIGHT) {
				entityMap->entities[i]->Tick(dt);
			}
		}
	}
}

void Game::RenderEntities() {
	for (uint i = 0; i < entityMap->entityCount; i++) {
		if (entityMap->entities[i] != nullptr) {
			if (entityMap->entities[i]->GetPosition().x + entityMap->entities[i]->GetSize().x >= camera->GetPosition().x && entityMap->entities[i]->GetPosition().x <= camera->GetPosition().x + SCRWIDTH &&
				entityMap->entities[i]->GetPosition().y + entityMap->entities[i]->GetSize().y >= camera->GetPosition().y && entityMap->entities[i]->GetPosition().y <= camera->GetPosition().y + SCRHEIGHT) {
				entityMap->entities[i]->Render(camera);
			}
		}
	}
}