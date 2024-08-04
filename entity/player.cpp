#include "player.h"
#include "../helper/utility.h"

Player::Player()
{
	Init(0, 0);
	respawnX = 0;
	respawnY = 0;
}

Player::Player(int x, int y) : Player()
{
	cc.x = x;
	cc.y = y;
	cc.Update();
	respawnX = x;
	respawnY = y;
}

void Player::TimerUpdate(float delta) {
	if (canBufferJump) {
		jumpBufferCount++;
		if (jumpBufferCount >= jumpBufferFrames) {
			jumpBufferCount = 0;
			canBufferJump = false;
		}
	}
	if (coyoteTimerOn) {
		coyoteCount++;
		if (coyoteCount >= coyoteFrames) {
			coyoteCount = 0;
			coyoteTimerOn = false;
		}
	}
	if (!canShoot) {
		shotCounter += delta;
		if (shotCounter >= shotCD) {
			canShoot = true;
			shotCounter = 0.0f;
		}
	}
	if (!canShootP) {
		shotPCounter += delta;
		if (shotPCounter >= shotPCD) {
			canShootP = true;
			shotPCounter = 0.0f;
		}
	}
	if (!canDash) {
		dashCDCounter += delta;
		if (dashCDCounter >= dashCD) {
			dashCDCounter = 0.0f;
			canDash = true;
		}
		if (playerState == PlayerState::DASH) {
			dashCounter += delta;
			if (dashCounter >= dashTime) {
				dashCounter = 0.0f;
				playerState = PlayerState::FALL;
				vx = 0;
				vy = 0;
				cc.isActive = true;
			}
		}
	}
	if (isInvincible) {
		invincibleCounter += delta;
		if (invincibleCounter >= invincibleTime) {
			isInvincible = false;
			invincibleCounter = 0.0f;
			cc.isActive = true;
		}
	}
	if (damagedRecently) {
		dmgCounter += delta;
		if (dmgCounter >= dmgCD) {
			dmgCounter = 0.0f;
			damagedRecently = false;
		}
	}
}

void Player::UpdateAnimation()
{
	switch (playerState)
	{
	case PlayerState::STAND:
		if (vx + projVX != 0) {
			if (abs(vx) > abs(projVX)) {
				source = Rectangle{ 32.0f * frameNumber, 144, 32, 48 };
				if (!facingRight) {
					source.width = -32;
				}
			}
			else {
				source = Rectangle{ 0, 192, 32, 48 };
				if (!facingRight) {
					source.width = -32;
				}
			}
		}
		else {
			source = Rectangle{ 0, 0, 32, 48 };
			if (!facingRight) {
				source.width = -32;
			}
		}
		break;

	case PlayerState::JUMP:
		source = Rectangle{ 0, 96, 32, 48};
		if (!facingRight) {
			source.width = -32;
		}
		break;

	case PlayerState::FALL:
		source = Rectangle{ 0, 48, 32, 48};
		if (!facingRight) {
			source.width = -32;
		}
		break;

	case PlayerState::DASH:
		source = Rectangle{ 32, 144, 32, 48 };
		if (!facingRight) {
			source.width = -32;
		}
		break;

	}
}

void Player::Update(float delta) {
	attackActive = false;
	hasBurnTicked = false;
	if (orbDirection) {
		offsetX += ORB_SPEED * delta;
		if (offsetX > MAX_ORB_DIST) {
			offsetX = MAX_ORB_DIST;
			orbDirection = false;
		}
	}
	else {
		offsetX -= ORB_SPEED * delta;
		if (offsetX < -MAX_ORB_DIST) {
			offsetX = -MAX_ORB_DIST;
			orbDirection = true;
		}
	}
	switch (playerState)
	{
	case PlayerState::STAND:
		if (vx + projVX != 0) {
			if (abs(vx) > abs(projVX)) {
				frameCounter += delta;
				if (frameCounter >= animInterval) {
					frameCounter = 0.0f;
					frameNumber++;
				}
				if (frameNumber > 3) {
					frameNumber = 0;
				}
			}
			else {
				frameCounter = 0;
			}
		}
		else {
			frameNumber = 0;
		}
		if (canBufferJump) {
			jumpsRemaining = MAX_JUMPS;
			Jump();
		}
		else if (!isGrounded) {
			playerState = PlayerState::FALL;
			jumpsRemaining--;
			coyoteTimerOn = true;
			coyoteCount = 0;
		}
		else {
			jumpsRemaining = MAX_JUMPS;
			pushBackCharges = MAX_CHARGES;
		}
		break;

	case PlayerState::JUMP:
		frameNumber = 0;
		ResistPlatformMomentum(delta);
		if (isGrounded) {
			ProcessGroundCollision();
		}
		else if (vy >= 0) {
			scale /= 2;
			playerState = PlayerState::FALL;
		}
		else{
			ApplyGravity(delta);
		}
		break;

	case PlayerState::FALL:
		frameNumber = 0;
		ResistPlatformMomentum(delta);
		if (isGrounded) {
			ProcessGroundCollision();
		}
		else if (canCoyoteJump) {
			Jump();
			jumpsRemaining++;
		}
		else {
			ApplyFallingGravity(delta);
		}
		break;
	
	case PlayerState::DASH:
		frameNumber = 0;
		// do something???
		break;
	}
	UpdateAnimation();
	HandleProjectileMomentum(delta);
	TimerUpdate(delta);
}

