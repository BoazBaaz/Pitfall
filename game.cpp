// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include <iostream>

#include "precomp.h"
#include "game.h"
#include "tmloader.h"

TMLoader tml;

TMLoader::Tilemap Tilemap1("tiled/tilemap1.csv");

Sprite CharacterSprites(new Surface("assets/character_sheet.png"), 11);
Sprite Tilesheet(new Surface("assets/pitfall_tilesheet.png"), 3, 8);

void Game::Init() {
	screen->Clear(0);
	tml.LoadTilemap(Tilemap1, ",");
	tml.DrawTilemap(Tilemap1, Tilesheet, screen, 0, 0);
}

void Game::Tick(float deltaTime) {
}