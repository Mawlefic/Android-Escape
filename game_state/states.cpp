#include "states.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdio.h>
#include <iostream>
#include "../helper/utility.h"

// Update the following when new states are added: GSM Constructor, GameState enum, New State functions

// enemy colors
const Color SENTRY_COLOR = Color{ 255, 200, 200 };
const Color HOMING_SENTRY_COLOR = Color{ 255, 175, 175 };
const Color FLOATER_COLOR = Color{ 255, 150, 150 };
const Color SHOT_FLOATER_COLOR = Color{ 255, 160, 160 };
const Color BOSS_COLOR = Color{ 255, 206, 214 };
const Color MINIBOSS_COLOR = Color{ 255, 214, 206 };

// gates colors
const Color BOSS_GATE_ENTRY_COLOR = Color{ 255, 71, 31 };
const Color BOSS_GATE_COLOR = Color{ 255, 81, 41 };
const Color MINIBOSS_GATE_ENTRY_COLOR = Color{ 255, 81, 31 };
const Color MINIBOSS_GATE_COLOR = Color{ 255, 91, 41 };

// upgrade colors
const Color DASH_UPGRADE = Color{ 255, 50, 50 };
const Color PUSH_UPGRADE = Color{ 255, 75, 75 };
const Color DOUBLE_JUMP_UPGRADE = Color{ 255, 100, 100 };
const Color HEALTH_UPGRADE = Color{ 255, 125, 125 };

// other
const Color BENCH = Color{ 255, 140, 140 };
const Color HAZARD = Color{ 76, 255, 0 };

// *** GAME STATE MANAGER ***

GSM::GSM(){}

GSM::~GSM() {
	for (int i = 0; i < TOTAL_STATE; i++) {
		delete states[i];
	}
}

void GSM::Init(int initialState)
{
	currentState = initialState;
	states[SPLASH_STATE] = new SplashState(this);
	states[SANDBOX_STATE] = new SandboxState(this);
	states[END_STATE] = new EndState(this);
	// ... add other states here
	states[currentState]->Load();
}

void GSM::SwitchState(int newState) {
	// add state transition
	states[currentState]->Unload();
	currentState = newState;
	states[currentState]->Load();
	changingState = true;
}

void GSM::Update(float delta) {
	// Either changing state or updating current state
	if (changingState) {
		transitionValue += delta;
		if (transitionValue >= transitionTimer) {
			transitionValue = 0.0f;
			changingState = false;
			fadeValue = 1;
		}
		if (transitionValue >= transitionTimer / 2) {
			fadeValue = (transitionTimer - transitionValue) / transitionValue;
		}
	}
	else {
		states[currentState]->Update(delta);
	}
}

void GSM::Draw()
{
	// Either changing state or drawing current state
	if (changingState) {
		if (transitionValue >= transitionTimer / 2) {
			BeginBlendMode(BLEND_ALPHA);
			states[currentState]->Draw();
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeValue));
			EndBlendMode();
		}
		else {
			ClearBackground(BLACK);
		}
	}
	else {
		states[currentState]->Draw();
	}
}

void GSM::Load()
{
	states[currentState]->Load();
}

void GSM::Unload()
{
	states[currentState]->Unload();
}

// *** GAME STATE ***

GameState::GameState(GSM* gsm)
{
	this->gsm = gsm;
}

// *** SPLASH STATE *** 

SplashState::SplashState(GSM* gsm) : GameState(gsm)
{

}

void SplashState::Update(float delta)
{
	if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
		gsm->SwitchState(SANDBOX_STATE);
		return;
	}
}

void SplashState::Draw()
{
	ClearBackground(DARKGRAY);
	DrawText(title, GetScreenWidth() / 2 - (MeasureText(title, size) / 2), GetScreenHeight() / 3, size, BLACK);
	DrawText(prompt, GetScreenWidth() / 2 - (MeasureText(prompt, size) / 2), 2 * GetScreenHeight() / 3, size, BLACK);
}

void SplashState::Load()
{

}

void SplashState::Unload()
{

}

// *** END STATE ***

EndState::EndState(GSM* gsm) : GameState(gsm)
{

}

void EndState::Update(float delta)
{

}

void EndState::Draw()
{
	ClearBackground(BLACK);
	DrawText(prompt, GetScreenWidth() / 2 - (MeasureText(prompt, size) / 2), GetScreenHeight() / 2, size, WHITE);
}

void EndState::Load()
{

}

void EndState::Unload()
{

}

// *** SANDBOX STATE ***

SandboxState::SandboxState(GSM* gsm) : GameState(gsm)
{
	pX = 0;
	pY = 0;
}

// 

void SandboxState::Update(float delta)
{
	if (enemies.boss.defeated) {
		gsm->SwitchState(END_STATE);
		return;
	}
	world.Update(delta);
	if (enemies.mBoss.enabled) {
		camera.target = Vector2{ (float)(player.cc.x + player.cc.width / 2), (float)(player.cc.y + player.cc.height / 2 - 200) };
	}
	else if (enemies.boss.enabled) {
		camera.target = Vector2{ (float)(player.cc.x + player.cc.width / 2), (float)(player.cc.y + player.cc.height / 2 - 100) };
	}
	else {
		camera.target = Vector2{ (float)(player.cc.x + player.cc.width / 2), (float)(player.cc.y + player.cc.height / 2) };
	}
	// Camera Boundaries
	// horizontal
	if (camera.target.x - (GetScreenWidth() / 2) < 0.0f) {
		camera.target.x = GetScreenWidth() / 2;
	}
	else if (camera.target.x + (GetScreenWidth() / 2) > tileMap.xTiles * TILE_SIZE) {
		camera.target.x = (tileMap.xTiles * TILE_SIZE) - GetScreenWidth() / 2;
	}
	// vertical
	if (camera.target.y - (GetScreenHeight() / 2) < 0.0f) {
		camera.target.y = GetScreenHeight() / 2;
	}
	else if (camera.target.y + (GetScreenHeight() / 2) > tileMap.yTiles * TILE_SIZE) {
		camera.target.y = (tileMap.yTiles * TILE_SIZE) - GetScreenHeight() / 2;
	}
}

