#pragma once

using namespace Tmpl8;

class GameObject {
	struct Transform { float2 position, velocity; };
public:
	// constructor / destructor
	GameObject(Sprite* sprite, float2 position);
	GameObject(Sprite* sprite, float x, float y);
	~GameObject();

	// member data access
	Sprite* GetSprite() { return sprite; }

	// special operations
	void Update(Input* input, float dt);
	void Collision();

	// attributes
	Transform transform;
	bool onScreen = false;

protected:
	// attributes
	Sprite* sprite;

};