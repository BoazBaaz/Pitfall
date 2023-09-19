#include "precomp.h"
#include "tilemap.h"

#include <iostream>;

TMLoader::Tilemap::~Tilemap() {
	delete map;
}

TMLoader::Tilesheet::~Tilesheet() {
	for (unsigned int i = 0; i < numTiles; i++)
		delete tiles[i];
	delete tiles;
}

void TMLoader::Tilemap::Initialize(char* delimiter) {
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
		char* token = strtok(buffer, delimiter);

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
			map[tokenCount++] = (result * sign);

			// get the next token
			token = strtok(NULL, delimiter);
		}
	}

	fclose(f);
}

void TMLoader::Tilesheet::Initialize() {
	for (uint i = 0; i < numTiles; i++) {
		tiles[i] = new Surface(tileSize.x, tileSize.y);
		int x = (i % columns) * tileSize.x;
		int y = (i / columns) * tileSize.y;
		tilesheet.CopyTo(tiles[i], x, -y);
	}
}

void TMLoader::DrawTilemap(Tilemap& tilemap, Tilesheet& tilesheet, Surface* screen, int x, int y) {
	uint tileWidth = tilesheet.tileSize.x;
	uint tileHeight = tilesheet.tileSize.y;
	for (uint i = 0; i < tilemap.mapSize; i++) {
		int xPos = (i % tilemap.columns) * tileWidth;
		int yPos = (i / tilemap.columns) * tileHeight;
		tilesheet.tiles[tilemap.map[i]]->CopyTo(screen, x + xPos, y + yPos);
	}
}


