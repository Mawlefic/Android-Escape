#include "world.h"
#include <iostream>

const KeyboardKey JUMP_KEY = KEY_C;
const KeyboardKey JUMP_KEY_2 = KEY_SPACE;
const KeyboardKey ABILITY_KEY = KEY_X;
const KeyboardKey ABILITY_KEY_2 = KEY_Z;
const KeyboardKey SWITCH_WEAPON = KEY_S;

World::World() {
	sounds[EXPLOSION] = LoadSound("audio/explosion.wav");
	sounds[BOSS_EXPLOSION] = LoadSound("audio/boss_explosion.wav");
	sounds[SHOOT] = LoadSound("audio/shoot.wav");
	for (int i = 0; i < TOTAL_SOUNDS; i++) {
		SetSoundVolume(sounds[i], 0.2f);
	}
	bgm = LoadMusicStream("audio/c1.ogg");
	SetMusicVolume(bgm, 0.2f);
	PlayMusicStream(bgm);
}

void World::Init(TileMap& tm, Player& p, std::vector<MovingTile>& mt, std::vector<Projectile>& pr, std::vector<Projectile>& epr, EnemyList& foes)
{
	player = &p;
	movingTiles = &mt;
	playerProjectiles = &pr;
	enemyProjectiles = &epr;
	tilemap = &tm;
	enemies = &foes;
	backup = foes;
}

void World::SpawnPlayerProjectile(int x, int y, float vx, float vy, float gravity)
{
	Projectile p{ x, y, vx, vy, gravity };
	p.SetLifeTime(2.0f);
	p.cc.y -= (p.cc.height / 2);
	p.cc.x -= (p.cc.width / 2);
	if (player->wType == WeaponType::REGULAR) {
		p.SetSize(20, 20);
		p.SetDamage(40);
		p.SetColor(DARKBLUE);
	}
	else if (player->wType == WeaponType::PUSHBACK) {
		p.SetSize(30, 30);
		p.SetDamage(10);
		p.SetColor(DARKGREEN);
	}
	playerProjectiles->push_back(p);
}

void World::SpawnEnemyProjectile(int x, int y, float vx, float vy, float gravity, int damage)
{
	Projectile p{ x, y, vx, vy, gravity };
	p.damage = damage;
	p.cc.y -= (p.cc.height / 2);
	p.cc.x -= (p.cc.width / 2);
	p.SetSize(30, 30);
	p.SetColor(BLACK);
	enemyProjectiles->push_back(p);
}

void World::SpawnTargetedEnemyProjectile(int x, int y, float speed, int damage)
{
	int xDist = (player->cc.x + player->cc.width / 2) - x;
	int yDist = (player->cc.y + player->cc.height / 2) - y;
	float angle = atan2(yDist, xDist);
	float vx = cos(angle) * speed;
	float vy = sin(angle) * speed;
	Projectile p{ x, y, vx, vy, 0.0f };
	p.SetSize(30, 30);
	p.SetColor(BLACK);
	p.SetLifeTime(5.0f);
	p.SetSecondaryColor(RED);
	p.damage = damage;
	enemyProjectiles->push_back(p);
}

void World::SpawnSprayProjectile(int x, int y, float speed, int damage)
{
	int sprayShots = 7;
	for (int i = 0; i <= sprayShots; i++) {
		float angle = (180 + ((180 / sprayShots) * i)) * DEG2RAD;
		float vx = cos(angle) * speed;
		float vy = sin(angle) * -speed;
		Projectile p{ x, y, vx, vy, 0.0f };
		p.SetSize(30, 30);
		p.SetColor(DARKPURPLE);
		p.SetSecondaryColor(YELLOW);
		p.SetLifeTime(5.0f);
		p.damage = damage;
		enemyProjectiles->push_back(p);
	}
}

void World::SpawnTrappingShots(int x, int y, float speed, int damage)
{
	int xDist = (player->cc.x + player->cc.width / 2) - x;
	int yDist = (player->cc.y + player->cc.height / 2) - y;
	float angle = atan2(yDist, xDist);
	for (int i = 0; i < 2; i++) {
		float altAngle = angle - (7 * DEG2RAD) + ((14 * i) * DEG2RAD);
		float vx = cos(altAngle) * speed;
		float vy = sin(altAngle) * speed;
		Projectile p{ x, y, vx, vy, 0.0f };
		p.SetSize(30, 30);
		p.SetColor(BLACK);
		p.SetSecondaryColor(DARKGREEN);
		p.SetLifeTime(5.0f);
		p.damage = damage;
		enemyProjectiles->push_back(p);
	}
}

