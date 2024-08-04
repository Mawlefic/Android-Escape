#include "enemy.h"
#include "raylib.h"
#include "../helper/utility.h"

void Enemy::Draw()
{
	if (cc.isActive) {
		DrawRectangle(cc.x, cc.y, cc.width, cc.height, GRAY);
	}
}

void Enemy::HandlePlayerDetection()
{
	float distX = (target->cc.x - cc.x);
	distX *= distX;
	float distY = (target->cc.y - cc.y);
	distY *= distY;
	spottedPlayer = detectionRadius >= sqrt(distX + distY);
}

void Enemy::SetTarget(Player& player)
{
	target = &player;
}

void Enemy::SetPosition(int x, int y)
{
	cc.x = x;
	cc.y = y;
	cc.Update();
}

// *** SENTRY FUNCTIONS ***

Sentry::Sentry() {
	detectionRadius = 100;
	cc = CollisionComponent(0, 0, 40, 40);
	cc.Update();
	spottedPlayer = false;
	facingRight = true;
	MAX_HP = 100;
	HP = MAX_HP;
}

Sentry::Sentry(int detectionRadius) : Sentry() {
	this->detectionRadius = detectionRadius;
}

Sentry& Sentry::operator=(const Sentry& s)
{
	cc = s.cc;
	detectionRadius = s.detectionRadius;
	target = s.target;
	return *this;
}

void Sentry::Update(float delta)
{
	if (target != nullptr) {
		HandlePlayerDetection();
	}
	TimerUpdate(delta);
}

void Sentry::HandlePlayerDetection()
{
	int targetX = target->cc.x + target->cc.width / 2;
	int targetY = target->cc.y + target->cc.height / 2;
	int selfX = cc.x + cc.width / 2;
	int selfY = cc.y + cc.height / 2;
	float distX = (targetX - selfX);
	distX *= distX;
	float distY = (targetY - selfY);
	distY *= distY;
	spottedPlayer = detectionRadius >= sqrt(distX + distY);
	if (spottedPlayer && (facingRight && (targetX < selfX)) || (!facingRight && (targetX > selfX))) {
		facingRight = !facingRight;
	}
}

void Sentry::TimerUpdate(float delta)
{
	if (!canShoot) {
		shotCounter += delta;
		if (shotCounter >= shotCD) {
			shotCounter = 0.0f;
			canShoot = true;
		}
	}
}

void Sentry::Draw()
{
	if (cc.isActive) {
		if (isHoming) {
			DrawRectangle(cc.x, cc.y, cc.width, cc.height, DARKBROWN);
		}
		else {
			DrawRectangle(cc.x, cc.y, cc.width, cc.height, GRAY);
		}
	}
}

// *** FLOATER FUNCTIONS ***

Floater::Floater()
{
	detectionRadius = 500;
	cc = CollisionComponent(0, 0, 60, 60);
	cc.Update();
	spottedPlayer = false;
	vx = 0;
	vy = 0;
	xRemainder = 0.0f;
	yRemainder = 0.0f;
	MAX_HP = 200;
	HP = MAX_HP;
}

Floater::Floater(int detectionRadius) : Floater()
{
	this->detectionRadius = detectionRadius;
}

Floater& Floater::operator=(const Floater& s)
{
	cc = s.cc;
	vx = s.vx;
	vy = s.vy;
	xRemainder = s.xRemainder;
	yRemainder = s.yRemainder;
	spottedPlayer = s.spottedPlayer;
	detectionRadius = s.detectionRadius;
	target = s.target;
	canShoot = s.canShoot;
	shotCounter = s.shotCounter;
	return *this;
}

void Floater::Draw()
{
	if (cc.isActive) {
		if (canShoot) {
			DrawRectangle(cc.x, cc.y, cc.width, cc.height, MAROON);
		}
		else {
			DrawRectangle(cc.x, cc.y, cc.width, cc.height, GRAY);
		}
	}
}

void Floater::Update(float delta)
{
	HandlePlayerDetection();
	float angle = 0.0f;
	if (spottedPlayer) {
		float x = target->cc.x - cc.x;
		float y = target->cc.y - cc.y;
		angle = atan2f(y, x);
		vx = cos(angle) * SPEED;
		vy = sin(angle) * SPEED;
		if (canShoot) {
			shotCounter += delta;
		}
	}
	else {
		vx = 0;
		vy = 0;
	}
	xRemainder += vx * delta;
	yRemainder += vy * delta;
	int moveX = floor(xRemainder);
	int moveY = floor(yRemainder);
	xRemainder -= moveX;
	yRemainder -= moveY;
	cc.x += moveX;
	cc.y += moveY;
	if (target->cc.isColliding(cc)) {
		target->TakeDamage(CONTACT_DMG);
		target->Push(angle);
	}
}

// *** MINIBOSS FUNCTIONS ***

