#pragma once

using namespace Tmpl8;

class TMLoader {
public:
	struct Tilemap {
		// constructor / destructor
		Tilemap(const char* filename) :
			file(filename) {
		}

		// attributes
		const char* file;
		const static int rows = 24, columns = 80;
		int map[rows * columns];
	};

	// special opperations
	void LoadTilemap(Tilemap& tilemap, char* delimiter);
	void DrawTilemap(Tilemap& tilemap, Sprite& tilesheet, Surface* screen, int x, int y);
};