void World::SpawnHomingEnemyProjectile(int x, int y, float speed, int damage, float lifeTime)
{
	Projectile p{ x, y, 0, 0, 0 };
	p.damage = damage;
	p.cc.y -= (p.cc.height / 2);
	p.cc.x -= (p.cc.width / 2);
	p.SetSize(30, 30);
	p.SetTarget(player->cc, speed / 3);
	p.SetLifeTime(lifeTime);
	p.SetColor(ORANGE);
	enemyProjectiles->push_back(p);
}

void World::SpawnHomingEnemyProjectile(int x, int y, float speed, int damage, float lifeTime, int size)
{
	Projectile p{ x, y, 0, 0, 0 };
	p.damage = damage;
	p.cc.y -= (p.cc.height / 2);
	p.cc.x -= (p.cc.width / 2);
	p.SetSize(size, size);
	p.SetTarget(player->cc, speed / 3);
	p.SetLifeTime(lifeTime);
	p.SetColor(ORANGE);
	enemyProjectiles->push_back(p);
}

void World::SetGatePointers(Gate& g, Gate& g2, Gate& g3, Gate& g4)
{
	bossEntry = &g;
	mBossEntry = &g2;
	bossGate = &g3;
	mBossGate = &g4;
}

void World::HandleUpgrade(CollectType ct)
{
	switch (ct)
	{
	case DOUBLE_JUMP:
		player->UnlockDoubleJump();
		text->promptMessage = "Jetpack Module Restored!\n\nDouble Jump has been unlocked!\n\nPress [C] or [SPACE] again to double jump";
		text->enabled = true;
		isPaused = true;
		break;

	case PUSH_BACK:
		player->UnlockPushback();
		text->promptMessage = "Sub-Weapon Restored!\n\nPush-Back Shot has been unlocked!\n\nPress [S] to switch weapons!";
		text->enabled = true;
		isPaused = true;
		break;

	case DASH:
		player->UnlockDash();
		text->promptMessage = "Phase Module Restored!\n\nDash has been unlocked!\n\nPress [Z] to dash!";
		text->enabled = true;
		isPaused = true;
		break;

	case HEALTH:
		player->IncreaseBonusHP();
		if (oneTimePrompt) {
			text->promptMessage = "Max HP has increased!";
			text->enabled = true;
			isPaused = true;
			oneTimePrompt = false;
		}
		break;
	}
}

void World::CheckBossSpawn()
{
	Miniboss& mb = enemies->mBoss;
	int xDist = player->cc.x - mb.cc.x;
	int yDist = player->cc.y - mb.cc.y;
	if (!mb.defeated && player->isGrounded && yDist > 0 && yDist <= 500 && xDist > -100) {
		playerProjectiles->clear();
		enemyProjectiles->clear();
		EnterMinibossZone();
		mb.enabled = true;
		return;
	}
	Boss& boss = enemies->boss;
	xDist = abs(player->cc.x - boss.cc.x);
	yDist = player->cc.y - boss.cc.y;
	if (!boss.defeated && player->isGrounded && yDist > 0 && yDist < 600 && xDist < 500) {
		playerProjectiles->clear();
		enemyProjectiles->clear();
		enemies->floaters.clear();
		EnterBossZone();
		boss.enabled = true;
	}
}

void World::SetCollectibles(std::vector<Collectible>& c)
{
	collectibles = &c;
}

void World::SetTextbox(TextBox& tb)
{
	text = &tb;
}

void World::RemoveInactiveProjectiles()
{
	// player projectiles
	for (int i = 0; i < playerProjectiles->size(); i++) {
		Projectile& p = playerProjectiles->at(i);
		if (!p.cc.isActive) {
			playerProjectiles->erase(playerProjectiles->begin() + i);
			i--;
		}
	}
	// enemy projectiles
	for (int i = 0; i < enemyProjectiles->size(); i++) {
		Projectile& p = enemyProjectiles->at(i);
		if (!p.cc.isActive) {
			enemyProjectiles->erase(enemyProjectiles->begin() + i);
			i--;
		}
	}
}

