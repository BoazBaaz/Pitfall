// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {
	class Tilesheet; 
	class Tilemap; 
	class GameObject;
	class Player;
	class Game : public TheApp {
	public:
		// game flow methods
		void Init();
		void Tick(float deltaTime);
		void Shutdown();

	private:
		// special opperations
		void ClampCamera();
		void UpdateEntities(float dt);
		void RenderEntities();

		int2 camPos;

		Tilesheet* tilesheet = 0;
		Tilemap* tilemap = 0;

		GameObject* enities = 0;
		Player* player = 0;
	};
}