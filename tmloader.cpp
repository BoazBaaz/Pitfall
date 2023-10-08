#include "precomp.h"
#include "gameObject.h"
#include "enemy.h"
#include "tmloader.h"

#include <iostream>

TMLoader::TMLoader(const char* file, uint columns, uint rows, uint2 tileSize) :
	columns(columns),
	rows(rows),
	tileSize(tileSize),
	mapSize(columns* rows),
	map(new int[columns * rows]),
	file(file) {
	ParseCSV();
}

// I got help with the the parsing of csv from ChatGPT
void TMLoader::ParseCSV() {
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

Tileset::Tileset(const char* filename, uint columns, uint rows, uint2 tileSize = 32) :
	columns(columns),
	rows(rows),
	tileSize(tileSize),
	mapSize(columns* rows),
	tileset(new Surface* [columns * rows]) {
	InitializeTileset(filename);
}

Tileset::~Tileset() {
	for (uint i = 0; i < mapSize; i++)
		delete tileset[i];
	delete tileset;
}

void Tileset::InitializeTileset(const char* filename) {
	Surface tiles(filename);
	for (uint i = 0; i < mapSize; i++) {
		tileset[i] = new Surface(tileSize.x, tileSize.y);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		tiles.CopyTo(tileset[i], -x, -y);
	}
}


Tilemap::Tilemap(const char* filename, Tileset* tileset, uint columns, uint rows) :
	TMLoader(filename, columns, rows, tileset->tileSize),
	tilemap(new Tile[columns * rows]),
	surface(new Surface(columns* tileset->tileSize.x, rows* tileset->tileSize.y)) {
	InitializeTilemap(tileset);
}

Tilemap::~Tilemap() {
	delete tilemap;
	delete surface;
}

void Tilemap::InitializeTilemap(Tileset* tileset) {
	for (uint i = 0; i < mapSize; i++) {
		tilemap[i] = Tile(map[i]);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		if (map[i] >= 0)
			tileset->GetTile(map[i])->CopyTo(surface, x, y);
	}
}

EntityMap::EntityMap(const char* filename, Tileset* tileset, uint columns, uint rows, GameObject* target, Tilemap* tilemap) :
	TMLoader(filename, columns, rows, tileset->tileSize) {
	InitializeEntities(tileset, target, tilemap);
}

void EntityMap::InitializeEntities(Tileset* tileset, GameObject* target, Tilemap* tilemap) {
	for (uint i = 0; i < mapSize; i++) {
		if (map[i] >= 0)
			entityCount++;
	}

	//TODO: Create a copy of an enemy and set the target and tilemap in the innit, then copy that to a new entity.

	entities = (new GameObject * [entityCount] {NULL});

	for (uint i = 0; i < mapSize; i++) {
		if (map[i] >= 0) {
			int x = (i % columns) * tileSize.x;
			int y = (i / columns) * tileSize.y;
			for (uint j = 0; j < entityCount; j++) {
				if (entities[j] == NULL) {
					switch (map[i]) {
						case 0:
							entities[j] = new Enemy(new Sprite(new Surface("assets/slime_sheet.png"), 4), float2(x, y));
							break;
						case 1:
							entities[j] = new FollowEnemy(new Sprite(new Surface("assets/bat_sheet.png"), 4), float2(x, y), 1, target, tilemap);
							break;
						case 2:
							entities[j] = new PathEnemy(new Sprite(new Surface("assets/test_player.png"), 1), float2(x, y), 1, true, 100);
							break;
						default:
							break;
					};
					if (entities[j] != NULL)
						break;
				}
			}
		}
	}
}

