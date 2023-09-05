// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "game.h"

Sprite ballSprite(new Surface("assets/ball.png"), 1);

void Game::Init() {
	ballSprite.Draw(screen, 0, 0);
}

void Game::Tick(float deltaTime) {
	//screen->Clear(0);
}