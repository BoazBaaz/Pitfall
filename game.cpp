// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "tilemap.h"
#include "gameobject.h"
#include "player.h"
#include "game.h"

#include <iostream>

// objects
Player player(new Sprite(new Surface("assets/character_sheet.png"), 11), float2(0, 500), 10);

// sprites
Sprite tileSheet(new Surface("assets/pitfall_tilesheet.png"), 8, 3);

// tilemap
TMLoader tml;
TMLoader::Tilemap tilemap1("tiled/tilemap1.csv");

void Game::Init() {
	tml.LoadTilemap(tilemap1, ",");
}

void Game::Tick(float dt) {
	screen->Clear(0);
	tml.DrawTilemap(tilemap1, tileSheet, screen, 0 - player.transform.position.x, 0);

	player.Update(input, dt);

	player.GetSprite()->Draw(screen, player.transform.position.x, 500);
}