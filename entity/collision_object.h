#pragma once
#include "raylib.h"

class CollisionComponent {
public:
	// functions
	CollisionComponent();
	CollisionComponent(int x, int y, int width, int height);
	bool isXAligned(CollisionComponent c1);
	bool isYAligned(CollisionComponent c1);
	bool isColliding(CollisionComponent c1);
	virtual void Draw();
	CollisionComponent Offset(int x, int y);
	void Log();
	void Draw(Color c);
	void Update();

	// attributes
	int x, y, width, height;
	int top, right, bottom, left;
	bool isActive;
};