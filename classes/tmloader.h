#pragma once

namespace Tmpl8 {
	class Entity;


	class Tileset {
	public:
		enum TileTypes { None = 0, Collision = 1, Ladder = 2, Water = 3, Death = 4 };
		struct Tile {
			// constructor / destructor
			Tile() = default;
			Tile(int tileID) : tileID(tileID) {
				switch (tileID) {
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 26:
				case 27:
				case 28:
				case 41:
				case 42:
				case 43:
				case 56:
				case 57:
				case 58:
				case 71:
				case 72:
				case 73:
					tileType = TileTypes::Collision;
					break;
				case 7:
				case 20:
				case 33:
					tileType = TileTypes::Ladder;
					break;
				case 63:
				case 64:
				case 74:
				case 75:
				case 76:
				case 77:
					tileType = TileTypes::Death;
					break;
				default:
					tileType = TileTypes::None;
					break;
				}
			};

			// attributes
			int tileID = -1;
			Surface surface;
			TileTypes tileType = TileTypes::None;
		};

		// constructor / destructor
		Tileset(const char* filename, uint columns, uint rows, uint2 tileSize);
		~Tileset();

		// special opperations
		void InitializeTileset(const char* filename);

		// member data access
		Tile* GetTile(int index) { return tiles[index]; }
		int GetTileID(int index) { return tiles[index]->tileID; }
		Surface* GetTileSurface(int index) { return &tiles[index]->surface; }
		Tileset::TileTypes GetTileType(int index) { return tiles[index]->tileType; }

		// attributes
		uint columns, rows;
		uint2 tileSize = 32;
		uint mapSize;

	private:
		// attributes
		Tile** tiles;
	};

	class Tilemap {
	public:
		// constructor / destructor
		Tilemap(const char* tilemap, Tileset* tileset, uint columns, uint rows);

		// member data access
		Tileset::Tile* GetTile(int index) { return tileset->GetTile(map[index]); }
		Tileset::Tile* GetTile(int x, int y) { return tileset->GetTile(map[x + y * columns]); }
		int GetTileID(int index) { return tileset->GetTileID(map[index]); }
		int GetTileID(int x, int y) { return tileset->GetTileID(map[x + y * columns]); }
		Surface* GetTileSurface(int index) { return tileset->GetTileSurface(map[index]); }
		Surface* GetTileSurface(int x, int y) { return tileset->GetTileSurface(map[x + y * columns]); }
		Tileset::TileTypes GetTileType(int index) { return tileset->GetTileType(map[index]); }
		Tileset::TileTypes GetTileType(int x, int y) { return tileset->GetTileType(map[x + y * columns]); }
		uint2 GetTileSize() { return tileset->tileSize; }

		// special opperations
		void ParseCSV(const char* file);

		// attributes
		uint columns, rows;
		uint mapSize;

	private:
		// attributes
		Tileset* tileset;
		int* map;
	};

	class EntityMap {
	public:
		// constructor / destructor
		EntityMap(Tilemap* tilemap, Entity* entityTypes[]);

		// special opperations
		void InitializeEntities(Entity* entityTypes[]);

		// attributes
		uint entityCount = 0;
		Tilemap* tilemap;
		Entity** entities;
	};
}