#include "precomp.h"
#include "gameobject.h"
#include "tmloader.h"

Tilemap::Tilemap(const char* filename, uint columns, uint rows) :
	file(filename),
	columns(columns),
	rows(rows),
	mapSize(rows * columns),
	map(new Tile[rows * columns]) {
	InitializeTilemap();
}

Tilemap::~Tilemap() {
	delete map;
}

Tilesheet::Tilesheet(const char* filename, uint columns, uint rows, uint2 tileSize) :
	columns(columns),
	rows(rows),
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

// I got help with the the parsing of csv from ChatGPT
void Tilemap::InitializeTilemap() {
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
		tiles[i] = new Surface(tileSize.x, tileSize.y);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		tilesheet.CopyTo(tiles[i], -x, -y);
	}
}

// I used this tutorial https://jonathanwhiting.com/tutorial/collision/
void Tilemap::Collision(Tilesheet* tilesheet, GameObject* object) {
	uint2 tileSize = tilesheet->tileSize;

	// get the tiles that the player corners is overlapping with
	int leftTile = object->GetPos().x / tileSize.x;
	int rightTile = (object->GetPos().x + object->GetSize().x) / tileSize.x;
	int topTile = object->GetPos().y / tileSize.y;
	int bottomTile = (object->GetPos().y + object->GetSize().y) / tileSize.y;

	// clamp the player collision to alway be inside the grid
	if (leftTile < 0) leftTile = 0;
	if (rightTile > columns) rightTile = columns;
	if (topTile < 0) topTile = 0;
	if (bottomTile > rows) bottomTile = rows;

	// go through all the tiles the player is overlapping
	for (int x = leftTile; x <= rightTile; x++) {
		for (int y = topTile; y <= bottomTile; y++) {

			// check if the tile is collidable
			if (map[x + y * columns].tileState == TileStates::collision) {

				// get the tile position in the world
				int tilePosX = x * tileSize.x;
				int tilePosY = y * tileSize.y;

				//int closestX = std::clamp(static_cast<int>(object->GetPos().x), tilePosX, tilePosX + tileSize - static_cast<int>(player->GetWidth()));
				//int closestY = std::clamp(static_cast<int>(object->GetPos().y), tilePosY - static_cast<int>(player->GetHeight()), tilePosY);
				//object->SetPos(closestX, closestY);

				object->SetVel(0.0f);
				object->onGround = true;
			}
		}
	}
}