void SandboxState::Draw()
{
	renderer.Render();
}

void SandboxState::Load()
{
	// add tile map
	renderer.Load();
	const char* mapPath = "map_data/whole_map.png";
	renderer.SetMap(mapPath);
	tileMap.Init(mapPath, pX, pY);

	// init player variables (after tilemap init)
	player.Init(pX, pY);
	player.respawnX = pX;
	player.respawnY = pY;

	LoadEntityData(mapPath);

	// camera settings
	camera.target = Vector2{ (float)(player.cc.x + player.cc.width / 2), (float)(player.cc.y + player.cc.height / 2)};
	camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	// load renderer and world
	world.Init(tileMap, player, mTiles, playerProjectiles, enemyProjectiles, enemies);
	world.SetGatePointers(bossEntry, mBossEntry, bossGate, mBossGate);
	world.SetCollectibles(collectibles);
	world.SetTextbox(tb);
	renderer.Init(tileMap, camera, player, mTiles, playerProjectiles, enemyProjectiles, enemies);
	renderer.SetGatePointers(bossEntry, mBossEntry, bossGate, mBossGate);
	renderer.SetCollectibles(collectibles);
	renderer.SetTextbox(tb);
}

void SandboxState::Unload()
{
	playerProjectiles.clear();
	enemyProjectiles.clear();
	mTiles.clear();
	renderer.Unload();
	tileMap.Reset();
}

// read from file later
void SandboxState::AddMovingPlatform(int x, int y, int valX, int valY, float time, float waitTime, bool isOffset)
{
	if (isOffset) {
		MovingTile mt{ x, y };
		mt.SetOffsetParameters(valX, valY, time, waitTime);
		mTiles.push_back(mt);
	}
	else {
		MovingTile mt{ x, y };
		mt.SetMoveParameters(valX, valY, time, waitTime);
		mTiles.push_back(mt);
	}
}

void SandboxState::LoadEntityData(const char* mapPath)
{
	Image mapImage = LoadImage(mapPath);
	// get map data
	for (int h = 0; h < mapImage.height; h++) {
		for (int w = 0; w < mapImage.width; w++) {
			Color iColor = GetImageColor(mapImage, w, h);
			if (ColorEqual(iColor, SENTRY_COLOR)) {
				Sentry sentry{ 500 };
				sentry.SetPosition(w * TILE_SIZE, (h - 1) * TILE_SIZE);
				sentry.SetTarget(player);
				enemies.addSentry(sentry);
			}
			else if (ColorEqual(iColor, HOMING_SENTRY_COLOR)) {
				Sentry sentry{ 1000 };
				sentry.SetPosition(w * TILE_SIZE, (h - 1) * TILE_SIZE);
				sentry.MAX_HP += 30;
				sentry.HP = sentry.MAX_HP;
				sentry.SetTarget(player);
				sentry.isHoming = true;
				enemies.addSentry(sentry);
			}
			else if (ColorEqual(iColor, FLOATER_COLOR)) {
				Floater floater{};
				floater.SetPosition(w * TILE_SIZE, (h - 1) * TILE_SIZE);
				floater.SetTarget(player);
				enemies.addFloater(floater);
			}
			else if (ColorEqual(iColor, SHOT_FLOATER_COLOR)) {
				Floater floater{};
				floater.canShoot = true;
				floater.SetPosition(w * TILE_SIZE, (h - 1) * TILE_SIZE);
				floater.SetTarget(player);
				enemies.addFloater(floater);
			}
			else if (ColorEqual(iColor, MINIBOSS_COLOR)) {
				enemies.mBoss.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				enemies.mBoss.enabled = false;
			}
			else if (ColorEqual(iColor, BOSS_COLOR)) {
				enemies.boss.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				enemies.boss.enabled = false;
			}
			else if (ColorEqual(iColor, BOSS_GATE_ENTRY_COLOR)) {
				bossEntry.cells.push_back(Cell{ w, h });
			}
			else if (ColorEqual(iColor, BOSS_GATE_COLOR)) {
				bossGate.cells.push_back(Cell{ w, h });
			}
			else if (ColorEqual(iColor, MINIBOSS_GATE_ENTRY_COLOR)) {
				mBossEntry.cells.push_back(Cell{ w, h });
			}
			else if (ColorEqual(iColor, MINIBOSS_GATE_COLOR)) {
				mBossGate.cells.push_back(Cell{ w, h });
			}
			else if (ColorEqual(iColor, DASH_UPGRADE)) {
				Collectible c{ DASH };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
			else if (ColorEqual(iColor, DOUBLE_JUMP_UPGRADE)) {
				Collectible c{ DOUBLE_JUMP };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
			else if (ColorEqual(iColor, PUSH_UPGRADE)) {
				Collectible c{ PUSH_BACK };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
			else if (ColorEqual(iColor, HEALTH_UPGRADE)) {
				Collectible c{ HEALTH };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
			else if (ColorEqual(iColor, BENCH)) {
				Collectible c{ RESPAWN };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
			else if (ColorEqual(iColor, HAZARD)) {
				Collectible c{ DANGER };
				c.SetPosition(w * TILE_SIZE, h * TILE_SIZE);
				collectibles.push_back(c);
			}
		}
	}
	bossGate.Activate();
	mBossGate.Activate();

	UnloadImage(mapImage);
}