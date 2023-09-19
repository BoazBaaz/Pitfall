#include "precomp.h"
#include "input.h"

Input::Input() {
	for (int i = 0; i < MAX_KEYS; i++) {
		keyboard.keys[i] = InputState::Released;
	}
	for (int i = 0; i < MAX_BUTTONS; i++) {
		mouse.buttons[i] = InputState::Released;
	}
}

void Input::MouseMove(int x, int y) {
	mouse.position.x = x, mouse.position.y = y;
	mouse.pixel = (x + SCRWIDTH * y);
}

void Input::UpdateInputState() {
	if (keyIndex >= 0) {
		if (keyboard.keys[keyIndex] == InputState::Up) { keyboard.keys[keyIndex] = InputState::Released; }
		else if (keyboard.keys[keyIndex] == InputState::Down) { keyboard.keys[keyIndex] = InputState::Hold; }
		keyIndex = -1;
	}
	if (buttonIndex >= 0) {
		if (mouse.buttons[buttonIndex] == InputState::Up) { mouse.buttons[buttonIndex] = InputState::Released; }
		else if (mouse.buttons[buttonIndex] == InputState::Down) { mouse.buttons[buttonIndex] = InputState::Hold; }
		buttonIndex = -1;
	}
}

Input::InputState Input::InputStateMod(int curState, int newState) {
	InputState returnState = (InputState)newState;
	if (!curState && !newState) { returnState = InputState::Released; }
	if (curState && newState) { returnState = InputState::Hold; }
	if (curState && !newState) { returnState = InputState::Up; }
	if (!curState && newState) { returnState = InputState::Down; }
	return returnState;
}