void Player::Draw(){
	if (!cc.isActive) {
		BeginBlendMode(BLEND_ALPHA);
		cc.Draw(Color{0, 121, 241, 100});
		EndBlendMode();
	}
	else {
		cc.Draw(BLUE);
	}
}

void Player::Init(int x, int y)
{
	// reset position
	cc = CollisionComponent(x, y, WIDTH, HEIGHT);
	cc.isActive = true;
	cc.Update();

	// set timer/buffer attributes
	canCoyoteJump = false;
	canBufferJump = false;
	coyoteTimerOn = false;
	jumpBufferCount = 0;
	coyoteCount = 0;

	// cooldown
	canShoot = true;
	canShootP = true;
	canDash = true;
	isInvincible = false;
	hasBurnTicked = false;
	shotCounter = 0.0f;
	shotPCounter = 0.0f;
	dashCounter = 0.0f;
	dashCDCounter = 0.0f;
	invincibleCounter = 0.0f;
	burnCounter = 0.0f;

	// set velocity/acceleration
	jumpsRemaining = 0;
	pushBackCharges = MAX_CHARGES;
	xRemainder = 0.0f;
	yRemainder = 0.0f;
	decel = 0.0f;
	scale = initialScale;
	platVelX = 0;
	vx = 0;
	vy = 0;
	projVX = 0;
	projVY = 0;
	dmgCounter = 0.0f;

	// bool attributes
	isGrounded = false;
	onMovingTile = false;
	facingRight = true;
	holdingDown = false;
	holdingUp = false;
	attackActive = false;
	damagedRecently = false;

	// other attributes
	playerState = PlayerState::FALL;
	wType = WeaponType::REGULAR;
	mountedTile = nullptr;
	HP = MAX_HP;
	bonusHP = 0;
	source = Rectangle{ 0, 0, 32, 48 };
	frameNumber = 0;
	frameCounter = 0.0f;
	offsetX = 0;

	// unlocks
	unlockedDash = false;
	unlockedPushBack = false;
	MAX_JUMPS = 1;
}

void Player::Jump() {
	if (canCoyoteJump || jumpsRemaining > 0) {
		// update state and variables
		playerState = PlayerState::JUMP;
		jumpsRemaining--;
		scale = initialScale;
		projVY = 0;
		if (!halfJump) {
			vy = JUMP_VEL;
		}
		else {
			vy = 3 * JUMP_VEL / 4;
		}
		halfJump = true;
		// reset buffer values
		canBufferJump = false;
		canCoyoteJump = false;
		jumpBufferCount = 0;
		coyoteCount = 0;
		if (onMovingTile && vx != 0 && SameSign((int)vx, mountedTile->moveX)) {
			platVelX = mountedTile->moveX * 60;
		}
	}
}

void Player::Dash()
{
	if (canDash) {
		cc.isActive = false;
		vy = 0;
		projVY = 0;
		projVX = 0;
		scale = initialScale;
		canDash = false;
		playerState = PlayerState::DASH;
		if (facingRight) {
			vx = DASH_SPEED;
		}
		else {
			vx = -DASH_SPEED;
		}
	}
}

void Player::MeleeAttack()
{
	SetAttackBoxDir();
	attackActive = true;
}

void Player::WalkLeft()
{
	facingRight = false;
	vx = -MOVE_SPEED;
}

void Player::WalkRight()
{
	facingRight = true;
	vx = MOVE_SPEED;
}

void Player::PushBack()
{
	pushBackCharges--;
	canShootP = false;
	if (holdingUp) {
		scale = initialScale;
		vy = 0;
		projVY = DASH_SPEED;
	}
	else if (holdingDown) {
		scale = initialScale;
		vy = 0;
		projVY = -DASH_SPEED;
	}
	else if (facingRight) {
		projVX = -DASH_SPEED;
	}
	else {
		projVX = DASH_SPEED;
	}
}

void Player::Push(float angle)
{
	vx = 0;
	vy = 0;
	scale = initialScale;
	projVX = cos(angle) * DASH_SPEED;
	projVY = sin(angle) * DASH_SPEED;
}

void Player::TakeDamage(int damage)
{
	HP -= damage;
	isInvincible = true;
	invincibleCounter = 0.0f;
	cc.isActive = false;
	damagedRecently = true;
	dmgCounter = 0.0f;
}

void Player::HandleProjectileMomentum(float delta)
{
	if (projVY != 0.0f) {
		MoveTowards(projVY, 0.0f, (DASH_SPEED / DECEL_TIME) * delta);
	}
	if (projVX != 0.0f) {
		if (isGrounded) {
			MoveTowards(projVX, 0.0f, (2 * DASH_SPEED / DECEL_TIME) * delta);
		}
		else {
			MoveTowards(projVX, 0.0f, (DASH_SPEED / DECEL_TIME) * delta);
		}
	}
}