void World::Update(float delta)
{
	// paused game, prompt messages, dialogue, etc
	if (isPaused) {
		PauseMusicStream(bgm);
		text->fadeValue += 2 * delta;
		if (text->fadeValue >= 1.0f) {
			text->fadeValue = 1.0f;
		}

		if (IsKeyPressed(ABILITY_KEY) || IsKeyPressed(ABILITY_KEY_2) || IsKeyPressed(JUMP_KEY) || IsKeyPressed(JUMP_KEY_2)) {
			isPaused = !isPaused;
			ResumeMusicStream(bgm);
			text->enabled = false;
		}
	}
	else
	{
		UpdateMusicStream(bgm);
		RemoveInactiveProjectiles();
		// player
		PlayerCheckGrounded();
		player->Update(delta);
		if (player->HP <= 0) {
			PlaySound(sounds[EXPLOSION]);
			player->Respawn();
			ResetEnemyPosition();
			mBossEntry->Deactivate();
			bossEntry->Deactivate();
		}
		HandlePlayerInput(delta);
		MovePlayer(delta);

		// enemies

		if (enemies->boss.enabled) {
			Boss& b = enemies->boss;
			b.Update(delta);
			if (b.HP <= 0) {
				PlaySound(sounds[BOSS_EXPLOSION]);
				b.enabled = false;
				b.defeated = true;
				OnBossDefeat();
			}
			switch (b.attack)
			{
			case ATK1:
				// nothing
				break;

			case ATK2:
				if (b.sprayCounter >= b.sprayCD) {
					b.sprayCounter = 0.0f;
					SpawnSprayProjectile(b.cc.x + b.cc.width / 2, b.cc.y + b.cc.height / 2, b.spraySpeed, b.PROJ_DMG);
				}
				if (b.targetedCounter >= b.targetedCD) {
					b.targetedCounter = 0.0f;
					SpawnTargetedEnemyProjectile(b.cc.x + b.cc.width / 2, b.cc.y + b.cc.height / 2, b.targetedSpeed / 2, b.PROJ_DMG);
				}
				break;

			case ATK3:
				if (b.trapCounter >= b.trapCD) {
					b.trapCounter = 0.0f;
					SpawnTrappingShots(b.cc.x + b.cc.width / 2, b.cc.y + b.cc.height / 2, b.spraySpeed, b.PROJ_DMG);
				}
				if (b.homingCounter >= b.homingCD) {
					b.homingCounter = 0.0f;
					SpawnHomingEnemyProjectile(b.cc.x + b.cc.width / 2, b.cc.y + b.cc.height / 2, 600.0f, b.PROJ_DMG, 10.0f, 50);
				}
				break;
			}
		}
		else if (enemies->mBoss.enabled) {
			Miniboss& mb = enemies->mBoss;
			mb.Update(delta);
			if (tilemap->isTileEnabled(GetCell(mb.cc.x), GetCell(mb.cc.y + mb.cc.height / 2)) || 
				tilemap->isTileEnabled(GetCell(mb.cc.right), GetCell(mb.cc.y + mb.cc.height / 2))) {
				mb.direction = !mb.direction;
			}
			if (mb.shotCounter >= mb.shotCD) {
				SpawnEnemyProjectile(mb.cc.x + mb.cc.width / 2, mb.cc.y + mb.cc.height / 2, 0.0f, mb.PROJ_SPEED * 1.5, 0.0f, mb.PROJ_DMG * 2);
				mb.shotCounter = 0.0f;
			}
			if (mb.secondaryShotCounter >= mb.secondaryShotCD) {
				SpawnTargetedEnemyProjectile(mb.cc.x + mb.cc.width / 2, mb.cc.y + mb.cc.height / 2, mb.PROJ_SPEED, mb.PROJ_DMG);
				mb.secondaryShotCounter = 0.0f;
			}
			if (mb.tertiaryShotCounter >= mb.tertiaryShotCD) {
				SpawnTrappingShots(mb.cc.x + mb.cc.width / 2, mb.cc.y + mb.cc.height / 2, mb.PROJ_SPEED, mb.PROJ_DMG);
				mb.tertiaryShotCounter = 0.0f;
			}
			if (mb.HP <= 0) {
				PlaySound(sounds[BOSS_EXPLOSION]);
				mb.enabled = false;
				mb.defeated = true;
				OnMinibossDefeat();
			}
		}
		else {
			// handle sentry
			std::vector<Sentry>& sentries = enemies->sentries;
			for (int i = 0; i < sentries.size(); i++) {
				Sentry& s = sentries[i];
				if (!s.cc.isActive) {
					continue;
				}
				if (s.HP <= 0) {
					PlaySound(sounds[EXPLOSION]);
					s.cc.isActive = false;
					continue;
				}
				s.Update(delta);
				if (s.spottedPlayer && s.canShoot) {
					if (s.isHoming) {
						if (s.facingRight) {
							SpawnHomingEnemyProjectile(s.cc.x + s.cc.width / 2 + 25, s.cc.y + s.cc.height / 2 - 20, s.BULLET_SPEED, s.PROJ_DMG, 5.0f);
						}
						else {
							SpawnHomingEnemyProjectile(s.cc.x + s.cc.width / 2 - 45, s.cc.y + s.cc.height / 2 - 20, s.BULLET_SPEED, s.PROJ_DMG, 5.0f);
						}
					}
					else {
						if (s.facingRight) {
							SpawnEnemyProjectile(s.cc.x + s.cc.width / 2 + 25, s.cc.y + s.cc.height / 2 - 20, s.BULLET_SPEED, 0.0f, 0.0f, s.PROJ_DMG);
						}
						else {
							SpawnEnemyProjectile(s.cc.x + s.cc.width / 2 - 45, s.cc.y + s.cc.height / 2 - 20, -s.BULLET_SPEED, 0.0f, 0.0f, s.PROJ_DMG);
						}
					}
					s.canShoot = false;
				}
			}

			// handle floater
			std::vector<Floater>& floaters = enemies->floaters;
			for (int i = 0; i < floaters.size(); i++) {
				Floater& f = floaters[i];
				if (!f.cc.isActive) {
					continue;
				}
				if (f.HP <= 0) {
					PlaySound(sounds[EXPLOSION]);
					f.cc.isActive = false;
					continue;
				}
				if (f.canShoot && f.shotCounter >= f.shotCD) {
					SpawnTargetedEnemyProjectile(f.cc.x + f.cc.width / 2, f.cc.y + f.cc.height / 2, f.PROJ_SPEED, f.PROJ_DMG);
					f.shotCounter = 0.0f;
				}
				f.Update(delta);
			}
			CheckBossSpawn();
		}

		// handle collectibles
		for (int i = 0; i < collectibles->size(); i++) {
			Collectible& c = collectibles->at(i);
			if (!c.enabled) {
				continue;
			}
			if (c.ct == DANGER) {
				bool oldState = player->cc.isActive;
				player->cc.isActive = true;
				if (player->cc.isColliding(c.cc) && !player->hasBurnTicked) {
					player->damagedRecently = true;
					player->dmgCounter = 0.0f;
					player->burnCounter += delta;
					player->hasBurnTicked = true;
					if (player->burnCounter >= player->burnTick) {
						player->burnCounter = 0.0f;
						player->HP -= 2;
						// hurt/burn sound
					}
				}
				player->cc.isActive = oldState;

			}
			else
			{
				bool oldState = player->cc.isActive;
				player->cc.isActive = true;
				if (player->cc.isColliding(c.cc)) {
					if (c.ct == RESPAWN) {
						if (player->respawnX != c.cc.x && player->respawnY != c.cc.y) {
							text->promptMessage = "Health has been restored!\n\nSpawn point has been set!";
							text->enabled = true;
							isPaused = true;
							player->SetSpawnPoint(c.cc.x, c.cc.y);
						}
						player->HP = (player->MAX_HP + player->bonusHP);
					}
					else {
						HandleUpgrade(c.ct);
						c.enabled = false;
					}
				}
				player->cc.isActive = oldState;
			}
		}

		// moving tiles
		for (int i = 0; i < movingTiles->size(); i++) {
			movingTiles->at(i).Update(delta);
		}
		HandlePlayerCollision();
		HandleProjectiles(delta);
	}
	
}

