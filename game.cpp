// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include <iostream>

#include "precomp.h"
#include "game.h"
#include "tmloader.h"

TMLoader tml;

TMLoader::Tilemap Tilemap1("tiled/tilemap1.csv");

Sprite CharacterSprites(new Surface("assets/character_sheet.png"), 1, 11);
Sprite Tilesheet(new Surface("assets/pitfall_tilesheet.png"), 3, 8);
float posX = 400;

void Game::Init() {
	screen->Clear(0);
	tml.LoadTilemap(Tilemap1, ",");
	CharacterSprites.SetFrame(0);
}

void Game::Tick(float deltaTime) {
	screen->Clear(0);
	tml.DrawTilemap(Tilemap1, Tilesheet, screen, 0 - posX, 0);
	if (m_Keyboard.keys[68] == InputState::Down) {
		posX += 1 * deltaTime;
	}
	if (m_Keyboard.keys[65] == InputState::Down) {
		posX -= 1 * deltaTime;
	}
	CharacterSprites.Draw(screen, posX, 300);

	UpdateInputState();
}

int Game::InputStateMod(int curState, int newState) {
	if (!curState && !newState) { newState = (int)InputState::Up; }
	if (curState && newState) { newState = (int)InputState::Down; }
	if (curState && !newState) { newState = (int)InputState::Released; }
	if (!curState && newState) { newState = (int)InputState::Pressed; }
	return newState;
}
void Game::UpdateInputState() {
	for (int i = 0; i < m_MaxKeys; i++) {
		if (m_Keyboard.keys[i] == InputState::Pressed) { m_Keyboard.keys[i] = InputState::Down; }
		else if (m_Keyboard.keys[i] == InputState::Released) { m_Keyboard.keys[i] = InputState::Up; }
	}
	for (int i = 0; i < m_MaxButtons; i++) {
		if (m_Mouse.buttons[i] == InputState::Pressed) { m_Mouse.buttons[i] = InputState::Up; }
		else if (m_Mouse.buttons[i] == InputState::Released) { m_Mouse.buttons[i] = InputState::Down; }
	}
}