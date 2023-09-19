#pragma once

using namespace Tmpl8;

class Input {
	enum class InputState { Released, Hold, Up, Down };
public:
	// constructor / destructor
	Input();
	~Input() = default;

	// member data access
	bool GetKey(int key) { return (keyboard.keys[key] == InputState::Hold) ? true : false; }
	bool GetKeyUp(int key) { return (keyboard.keys[key] == InputState::Up) ? true : false; }
	bool GetKeyDown(int key) { return (keyboard.keys[key] == InputState::Down) ? true : false; }
	bool GetMouseButton(int button) { return (mouse.buttons[button] == InputState::Hold) ? true : false; }
	bool GetMouseButtonUp(int button) { return (mouse.buttons[button] == InputState::Up) ? true : false; }
	bool GetMouseButtonDown(int button) { return (mouse.buttons[button] == InputState::Down) ? true : false; }
	int2 GetMousePos() { return mouse.position; }
	int GetMousePosPixel() { return mouse.pixel; }
	float GetScrollWheel() { return mouse.scroll; }

	// special operations
	void KeyInput(int key, bool action) { keyboard.keys[key] = InputStateMod((int)keyboard.keys[key], action), keyIndex = key; }
	void MouseInput(int button, bool action) { mouse.buttons[button] = InputStateMod((int)mouse.buttons[button], action), buttonIndex = button; }
	void MouseWheel(float y) { mouse.scroll = y; }
	void MouseMove(int x, int y);
	void UpdateInputState();
private:
	// special operations
	InputState InputStateMod(int curState, int newState);

	// static attributes
	const static uint MAX_KEYS = 256;
	const static uint MAX_BUTTONS = 6;

	// attributes
	int keyIndex = -1;
	int buttonIndex = -1;

	struct Keyboard {
		InputState keys[MAX_KEYS] = {};
	} keyboard;
	struct Mouse {
		InputState buttons[MAX_BUTTONS] = {};
		int2 position = ( 0, 0 );
		int pixel = 0;
		float scroll = 0;
	} mouse;
};