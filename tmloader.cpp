#include "precomp.h"
#include "tmloader.h"

void TMLoader::LoadTilemap(Tilemap& tilemap, char* delimiter) {
	// got help with the the parsing of csv from ChatGPT
	FILE* file = fopen(tilemap.file, "r");

	// return if you failed to open the file
	if (file == nullptr) {
		return;
	}

	int tokenCount = 0;
	char buffer[1024];

	// get 1024 characters from .csv file and load the into the buffer, then loop through all of the characters
	while (fgets(buffer, sizeof(buffer), file)) {

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
			tilemap.map[tokenCount++] = result * sign;

			// get the next token
			token = strtok(NULL, delimiter);
		}
	}

	fclose(file);
}

void TMLoader::DrawTilemap(Tilemap& tilemap, Sprite& tilesheet, Surface* screen, int x, int y) {
	int tileWidth = tilesheet.GetWidth();
	int tileHeight = tilesheet.GetHeight();
	for (int i = 0; i < sizeof(tilemap.map); i++) {
		if (tilemap.map[i] >= 0 && tilemap.map[i] < tilesheet.Frames()) {
			int xPos = (i % tilemap.columns) * tileWidth;
			int yPos = (i / tilemap.columns) * tileHeight;
			tilesheet.SetFrame(tilemap.map[i]);
			tilesheet.Draw(screen, x + xPos, y + yPos);
		}
	}
}