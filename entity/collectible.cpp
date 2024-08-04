#include "collectible.h"

Collectible::Collectible(CollectType collectType)
{
	ct = collectType;
	if (ct == DANGER) {
		CollisionComponent(0, 0, 20, 20);
	}
	else {
		cc = CollisionComponent(0, 0, 40, 40);
	}
	enabled = true;
}

void Collectible::SetPosition(int x, int y) {
	cc.x = x;
	cc.y = y;
	cc.Update();
}

void Collectible::Draw()
{
	if (enabled) {
		if (ct == DANGER) {
			DrawRectangle(cc.x, cc.y, 20, 20, RED);
		}
		else {
			DrawRectangleGradientH(cc.x, cc.y, cc.width, cc.height, YELLOW, SKYBLUE);
		}
	}
}

void Collectible::Update(float delta)
{
	// nothing???
}
