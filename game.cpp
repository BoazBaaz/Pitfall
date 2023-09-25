// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "player.h"
#include "camera.h"
#include "game.h"

#include <iostream>

#define TM_COLUMNS	128
#define TM_ROWS		64
#define TS_COLUMNS	10
#define TS_ROWS		4


void Game::Init() {
	tilesheet = new Tilesheet("assets/Tileset.png", 10, 4);
	tilemap = new Tilemap("tiled/tilemap2.csv", TM_COLUMNS, TM_ROWS);

	player = new Player(input, new Sprite(new Surface("assets/character_sheet.png"), 11), float2(SCRWIDTH / 2, SCRHEIGHT / 2), 600, 400);

	camera = new Camera(TM_COLUMNS * tilesheet->tileSize - SCRWIDTH, TM_ROWS * tilesheet->tileSize - SCRHEIGHT, 5);
}

void Game::Tick(float dt) {
	dt /= 1000;
	screen->Clear(0);

	// UPDATE
	UpdateEntities(dt);
	player->Update(dt);

	// COLLISION
	tilemap->Collision(tilesheet, player);

	// CAMERA
 	camera->SetTarget(player->GetPos());
	camera->Update(dt);

	// RENDER TILEMAPS
	tilemap->Render(tilesheet, screen, camera->GetPos());

	// RENDER ENTITIES
	RenderEntities();
	player->Render(screen);



	// GIZMO
	screen->Box(player->GetPos().x,
				player->GetPos().y,
				player->GetPos().x + player->GetWidth(),
				player->GetPos().y + player->GetHeight(),
				0xFF0000);
	screen->Box(camera->GetPos().x,
				camera->GetPos().y,
				camera->GetPos().x + SCRWIDTH,
				camera->GetPos().y + SCRHEIGHT,
				0x0000FF);
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