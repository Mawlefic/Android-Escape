#include "projectile.h"
#include "raylib.h"
#include <cmath>
#include <stdio.h>

// *** PROJECTILE ***

Projectile::Projectile() : GRAVITY(0.0f)
{
	enabled = true;
	cc.x = 0;
	cc.y = 0;
	cc.width = 15;
	cc.height = 15;
	cc.Update();
	vx = 0;
	vy = 0;
	xRemainder = 0.0f;
	yRemainder = 0.0f;
	color = WHITE;
	secondaryColor = WHITE;
	damage = 0;
	// homing attributes
	SPEED = 0.0f;
	isHoming = false;
	target = nullptr;
}

Projectile::Projectile(int x, int y) : Projectile(){
	cc.x = x;
	cc.y = y;
	cc.Update();
}

Projectile::Projectile(int x, int y, float vx, float vy) : Projectile(x, y) {
	this->vx = vx;
	this->vy = vy;
}

Projectile::Projectile(int x, int y, float vx, float vy, float G) : GRAVITY(G){
	cc.x = x;
	cc.y = y;
	cc.width = 15;
	cc.height = 15;
	cc.Update();
	this->vx = vx;
	this->vy = vy;
	xRemainder = 0.0f;
	yRemainder = 0.0f;
	enabled = true;
	color = WHITE;
	secondaryColor = WHITE;
	damage = 0;
	// homing attributes
	SPEED = 0.0f;
	isHoming = false;
	target = nullptr;
}

Projectile& Projectile::operator=(const Projectile& p)
{
	cc = p.cc;
	enabled = p.enabled;
	vx = p.vx;
	vy = p.vy;
	xRemainder = p.xRemainder;
	yRemainder = p.yRemainder;
	lifeTime = p.lifeTime;
	currentLifeTime = p.currentLifeTime;
	color = p.color;
	secondaryColor = p.secondaryColor;
	damage = p.damage;
	GRAVITY = p.GRAVITY;
	target = p.target;
	isHoming = p.isHoming;
	SPEED = p.SPEED;
	return *this;
}

void Projectile::Draw() {
	if (enabled) {
		DrawCircleGradient(cc.x + cc.width / 2, cc.y + cc.height / 2, (cc.height / 2) * 1.2, secondaryColor, color);
	}
}

void Projectile::Update(float delta) {
	if (enabled) {
		currentLifeTime -= delta;
		if (isHoming) {
			int xDist = (target->x + target->width / 2) - cc.x;
			int yDist = (target->y +target->height / 2) - cc.y;
			float angle = atan2(yDist, xDist);
			float dist = sqrt((xDist * xDist) + (yDist * yDist));
			if (dist >= 2.0f) {
				vx = cos(angle) * SPEED;
				vy = sin(angle) * SPEED;
				xRemainder += vx * delta;
				yRemainder += vy * delta;
				int moveX = round(xRemainder);
				int moveY = round(yRemainder);
				xRemainder -= moveX;
				yRemainder -= moveY;
				cc.x += moveX;
				cc.y += moveY;
				cc.Update();
			}
		}
		else {
			vy += GRAVITY * delta;
			xRemainder += vx * delta;
			yRemainder += vy * delta;
			int moveX = round(xRemainder);
			int moveY = round(yRemainder);
			xRemainder -= moveX;
			yRemainder -= moveY;
			cc.x += moveX;
			cc.y += moveY;
			cc.Update();
		}
	}
	if (currentLifeTime <= 0.0f) {
		enabled = false;
		cc.isActive = false;
	}
}

void Projectile::SetSize(int x, int y)
{
	int diffX = (cc.width - x) / 2;
	int diffY = (cc.height - y) / 2;
	cc.x += diffX;
	cc.y += diffY;
	cc.width = x;
	cc.height = y;
	cc.Update();
}

void Projectile::SetColor(Color c)
{
	color = c;
}

void Projectile::SetSecondaryColor(Color c) {
	secondaryColor = c;
}

void Projectile::SetDamage(int damage)
{
	this->damage = damage;
}

void Projectile::SetLifeTime(float lifetime)
{
	lifeTime = lifetime;
	currentLifeTime = lifetime;
}

void Projectile::SetTarget(CollisionComponent& c, float speed)
{
	target = &c;
	isHoming = true;
	SPEED = speed;
}

bool Projectile::isExpired()
{
	return currentLifeTime <= 0;
}