Miniboss::Miniboss()
{
	cc.x = 0;
	cc.y = 0;
	cc.width = 280;
	cc.height = 120;
	cc.Update();
}

Miniboss& Miniboss::operator=(const Miniboss& m)
{
	cc = m.cc;
	cc.Update();
	enabled = m.enabled;
	defeated = m.defeated;
	shotCD = m.shotCD;
	shotCounter = m.shotCounter;
	secondaryShotCD = m.secondaryShotCD;
	secondaryShotCounter = m.secondaryShotCounter;
	tertiaryShotCD = m.tertiaryShotCD;
	tertiaryShotCounter = m.tertiaryShotCounter;
	spawnTime = m.spawnTime;
	xRemainder = m.xRemainder;
	PROJ_SPEED = m.PROJ_SPEED;
	HP = m.MAX_HP;
	return *this;
}

void Miniboss::Update(float delta)
{
	if (spawnTime <= 1.0f) {
		spawnTime += delta;
		if (spawnTime > 1.0f) {
			HP = MAX_HP;
		}
	}
	else {
		shotCounter += delta;
		if (HP < MAX_HP / 2) {
			secondaryShotCounter += delta;
			xRemainder += SPEED * delta;
		}
		if (HP < MAX_HP / 4) {
			tertiaryShotCounter += delta;
			xRemainder += (SPEED * delta) / 2;
		}
		xRemainder += SPEED * delta;
		int move = floor(xRemainder);
		xRemainder -= move;
		if (direction) {
			cc.x += move;
		}
		else {
			cc.x -= move;
		}
		cc.Update();
	}
}

void Miniboss::SetPosition(int x, int y)
{
	cc.x = x - cc.width / 2;
	cc.y = y - cc.height / 2;
	cc.Update();
}

void Miniboss::Draw()
{
	if (enabled) {
		DrawRectangle(cc.x, cc.y, cc.width, cc.height, Fade(DARKGRAY, spawnTime));
	}
}

// *** BOSS FUNCTIONS ***

Boss::Boss()
{
	cc.x = 0;
	cc.y = 0;
	cc.width = 300;
	cc.height = 300;
	cc.Update();
	attack = ATK2;
	isMoving = false;
	enabled = false;
	defeated = false;
}

Boss& Boss::operator=(const Boss& b)
{
	cc = b.cc;
	cc.Update();
	SPEED = b.SPEED;
	xRemainder = b.xRemainder;
	leftX = b.leftX;
	middleX = b.middleX;
	rightX = b.rightX;
	destX = b.destX;
	attack = b.attack;
	enabled = b.enabled;
	defeated = b.defeated;
	isMoving = b.isMoving;
	patternCounter = b.patternCounter;
	targetedCounter = b.targetedCounter;
	sprayCounter = b.sprayCounter;
	trapCounter = b.trapCounter;
	homingCounter = b.homingCounter;
	HP = b.HP;
	spawnTime = b.spawnTime;
	return *this;
}

void Boss::GetNextPattern()
{
	isMoving = true;
	switch (attack)
	{
	case ATK1:
		attack = ATK2;
		destX = middleX;
		break;

	case ATK2:
		attack = ATK3;
		targetedCounter = 0.0f;
		sprayCounter = 0.0f;
		destX = rightX;
		break;

	case ATK3:
		attack = ATK1;
		trapCounter = 0.0f;
		homingCounter = 0.0f;
		destX = leftX;
		break;
	}
}

void Boss::ProcessPattern(float delta)
{
	switch (attack)
	{
	case ATK1:
		// pattern 1
		break;

	case ATK2:
		// pattern 2
		sprayCounter += delta;
		targetedCounter += delta;
		break;

	case ATK3:
		trapCounter += delta;
		homingCounter += delta;
		// pattern 3
		break;

	}
}

void Boss::Update(float delta)
{
	if (spawnTime <= 1.0f) {
		spawnTime += delta;
		if (spawnTime > 1.0f) {
			HP = MAX_HP;
		}
	}
	else {
		if (isMoving) {
			xRemainder += SPEED * delta;
			int move = floor(xRemainder);
			xRemainder -= move;
			MoveTowards(cc.x, destX, move);
			if (cc.x == destX) {
				isMoving = false;
			}
		}
		else {
			ProcessPattern(delta);
			patternCounter += delta;
			if (patternCounter >= patternDuration) {
				patternCounter = 0.0f;
				GetNextPattern();
			}
		}
	}
}

void Boss::SetPosition(int x, int y)
{
	cc.x = x - cc.width / 2;
	cc.y = y - cc.height / 2;
	middleX = cc.x;
	leftX = middleX - (40 * TILE_SIZE);
	rightX = middleX + (40 * TILE_SIZE);
	cc.Update();
}

void Boss::Draw()
{
	if (enabled) {
		DrawRectangle(cc.x, cc.y, cc.width, cc.height, Fade(DARKGRAY, spawnTime));
	}
}
