#include "collision_object.h"
#include "raylib.h"
#include "../helper/utility.h"
#include <stdio.h>

CollisionComponent::CollisionComponent() {
	x = 0;
	y = 0;
	width = 20;
	height = 20;
	left = x;
	right = x + width;
	top = y;
	bottom = y + height;
	isActive = true;
}

CollisionComponent::CollisionComponent(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->left = x;
	this->right = x + width;
	this->top = y;
	this->bottom = y + height;
	isActive = true;
	this->Update();
}

void CollisionComponent::Draw() {
	DrawRectangle(x, y, width, height, BLACK);
}

void CollisionComponent::Draw(Color c) {
	DrawRectangle(x, y, width, height, c);
}

void CollisionComponent::Update()
{
	left = x;
	right = x + width;
	top = y;
	bottom = y + height;
}

bool CollisionComponent::isXAligned(CollisionComponent c1)
{
	if (!c1.isActive || !this->isActive) {
		return false;
	}
	return InRange(left, c1.left, c1.right) || InRange(right, c1.left, c1.right) || InRange(c1.left, left, right)
		|| InRange(c1.right, left, right);
}

bool CollisionComponent::isYAligned(CollisionComponent c1)
{
	if (!c1.isActive || !this->isActive) {
		return false;
	}
	return InRange(top, c1.top, c1.bottom) || InRange(bottom, c1.top, c1.bottom) || InRange(c1.top, top, bottom)
		|| InRange(c1.bottom, top, bottom);
}

bool CollisionComponent::isColliding(CollisionComponent c1)
{
	if (!c1.isActive || !this->isActive) {
		return false;
	}
	return ((x < (c1.x + c1.width) && (x + width) > c1.x) &&
		(y < (c1.y + c1.height) && (y + height) > c1.y));
}

CollisionComponent CollisionComponent::Offset(int xOffset, int yOffset) {
	return CollisionComponent(x + xOffset, y + yOffset, width, height);
}

void CollisionComponent::Log() {
	printf("Top: %d, Bottom: %d\nLeft: %d, Right: %d\nX: %d, Y: %d\nWidth: %d, Height: %d\n", top, bottom, left, right, x, y, width, height);
}