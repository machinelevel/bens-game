
/************************************************************\
	Bubble.cpp
	Files for controlling the Bubbles in Ben's project
\************************************************************/

#include <stdio.h>
// #include <glut.h>
#include <glut.h>
#include "genincludes.h"
#include "umath.h"
#include "upmath.h"
#include "camera.h"
#include "controls.h"
#include "timer.h"
#include "random.h"
#include "cellgrid.h"
#include "glider.h"
#include "projectile.h"
#include "texture.h"
#include "glowSpark.h"
#include "hud.h"
#include "bubble.h"
#include "cellport.h"
#include "sound.h"
#include "BOSS.h"

Bubble	*Bubble::msFirst = NULL;
float	gShieldFlickerTimer = 0.0f;

static int32	sBubbleTypeTextures[BUBBLE_TYPE_HOWMANY] = {
	-1,
	TEXID_ICON_HEALTH,
	TEXID_ICON_AMMO,
	TEXID_ICON_ATTITUDE,

	TEXID_WEAPON_BLASTER,
	TEXID_WEAPON_MISSILE,
	TEXID_WEAPON_SLINGSHOT,
	TEXID_WEAPON_SWORD,
	TEXID_WEAPON_MISSILE,
	TEXID_WEAPON_BALLISTIC,
	TEXID_WEAPON_CROSSBOW,
	TEXID_WEAPON_PARTICLE_ACCELERATOR,

	TEXID_SHIELD_POX,
	TEXID_SHIELD_COLDS,
	TEXID_SHIELD_RASH,
	TEXID_SHIELD_BARF,
	TEXID_SHIELD_FEVER,
	TEXID_SHIELD_HAIR,
	TEXID_SHIELD_FOOT,
};

Bubble::Bubble(int32 type)
{
	int32	i;

	/**** add to beginning of list ****/
	mPrev = NULL;
	mNext = msFirst;
	if (mNext) mNext->mPrev = this;
	msFirst = this;

	mType = type;
	mFlags = 0;
	mSize = 4.0f;
	mBubbleTexture = TEXID_FULLBUBBLE;
	mPrizeTexture = 0;
	pfSetVec3(mPosition, 0, 0, 0);
	pfSetVec3(mVelocity, 0, 0, 0);
	mLifeTimer = 0.0f;
	mBobTimer = RANDOM_IN_RANGE(0.0f, 1.0f);
	mSpinTimer = RANDOM_IN_RANGE(0.0f, 1.0f);
	mSpawnDrop = RANDOM_IN_RANGE(50.0f, 150.0f);
}

Bubble::~Bubble()
{
	/**** remove from list ****/
	if (msFirst == this) msFirst = mNext;
	if (mPrev) mPrev->mNext = mNext;
	if (mNext) mNext->mPrev = mPrev;
}

void Bubble::Think(void)
{
	if (!(mFlags & BUBBLE_FLAG_ACTIVE)) return;
}

void Bubble::Move(void)
{
	float	floor = gCells->mCenter[PF_Z];
	int32	i;
	pfVec3	force = {0,0,0};
	float	sval, cval;

	if (!(mFlags & BUBBLE_FLAG_ACTIVE)) return;

	pfAddScaledVec3(mPosition, mPosition, DeltaTime, mVelocity);

	/**** Hi I'm Bob ****/
	mBobTimer += 0.2f * DeltaTime;
	if (mBobTimer > 1.0f) mBobTimer -= 1.0f;
	pfSinCos(mBobTimer * 360.0f, &sval, &cval);
	mPosition[PF_Z] = (0.5f * mSize) + (0.25f * mSize * sval);

	mSpawnDrop -= 5.0f * DeltaTime;
	if (mSpawnDrop < 0.0f) mSpawnDrop = 0.0f;
	mPosition[PF_Z] += mSpawnDrop;

	/**** Hi Bob, I'm Spin ****/
	mSpinTimer += 1.0f * DeltaTime;
	if (mSpinTimer > 16.0f) mSpinTimer -= 16.0f;
}

