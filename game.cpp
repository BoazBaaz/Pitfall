// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "tmloader.h"
#include "game.h"

#include <iostream>

TMLoader tml;

TMLoader::Tilemap Tilemap1("tiled/tilemap1.csv");
float2 tmPos(0, 0);

Sprite CharacterSprites(new Surface("assets/character_sheet.png"), 11);
Sprite Tilesheet(new Surface("assets/pitfall_tilesheet.png"), 8, 3);

namespace Tmpl8 {
	void Game::Init() {
		screen->Clear(0);
		tml.LoadTilemap(Tilemap1, ",");
		CharacterSprites.SetFrame(0);
	}

	void Game::Tick(float deltaTime) {
		screen->Clear(0);
		tml.DrawTilemap(Tilemap1, Tilesheet, screen, 0 - tmPos.x, 0);

		if (input->GetKey(68)) {
			tmPos.x += 1 * deltaTime;
		}
		else if (input->GetKey(65)) {
			tmPos.x -= 1 * deltaTime;
		}

		CharacterSprites.DrawScaled(screen, tmPos.x, 500, CharacterSprites.GetWidth(), CharacterSprites.GetHeight());
	}
}