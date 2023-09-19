#pragma once

using namespace Tmpl8;

class TMLoader {
public:
	struct Tilemap {
		// constructor / destructor
		Tilemap(const char* filename, uint rows, uint columns) :
			file(filename),
			rows(rows),
			columns(columns),
			mapSize(rows * columns),
			map(new int [rows * columns]) {
			Initialize(",");
		}
		~Tilemap();

		// special opperations
		void Initialize(char* delimiter);

		// attributes
		const char* file;
		uint rows, columns;  // the number of rows and columns of the tilemap
		uint mapSize; // the size of the tilemap
		int* map;
	};

	struct Tilesheet {
		// constructor / destructor
		Tilesheet(const char* filename, uint rows, uint columns, uint2 tileSize) :
			tilesheet(filename),
			rows(rows),
			columns(columns),
			numTiles(rows * columns),
			tileSize(tileSize),
			tiles(new Surface* [rows * columns]) {
			Initialize();
		}
		~Tilesheet();

		// special opperations
		void Initialize();

		// attributes
		uint rows, columns; // the number of rows and columns of the tilesheet
		uint numTiles; // the number of total tiles
		uint2 tileSize; // the size of one tile
		Surface tilesheet;
		Surface** tiles;
	};

	// special opperations
	void DrawTilemap(Tilemap& tilemap, Tilesheet& tilesheet, Surface* screen, int x, int y);
};

