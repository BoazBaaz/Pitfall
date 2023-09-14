#pragma once

namespace Tmpl8 {
	class Input {
		enum class InputState { Up, Down, Released, Pressed };
	public:
		// constructor / destructor
		Input();
		~Input() = default;
		
		// member data access
		bool GetKey(int key) { return (m_Keyboard.keys[key] == InputState::Down) ? true : false; }
		bool GetKeyUp(int key) { return (m_Keyboard.keys[key] == InputState::Released) ? true : false; }
		bool GetKeyDown(int key) { return (m_Keyboard.keys[key] == InputState::Pressed) ? true : false; }
		bool GetMouseButton(int button) { return (m_Mouse.buttons[button] == InputState::Down) ? true : false; }
		bool GetMouseButtonUp(int button) { return (m_Mouse.buttons[button] == InputState::Released) ? true : false; }
		bool GetMouseButtonDown(int button) { return (m_Mouse.buttons[button] == InputState::Pressed) ? true : false; }
		int2 GetMousePos() { return m_Mouse.position; }
		int GetMousePosPixel() { return m_Mouse.pixel; }
		float GetScrollWheel() { return m_Mouse.scroll; }

		// special operations
		void KeyInput(int key, bool action) { m_Keyboard.keys[key] = (InputState) InputStateMod((int) m_Keyboard.keys[key], action); }
		void MouseInput(int button, bool action) { m_Mouse.buttons[button] = (InputState) InputStateMod((int) m_Mouse.buttons[button], action); }
		void MouseWheel(float y) { m_Mouse.scroll = y; }
		void MouseMove(int x, int y);
		void UpdateInputState();
	private:
		// special operations
		int InputStateMod(int curState, int newState); // might be to heacy on the CPU, need to make somehting else

		// static attributes
		const static uint m_MaxKeys = 256; // might need to change this is there are not enought keys
		const static uint m_MaxButtons = 6;

		// attributes
		struct Keyboard {
			InputState keys[m_MaxKeys] = {};
		} m_Keyboard;
		struct Mouse {
			InputState buttons[m_MaxButtons] = {};
			int2 position;
			int pixel = 0;
			float scroll = 0;
		} m_Mouse;
	};
}