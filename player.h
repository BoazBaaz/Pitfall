#pragma once

using namespace Tmpl8;

class Player : public GameObject {
public:
	// constructor / destructor
	Player(Sprite* sprite, int2 position);
	Player(Sprite* sprite, int2 position, float speed);
	~Player();

	void Update(Input* input, float dt);

private:
	// attributes
	float speed;
	int spriteWidth, spriteHeight;
};