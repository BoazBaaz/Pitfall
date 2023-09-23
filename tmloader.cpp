#include "precomp.h"
#include "gameobject.h"
#include "player.h"
#include "tmloader.h"

Tilemap::Tilemap(const char* filename, uint rows, uint columns, int2 tilemapStart) :
	file(filename),
	position(tilemapStart),
	rows(rows),
	columns(columns),
	mapSize(rows* columns),
	map(new Tile[rows * columns]) {
	InitializeTilemap();
}

Tilemap::~Tilemap() {
	delete map;
}

Tilesheet::Tilesheet(const char* filename, uint rows, uint columns, uint tileSize) :
	rows(rows),
	columns(columns),
	numTiles(rows* columns),
	tileSize(tileSize),
	tiles(new Surface* [rows * columns]) {
	InitializeTilesheet(filename);
}

Tilesheet::~Tilesheet() {
	for (unsigned int i = 0; i < numTiles; i++)
		delete tiles[i];
	delete tiles;
}

void Tilemap::InitializeTilemap() {
	// got help with the the parsing of csv from ChatGPT
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

			// add the result to the map array
			map[tokenCount++] = Tile(result * sign);

			// get the next token
			token = strtok(NULL, ",");
		}
	}

	fclose(f);
}

void Tilesheet::InitializeTilesheet(const char* filename) {
	Surface tilesheet(filename);
	for (uint i = 0; i < numTiles; i++) {
		tiles[i] = new Surface(tileSize, tileSize);
		int x = (i % columns) * tileSize;
		int y = (i / columns) * tileSize;
		tilesheet.CopyTo(tiles[i], -x, -y);
	}
}

void Tilemap::Render(Tilesheet* tilesheet, Surface* screen, int2 camPos) {
	for (int i = 0; i < mapSize; i++) {
		int tileX = ((i % columns) * tilesheet->tileSize) + position.x - camPos.x;
		int tileY = ((i / columns) * tilesheet->tileSize) + position.y - camPos.y;

		if (map[i].tileID >= 0) {
			if (tileX + tilesheet->tileSize > 0 && tileX < SCRWIDTH &&
				tileY + tilesheet->tileSize > 0 && tileY < SCRHEIGHT) {
				tilesheet->GetTile(map[i].tileID)->CopyTo(screen, tileX, tileY);
			}
		}
	}
}

void Tilemap::Collision(Tilesheet* tilesheet, Player* object) {
	// I used this tutorial https://jonathanwhiting.com/tutorial/collision/

	int tileSize = static_cast<int>(tilesheet->tileSize);

	int leftTile = object->left / tileSize;
	int rightTile = object->right / tileSize;
	int topTile = object->top / tileSize;
	int bottomTile = object->bottom / tileSize;

	if (leftTile < 0) leftTile = 0;
	if (rightTile > columns) rightTile = columns;
	if (topTile < 0) topTile = 0;
	if (bottomTile > rows) bottomTile = rows;

	for (int x = leftTile; x <= rightTile; x++) {
		for (int y = topTile; y <= bottomTile; y++) {
			Tile tile = map[x + y * columns];
			if (tile.tileState == TileStates::collision) {
				object->onGround = true;
				object->SetVel(0.0f);
				//int tilePosX = position.x + (x * tileSize);
				//int tilePosY = position.y + (y * tileSize);

				//int closestX = clamp(object->GetPos().x, tilePosX, tilePosX + tileSize);
				//int closestY = clamp(object->GetPos().y, tilePosY, tilePosY + tileSize);
				
				//object->SetPos(closestX, closestY);
			}
		}
	}
}