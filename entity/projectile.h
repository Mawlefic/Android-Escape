#pragma once
#include "collision_object.h"
#include "entity.h"

class Projectile : public Entity {
private:
	CollisionComponent* target;
	float GRAVITY, SPEED;
	Color color, secondaryColor;
	float vx, vy, xRemainder, yRemainder;
	float lifeTime = 2.0f;
	float currentLifeTime = lifeTime;

public:
	// variables
	CollisionComponent cc;
	int damage;
	bool enabled, isHoming;

	// functions
	Projectile();
	Projectile(int x, int y);
	Projectile(int x, int y, float vx, float vy);
	Projectile(int x, int y, float vx, float vy, float G);
	Projectile& operator=(const Projectile& p);
	void Update(float delta) override;
	void Draw() override;
	void SetSize(int x, int y);
	void SetColor(Color c);
	void SetSecondaryColor(Color c);
	void SetDamage(int damage);
	void SetLifeTime(float lifetime);
	void SetTarget(CollisionComponent& c, float speed);
	bool isExpired();
};