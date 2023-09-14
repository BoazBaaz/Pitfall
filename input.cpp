#include "precomp.h"
#include "input.h"

namespace Tmpl8 {
	Input::Input() {
		for (int i = 0; i < m_MaxKeys; i++) {
			m_Keyboard.keys[i] = InputState::Released;
		}
		for (int i = 0; i < m_MaxButtons; i++) {
			m_Mouse.buttons[i] = InputState::Released;
		}
	}

	void Input::MouseMove(int x, int y) {
		m_Mouse.position.x = x, m_Mouse.position.y = y;
		m_Mouse.pixel = (x + SCRWIDTH * y);
	}

	void Input::UpdateInputState() {
		for (int i = 0; i < m_MaxKeys; i++) {
			if (m_Keyboard.keys[i] == InputState::Pressed) { m_Keyboard.keys[i] = InputState::Down; }
			else if (m_Keyboard.keys[i] == InputState::Released) { m_Keyboard.keys[i] = InputState::Up; }
		}
		for (int i = 0; i < m_MaxButtons; i++) {
			if (m_Mouse.buttons[i] == InputState::Pressed) { m_Mouse.buttons[i] = InputState::Up; }
			else if (m_Mouse.buttons[i] == InputState::Released) { m_Mouse.buttons[i] = InputState::Down; }
		}
	}

	int Input::InputStateMod(int curState, int newState) {
		if (!curState && !newState) { newState = (int) InputState::Up; }
		if (curState && newState) { newState = (int) InputState::Down; }
		if (curState && !newState) { newState = (int) InputState::Released; }
		if (!curState && newState) { newState = (int) InputState::Pressed; }
		return newState;
	}
}