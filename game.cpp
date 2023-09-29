// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "gameobject.h"
#include "player.h"
#include "tmloader.h"
#include "camera.h"
#include "game.h"

#include <iostream>

#define TS_COLUMNS	10
#define TS_ROWS		4
#define TILE_SIZE_X	32
#define TILE_SIZE_Y	32
#define TM_COLUMNS	128
#define TM_ROWS		64


void Game::Init() {
	tilesheet = new Tilesheet("assets/Tileset.png", TS_COLUMNS, TS_ROWS, uint2(TILE_SIZE_X, TILE_SIZE_Y));
	tilemap = new Tilemap("tiled/tilemap2.csv", TM_COLUMNS, TM_ROWS);

	player = new Player(input, new Sprite(new Surface("assets/test_player.png"), 1), float2(64, 128), uint2(48, 48), 800, 400);

	camera = new Camera(screen, TM_COLUMNS * TILE_SIZE_X, TM_ROWS * TILE_SIZE_Y, 1000);
	camera->SetTarget(player);
}

void Game::Tick(float dt) {
	dt /= 1000; // convert dt milliseconds to seconds
	screen->Clear(0);

	// UPDATE
	UpdateEntities(dt);
	player->Update(dt);
	camera->Update(dt);

	// COLLISION
	tilemap->Collision(tilesheet, player, dt);

	// RENDER
	camera->RenderTilemap(tilemap, tilesheet);
	RenderEntities();
	camera->RenderTarget(); // the player
}

void Game::Shutdown() {
	delete tilesheet;
	delete tilemap;
	delete player;
	delete camera;
}

void Game::UpdateEntities(float dt) {
}

void Game::RenderEntities() {
}