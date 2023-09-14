// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {
	class Game : public TheApp {
	public:
		// game flow methods
		void Init();
		void Tick(float deltaTime);
		void Shutdown() { /* implement if you want to do something on exit */ }
	};
}