void Bubble::React(void)
{
	float	xmin, xmax, ymin, ymax;
	float	bounce = 1.0f;

	/**** bounary check ****/
	gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);
	
	if (mPosition[PF_X] < xmin) {
		mPosition[PF_X] = xmin;
		if (mVelocity[PF_X] < 0.0f) {
			mVelocity[PF_X] *= -bounce;
		}
	}
	if (mPosition[PF_X] > xmax) {
		mPosition[PF_X] = xmax;
		if (mVelocity[PF_X] > 0.0f) {
			mVelocity[PF_X] *= -bounce;
		}
	}
	if (mPosition[PF_Y] < ymin) {
		mPosition[PF_Y] = ymin;
		if (mVelocity[PF_Y] < 0.0f) {
			mVelocity[PF_Y] *= -bounce;
		}
	}
	if (mPosition[PF_Y] > ymax) {
		mPosition[PF_Y] = ymax;
		if (mVelocity[PF_Y] > 0.0f) {
			mVelocity[PF_Y] *= -bounce;
		}
	}
}

void Bubble::Draw(void)
{
	int32		i;
	pfMatrix	mat;
	pfVec3		up, over, camPos, toCam, camUp;
	pfVec3		verts[4];
	float		scale, spinSval, spinCval;
	bool		sideways = false;
	bool		doubleBubble = false;
	bool		shield = false;
	float		spinSpeed = 360.0f;

	if (!(mFlags & BUBBLE_FLAG_ACTIVE)) return;

	switch (mType) {
	case BUBBLE_TYPE_SHIELD_POX:
	case BUBBLE_TYPE_SHIELD_COLDS:
	case BUBBLE_TYPE_SHIELD_RASH:
	case BUBBLE_TYPE_SHIELD_BARF:
	case BUBBLE_TYPE_SHIELD_FEVER:
	case BUBBLE_TYPE_SHIELD_HAIR:
	case BUBBLE_TYPE_SHIELD_BANDAID:
		shield = true;
		sideways = true;
		if (gCells->mTotalBadCells > 0) doubleBubble = true;
		spinSpeed = 45.0f;
		gShieldFlickerTimer -= DeltaTime;
		if (gShieldFlickerTimer < 0.0f) gShieldFlickerTimer = 0.0f;
		break;
	case BUBBLE_TYPE_ATTITUDE:
	case BUBBLE_TYPE_HEALTH:
	case BUBBLE_TYPE_AMMO:
		sideways = true;
		break;
	default:
		break;
	}


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(true);

	UseLibTexture(mBubbleTexture);
	
	pfCopyVec3(camPos, GlobalCameraMatrix[PF_T]);

	pfSinCos(mSpinTimer * spinSpeed, &spinSval, &spinCval);

	/**** Now construct our up and over vectors ****/
	pfSubVec3(toCam, camPos, mPosition);
	pfCopyVec3(camUp, GlobalCameraMatrix[PF_Z]);
//	pfAddScaledVec3(camUp, camUp, RANDOM_IN_RANGE(-1.0f, 1.0f), GlobalCameraMatrix[PF_X]);
	pfCrossVec3(over, toCam, camUp);
	pfCrossVec3(up, over, toCam);
	pfNormalizeVec3(up);
	pfNormalizeVec3(over);
	pfNormalizeVec3(toCam);

	if (doubleBubble) {
		scale = 0.4f * mSize;
		pfAddScaledVec3(verts[0], mPosition,  scale, up);
		pfAddScaledVec3(verts[1], mPosition,  scale, up);
		pfAddScaledVec3(verts[2], mPosition, -scale, up);
		pfAddScaledVec3(verts[3], mPosition, -scale, up);
		pfAddScaledVec3(verts[0], verts[0],  scale, over);
		pfAddScaledVec3(verts[1], verts[1], -scale, over);
		pfAddScaledVec3(verts[2], verts[2], -scale, over);
		pfAddScaledVec3(verts[3], verts[3],  scale, over);
		glBegin(GL_QUADS);
		glColor4f(1.0f - (gShieldFlickerTimer*RANDOM0TO1), 1.0f - (gShieldFlickerTimer*RANDOM0TO1), 1.0f - (gShieldFlickerTimer*RANDOM0TO1), 0.25f);
		glTexCoord2f(0, 0);
		glVertex3fv(verts[0]);
		glTexCoord2f(0, 1);
		glVertex3fv(verts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(verts[2]);
		glTexCoord2f(1, 0);
		glVertex3fv(verts[3]);
		glEnd();
	}

	scale = 0.5f * mSize;
	pfAddScaledVec3(verts[0], mPosition,  scale, up);
	pfAddScaledVec3(verts[1], mPosition,  scale, up);
	pfAddScaledVec3(verts[2], mPosition, -scale, up);
	pfAddScaledVec3(verts[3], mPosition, -scale, up);
	pfAddScaledVec3(verts[0], verts[0],  scale, over);
	pfAddScaledVec3(verts[1], verts[1], -scale, over);
	pfAddScaledVec3(verts[2], verts[2], -scale, over);
	pfAddScaledVec3(verts[3], verts[3],  scale, over);
	if (gWinTimer > 0.0f) {
	} else {
		glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
		glTexCoord2f(0, 0);
		glVertex3fv(verts[0]);
		glTexCoord2f(0, 1);
		glVertex3fv(verts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(verts[2]);
		glTexCoord2f(1, 0);
		glVertex3fv(verts[3]);
		glEnd();
	}

	if (sBubbleTypeTextures[mType] >= 0) {
		UseLibTexture(sBubbleTypeTextures[mType]);
		scale *= 0.6f;
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glBegin(GL_QUADS);
		if (sideways) {
			glTexCoord2f(0, 1);
			glVertex3f(mPosition[PF_X] - (scale*spinSval), mPosition[PF_Y] - (scale*spinCval), mPosition[PF_Z] + scale);
			glTexCoord2f(1, 1);
			glVertex3f(mPosition[PF_X] + (scale*spinSval), mPosition[PF_Y] + (scale*spinCval), mPosition[PF_Z] + scale);
			glTexCoord2f(1, 0);
			glVertex3f(mPosition[PF_X] + (scale*spinSval), mPosition[PF_Y] + (scale*spinCval), mPosition[PF_Z] - scale);
			glTexCoord2f(0, 0);
			glVertex3f(mPosition[PF_X] - (scale*spinSval), mPosition[PF_Y] - (scale*spinCval), mPosition[PF_Z] - scale);
		} else {
			glTexCoord2f(1, 1);
			glVertex3f(mPosition[PF_X] - (scale*spinSval), mPosition[PF_Y] - (scale*spinCval), mPosition[PF_Z] + scale);
			glTexCoord2f(1, 0);
			glVertex3f(mPosition[PF_X] + (scale*spinSval), mPosition[PF_Y] + (scale*spinCval), mPosition[PF_Z] + scale);
			glTexCoord2f(0, 0);
			glVertex3f(mPosition[PF_X] + (scale*spinSval), mPosition[PF_Y] + (scale*spinCval), mPosition[PF_Z] - scale);
			glTexCoord2f(0, 1);
			glVertex3f(mPosition[PF_X] - (scale*spinSval), mPosition[PF_Y] - (scale*spinCval), mPosition[PF_Z] - scale);
		}
		glEnd();
	}

	glPopMatrix();
}

int32 Bubble::CountTypes(int32 type)
{
	Bubble	*pro;
	int32	count = 0;
	for (pro = msFirst; pro; pro = pro->mNext) {
		if (pro->mType == type) count++;
	}
	return(count);
}

void Bubble::SpawnType(int32 type)
{
	Bubble	*bub;
	float	xmin, xmax, ymin, ymax;

	gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);

	bub = new Bubble(type);
	SETFLAG(bub->mFlags, BUBBLE_FLAG_ACTIVE);
	pfSetVec4(bub->mColor, 1.0f, 1.0f, 1.0f, 0.25f);

	switch (type) {
	case BUBBLE_TYPE_ATTITUDE:
		pfSetVec3(bub->mPosition, xmin + 0.0f, ymin + 0.0f, 0.0f);
		pfSetVec3(bub->mVelocity, 0, 0, 0);
		bub->mSize = 8.0f;
		break;
	case BUBBLE_TYPE_HEALTH:
		pfSetVec3(bub->mPosition, xmax - 0.0f, ymin + 0.0f, 0.0f);
		pfSetVec3(bub->mVelocity, 0, 0, 0);
		bub->mSize = 8.0f;
		break;
	case BUBBLE_TYPE_AMMO:
		pfSetVec3(bub->mPosition, xmin + 0.0f, ymax - 0.0f, 0.0f);
		pfSetVec3(bub->mVelocity, 0, 0, 0);
		bub->mSize = 8.0f;
		break;
	case BUBBLE_TYPE_SHIELD_POX:
	case BUBBLE_TYPE_SHIELD_COLDS:
	case BUBBLE_TYPE_SHIELD_RASH:
	case BUBBLE_TYPE_SHIELD_BARF:
	case BUBBLE_TYPE_SHIELD_FEVER:
	case BUBBLE_TYPE_SHIELD_HAIR:
	case BUBBLE_TYPE_SHIELD_BANDAID:
		pfSetVec3(bub->mPosition, xmax - 0.0f, ymax - 0.0f, 0.0f);
		pfSetVec3(bub->mVelocity, 0, 0, 0);
		bub->mSize = 16.0f;
		break;
	default:
		bub->mPosition[PF_X] = RANDOM_IN_RANGE(xmin + 40.0f, xmax - 40.0f);
		bub->mPosition[PF_Y] = RANDOM_IN_RANGE(ymin + 40.0f, ymax - 40.0f);
		bub->mPosition[PF_Z] = 0.0f;
		bub->mVelocity[PF_X] = RANDOM_IN_RANGE(-10.0f, 10.0f);
		bub->mVelocity[PF_Y] = RANDOM_IN_RANGE(-10.0f, 10.0f);
		bub->mVelocity[PF_Z] = 0.0f;
		break;
	}
}

void Bubble::ThinkAll(void)
{
	Bubble	*pro;
	int		shieldType = 0;

	/**** need to spawn any new ones? ****/
	if (CountTypes(BUBBLE_TYPE_HEALTH) == 0) SpawnType(BUBBLE_TYPE_HEALTH);
	if (CountTypes(BUBBLE_TYPE_AMMO) == 0) SpawnType(BUBBLE_TYPE_AMMO);
	if (CountTypes(BUBBLE_TYPE_ATTITUDE) == 0) SpawnType(BUBBLE_TYPE_ATTITUDE);

	if (CountTypes(BUBBLE_TYPE_SWORD) == 0) SpawnType(BUBBLE_TYPE_SWORD);
	if (CountTypes(BUBBLE_TYPE_CROSSBOW) == 0) SpawnType(BUBBLE_TYPE_CROSSBOW);
	if (CountTypes(BUBBLE_TYPE_SPLIT) == 0) SpawnType(BUBBLE_TYPE_SPLIT);
	if (CountTypes(BUBBLE_TYPE_SLINGSHOT) == 0) SpawnType(BUBBLE_TYPE_SLINGSHOT);

	switch (gBoss->mBossType) {
	case BOSS_TYPE_ROBARF:		shieldType = BUBBLE_TYPE_SHIELD_BARF;	break;
	case BOSS_TYPE_FIREMAN:		shieldType = BUBBLE_TYPE_SHIELD_FEVER;	break;
	case BOSS_TYPE_EVILCHICKEN:	shieldType = BUBBLE_TYPE_SHIELD_POX;	break;
	case BOSS_TYPE_ICEMAN:		shieldType = BUBBLE_TYPE_SHIELD_COLDS;	break;
	case BOSS_TYPE_TORNADO:		shieldType = BUBBLE_TYPE_SHIELD_RASH;	break;
	case BOSS_TYPE_QBALL:		shieldType = BUBBLE_TYPE_SHIELD_HAIR;	break;
	case BOSS_TYPE_VAMPIRE:		shieldType = BUBBLE_TYPE_SHIELD_BANDAID;	break;
	default: break;
	}
	if (CountTypes(shieldType) == 0) SpawnType(shieldType);
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Think();
	}
}

