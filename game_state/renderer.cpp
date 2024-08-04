#include "renderer.h"
#include <rlgl.h>
#include <iostream>

Renderer::Renderer() {
	// get uniform location
	mapShader = LoadShader(0, "shaders/tilemap.frag");
	screenLoc = GetShaderLocation(mapShader, "screenSize");
	setLoc = GetShaderLocation(mapShader, "tileset");
	mapLoc = GetShaderLocation(mapShader, "tilemap");
	widthLoc = GetShaderLocation(mapShader, "width");
	heightLoc = GetShaderLocation(mapShader, "height");
	offsetLoc = GetShaderLocation(mapShader, "offset");
	
	// set const uniform values
	float screenSize[2]{ (float)GetScreenWidth(), (float)GetScreenHeight() };
	textures[TILESET] = LoadTexture("sprites/tileset.png");
	textures[BATTERY] = LoadTexture("sprites/battery.png");
	textures[PLUG] = LoadTexture("sprites/plug.png");
	textures[DASHING] = LoadTexture("sprites/dash.png");
	textures[LASER] = LoadTexture("sprites/hazard.png");
	textures[BULLET] = LoadTexture("sprites/bullet.png");
	textures[JETPACK] = LoadTexture("sprites/jetpack.png");
	textures[PLAYER] = LoadTexture("sprites/player.png");
	textures[COMPANION] = LoadTexture("sprites/companion.png");
	textures[SENTRY] = LoadTexture("sprites/sentry.png");
	textures[DROPPER] = LoadTexture("sprites/dropper.png");
	textures[FINAL_BOSS] = LoadTexture("sprites/fboss.png");
	SetShaderValue(mapShader, screenLoc, &screenSize, RL_SHADER_UNIFORM_VEC2);
}

Renderer::~Renderer()
{
	UnloadShader(mapShader);
	for (int i = 0; i < TOTAL_TEXTURES; i++) {
		UnloadTexture(textures[i]);
	}
}

void Renderer::Init(TileMap& tm, Camera2D& cam, Player& p, std::vector<MovingTile>& mt, std::vector<Projectile>& pr, std::vector<Projectile>& epr, EnemyList& foes)
{
	tilemap = &tm;
	camera = &cam;
	player = &p;
	enemies = &foes;
	movingTiles = &mt;
	playerProjectiles = &pr;
	enemyProjectiles = &epr;
}

void Renderer::SetMap(const char* newMapPath)
{
	Image mapImage = LoadImage(newMapPath);
	ImageFlipVertical(&mapImage);
	textures[TILEMAP] = LoadTextureFromImage(mapImage);
	mapWidth = mapImage.width * TILE_SIZE;
	mapHeight = mapImage.height * TILE_SIZE;
	SetShaderValue(mapShader, widthLoc, &mapWidth, RL_SHADER_UNIFORM_FLOAT);
	SetShaderValue(mapShader, heightLoc, &mapHeight, RL_SHADER_UNIFORM_FLOAT);
	UnloadImage(mapImage);
}

void Renderer::SetGatePointers(Gate& g, Gate& g2, Gate& g3, Gate& g4)
{
	bossEntry = &g;
	mBossEntry = &g2;
	bossGate = &g3;
	mBossGate = &g4;
}

void Renderer::SetCollectibles(std::vector<Collectible>& c)
{
	collectibles = &c;
}

