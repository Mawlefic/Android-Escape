#pragma once
#include "collision_object.h"
#include "tile.h"
#include "entity.h"


enum class PlayerState {
	STAND, JUMP, FALL, DASH
};

enum class WeaponType {
	REGULAR, PUSHBACK
};

class Player : public Entity {
private:
	// const attributes
	const int WIDTH = 32;
	const int HEIGHT = 72;
	const int PLAT_CONSTANT = 2;
	const float JUMP_VEL = -850.0f;
	const float MOVE_SPEED = 350.0f;
	const float DASH_SPEED = 900.0f;
	const float BULLET_SPEED = 1000.0f;
	const float GRAVITY = 240.0f;
	const float initialScale = 3.0f;
	const float scaleFactor = 16.0f;
	const float MAX_SCALE = 32.0f;
	const float MAX_FALL_SCALE = MAX_SCALE * 2.0f;
	const float DECEL_TIME = 1.5f;
	int MAX_JUMPS = 1;

	// timer/buffer attributes
	const int jumpBufferFrames = 7;
	const int coyoteFrames = 6;
	bool canCoyoteJump, canBufferJump, coyoteTimerOn;
	int jumpBufferCount, coyoteCount;

	// other attributes
	// Texture2D texture
	MovingTile* mountedTile;

	// functions
	void ProcessGroundCollision();
	void TimerUpdate(float delta);

public:
	// const variables
	const float MAX_FALL_SPEED = 1000;
	int MAX_CHARGES = 3;
	const int MAX_HP = 100;
	const float animInterval = 0.10f;
	const float MAX_ORB_DIST = 45.0f;
	const float ORB_SPEED = 150.0f;

	// variables
	int HP = MAX_HP;
	int bonusHP = 0;
	int respawnX, respawnY;
	float xRemainder, yRemainder, decel, scale, platVelX, vx, vy, projVX, projVY, dmgCounter;
	bool isGrounded, halfJump, onMovingTile, holdingUp, holdingDown, facingRight, attackActive, damagedRecently;
	int jumpsRemaining = 0;
	int pushBackCharges = MAX_CHARGES;
	Rectangle source;
	CollisionComponent cc;
	CollisionComponent attackBox;
	PlayerState playerState;
	WeaponType wType;
	float frameCounter = 0.0f;
	int frameNumber = 0;
	bool orbDirection = false;
	float offsetX = 0;

	// cooldown
	const float dmgCD = 1.0f;
	const float shotCD = 0.3f;
	const float shotPCD = 0.05f;
	const float dashCD = 0.75f;
	const float invincibleTime = 1.0f;
	const float dashTime = 0.2f;
	const float burnTick = 0.5f;
	bool canShoot, canShootP, canDash, isInvincible, hasBurnTicked;
	float shotCounter, shotPCounter, dashCounter, dashCDCounter, invincibleCounter, burnCounter;

	// ability unlock
	bool dealsDashDamage;
	bool unlockedDash;
	bool unlockedPushBack;

	// functions
	Player();
	Player(int x, int y);
	void Update(float delta) override;
	void UpdateAnimation();
	void HandlePlatformMomentum(float delta);
	void ApplyGravity(float delta);
	void ApplyFallingGravity(float delta);
	void Draw() override;
	void Init(int x, int y);
	void Jump();
	void Dash();
	void MeleeAttack();
	void WalkLeft();
	void WalkRight();
	void PushBack();
	void Push(float angle);
	void TakeDamage(int damage);
	void HandleProjectileMomentum(float delta);
	void StartJBufferTimer();
	void FastFall();
	void ProcessFallingJumpInput();
	void ResistPlatformMomentum(float delta);
	void Respawn();
	void SetSpawnPoint(int x, int y);
	void SetMountedTile(MovingTile& mt);
	void SetAttackBoxDir();
	void ResetMountedTile();
	Vector2 GetBulletSpeed();

	// upgrade functions
	void IncreaseBonusHP();
	void UnlockDash();
	void UnlockPushback();
	void UnlockDoubleJump();
};