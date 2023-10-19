// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {
	class Camera;
	class Tileset;
	class Tilemap;
	class EntityMap;
	class GameObject;
	class Player;
	class Rope;
	class Game : public TheApp {
	public:
		// game flow methods
		void Init();
		void Tick(float deltaTime);
		void Shutdown();

	private:
		// special opperations
		void UpdateEntities(float dt);
		void RenderEntities();

		int score = 0;
		Camera* camera;
		Tileset* mainTileset;
		Tilemap* mainTilemap;
		Tileset* entityTileset;
		Tilemap* entityTilemap;
		EntityMap* entityMap;
		Player* player;
		Rope* rope;
	};
}