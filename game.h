// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {
	class Game : public TheApp {
		enum class InputState { Up, Down, Released, Pressed };
	public:
		// game flow methods
		void Init();
		void Tick(float deltaTime);
		void Shutdown() { /* implement if you want to do something on exit */ }
		// input handling
		void KeyUp(int key) { m_Keyboard.keys[key] = (InputState)InputStateMod((int)m_Keyboard.keys[key], 0); }
		void KeyDown(int key) { m_Keyboard.keys[key] = (InputState)InputStateMod((int)m_Keyboard.keys[key], 1); }
		void MouseUp(int button) { m_Mouse.buttons[button] = (InputState)InputStateMod((int)m_Mouse.buttons[button], 0); }
		void MouseDown(int button) { m_Mouse.buttons[button] = (InputState)InputStateMod((int)m_Mouse.buttons[button], 1); }
		void MouseMove(int x, int y) { m_Mouse.position.x = x, m_Mouse.position.y = y; }
		void MouseWheel(float scroll) { m_Mouse.scrollWheel = scroll; }
		int InputStateMod(int curState, int newState);
		void UpdateInputState();
	private:
		// static attributes
		const static int m_MaxKeys = 256;
		const static int m_MaxButtons = 6;
		
		// attributes
		struct Keyboard {
			InputState keys[m_MaxKeys] = {};
		} m_Keyboard;
		struct Mouse {
			InputState buttons[m_MaxButtons] = {};
			int2 position;
			float scrollWheel = 0;
			int pixel = 0;
		} m_Mouse;

		TMLoader tml;
	};
}