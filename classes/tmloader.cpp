#include "precomp.h"
#include "gameObject.h"
#include "enemy.h"
#include "rope.h"
#include "tmloader.h"

#include <iostream>

Tileset::Tileset(const char* filename, uint columns, uint rows, uint2 tileSize = 32) :
	columns(columns),
	rows(rows),
	tileSize(tileSize),
	mapSize(columns* rows),
	tiles(new Tile* [columns * rows]) {
	InitializeTileset(filename);
}

Tileset::~Tileset() {
	for (uint i = 0; i < mapSize; i++)
		delete tiles[i];
	delete tiles;
}

void Tileset::InitializeTileset(const char* filename) {
	Surface tileset(filename);
	for (uint i = 0; i < mapSize; i++) {
		tiles[i] = new Tile(i);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		tileset.CopyTo(&tiles[i]->surface, -x, -y);
	}
}


Tilemap::Tilemap(const char* filename, Tileset* tileset, uint columns, uint rows) :
	columns(columns),
	rows(rows),
	mapSize(columns * rows),
	tileset(tileset),
	map(new int[columns * rows]) {
	ParseCSV(filename);
}

// I got help with the the parsing/converting to int of csv from ChatGPT
void Tilemap::ParseCSV(const char* file) {
	// open the file
	FILE* f = fopen(file, "r");

	// return if you failed to open the file
	if (f == nullptr) {
		return;
	}

	int tokenCount = 0;
	char buffer[1024];

	// get 1024 characters from .csv file and load the into the buffer, then loop through all of the characters
	while (fgets(buffer, sizeof(buffer), f)) {

		// put all the characters until the delimiter into the token
		char* token = strtok(buffer, ",");

		while (token != NULL) {
			int result = 0;
			short sign = 1;

			// check if the token in a negative value, if it is set the sign to -1 and move to the next char in the token
			if (*token == '-')
				sign = -1, token++;

			// loop through all the characters in the token
			while (*token) {

				// check if the character is a number, if it is convert it to a intiger and add it to the result
				if (*token >= '0' && *token <= '9')
					result = result * 10 + (*token - '0');
				else
					break;
				token++;
			}

			map[tokenCount++] = int(result * sign);

			// get the next token
			token = strtok(NULL, ",");
		}
	}

	fclose(f);
}

EntityMap::EntityMap(Tilemap* tilemap, Entity* entityTypes[]) :
	tilemap(tilemap) {
	InitializeEntities(entityTypes);
}

void EntityMap::InitializeEntities(Entity* entityTypes[]) {
	for (uint i = 0; i < tilemap->mapSize; i++) {
		if (tilemap->GetTileID(i) >= 0)
			entityCount++;
	}

	entities = new Entity* [entityCount] {nullptr};

	for (uint i = 0; i < tilemap->mapSize; i++) {
		if (tilemap->GetTileID(i) >= 0) {
			int x = (i % tilemap->columns) * tilemap->GetTileSize().x;
			int y = (i / tilemap->columns) * tilemap->GetTileSize().y;
			for (uint j = 0; j < entityCount; j++) {
				if (entities[j] != nullptr) {
					entities[j] = new Entity(*entityTypes[tilemap->GetTile(i)->tileID]);
					entities[j]->SetPosition(x, y);
					break;
				}
			}
		}
	}
}

