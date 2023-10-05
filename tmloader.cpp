#include "precomp.h"
#include "tmloader.h"

Tileset::Tileset(const char* filename, uint columns, uint rows, uint2 tileSize) :
	columns(columns),
	rows(rows),
	numTiles(rows* columns),
	tileSize(tileSize),
	tiles(new Surface* [rows * columns]) {
	InitializeTileset(filename);
}

Tileset::~Tileset() {
	for (unsigned int i = 0; i < numTiles; i++)
		delete tiles[i];
	delete tiles;
}

void Tileset::InitializeTileset(const char* filename) {
	Surface tileset(filename);
	for (uint i = 0; i < numTiles; i++) {
		tiles[i] = new Surface(tileSize.x, tileSize.y);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		tileset.CopyTo(tiles[i], -x, -y);
	}

}

Tilemap::Tilemap(const char* filename, Tileset* tileset, uint columns, uint rows) :
	columns(columns),
	rows(rows),
	mapSize(rows* columns),
	tileSize(tileset->tileSize),
	map(new Tile[columns * rows]),
	surface(new Surface(columns * tileset->tileSize.x, rows * tileset->tileSize.y)) {
	InitializeTilemap(filename, tileset);
}

Tilemap::~Tilemap() {
	delete map;
	delete surface;
}

// I got help with the the parsing of csv from ChatGPT
void Tilemap::InitializeTilemap(const char* filename, Tileset* tileset) {
	// open the file
	FILE* f = fopen(filename, "r");

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

			// calculate the position of the tile on the surface
			int x = (tokenCount % columns) * tileSize.x;
			int y = (tokenCount / columns) * tileSize.y;

			// draw the tile to the tilemap and add the result to the map array
			int index = result * sign;
			if (index >= 0)
				tileset->GetSurface(index)->CopyTo(surface, x, y);
			else
				surface->Bar(x, y, x + tileSize.x, y + tileSize.y, 0x0);
			map[tokenCount] = Tile(index);

			// increase the token count
			tokenCount++;

			// get the next token
			token = strtok(NULL, ",");
		}
	}

	fclose(f);
}