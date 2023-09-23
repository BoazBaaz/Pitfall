#pragma once

namespace Tmpl8 {
	class Tilesheet {
	public:
		// constructor / destructor
		Tilesheet(const char* filename, uint rows, uint columns, uint tileSize = 32);
		~Tilesheet();

		// member data access
		Surface* GetTile(int index) { return tiles[index]; }
		Surface* GetTile(int x, int y) { return tiles[x + y * columns]; }

		// special opperations
		void InitializeTilesheet(const char* filename);

		// attributes
		uint rows, columns; // the number of rows and columns of the tilesheet
		uint numTiles; // the number of total tiles
		uint tileSize; // the size of a tile in pixels
	private:
		// attributes
		Surface** tiles;
	};

	class Tilemap {
		enum TileStates { none = 0, collision = 1, ladder = 2, water = 3 };
		struct Tile {
			Tile() = default;
			Tile(int tileID) : tileID(tileID) {
				switch (tileID) {
					case 0:
					case 6:
						tileState = TileStates::collision;
						break;
					case 4:
					case 5:
						tileState = TileStates::ladder;
						break;
					default:
						tileState = TileStates::none;
						break;
				}
			}
			int tileID = -1;
			TileStates tileState = TileStates::none;
		};
	public:
		// constructor / destructor
		Tilemap(const char* filename, uint rows, uint columns, int2 tilemapStart);
		~Tilemap();

		// member data access
		Tile GetTile(int index) { return map[index]; }
		Tile GetTile(int x, int y) { return map[x + y * columns]; }

		// special opperations
		void InitializeTilemap();
		void Render(Tilesheet* tilesheet, Surface* screen, int2 camPos);
		void Collision(Tilesheet* tilesheet, Player* object);

		// attributes
		uint rows, columns; // the number of rows and columns of the tilemap
		uint mapSize; // the size of the tilemap
		int2 position;

	private:
		// attributes
		const char* file;
		Tile* map;
	};
}