// *** PLAYER INPUT HANDLING ***

void World::HandlePlayerInput(float delta)
{
	switch (player->playerState)
	{
	case PlayerState::STAND:
		ProcessStandInput(delta);
		break;

	case PlayerState::JUMP:
		ProcessJumpInput(delta);
		break;

	case PlayerState::FALL:
		ProcessFallInput(delta);
		break;

	case PlayerState::DASH:
		ProcessDashInput(delta);
		break;
	}
}

void World::ProcessStandInput(float delta)
{
	// handle horizontal movement
	if (IsKeyDown(KEY_LEFT)) {
		player->WalkLeft();
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		player->WalkRight();
	}
	else {
		player->vx = 0;
	}

	// handle vertical input
	if (IsKeyPressed(JUMP_KEY) || IsKeyPressed(JUMP_KEY_2)) {
		player->Jump();
	}

	// actions
	player->holdingUp = IsKeyDown(KEY_UP);
	player->holdingDown = IsKeyDown(KEY_DOWN);
	// switch weapon
	if (player->unlockedPushBack && IsKeyPressed(SWITCH_WEAPON)) {
		if (player->wType == WeaponType::REGULAR) {
			player->wType = WeaponType::PUSHBACK;
		}
		else {
			player->wType = WeaponType::REGULAR;
		}
	}
	// shoot
	if (IsKeyPressed(ABILITY_KEY) && player->unlockedPushBack && player->wType == WeaponType::PUSHBACK && player->pushBackCharges > 0 && player->canShootP) {
		player->PushBack();
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	if (IsKeyDown(ABILITY_KEY) && player->wType == WeaponType::REGULAR && player->canShoot) {
		player->canShoot = false;
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	// dash
	if (player->unlockedDash && IsKeyPressed(ABILITY_KEY_2)) {
		player->Dash();
	}
}

void World::ProcessJumpInput(float delta)
{
	// handle horizontal movement
	if (IsKeyDown(KEY_LEFT)) {
		player->WalkLeft();
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		player->WalkRight();
	}
	else {
		player->vx = 0;
	}

	// handle vertical movement
	if (IsKeyUp(JUMP_KEY) && IsKeyUp(JUMP_KEY_2)) {
		player->FastFall();
	}
	else if (IsKeyPressed(JUMP_KEY) || IsKeyPressed(JUMP_KEY_2)) {
		player->StartJBufferTimer();
		player->Jump();
	}

	// actions
	player->holdingUp = IsKeyDown(KEY_UP);
	player->holdingDown = IsKeyDown(KEY_DOWN);
	// switch
	if (player->unlockedPushBack && IsKeyPressed(SWITCH_WEAPON)) {
		if (player->wType == WeaponType::REGULAR) {
			player->wType = WeaponType::PUSHBACK;
		}
		else {
			player->wType = WeaponType::REGULAR;
		}
	}
	// shoot 
	if (IsKeyPressed(ABILITY_KEY) && player->unlockedPushBack && player->wType == WeaponType::PUSHBACK && player->pushBackCharges > 0 && player->canShootP) {
		player->PushBack();
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	if (IsKeyDown(ABILITY_KEY) && player->wType == WeaponType::REGULAR && player->canShoot) {
		player->canShoot = false;
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	// dash
	if (player->unlockedDash && IsKeyPressed(ABILITY_KEY_2)) {
		player->Dash();
	}
}

void World::ResetEnemyPosition()
{
	*enemies = backup;
}

void World::ProcessFallInput(float delta)
{
	// handle horizontal movement
	if (IsKeyDown(KEY_LEFT)) {
		player->WalkLeft();
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		player->WalkRight();
	}
	else {
		player->vx = 0;
	}

	// handle vertical movement
	if (IsKeyPressed(JUMP_KEY) || IsKeyPressed(JUMP_KEY_2)) {
		player->ProcessFallingJumpInput();
		player->Jump();
	}

	// actions
	player->holdingUp = IsKeyDown(KEY_UP);
	player->holdingDown = IsKeyDown(KEY_DOWN);
	// switch 
	if (player->unlockedPushBack && IsKeyPressed(SWITCH_WEAPON)) {
		if (player->wType == WeaponType::REGULAR) {
			player->wType = WeaponType::PUSHBACK;
		}
		else {
			player->wType = WeaponType::REGULAR;
		}
	}
	// shoot
	if (IsKeyPressed(ABILITY_KEY) && player->unlockedPushBack && player->wType == WeaponType::PUSHBACK && player->pushBackCharges > 0 && player->canShootP) {
		player->PushBack();
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	if (IsKeyDown(ABILITY_KEY) && player->wType == WeaponType::REGULAR && player->canShoot) {
		player->canShoot = false;
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	// dash
	if (player->unlockedDash && IsKeyPressed(ABILITY_KEY_2)) {
		player->Dash();
	}
}

void World::ProcessDashInput(float delta)
{
	// switch
	if (player->unlockedPushBack && IsKeyPressed(SWITCH_WEAPON)) {
		if (player->wType == WeaponType::REGULAR) {
			player->wType = WeaponType::PUSHBACK;
		}
		else {
			player->wType = WeaponType::REGULAR;
		}
	}
	// shoot
	if (IsKeyPressed(ABILITY_KEY) && player->unlockedPushBack && player->wType == WeaponType::PUSHBACK && player->pushBackCharges > 0 && player->canShootP) {
		player->PushBack();
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
	if (IsKeyDown(ABILITY_KEY) && player->wType == WeaponType::REGULAR && player->canShoot) {
		player->canShoot = false;
		SpawnPlayerProjectile(player->cc.x + (player->cc.width / 2) + player->offsetX, player->cc.y + (player->cc.height / 2), player->GetBulletSpeed().x, player->GetBulletSpeed().y, 0.0f);
	}
}

// *** PLAYER HANDLING FUNCTIONS ***

void World::MovePlayer(float delta)
{
	if ((player->vy + player->projVY) > player->MAX_FALL_SPEED) {
		player->vy = player->MAX_FALL_SPEED;
	}
	MoveX((player->vx + player->projVX) * delta);
	MoveY((player->vy + player->projVY) * delta);
}

void World::MoveX(float amount) {
	player->xRemainder += amount;
	int move = round(player->xRemainder);
	if (move != 0) {
		player->xRemainder -= move;
		move = GetCollisionDistX(move);
		player->cc.x += move;
		player->cc.Update();
	}
}

void World::MoveY(float amount) {
	player->yRemainder += amount;
	int move = round(player->yRemainder);
	if (move != 0) {
		player->yRemainder -= move;
		move = GetCollisionDistY(move);
		player->cc.y += move;
		player->cc.Update();
	}
}

int World::GetCollisionDistX(int maxDistance) {
	int top = GetCell(player->cc.top);
	int bottom = GetCell(player->cc.bottom - 1);
	if (maxDistance > 0) {
		// static tile collisions
		int closeEdge = GetCell(player->cc.right);
		int farEdge = GetCell(player->cc.right + maxDistance);
		int sDist = maxDistance; // right edge of far cell
		for (int x = farEdge; x >= closeEdge; x--) {
			for (int y = top; y <= bottom; y++) {
				if (tilemap->isTileEnabled(x, y)) {
					sDist = (x * TILE_SIZE) - player->cc.right;
					player->vx = 0;
					player->platVelX = 0;
					player->projVX = 0;
					break;
				}
			}
		}
		sDist = std::min(sDist, maxDistance);
		// moving tile collisions
		int mDist = maxDistance;
		for (int i = 0; i < movingTiles->size(); i++) {
			MovingTile& mTile = movingTiles->at(i);
			if (!mTile.enabled) {
				continue;
			}
			if (player->cc.isYAligned(mTile.cc)) {
				CollisionComponent offset = player->cc.Offset(maxDistance, 0);
				if (offset.isColliding(mTile.cc)) {
					mDist = std::min(mDist, mTile.cc.left - player->cc.right);
					player->vx = 0;
					player->platVelX = 0;
					player->projVX = 0;
				}
			}
		}
		// return earliest collision between static and moving platforms
		return std::min(sDist, mDist);
	}
	else {
		// static tile collisions
		int closeEdge = GetCell(player->cc.left);
		int farEdge = GetCell(player->cc.left + maxDistance);
		int sDist = maxDistance;
		for (int x = farEdge; x <= closeEdge; x++) {
			for (int y = top; y <= bottom; y++) {
				if (tilemap->isTileEnabled(x, y)) {
					sDist = ((x + 1) * TILE_SIZE) - player->cc.left;
					player->vx = 0;
					player->platVelX = 0;
					player->projVX = 0;
					break;
				}
			}
		}
		sDist = std::max(sDist, maxDistance);
		// moving tile collisions
		int mDist = maxDistance;
		for (int i = 0; i < movingTiles->size(); i++) {
			MovingTile& mTile = movingTiles->at(i);
			if (!mTile.enabled) {
				continue;
			}
			if (player->cc.isYAligned(mTile.cc)) {
				CollisionComponent offset = player->cc.Offset(maxDistance, 0);
				if (offset.isColliding(mTile.cc)) {
					mDist = std::max(mDist, mTile.cc.right - player->cc.left);
					player->vx = 0;
					player->platVelX = 0;
					player->projVX = 0;
				}
			}
		}
		// return earliest collision between static and moving platforms
		return std::max(sDist, mDist);
	}
}

int World::GetCollisionDistY(int maxDistance) {
	int left = GetCell(player->cc.left);
	int right = GetCell(player->cc.right - 1);
	if (maxDistance > 0) {
		// static tile collisions
		int closeEdge = GetCell(player->cc.bottom);
		int farEdge = GetCell(player->cc.bottom + maxDistance);
		int sDist = maxDistance; // right edge of far cell
		for (int y = farEdge; y >= closeEdge; y--) {
			for (int x = left; x <= right; x++) {
				if (tilemap->isTileEnabled(x, y)) {
					sDist = (y * TILE_SIZE) - player->cc.bottom;
					player->vy = 0;
					player->projVY = 0;
					break;
				}
			}
		}
		sDist = std::min(sDist, maxDistance);
		// moving tile collisions
		int mDist = maxDistance;
		for (int i = 0; i < movingTiles->size(); i++) {
			MovingTile& mTile = movingTiles->at(i);
			if (!mTile.enabled) {
				continue;
			}
			if (player->cc.isXAligned(mTile.cc)) {
				CollisionComponent offset = player->cc.Offset(0, maxDistance);
				if (offset.isColliding(mTile.cc)) {
					mDist = std::min(mDist, mTile.cc.top - player->cc.bottom);
					player->vy = 0;
					player->projVY = 0;
				}
			}
		}
		// return earliest collision between static and moving platforms
		return std::min(sDist, mDist);
	}
	else {
		// static tile collisions
		int closeEdge = GetCell(player->cc.top);
		int farEdge = GetCell(player->cc.top + maxDistance);
		int sDist = maxDistance; // left edge of far cell
		for (int y = farEdge; y <= closeEdge; y++) {
			for (int x = left; x <= right; x++) {
				if (tilemap->isTileEnabled(x, y)) {
					sDist = ((y + 1) * TILE_SIZE) - player->cc.top;
					player->vy = 0;
					player->projVY = 0;
					break;
				}
			}
		}
		sDist = std::max(sDist, maxDistance);
		// moving tile collisions
		int mDist = maxDistance;
		for (int i = 0; i < movingTiles->size(); i++) {
			MovingTile& mTile = movingTiles->at(i);
			if (!mTile.enabled) {
				continue;
			}
			if (player->cc.isXAligned(mTile.cc)) {
				CollisionComponent offset = player->cc.Offset(0, maxDistance);
				if (offset.isColliding(mTile.cc)) {
					mDist = std::max(mDist, mTile.cc.bottom - player->cc.top);
					player->vy = 0;
					player->projVY = 0;
				}
			}
		}
		// return earliest collision between static and moving platforms
		return std::max(sDist, mDist);
	}
}

void World::MoveFlatX(int amount)
{
	if (amount != 0) {
		amount = GetCollisionDistX(amount);
		player->cc.x += amount;
		player->cc.Update();
	}
}

void World::MoveFlatY(int amount)
{
	if (amount != 0) {
		amount = GetCollisionDistY(amount);
		player->cc.y += amount;
		player->cc.Update();
	}
}

void World::HandlePlayerCollision()
{
	for (int i = 0; i < movingTiles->size(); i++) {
		MovingTile& mTile = movingTiles->at(i);
		if (!mTile.enabled || (mTile.moveX == 0 && mTile.moveY == 0)) {
			continue;
		}
		if ((mTile.cc.top - mTile.moveY) == player->cc.bottom && player->cc.isXAligned(mTile.cc.Offset(-mTile.moveX, 0))) {
			mTile.enabled = false;
			MoveFlatX(mTile.moveX);
			MoveFlatY(mTile.moveY);
			mTile.enabled = true;
		}
		else if (mTile.cc.isColliding(player->cc)) {
			// platform is moving in both X & Y Direction
			if (mTile.moveX != 0 && mTile.moveY != 0) {
				CollisionComponent mtc = mTile.cc.Offset(-mTile.moveX, -mTile.moveY);
				int timeX = 0;
				int displacementX = 0;
				if (mTile.moveX > 0) {
					timeX = abs((mtc.right - player->cc.left) / mTile.moveX);
					displacementX = mTile.moveX - (player->cc.left - mtc.right);
				}
				else {
					timeX = abs((mtc.left - player->cc.right) / mTile.moveX);
					displacementX = mTile.moveX - (player->cc.right - mtc.left);
				}
				int timeY = 0;
				int displacementY = 0;
				if (mTile.moveY > 0) {
					timeY = abs((mtc.bottom - player->cc.top) / mTile.moveY);
					displacementY = mTile.moveY - (player->cc.top - mtc.bottom);
				}
				else {
					timeY = abs((mtc.top - player->cc.bottom) / mTile.moveY);
					displacementY = mTile.moveY - (player->cc.bottom - mtc.top);
				}
				bool xAxisCollision = timeX < timeY;
				if (xAxisCollision) {
					mTile.enabled = false;
					MoveFlatX(displacementX);
					mTile.enabled = true;
				}
				else {
					mTile.enabled = false;
					MoveFlatY(displacementY);
					mTile.enabled = true;
				}
			}
			else if (mTile.moveY == 0) {
				CollisionComponent mtc = mTile.cc.Offset(-mTile.moveX, -mTile.moveY);
				int timeX = 0;
				int displacementX = 0;
				if (mTile.moveX > 0) {
					timeX = abs((mtc.right - player->cc.left) / mTile.moveX);
					displacementX = mTile.moveX - (player->cc.left - mtc.right);
				}
				else {
					timeX = abs((mtc.left - player->cc.right) / mTile.moveX);
					displacementX = mTile.moveX - (player->cc.right - mtc.left);
				}
				mTile.enabled = false;
				MoveFlatX(displacementX);
				mTile.enabled = true;
			}
			else if (mTile.moveX == 0) {
				CollisionComponent mtc = mTile.cc.Offset(-mTile.moveX, -mTile.moveY);
				int timeY = 0;
				int displacementY = 0;
				if (mTile.moveY > 0) {
					timeY = abs((mtc.bottom - player->cc.top) / mTile.moveY);
					displacementY = mTile.moveY - (player->cc.top - mtc.bottom);
				}
				else {
					timeY = abs((mtc.top - player->cc.bottom) / mTile.moveY);
					displacementY = mTile.moveY - (player->cc.bottom - mtc.top);
				}
				mTile.enabled = false;
				MoveFlatY(displacementY);
				mTile.enabled = true;
			}
		}
	}
}

bool World::TouchingWall(bool wallDir)
{
	int top = GetCell(player->cc.top);
	int bottom = GetCell(player->cc.bottom - 1);
	if (wallDir) {
		if (player->cc.left % TILE_SIZE != 0) {
			return false;
		}
		int leftCell = GetCell(player->cc.left - 1);
		for (int y = top; y <= bottom; y++) {
			if (tilemap->isTileEnabled(leftCell, y)) {
				return true;
			}
		}
	}
	else {
		if (player->cc.right % TILE_SIZE != 0) {
			return false;
		}
		int rightCell = GetCell(player->cc.right);
		for (int y = top; y <= bottom; y++) {
			if (tilemap->isTileEnabled(rightCell, y)) {
				return true;
			}
		}
	}
	return false;
}

void World::PlayerCheckGrounded() {
	bool ridingMTile = false;
	for (int i = 0; i < movingTiles->size(); i++) {
		MovingTile& mTile = movingTiles->at(i);
		if (player->cc.bottom == mTile.cc.top && player->cc.isXAligned(mTile.cc)) {
			ridingMTile = true;
			player->isGrounded = true;
			player->SetMountedTile(mTile);
			return;
		}
	}
	if (!ridingMTile) {
		player->ResetMountedTile();
	}
	if (player->cc.bottom % TILE_SIZE == 0) {
		int leftCell = GetCell(player->cc.left + 1);
		int rightCell = GetCell(player->cc.right - 1);
		int y = GetCell(player->cc.bottom);
		for (int x = leftCell; x <= rightCell; x++) {
			if (tilemap->isTileEnabled(x, y)) {
				player->isGrounded = true;
				return;
			}
		}
	}
	player->isGrounded = false;
}

inline int World::GetCell(int position) {
	return position < 0 ? (position / TILE_SIZE) - 1 : position / TILE_SIZE;
}

// *** ENEMY/BOSS FUNCTIONS ***

void World::EnterBossZone()
{
	// spawn boss
	bossEntry->Activate();
}

void World::EnterMinibossZone()
{
	// spawn miniboss
	mBossEntry->Activate();
}

void World::OnBossDefeat() {
	enemyProjectiles->clear();
	bossGate->Deactivate();
	bossEntry->Deactivate();
	// go to end cutscene???
}

void World::OnMinibossDefeat() {
	enemyProjectiles->clear();
	mBossGate->Deactivate();
	mBossEntry->Deactivate();
}

// *** PROJECTILE FUNCTIONS ***

void World::HandleProjectiles(float delta)
{
	// player
	int initialSize = playerProjectiles->size();
	for (int i = 0; i < playerProjectiles->size(); i++) {
		playerProjectiles->at(i).Update(delta);
		bool collided = false;
		if (enemies->boss.enabled) {
			if (playerProjectiles->at(i).cc.isColliding(enemies->boss.cc)) {
				enemies->boss.HP -= playerProjectiles->at(i).damage;
				playerProjectiles->at(i).cc.isActive = false;
				collided = true;
				continue;
			}
		}
		else if (enemies->mBoss.enabled) {
			if (playerProjectiles->at(i).cc.isColliding(enemies->mBoss.cc)) {
				enemies->mBoss.HP -= playerProjectiles->at(i).damage;
				playerProjectiles->at(i).cc.isActive = false;
				collided = true;
				continue;
			}
		}
		else {
			// check sentries
			std::vector<Sentry>& sentries = enemies->sentries;
			for (int j = 0; j < sentries.size(); j++) {
				if (playerProjectiles->at(i).cc.isColliding(sentries[j].cc)) {
					sentries[j].HP -= playerProjectiles->at(i).damage;
					playerProjectiles->at(i).cc.isActive = false;
					collided = true;
					break;
				}
			}

			// check floaters
			std::vector<Floater>& floaters = enemies->floaters;
			for (int j = 0; j < floaters.size() && !collided; j++) {
				if (playerProjectiles->at(i).cc.isColliding(floaters[j].cc)) {
					floaters[j].HP -= playerProjectiles->at(i).damage;
					playerProjectiles->at(i).cc.isActive = false;
					collided = true;
					break;
				}
			}
		}
		if (!collided) {
			// check for tile collision
			int cellX = GetCell(playerProjectiles->at(i).cc.x + playerProjectiles->at(i).cc.width / 2);
			int cellY = GetCell(playerProjectiles->at(i).cc.y + playerProjectiles->at(i).cc.height / 2);
			if (tilemap->isTileEnabled(cellX, cellY)) {
				playerProjectiles->at(i).cc.isActive = false;
				collided = true;
			}
		}
	}

	// enemy
	for (int i = 0; i < enemyProjectiles->size(); i++) {
		enemyProjectiles->at(i).Update(delta);
		if (enemyProjectiles->at(i).cc.isColliding(player->cc)) {
			player->TakeDamage(enemyProjectiles->at(i).damage);
			enemyProjectiles->at(i).cc.isActive = false;
		}
		else {
			int cellX = GetCell(enemyProjectiles->at(i).cc.x + enemyProjectiles->at(i).cc.width / 2);
			int cellY = GetCell(enemyProjectiles->at(i).cc.y + enemyProjectiles->at(i).cc.height / 2);
			if (tilemap->isTileEnabled(cellX, cellY)) {
				enemyProjectiles->at(i).cc.isActive = false;
			}
		}
	}
}