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
Player player(new Sprite(new Surface("assets/character_sheet.png"), 11), float2(0, 500), 1);

// tilemap
TMLoader tml;
TMLoader::Tilemap tilemap1("tiled/tilemap1.csv", 24, 80);
TMLoader::Tilesheet tilesheet("assets/pitfall_tilesheet.png", 3, 8, uint2(32));


void Game::Init() {

}

void Game::Tick(float dt) {
	screen->Clear(0);
	tml.DrawTilemap(tilemap1, tilesheet, screen, 0 - player.transform.position.x, 0);

	player.Update(input, dt);

	player.GetSprite()->Draw(screen, player.transform.position.x, 500);
}