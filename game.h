// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {
	class Tileset; 
	class Tilemap; 
	class GameObject;
	class Player;
	class Enemy;
	class Camera;
	class Game : public TheApp {
	public:
		// game flow methods
		void Init();
		void Tick(float deltaTime);
		void Shutdown();

	private:
		// special opperations
		void UpdateEntities(float dt);
		void CollisionEntities();
		void RenderEntities();

		int score = 0;
		Camera* camera;
		Tilemap* tilemap;
		GameObject* entities[20] = { nullptr };
		Player* player;
	};
}