void Bubble::RemoveDead(void)
{
	Bubble	*pro, *next;

	pro = msFirst;
	while (pro) {
		next = pro->mNext;
		if ((gGameMode != GAME_MODE_PLAY) || !(pro->mFlags & BUBBLE_FLAG_ACTIVE)) {
			delete pro;
		}
		pro = next;
	}
}

bool Bubble::CheckGliderCollisions(Glider *glide)
{
	Bubble	*pro;
	pfVec3	diff;
	float	sqDist;
	bool	sayYes = false;

	for (pro = msFirst; pro; pro = pro->mNext) {
		if (pro->mFlags & BUBBLE_FLAG_ACTIVE) {
			if (pro->mSpawnDrop <= 0.0f) {
				PFSUB_VEC2(diff, glide->mMatrix[PF_T], pro->mPosition);
				diff[PF_Z] = 0.0f;
				sqDist = pfDotVec3(diff, diff);
				if (sqDist < ((0.75f * pro->mSize) * (0.75f * pro->mSize))) {
					/**** Yep, we hit it. ****/
					sayYes = false;
					switch (pro->mType) {
					case BUBBLE_TYPE_HEALTH:
						playSound2D(SOUND_HEALTH_FILL);
						gHealthLevel[HEALTH_TYPE_HEALTH] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						break;
					case BUBBLE_TYPE_AMMO:
						playSound2D(SOUND_HEALTH_FILL);
						gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						break;
					case BUBBLE_TYPE_ATTITUDE:
						playSound2D(SOUND_HEALTH_FILL);
						gHealthLevel[HEALTH_TYPE_ATTITUDE] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						break;
					case BUBBLE_TYPE_CROSSBOW:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[0] = PROJECTILE_TYPE_CROSSBOW;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_SWORD:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[0] = PROJECTILE_TYPE_SWORD;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_BLASTER:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[0] = PROJECTILE_TYPE_BLASTER;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_SLINGSHOT:
						glide->mCurrentWeapon[0] = PROJECTILE_TYPE_SLINGSHOT;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_MISSILE:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[1] = PROJECTILE_TYPE_MISSILE;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_SPLIT:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[0] = PROJECTILE_TYPE_SPLIT;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_BALLISTIC:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[1] = PROJECTILE_TYPE_BALLISTIC;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_PARTICLE_ACCELERATOR:
						playSound2D(SOUND_BUBBLE_POP);
						glide->mCurrentWeapon[1] = PROJECTILE_TYPE_PARTICLE_ACCELERATOR;
						gHealthLevel[HEALTH_TYPE_AMMO] += 0.25f;
						if (gHealthLevel[HEALTH_TYPE_AMMO] > 1.0f) gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						sayYes = true;
						hudGetTripleWeapon(glide, sBubbleTypeTextures[pro->mType]);
						break;
					case BUBBLE_TYPE_SHIELD_POX:
					case BUBBLE_TYPE_SHIELD_COLDS:
					case BUBBLE_TYPE_SHIELD_RASH:
					case BUBBLE_TYPE_SHIELD_BARF:
					case BUBBLE_TYPE_SHIELD_FEVER:
					case BUBBLE_TYPE_SHIELD_HAIR:
					case BUBBLE_TYPE_SHIELD_BANDAID:
						if (gCells->mTotalBadCells == 0) {
							if (gWinTimer <= 0.0f) {
								gWinTimer = 5.0f;
								getShield(sBubbleTypeTextures[pro->mType]);
								playSound2D(VO_BOTH_COMPLETE, 1.0f, RANDOM_IN_RANGE(0.9f, 1.1f));
							}
						} else {
							gShieldFlickerTimer = 1.0f;
						}
						break;
					default:
						CLRFLAG(pro->mFlags, BUBBLE_FLAG_ACTIVE);
						break;
					}
					if (sayYes) {
						if (RANDOM0TO1 < 0.5f) {
							glide->SimpleGameVoice(VO_BEN_YEAH, VO_SET_YEAH);
						} else {
							glide->SimpleGameVoice(VO_BEN_YEAH, VO_SET_GOTIT);
						}
					}
					return(true);
				}
			}
		}
	}
	return(false);
}

void Bubble::MoveAll(void)
{
	Bubble	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Move();
	}
}

void Bubble::ReactAll(void)
{
	Bubble	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->React();
	}
}

void Bubble::DrawAll(void)
{
	Bubble	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Draw();
	}
}



