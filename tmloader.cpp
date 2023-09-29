#include "precomp.h"
#include "gameobject.h"
#include "tmloader.h"

Tilemap::Tilemap(const char* filename, uint columns, uint rows) :
	file(filename),
	columns(columns),
	rows(rows),
	mapSize(rows* columns),
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

// I help from this tutorial https://jonathanwhiting.com/tutorial/collision/
void Tilemap::Collision(Tilesheet* tilesheet, GameObject* object, Surface* screen) {
	// get the tiles that the player corners is overlapping with
	int leftTile = object->GetPos().x / tilesheet->tileSize.x;
	int rightTile = (object->GetPos().x + object->GetSize().x) / tilesheet->tileSize.x;
	int topTile = object->GetPos().y / tilesheet->tileSize.y;
	int bottomTile = (object->GetPos().y + object->GetSize().y) / tilesheet->tileSize.y;

	// clamp the player collision to alway be inside the grid
	if (leftTile < 0) leftTile = 0;
	if (rightTile > columns) rightTile = columns;
	if (topTile < 0) topTile = 0;
	if (bottomTile > rows) bottomTile = rows;

	// collision check for the X axis
	if (object->GetVel().x != 0) {
		int tileX = 0;

		if (object->GetVel().x < 0) {
			tileX = leftTile;
		}
		else if (object->GetVel().x > 0) {
			tileX = rightTile;
		}

		for (int y = topTile; y <= bottomTile; y++) {
			if (map[tileX + y * columns].tileState == TileStates::collision) {
				object->SetVel(0.0f, object->GetVel().y);
				object->SetPos(object->GetLastPos().x, object->GetPos().y);
			}
			screen->Box(tileX * tilesheet->tileSize.x, y * tilesheet->tileSize.y, tileX * tilesheet->tileSize.x + tilesheet->tileSize.x, y * tilesheet->tileSize.y + tilesheet->tileSize.y, 0xFF0000);
		}
	}

	// collision check for the Y axis
	if (object->GetVel().y != 0) {
		int tileY = 0;

		if (object->GetVel().y < 0) {
			tileY = topTile;
		}
		else if (object->GetVel().y > 0) {
			tileY = bottomTile;
		}

		bool grounded = false;

		for (int x = leftTile; x <= rightTile; x++) {
			if (map[x + tileY * columns].tileState == TileStates::collision) {
				object->SetVel(object->GetVel().x, 0.0f);
				object->SetPos(object->GetPos().x, object->GetLastPos().y);
				if (tileY == bottomTile)
					grounded = true;
			}
			screen->Box(x * tilesheet->tileSize.x, tileY * tilesheet->tileSize.y, x * tilesheet->tileSize.x + tilesheet->tileSize.x, tileY * tilesheet->tileSize.y + tilesheet->tileSize.y, 0x0000FF);
		}
		object->onGround = grounded;
	}
}