// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "tmloader.h"
#include "gameobject.h"
#include "player.h"
#include "game.h"

#include <iostream>

#define TM_ROWS		24
#define TM_COLUMNS	80
#define TM_STARTX	0
#define TM_STARTY	0


void Game::Init() {
	tilesheet = new Tilesheet("assets/pitfall_tilesheet.png", 3, 8);
	tilemap = new Tilemap("tiled/tilemap1.csv", TM_ROWS, TM_COLUMNS, int2(TM_STARTX, TM_STARTY));

	player = new Player(input, new Sprite(new Surface("assets/character_sheet.png"), 11), int2(SCRWIDTH / 2, SCRHEIGHT / 2), 50, 400);

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
	camPos.x = player->GetPos().x;
	camPos.y = player->GetPos().y;
	ClampCamera();

	// RENDER TILEMAPS
	tilemap->Render(tilesheet, screen, camPos);

	// RENDER ENTITIES
	RenderEntities();
	player->Render(screen);



	// GIZMO
	screen->Box(player->GetPos().x,
				player->GetPos().y,
				player->GetPos().x + player->GetWidth(),
				player->GetPos().y + player->GetHeight(),
				0xFF0000);
	screen->Box(camPos.x,
				camPos.y,
				camPos.x + SCRWIDTH,
				camPos.y + SCRHEIGHT,
				0x0000FF);
}

void Game::Shutdown() {
	delete tilesheet;
	delete tilemap;
	delete player;
}

void Game::ClampCamera() {
	int maxX = TM_COLUMNS * tilesheet->tileSize - SCRWIDTH;
	int maxY = TM_ROWS * tilesheet->tileSize - SCRHEIGHT;

	// Clamp the camera position to stay within the tilemap boundaries
	camPos.x = clamp(camPos.x, 0, maxX);
	camPos.y = clamp(camPos.y, 0, maxY);
}

void Game::UpdateEntities(float dt) {
}

void Game::RenderEntities() {
}