void Renderer::Render() {
	ClearBackground(BLACK);
	// ClearBackground(Color{ 76, 255, 97, 255 });
	BeginMode2D(*camera);
	DrawTileMap();
	DrawGates();
	for (int i = 0; i < movingTiles->size(); i++) {
		movingTiles->at(i).Draw();
	}
	for (int i = 0; i < collectibles->size(); i++) {
		Collectible& c = collectibles->at(i);
		if (c.ct == RESPAWN) {
			DrawTextureEx(textures[PLUG], Vector2{ (float)c.cc.x - c.cc.width, (float)c.cc.y - c.cc.height - 10 }, 0.0f, 2.0f, WHITE);
		}
		else if (c.enabled) {
			switch (c.ct)
			{
			case DOUBLE_JUMP:
				DrawTextureEx(textures[JETPACK], Vector2{ (float)c.cc.x - c.cc.width, (float)c.cc.y - c.cc.height }, 0.0f, 2.0f, WHITE);
				break;

			case PUSH_BACK:
				DrawTextureEx(textures[BULLET], Vector2{ (float)c.cc.x - c.cc.width, (float)c.cc.y - c.cc.height }, 0.0f, 2.0f, WHITE);
				break;

			case DASH:
				DrawTextureEx(textures[DASHING], Vector2{ (float)c.cc.x - c.cc.width, (float)c.cc.y - c.cc.height }, 0.0f, 2.0f, WHITE);
				break;

			case HEALTH:
				DrawTextureEx(textures[BATTERY], Vector2{ (float)c.cc.x - c.cc.width, (float)c.cc.y - c.cc.height }, 0.0f, 2.0f, WHITE);
				break;

			case DANGER:
				DrawTextureEx(textures[LASER], Vector2{ (float)c.cc.x, (float)c.cc.y}, 0.0f, 0.5f, WHITE);
				break;
			}
		}
	}
	for (int i = 0; i < playerProjectiles->size(); i++) {
		playerProjectiles->at(i).Draw();
	}
	for (int i = 0; i < enemyProjectiles->size(); i++) {
		enemyProjectiles->at(i).Draw();
	}
	// *** ENEMIES START ***
	
	// sentries
	std::vector<Sentry>& sentries = enemies->sentries;
	for (int i = 0; i < sentries.size(); i++) {
		Sentry& s = sentries[i];
		if (s.cc.isActive) {
			float sourceWidth = 40;
			if (!s.facingRight) {
				sourceWidth = -40;
			}
			if (s.isHoming) {
				DrawTexturePro(textures[SENTRY], Rectangle{ 0, 0, sourceWidth, 40 }, Rectangle{ (float)s.cc.x - (s.cc.width / 2), (float)s.cc.y - (s.cc.height / 2), 60, 60 },
					Vector2{ 0, 0 }, 0.0f, WHITE);
				DrawTexturePro(textures[SENTRY], Rectangle{ 0, 0, sourceWidth, 40 }, Rectangle{ (float)s.cc.x - (s.cc.width / 2), (float)s.cc.y - (s.cc.height / 2), 60, 60 },
					Vector2{ 0, 0 }, 0.0f, Fade(RED, 0.7f));
			}
			else {
				DrawTexturePro(textures[SENTRY], Rectangle{ 0, 0, sourceWidth, 40 }, Rectangle{ (float)s.cc.x - (s.cc.width / 2), (float)s.cc.y - (s.cc.height / 2), 60, 60 },
					Vector2{ 0, 0 }, 0.0f, WHITE);
			}
		}
	}

	// floaters
	std::vector<Floater>& floaters = enemies->floaters;
	for (int i = 0; i < floaters.size(); i++) {
		Floater& f = floaters[i];
		if (f.cc.isActive) {
			if (f.canShoot) {
				DrawTextureEx(textures[COMPANION], Vector2{ (float)f.cc.x, (float)f.cc.y }, 0.0f, 3.0f, WHITE);
				DrawTextureEx(textures[COMPANION], Vector2{ (float)f.cc.x, (float)f.cc.y }, 0.0f, 3.0f, Fade(RED, 0.7f));
			}
			else {
				DrawTextureEx(textures[COMPANION], Vector2{ (float)f.cc.x, (float)f.cc.y }, 0.0f, 3.0f, WHITE);
			}
		}
	}
	// draw boss
	Boss& b = enemies->boss;
	if (b.enabled) {
		DrawTextureEx(textures[FINAL_BOSS], Vector2{ (float)b.cc.x, (float)b.cc.y }, 0.0f, 3.0f, Fade(WHITE, b.spawnTime));
	}

	// draw miniboss
	Miniboss& mb = enemies->mBoss;
	if (mb.enabled) {
		DrawTextureEx(textures[DROPPER], Vector2{ (float)mb.cc.x, (float)mb.cc.y }, 0.0f, 2.0f, Fade(WHITE, mb.spawnTime));
	}

	// *** ENEMIES END ***

	DrawPlayer();
	EndMode2D();

	// *** UI ***
	
	// HP BAR
	DrawRectangle(0, 0, 590, 40, Color{60, 40, 0, 255});
	DrawRectangle(5, 5, 580, 30, RAYWHITE);
	DrawText("HP", 10, 10, 24, DARKBLUE);
	DrawRectangle(50, 10, (player->MAX_HP + player->bonusHP) * 4, 20, ORANGE);
	DrawRectangle(50, 10, player->HP * 4, 20, BLUE);

	// Draw Weapon Icon
	DrawText("Shot Type", 10, GetScreenHeight() - 115, 15, WHITE);
	if (player->wType == WeaponType::REGULAR) {
		DrawCircle(50, GetScreenHeight() - 50, 50, Color{ 60, 40, 0, 255 });
		DrawCircle(50, GetScreenHeight() - 50, 45, RAYWHITE);
		DrawCircleGradient(50, GetScreenHeight() - 50, 15, WHITE, DARKBLUE);
	}
	else {
		DrawCircle(50, GetScreenHeight() - 50, 50, Color{ 60, 40, 0, 255 });
		DrawCircle(50, GetScreenHeight() - 50, 45, RAYWHITE);
		DrawCircleGradient(50, GetScreenHeight() - 50, 25, WHITE, DARKGREEN);

	}

	if (player->damagedRecently) {
		BeginBlendMode(BLEND_ALPHA);
		DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, 3 * GetScreenWidth() / 4, Fade(BLACK, 0.0), Fade(YELLOW, 0.5));
		DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, 3 * GetScreenWidth() / 4, Fade(BLACK, 0.0), Fade(BLUE, 0.3));
		EndBlendMode();
	}
	else {
		BeginBlendMode(BLEND_ALPHA);
		DrawCircleGradient(GetScreenWidth() / 2, GetScreenHeight() / 2, 3 * GetScreenWidth() / 4, Fade(BLACK, 0.0), Fade(BLACK, 0.8));
		EndBlendMode();
	}

	// Draw optional UI (interacting with save point or item pickup)
	if (text->enabled) {
		text->Draw();
	}
}

