#pragma once
#include "entity.h"
#include "collision_object.h"

enum CollectType {
	DOUBLE_JUMP, PUSH_BACK, DASH, DAMAGE_DASH, HEALTH, RESPAWN, DANGER
};

class Collectible : public Entity{
public:
	// variables
	CollectType ct;
	CollisionComponent cc;
	bool enabled;

	// functions
	Collectible(CollectType collectType);
	void SetPosition(int x, int y);
	void Draw() override;
	void Update(float delta) override;
};