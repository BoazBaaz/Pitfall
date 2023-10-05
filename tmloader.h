#pragma once

namespace Tmpl8 {
	class Tileset {
	public:
		// constructor / destructor
		Tileset(const char* filename, uint columns, uint rows, uint2 tileSize = 32);
		~Tileset();

		// special opperations
		void InitializeTileset(const char* filename);

		// member data access
		Surface* GetSurface(int index) { return tiles[index]; }
		Surface* GetSurface(int x, int y) { return tiles[x + y * columns]; }

		// attributes
		uint columns, rows; // the number of rows and columns of the tileset
		uint numTiles; // the number of total tiles
		uint2 tileSize; // the size of a tile in pixels

	private:
		// attributes
		Surface** tiles;
	};


	class Tilemap {
	public:
		enum TileStates { none = 0, collision, ladder, death };
	private:
		struct Tile {
			Tile() = default;
			Tile(int tileID) : tileID(tileID) {
				switch (tileID) {
					case -1:
					case 8:	/*Cobweb1*/
					case 9:	/*Cobweb2*/
					case 10: /*Cobweb3*/
					case 11: /*Cobweb4*/
					case 12: /*Cage Top*/
					case 21: /*Sign Left*/
					case 25: /*Cage Bottom*/
					case 34: /*Sign Right*/
					case 35: /*Chain1*/
					case 36: /*Chain2*/
					case 37: /*Chain3*/
					case 38: /*Chain Top*/
					case 46: /*Fase Grey*/
					case 47: /*Barrel*/
					case 49: /*Door Top Open*/
					case 50: /*Chain Half*/
					case 51: /*Chain Bottom*/
					case 59: /*Fase Brown*/
					case 62: /*Door Bottom Open*/
						tileState = TileStates::none;
						break;
					case 7: /*Ladder Top*/
					case 20: /*Ladder Middle*/
					case 33: /*Ladder Bottom*/
						tileState = TileStates::ladder;
						break;
					case 63:
					case 64:
					case 74:
					case 75:
					case 76:
					case 77:
						tileState = TileStates::death;
						break;
					default:
						tileState = TileStates::collision;
						break;
				}
			}
			int tileID = -1;
			TileStates tileState = TileStates::none;
		};
	public:

		// constructor / destructor
		Tilemap(const char* filename, Tileset* tileset, uint columns, uint rows);
		~Tilemap();

		// member data access
		Tile GetTile(int index) { return map[index]; }
		Tile GetTile(int x, int y) { return map[x + y * columns]; }
		Surface* GetSurface() { return surface; }

		// special opperations
		void InitializeTilemap(const char* filename, Tileset* tileset);

		// attributes
		uint columns, rows; // the number of rows and columns of the tilemap
		uint mapSize; // the size of the tilemap
		uint2 tileSize; // the size of a tile in pixels

	private:
		// attributes
		Tile* map;
		Surface* surface;
	};
}