void Renderer::DrawTileMap()
{
	BeginShaderMode(mapShader);
	offset[0] = camera->target.x - camera->offset.x;
	offset[1] = -camera->target.y - camera->offset.y;
	SetShaderValue(mapShader, offsetLoc, &offset, RL_SHADER_UNIFORM_VEC2);
	SetShaderValueTexture(mapShader, setLoc, textures[TILESET]);
	SetShaderValueTexture(mapShader, mapLoc, textures[TILEMAP]);
	DrawRectangle(0, 0, mapWidth, mapHeight, WHITE);
	EndShaderMode();
}

void Renderer::DrawGates()
{
	if (bossEntry->enabled) {
		for (int i = 0; i < bossEntry->cells.size(); i++) {
			Cell c = bossEntry->cells[i];
			DrawRectangle(c.x * TILE_SIZE, c.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
		}
	}
	if (bossGate->enabled) {
		for (int i = 0; i < bossGate->cells.size(); i++) {
			Cell c = bossGate->cells[i];
			DrawRectangle(c.x * TILE_SIZE, c.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
		}
	}
	if (mBossEntry->enabled) {
		for (int i = 0; i < mBossEntry->cells.size(); i++) {
			Cell c = mBossEntry->cells[i];
			DrawRectangle(c.x * TILE_SIZE, c.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
		}
	}
	if (mBossGate->enabled) {
		for (int i = 0; i < mBossGate->cells.size(); i++) {
			Cell c = mBossGate->cells[i];
			DrawRectangle(c.x * TILE_SIZE, c.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
		}
	}
}

void Renderer::DrawPlayer()
{
	if (player->orbDirection) {
		// draw player
		Color tint = WHITE;
		if (!player->cc.isActive) {
			tint = Fade(BLUE, 0.4f);
		}
		BeginBlendMode(BLEND_ALPHA);
		DrawTexturePro(textures[PLAYER], player->source, Rectangle{ (float)(player->cc.x - 8), (float)(player->cc.y), 32 * 1.5, 48 * 1.5 }, Vector2{ 0, 0 }, 0.0f, tint);
		
		// draw orb
		if (player->wType == WeaponType::PUSHBACK) {
			DrawCircleGradient(player->cc.x + 20 + player->offsetX, player->cc.y + 36, 10, WHITE, DARKGREEN);
		}
		else {
			DrawCircleGradient(player->cc.x + 20 + player->offsetX, player->cc.y + 36, 10, WHITE, DARKBLUE);
		}
	}
	else {
		// draw orb
		if (player->wType == WeaponType::PUSHBACK) {
			DrawCircleGradient(player->cc.x + 20 + player->offsetX, player->cc.y + 36, 10, WHITE, DARKGREEN);
		}
		else {
			DrawCircleGradient(player->cc.x + 20 + player->offsetX, player->cc.y + 36, 10, WHITE, DARKBLUE);
		}
		// draw player
		Color tint = WHITE;
		if (!player->cc.isActive) {
			tint = Fade(BLUE, 0.4f);
		}
		BeginBlendMode(BLEND_ALPHA);
		DrawTexturePro(textures[PLAYER], player->source, Rectangle{ (float)(player->cc.x - 8), (float)(player->cc.y), 32 * 1.5, 48 * 1.5 }, Vector2{ 0, 0 }, 0.0f, tint);
	}
	EndBlendMode();
}

void Renderer::SetTextbox(TextBox& tb)
{
	text = &tb;
}

void Renderer::Load()
{
	mapShader = LoadShader(0, "shaders/tilemap.frag");
	// set const uniform values
	float screenSize[2]{ (float)GetScreenWidth(), (float)GetScreenHeight() };
	textures[TILESET] = LoadTexture("sprites/tileset.png");
	SetShaderValue(mapShader, screenLoc, &screenSize, RL_SHADER_UNIFORM_VEC2);
}

void Renderer::Unload()
{
	UnloadShader(mapShader);
	for (int i = 0; i < TOTAL_TEXTURES; i++) {
		UnloadTexture(textures[i]);
	}
}