void Player::StartJBufferTimer()
{
	canBufferJump = true;
	jumpBufferCount = 0;
}

void Player::FastFall()
{
	scale = MAX_SCALE;
}

void Player::ProcessFallingJumpInput()
{
	if (coyoteTimerOn) {
		canCoyoteJump = true;
		coyoteTimerOn = false;
		coyoteCount = 0;
	}
	else {
		canBufferJump = true;
		jumpBufferCount = 0;
	}
}

void Player::ResistPlatformMomentum(float delta)
{
	if (platVelX != 0 && !SameSign(platVelX, vx)) {
		MoveTowards(platVelX, 0, (MOVE_SPEED * 2) * delta);
	}
}

void Player::HandlePlatformMomentum(float delta) {
	if (platVelX != 0) {
		MoveTowards(platVelX, 0, decel * delta);
	}
}

void Player::ApplyGravity(float delta)
{
	vy += (GRAVITY * scale) * delta;
	scale += (scaleFactor * delta);
	if (scale > MAX_SCALE) {
		scale = MAX_SCALE;
	}
}

void Player::ApplyFallingGravity(float delta)
{
	vy += (GRAVITY * scale) * delta;
	scale += (scaleFactor * delta);
	if (scale > MAX_FALL_SCALE) {
		scale = MAX_FALL_SCALE;
	}

}

void Player::ProcessGroundCollision() {
	playerState = PlayerState::STAND;
	scale = initialScale;
	platVelX = 0;
	vy = 0;
	projVY = 0;
	halfJump = false;
}

void Player::Respawn()
{
	// reset position
	cc = CollisionComponent(respawnX, respawnY - (3 * WIDTH / 4), WIDTH, HEIGHT);
	cc.isActive = true;
	cc.Update();

	// set timer/buffer attributes
	canCoyoteJump = false;
	canBufferJump = false;
	coyoteTimerOn = false;
	jumpBufferCount = 0;
	coyoteCount = 0;

	// cooldown
	canShoot = true;
	canShootP = true;
	canDash = true;
	isInvincible = false;
	shotCounter = 0.0f;
	shotPCounter = 0.0f;
	dashCounter = 0.0f;
	dashCDCounter = 0.0f;
	invincibleCounter = 0.0f;


	// set velocity/acceleration
	jumpsRemaining = 0;
	pushBackCharges = MAX_CHARGES;
	xRemainder = 0.0f;
	yRemainder = 0.0f;
	decel = 0.0f;
	scale = initialScale;
	platVelX = 0;
	vx = 0;
	vy = 0;
	projVX = 0;
	projVY = 0;

	// bool attributes
	isGrounded = false;
	onMovingTile = false;
	facingRight = true;
	holdingDown = false;
	holdingUp = false;
	attackActive = false;
	damagedRecently = false;

	// other attributes
	playerState = PlayerState::FALL;
	mountedTile = nullptr;
	HP = MAX_HP;
	source = Rectangle{ 0, 0, 32, 48 };
}

void Player::SetSpawnPoint(int x, int y)
{
	respawnX = x;
	respawnY = y;
}

void Player::SetMountedTile(MovingTile& mt)
{
	onMovingTile = true;
	mountedTile = &mt;
}

void Player::SetAttackBoxDir()
{
	int lanceLength = cc.width * 3;
	int lanceThickness = cc.width;
	if (holdingUp) {
		attackBox = CollisionComponent(cc.left, cc.top - lanceLength, lanceThickness, lanceLength);
		attackBox.Update();
	}
	else if (holdingDown) {
		attackBox = CollisionComponent(cc.left, cc.bottom, lanceThickness, lanceLength);
		attackBox.Update();
	}
	else if (facingRight) {
		attackBox = CollisionComponent(cc.right, cc.top + (cc.height / 2) - (lanceThickness / 2), lanceLength, lanceThickness);
		attackBox.Update();
	}
	else {
		attackBox = CollisionComponent(cc.left - lanceLength, cc.top + (cc.height / 2) - (lanceThickness / 2), lanceLength, lanceThickness);
		attackBox.Update();
	}
}

void Player::ResetMountedTile()
{
	onMovingTile = false;
	mountedTile = nullptr;
}

Vector2 Player::GetBulletSpeed()
{
	if (holdingUp) {
		return Vector2{ 0, -BULLET_SPEED };
	}
	else if (holdingDown) {
		return Vector2{ 0, BULLET_SPEED };
	}
	else if (facingRight) {
		return Vector2{ BULLET_SPEED, 0 };
	}
	return Vector2{ -BULLET_SPEED, 0 };
}

void Player::IncreaseBonusHP()
{
	bonusHP += 10;
	HP += 10;
}

void Player::UnlockDash()
{
	unlockedDash = true;
}

void Player::UnlockPushback() {
	unlockedPushBack = true;
}

void Player::UnlockDoubleJump()
{
	MAX_JUMPS = 2;
}
