// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include <iostream>

#include "precomp.h"
#include "game.h"

Sprite tileSprite(new Surface("assets/tiles/sheet.png"), 1);

void Game::Init() {
	screen->Clear(0);
	LoadTilemap("tiled/pitfall_GroundTiles1.csv", groundTiles1, ",");
	std::cout << groundTiles1 << ", ";
}

void Game::Tick(float deltaTime) {
}

void Game::LoadTilemap(const char* filename, char* tilemap, char* delimiter) {
	// got help with the the parsing of csv from ChatGPT
	FILE* file = fopen(filename, "r");

	if (file == nullptr) {
		return;
	}

	int tokenCount = 0;
	char buffer[1024];

	while (fgets(buffer, sizeof(buffer), file)) {
		char* token = strtok(buffer, delimiter);
		while (token != NULL) {
			tilemap[tokenCount++] = *token;
			token = strtok(NULL, delimiter);
		}
	}

	fclose(file);
}