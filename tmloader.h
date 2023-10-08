#pragma once

namespace Tmpl8 {
	class GameObject;
	class TMLoader {
	public:
		// constructor / destructor
		TMLoader(const char* file, uint columns, uint rows, uint2 tileSize);
		~TMLoader() { delete map; }

		// attributes
		uint columns, rows;
		uint2 tileSize = 32;
		uint mapSize;

	protected:
		// special opperations
		void ParseCSV();

		// attributes
		int* map;
		const char* file;
	};



	class Tileset {
	public:
		// constructor / destructor
		Tileset(const char* filename, uint columns, uint rows, uint2 tileSize);
		~Tileset();

		// special opperations
		void InitializeTileset(const char* filename);

		// member data access
		Surface* GetTile(int index) { return tileset[index]; }
		Surface* GetTile(int x, int y) { return tileset[x + y * columns]; }

		// attributes
		uint columns, rows;
		uint2 tileSize = 32;
		uint mapSize;

	private:
		// attributes
		Surface** tileset;
	};

	class Tilemap : public TMLoader {
	public:
		enum TileStates { none = 0, collision, ladder, water, death };
	private:
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
						tileState = TileStates::collision;
						break;
					case 7:
					case 20:
					case 33:
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
						tileState = TileStates::none;
						break;
				}
			}

			// attributes
			int tileID = -1;
			TileStates tileState = TileStates::none;
		};
	public:
		// constructor / destructor
		Tilemap(const char* filename, Tileset* tileset, uint columns, uint rows);
		~Tilemap();

		// member data access
		Tile GetTile(int index) { return tilemap[index]; }
		Tile GetTile(int x, int y) { return tilemap[x + y * columns]; }
		Surface* GetSurface() { return surface; }

		// special opperations
		void InitializeTilemap(Tileset* tileset);

	private:
		// attributes
		Tile* tilemap;
		Surface* surface;
	};

	class EntityMap : TMLoader {
	public:
		// constructor / destructor
		EntityMap(const char* filename, Tileset* tileset, uint columns, uint rows, GameObject* target, Tilemap* tilemap);

		// special opperations
		void InitializeEntities(Tileset* tileset, GameObject* target, Tilemap* tilemap);

		// attributes
		uint entityCount = 0;
		GameObject** entities;
	};
}