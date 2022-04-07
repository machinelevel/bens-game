
/************************************************************\
	glider.cpp
	Files for controlling the glider in Ben's project
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "genincludes.h"
#include "upmath.h"
#include "camera.h"
#include "controls.h"
#include "timer.h"
#include "random.h"
#include "cellgrid.h"
#include "glider.h"
#include "texture.h"
#include "projectile.h"
#include "hud.h"
#include "bubble.h"
#include "cellport.h"
#include "sound.h"
#include "draw.h"
#include "slides.h"

Glider	*gActiveGliderList = NULL;
Glider	*gGliders[2] = {NULL, NULL};

unsigned char	gGliderKeys[MAX_NUM_GLIDERS][GLIDER_KEY_HOWMANY] = {
	{ 226, 225, 228, 227, ' ', '/' },
	{ 'a', 'd', 'w', 's', '\t', 'q' },
};

#define MIN_SWORD_GLOW_SPEED	(30.0f)
#define MIN_SWORD_DAMAGE_SPEED	(40.0f)
#define SWORD_GLOW_FACTOR		(0.05f)


Glider::Glider()
{
	Glider	*g;

	mFlags = 0;
	pfMakeIdentMat(mMatrix);
	pfCopyMat(mDrawMatrix, mMatrix);
	pfCopyMat(mLastMatrix, mMatrix);
	pfSetVec3(mVelocity, 0, 0, 0);
	mHoverHeight = 0.5f;
	mGliderCamDist = 30.0f;
	mLightningIndex = -1;
	mCurrentWeapon[0] = PROJECTILE_TYPE_SMALL;
	mCurrentWeapon[1] = PROJECTILE_TYPE_SMALL;
	mWeaponGlowSpark = new GlowSpark(GLOWSPARK_TYPE_NORMAL, 64);
	mWeaponGlowSpark2 = new GlowSpark(GLOWSPARK_TYPE_BIGSOFT, 64);
	mTrailNext = -1;
	mFlying = false;
	mVoiceTimer = 5.0f;
	mRapidFireTimer = 0.0f;

	mPlayerNum = 0;
	for (g = gActiveGliderList; g; g = g->mNext) {
		if (g->mPlayerNum >= mPlayerNum) {
			mPlayerNum = g->mPlayerNum + 1;
		}
	}

	mPlayerImageNum = mPlayerNum;
	mBoardImageNum = mPlayerNum;
	sprintf(mName, "Player %d", 1 + mPlayerNum);

	/**** add it to the list ****/
	mNext = gActiveGliderList;
	gActiveGliderList = this;
}

Glider::~Glider()
{
	Glider	*g;
	if (mWeaponGlowSpark) delete mWeaponGlowSpark;
	if (mWeaponGlowSpark2) delete mWeaponGlowSpark2;

	/**** remove it from the list ****/
	if (gActiveGliderList == this) {
		gActiveGliderList = mNext;
	}
	for (g = gActiveGliderList; g; g = g->mNext) {
		if (g->mNext == this) {
			g->mNext = g->mNext->mNext;
		}
	}
}

void Glider::Thrust(float amount)
{
	pfAddScaledVec3(mVelocity, mVelocity, amount * DeltaTime, mMatrix[PF_Y]);
	if (amount > 1.0f || mSpeed > 10.0f) {
		float pitch = 0.01f * mSpeed;
		if (pitch < 0.01f) pitch = 0.01f;
//		if (pitch > 0.5f) pitch = 0.5f;
#ifdef WIN32
		playSound2D(SOUND_JET_1+mPlayerNum, 0.8f, pitch);
#endif
	}
}

void Glider::Turn(float amount)
{
	pfPreRotMat(mMatrix, amount * DeltaTime, 0.0f, 0.0f, 1.0f, mMatrix);
}

void Glider::Fire(int32 weapon)
{
	pfMatrix	mat;

	pfCopyMat(mat, mDrawMatrix);
	if (mat[PF_T][PF_Z] < mHoverHeight) {
		mat[PF_T][PF_Z] = mHoverHeight;
		pfAddScaledVec3(mat[PF_T], mat[PF_T], 1.0f, mat[PF_Y]);
	}

//	if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f) {
//		weapon = PROJECTILE_TYPE_SMALL;
//	}
//	Projectile::Launch(weapon, mat, this);

	if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f
		|| gHealthLevel[HEALTH_TYPE_HEALTH] <= 0.0f
		|| gHealthLevel[HEALTH_TYPE_ATTITUDE] <= 0.0f) {
		/**** out of ammo; just fire simple shot ****/
		Projectile::Launch(PROJECTILE_TYPE_SMALL, mat, this);
		playSound2D(SOUND_FIRE_1, 0.9f, RANDOM_IN_RANGE(0.9f, 1.1f));
	} else {
		bool triple = true;

		if (weapon == PROJECTILE_TYPE_SPLIT) triple = false;
		if (weapon == PROJECTILE_TYPE_SLINGSHOT) triple = false;

		if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f) {
			playSound2D(SOUND_FIRE_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
		} else {
			switch (weapon) {
				case PROJECTILE_TYPE_CROSSBOW:
					playSound2D(SOUND_BLASTER_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
					break;
				case PROJECTILE_TYPE_SWORD:
					playSound2D(SOUND_SWISH_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
					break;
				default:
					playSound2D(SOUND_FIRE_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
					break;
			}
		}

		if (triple) {
			Projectile::Launch(weapon, mat, this);
			pfAddScaledVec3(mat[PF_Y], mat[PF_Y],  0.1f, mat[PF_X]);
			Projectile::Launch(weapon, mat, this);
			pfAddScaledVec3(mat[PF_Y], mat[PF_Y], -0.2f, mat[PF_X]);
			Projectile::Launch(weapon, mat, this);
		} else {
			Projectile::Launch(weapon, mat, this);
		}
	}

	if (weapon == PROJECTILE_TYPE_SPLIT) {
		gHealthLevel[HEALTH_TYPE_AMMO] -= 0.05f;
	} else {
		gHealthLevel[HEALTH_TYPE_AMMO] -= 0.01f;
	}
	if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f) {
		gHealthLevel[HEALTH_TYPE_AMMO] = 0.0f;
//		mCurrentWeapon[0] = PROJECTILE_TYPE_SMALL;
	}
}

void Glider::Think(void)
{
	extern pfVec3	CameraSwingVeloc;
	float	xstick1 = JoystickVals[0];
	float	ystick1 = JoystickVals[1];
	float	xstick2 = JoystickVals[2];
	float	ystick2 = JoystickVals[3];

	mVoiceTimer -= DeltaTime;
	if (mVoiceTimer < 0.0f) mVoiceTimer = 0.0f;

	mTurning = 0.0f;
	mThrusting = 0.0f;
	mBraking = 0.0f;
	pfSetVec3(CameraSwingVeloc, 0.0f, 0.0f, 0.0f);

	if (gWinTimer > 0.0f) {
		mBraking = 10.0f;
	}

	ManageTripleWeapons();

	/**** joystick ****/
	if (mPlayerNum == 0) {
		if (PF_ABSGT(xstick1, 0.2f)) mTurning -= 360.0f * xstick1;
		if (mCurrentWeapon[0] == PROJECTILE_TYPE_SLINGSHOT) {
			if (ButtonDown(CONTROL_BUTTON_FIRE1)) {
				mRapidFireTimer -= DeltaTime;
				if (mRapidFireTimer <= 0.0f) {
					Fire(mCurrentWeapon[0]);
					mRapidFireTimer += 0.1f;
					if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f
						|| gHealthLevel[HEALTH_TYPE_HEALTH] <= 0.0f
						|| gHealthLevel[HEALTH_TYPE_ATTITUDE] <= 0.0f) {
						mRapidFireTimer += 1.0f;
					}
				}
//			} else {
//				mRapidFireTimer = 0.0f;
			}
		} else {
			if (ButtonPressed(CONTROL_BUTTON_FIRE1)) Fire(mCurrentWeapon[0]);
		}
		if (ButtonPressed(CONTROL_BUTTON_FIRE2)) Fire(mCurrentWeapon[1]);
		if (ButtonDown(CONTROL_BUTTON_BRAKES)) mBraking = 10.0f;
		if (ButtonDown(CONTROL_BUTTON_THRUST)) mThrusting = 20.0f;

		if (PF_ABSGT(xstick2, 0.2f)) {
			CameraSwingVeloc[PF_X] += 360.0f * xstick2;
			CameraSwingVeloc[PF_Z] += 180.0f * PF_ABS(xstick2);	/**** bring it down close to level so we can look around ****/
		}
		if (PF_ABSGT(ystick2, 0.2f)) mGliderCamDist += UnscaledDeltaTime * 40.0f * ystick2;
	}

	/**** keys ****/
	if (mCurrentWeapon[0] == PROJECTILE_TYPE_SLINGSHOT && gHealthLevel[HEALTH_TYPE_AMMO] > 0.0f) {
		if (KeysDown[gGliderKeys[mPlayerNum][GLIDER_KEY_FIRE1]]) {
			mRapidFireTimer -= DeltaTime;
			if (mRapidFireTimer <= 0.0f) {
				Fire(mCurrentWeapon[0]);
				mRapidFireTimer += 0.1f;
				if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f
					|| gHealthLevel[HEALTH_TYPE_HEALTH] <= 0.0f
					|| gHealthLevel[HEALTH_TYPE_ATTITUDE] <= 0.0f) {
					mRapidFireTimer += 1.0f;
				}
			}
//		} else {
//			mRapidFireTimer = 0.0f;
		}
	} else {
		if (KeysPressed[gGliderKeys[mPlayerNum][GLIDER_KEY_FIRE1]]) Fire(mCurrentWeapon[0]);
	}
	if (KeysPressed[gGliderKeys[mPlayerNum][GLIDER_KEY_SWITCH]]) hudSwapTripleWeapon(this);

	if (KeysDown[gGliderKeys[mPlayerNum][GLIDER_KEY_TURN_LEFT]]) mTurning =  180.0f;
	if (KeysDown[gGliderKeys[mPlayerNum][GLIDER_KEY_TURN_RIGHT]]) mTurning = -180.0f;
	if (KeysDown[gGliderKeys[mPlayerNum][GLIDER_KEY_THRUST]]) mThrusting = 20.0f;
	if (KeysDown[gGliderKeys[mPlayerNum][GLIDER_KEY_BRAKE]]) mBraking = 10.0f;

	if (gTripleRepeaterTimer[mPlayerNum] > 0.0f) {
		mTurning = 1000.0f;
	}

	if (gWinTimer > 0.0f) {
		CameraSwingVeloc[PF_X] = 90.0f;
		CameraSwingVeloc[PF_Z] = 20.0f;	/**** bring it down close to level so we can look around ****/
		if (mGliderCamDist < 10.0f)  mGliderCamDist += UnscaledDeltaTime * 40.0f;
		if (mGliderCamDist > 12.0f) mGliderCamDist -= UnscaledDeltaTime * 40.0f;
	} else {

		if (KeysDown['i'] || (gNumPlayers == 1 && KeysDown['w'])) mGliderCamDist += UnscaledDeltaTime * 80.0f;
		if (KeysDown['k'] || (gNumPlayers == 1 && KeysDown['s'])) mGliderCamDist -= UnscaledDeltaTime * 80.0f;
		if (KeysDown['j'] || (gNumPlayers == 1 && KeysDown['a'])) {
			CameraSwingVeloc[PF_X] += 90.0f;
			CameraSwingVeloc[PF_Z] += 45.0f;	/**** bring it down close to level so we can look around ****/
		}
		if (KeysDown['l'] || (gNumPlayers == 1 && KeysDown['d'])) {
			CameraSwingVeloc[PF_X] -= 90.0f;
			CameraSwingVeloc[PF_Z] += 45.0f;	/**** bring it down close to level so we can look around ****/
		}
		Bubble::CheckGliderCollisions(this);
	}
	if (mGliderCamDist < 6.0f) mGliderCamDist = 6.0f;
	if (mGliderCamDist > 100.0f) mGliderCamDist = 100.0f;

	/**** Check for sword damage ****/
	if (mCurrentWeapon[0] == PROJECTILE_TYPE_SWORD) {
		pfVec3	center;
		float	dot = pfDotVec3(mVelocity, mMatrix[PF_Y]);
		if (dot > MIN_SWORD_DAMAGE_SPEED) {
			pfAddScaledVec3(center, mMatrix[PF_T], 2.0f, mMatrix[PF_Y]);
			if (gCells->KillCellsInRange(center, 1, 0)) {
				playSound2D(SOUND_SWORD_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
			}
		}
	}
}

void Glider::Move(void)
{
	float	floor = gCells->mCenter[PF_Z];
	float	xmin, ymin, xmax, ymax;
	float	xtilt = 0.0f, ytilt = 0.0f;
	float	over, sval, cval, quarter;
	pfVec3	vec, slope;
	float	pipeGravity = 200.0f;
	float	xht, yht, dot;
	float	maxSpeed = 200.0f;
//	static bool	newFlying = false, oldFlying = false;
	bool	oldFly = mFlying;

	if (gGameMode == GAME_MODE_SLIDES) return;

	pfCopyMat(mLastMatrix, mMatrix);
	Turn(mTurning);
	Thrust(mThrusting);

	gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);

	if (mFlags & GLIDER_FLAG_ON_HALFPIPE) {
		/**** apply slope gravity ****/
		pfCopyVec3(slope, mDrawMatrix[PF_Z]);
		slope[PF_Z] = 0.0f;
		pfAddScaledVec3(mVelocity, mVelocity, pipeGravity * DeltaTime, slope);
	} else {
		/**** apply brakes ****/
		pfAddScaledVec3(mVelocity, mVelocity, -mBraking * DeltaTime, mVelocity);
	}

	/**** apply side-braking ****/
	float sideBrake = 0.4f;
	if (mThrusting > 0.0f) sideBrake = 1.0f;
	dot = pfDotVec3(mVelocity, mMatrix[PF_X]);
	pfAddScaledVec3(mVelocity, mVelocity, -sideBrake * dot * DeltaTime, mMatrix[PF_X]);

	mSpeed = pfNormalizeVec3(mVelocity);
	if (mSpeed <= 0.0f) {
		pfSetVec3(mVelocity, 0.0f, 0.0f, 0.0f);
	} else {
		if (mSpeed > maxSpeed) mSpeed = maxSpeed;
		pfScaleVec3(mVelocity, mSpeed, mVelocity);
		mVelocity[PF_Z] = 0.0f;
	}

	pfAddScaledVec3(mMatrix[PF_T], mMatrix[PF_T], DeltaTime, mVelocity);
	mMatrix[PF_T][PF_Z] = floor;

	/**** position the draw matrix, based on halfpipe action, etc ****/
	pfCopyMat(mDrawMatrix, mMatrix);
	quarter = (0.25f * 2.0f * PF_PI * gHalfPipeRadius);
	CLRFLAG(mFlags, GLIDER_FLAG_ON_HALFPIPE);

	mFlying = false;

	if (gHalfPipeRadius > 0.0f) {
		if (mMatrix[PF_T][PF_X] < xmin) {
			SETFLAG(mFlags, GLIDER_FLAG_ON_HALFPIPE);
			over = -(mMatrix[PF_T][PF_X] - xmin);
			xtilt = -(over * 90.0f) / quarter;
			if (xtilt > 90.0f) xtilt = 90.0f;
			else if (xtilt < -90.0f) xtilt = -90.0f;
			if (over > quarter) {
				mDrawMatrix[PF_T][PF_X] = xmin - gHalfPipeRadius;
				xht = gHalfPipeRadius + (over - quarter);
				mFlying = true;
			} else {
				pfSinCos(xtilt, &sval, &cval);
				mDrawMatrix[PF_T][PF_X] = xmin + (gHalfPipeRadius * sval);
				xht = gHalfPipeRadius * (1.0f - cval);
			}
		} else if (mMatrix[PF_T][PF_X] > xmax) {
			SETFLAG(mFlags, GLIDER_FLAG_ON_HALFPIPE);
			over = (mMatrix[PF_T][PF_X] - xmax);
			xtilt = (over * 90.0f) / quarter;
			if (xtilt > 90.0f) xtilt = 90.0f;
			else if (xtilt < -90.0f) xtilt = -90.0f;
			if (over > quarter) {
				mDrawMatrix[PF_T][PF_X] = xmax + gHalfPipeRadius;
				xht = gHalfPipeRadius + (over - quarter);
				mFlying = true;
			} else {
				pfSinCos(xtilt, &sval, &cval);
				mDrawMatrix[PF_T][PF_X] = xmax + (gHalfPipeRadius * sval);
				xht = gHalfPipeRadius * (1.0f - cval);
			}
		} else {
		}

		if (mMatrix[PF_T][PF_Y] < ymin) {
			SETFLAG(mFlags, GLIDER_FLAG_ON_HALFPIPE);
			over = -(mMatrix[PF_T][PF_Y] - ymin);
			ytilt = -(over * 90.0f) / quarter;
			if (ytilt > 90.0f) ytilt = 90.0f;
			else if (ytilt < -90.0f) ytilt = -90.0f;
			if (over > quarter) {
				mDrawMatrix[PF_T][PF_Y] = ymin - gHalfPipeRadius;
				yht = gHalfPipeRadius + (over - quarter);
				mFlying = true;
			} else {
				pfSinCos(ytilt, &sval, &cval);
				mDrawMatrix[PF_T][PF_Y] = ymin + (gHalfPipeRadius * sval);
				yht = gHalfPipeRadius * (1.0f - cval);
			}
		} else if (mMatrix[PF_T][PF_Y] > ymax) {
			SETFLAG(mFlags, GLIDER_FLAG_ON_HALFPIPE);
			over = (mMatrix[PF_T][PF_Y] - ymax);
			ytilt = (over * 90.0f) / quarter;
			if (ytilt > 90.0f) ytilt = 90.0f;
			else if (ytilt < -90.0f) ytilt = -90.0f;
			if (over > quarter) {
				mDrawMatrix[PF_T][PF_Y] = ymax + gHalfPipeRadius;
				yht = gHalfPipeRadius + (over - quarter);
				mFlying = true;
			} else {
				pfSinCos(ytilt, &sval, &cval);
				mDrawMatrix[PF_T][PF_Y] = ymax + (gHalfPipeRadius * sval);
				yht = gHalfPipeRadius * (1.0f - cval);
			}
		} else {
		}

		if (mFlying && !oldFly) {
			if (RANDOM0TO1 < 0.5f) {
				SimpleGameVoice(VO_BEN_WHOOPEE, VO_SET_WHOOPEE, true);
			}
		}

		pfCopyVec3(vec, mDrawMatrix[PF_T]);

		if (xtilt != 0.0f) {
			pfPostRotMat(mDrawMatrix, mDrawMatrix, -xtilt, 0.0f, 1.0f, 0.0f);
			vec[PF_Z] = PF_MAX2(vec[PF_Z], xht);
		}
		if (ytilt != 0.0f) {
			pfPostRotMat(mDrawMatrix, mDrawMatrix,  ytilt, 1.0f, 0.0f, 0.0f);
			vec[PF_Z] = PF_MAX2(vec[PF_Z], yht);
		}
		pfCopyVec3(mDrawMatrix[PF_T], vec);
	}
}

pfVec3 tempVec = {0,0,0};

bool flatSegsCross(pfVec3 v1a, pfVec3 v1b, pfVec3 v2a, pfVec3 v2b)
{
	float	sign;
	pfVec3	edge1, edge2, edge3, edge4, vc;
	/**** The segments cross if and only if they form a convex quad ****/
	pfSubVec3(edge1, v1a, v2b);
	pfSubVec3(edge2, v2b, v1b);
	pfSubVec3(edge3, v1b, v2a);
	pfSubVec3(edge4, v2a, v1a);
	pfCrossVec3(vc, edge1, edge2);
	sign = vc[PF_Z];
	pfCrossVec3(vc, edge2, edge3);
	if (sign * vc[PF_Z] < 0.0f) return(false);
	pfCrossVec3(vc, edge3, edge4);
	if (sign * vc[PF_Z] < 0.0f) return(false);
	pfCrossVec3(vc, edge4, edge1);
	if (sign * vc[PF_Z] < 0.0f) return(false);
	return(true);
}

void Glider::React(void)
{
	float	xmin, xmax, ymin, ymax, dot;
	float	bounce = 0.5f;
	OneCell	*pcell;
	int32	i, wall, hClose, vClose, iClose, meter;
	pfVec3	posClose, vec, up, along, across, path;
	Glider	*g;
	float	gliderBumpDist = 2.0f;
	float	dist;
	pfVec3	vDiff;

	/**** check for collisions with other gliders ****/
	for (g = gActiveGliderList; g; g = g->mNext) {
		if (g != this) {
			pfSubVec3(vDiff, mMatrix[PF_T], g->mMatrix[PF_T]);
			vDiff[PF_Z] = 0.0f;
			dot = pfDotVec3(vDiff, vDiff);
			if (dot < 0.0001f) {
				/**** they're right on top of each other. Avoid a singularity ****/
				mMatrix[PF_T][PF_X] += 0.1f;
				pfSubVec3(vDiff, mMatrix[PF_T], g->mMatrix[PF_T]);
				vDiff[PF_Z] = 0.0f;
				dot = pfDotVec3(vDiff, vDiff);
			}
			if (dot < gliderBumpDist*gliderBumpDist) {
				dist = pfNormalizeVec3(vDiff);
				pfAddScaledVec3(mMatrix[PF_T], mMatrix[PF_T], 0.5f * (gliderBumpDist-dist), vDiff);
				pfAddScaledVec3(g->mMatrix[PF_T], g->mMatrix[PF_T], -0.5f * (gliderBumpDist-dist), vDiff);
				if (g->mSpeed > 50.0f || mSpeed > 50.0f) {
					/**** high-speed collision! fun! ****/
					gHealthLevel[HEALTH_TYPE_HEALTH] = 1.0f;
					gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
					gHealthLevel[HEALTH_TYPE_ATTITUDE] = 1.0f;
				}
			}
		}
	}

	/**** bounary check ****/
	gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);
	
	if (gHalfPipeRadius <= 0.0f) {
		if (mMatrix[PF_T][PF_X] < xmin) {
			mMatrix[PF_T][PF_X] = xmin;
			if (mVelocity[PF_X] < 0.0f) {
				mVelocity[PF_X] *= -bounce;
			}
		}
		if (mMatrix[PF_T][PF_X] > xmax) {
			mMatrix[PF_T][PF_X] = xmax;
			if (mVelocity[PF_X] > 0.0f) {
				mVelocity[PF_X] *= -bounce;
			}
		}
		if (mMatrix[PF_T][PF_Y] < ymin) {
			mMatrix[PF_T][PF_Y] = ymin;
			if (mVelocity[PF_Y] < 0.0f) {
				mVelocity[PF_Y] *= -bounce;
			}
		}
		if (mMatrix[PF_T][PF_Y] > ymax) {
			mMatrix[PF_T][PF_Y] = ymax;
			if (mVelocity[PF_Y] > 0.0f) {
				mVelocity[PF_Y] *= -bounce;
			}
		}
	}

	if (gGameMode == GAME_MODE_PLAY) {
		/**** Did we bump a bad cell? ****/
		mLightningIndex = -1;
		pcell = gCells->GetClosestCell(mMatrix[PF_T], NULL, NULL, &iClose);
		gCells->GetPosFromIndex(iClose, posClose);
		if (pcell) {
			if (pcell->shift & (CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT)) {
				pfSubVec3(vec, posClose, mMatrix[PF_T]);
	//printf("hit %.2f %.2f %.2f\n", posClose[0], posClose[1], posClose[2]);
	//printf(" loc %.2f %.2f %.2f\n", mMatrix[PF_T][0], mMatrix[PF_T][1], mMatrix[PF_T][2]);
				vec[PF_Z] = 0.0f;
				pfNormalizeVec3(vec);
	//printf("  vec %.2f %.2f %.2f\n", vec[0], vec[1], vec[2]);
				dot = pfDotVec3(vec, mVelocity);
				pfAddScaledVec3(mVelocity, mVelocity, -200.0f * DeltaTime, vec);
				pcell->flags |= CELL_FLAG_BOUNCE;
				gHealthLevel[HEALTH_TYPE_HEALTH] -= 0.25f * DeltaTime;
				if (gHealthLevel[HEALTH_TYPE_HEALTH] < 0.0f) gHealthLevel[HEALTH_TYPE_HEALTH] = 0.0f;
				SimpleGameVoice(VO_BEN_OW, VO_SET_OW);
			}
		}
	}

	/**** Did we bump a wall? ****/
	if (pfSqrDistancePt3(mMatrix[PF_T], mLastMatrix[PF_T]) > 0.0001f) {
		for (wall = 0; wall < gCells->mNumWalls; wall++) {
			if (flatSegsCross(mMatrix[PF_T], mLastMatrix[PF_T], gCells->mWalls[wall][0], gCells->mWalls[wall][1])) {
				pfSetVec3(up, 0.0f, 0.0f, 1.0f);
				pfSubVec3(along, gCells->mWalls[wall][0], gCells->mWalls[wall][1]);
				pfCrossVec3(across, up, along);
				pfNormalizeVec3(across);
				pfSubVec3(path, mMatrix[PF_T], mLastMatrix[PF_T]);
				dot = pfDotVec3(path, across);
				if (dot > 0.0f) pfScaleVec3(across, -1.0f, across);

				dot = pfDotVec3(mVelocity, across);
				pfAddScaledVec3(mVelocity, mVelocity, 10.0f-(dot*2), across);
				pfAddScaledVec3(mMatrix[PF_T], mLastMatrix[PF_T], 0.01f, across);
				meter = HEALTH_TYPE_ATTITUDE;

				playSound2D(SOUND_ELECTRICAL_BOING);
				gHealthLevel[meter] -= 0.1f;
				if (gHealthLevel[meter] < 0.0f) gHealthLevel[meter] = 0.0f;
			}
		}
	}
	ManageTrail();
}

void Glider::ManageTrail(void)
{
	int	i;
	pfVec3	over, up = {0,0,1};
	float	sqSpeed, width = 1.0f;

	if (mTrailNext < 0) {
		for (i = 0; i < GLIDER_TRAIL_LENGTH; i++) {
			pfCopyVec3(mTrailVerts[0][i], mDrawMatrix[PF_T]);
			pfCopyVec3(mTrailVerts[1][i], mDrawMatrix[PF_T]);
			mTrailNext = 0;
		}
	}
	sqSpeed = pfDotVec3(mVelocity, mVelocity);
	if (sqSpeed > 0.0f) {
		pfCrossVec3(over, mVelocity, up);
		pfNormalizeVec3(over);
		pfAddScaledVec3(mTrailVerts[0][mTrailNext], mDrawMatrix[PF_T],  width, over);
		pfAddScaledVec3(mTrailVerts[1][mTrailNext], mDrawMatrix[PF_T], -width, over);
		mTrailNext = (mTrailNext+1) % GLIDER_TRAIL_LENGTH;
	}
}

void Glider::DrawTrail(void)
{
	int		i, j, index;
	float	sqDist;
	pfVec3	mid;
	pfVec4	midHue = {1,1,1,1};
	pfVec4	sideHue = {1,1,1,0};

	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);

	if (gNumPlayers == 1) {
		pfSetVec4(midHue, 1, 1, 1, 1);
		pfSetVec4(sideHue, 1, 1, 1, 0);
	} else {
		if (mPlayerNum == 0) {
			pfSetVec4(midHue, 1, 1, 1, 1);
			pfSetVec4(sideHue, 1, 1, 1, 0);
		} else {
			pfSetVec4(midHue, 209.0f/255.0f, 97.0f/255.0f, 252.0f/255.0f, 1);
			pfSetVec4(sideHue, 209.0f/255.0f, 97.0f/255.0f, 252.0f/255.0f, 0);
		}
	}

	for (j = 0; j < 2; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		index = mTrailNext;
		for (i = 0; i < GLIDER_TRAIL_LENGTH; i++) {
			index--;
			if (index < 0) index = GLIDER_TRAIL_LENGTH-1;
			pfCombineVec3(mid, 0.5f, mTrailVerts[0][index], 0.5f, mTrailVerts[1][index]);
			midHue[3] = 1.0f-((float)i/(float)GLIDER_TRAIL_LENGTH);
			if (mPlayerNum == 0) {
				midHue[3] *= 0.5f;
			} else {
				midHue[3] *= 1.0f;
			}
			/**** Fade the tail in gracefully ****/
			sqDist = pfSqrDistancePt3(mid, mDrawMatrix[PF_T]);
			sqDist *= 0.5f;
			if (sqDist > 1.0f) sqDist = 1.0f;
			midHue[3] *= sqDist;
			
			glColor4fv(sideHue);
			glVertex3fv(mTrailVerts[j][index]);
			glColor4fv(midHue);
			glVertex3fv(mid);
		}
		glEnd();
	}

}

void Glider::SimpleGameVoice(int boyVoice, int girlVoice, bool ignoreTimer)
{
	if (ignoreTimer || mVoiceTimer <= 0.0f) {
		playSound2D(boyVoice, 0.5f, RANDOM_IN_RANGE(0.9f, 1.1f));
		mVoiceTimer = RANDOM_IN_RANGE(5.0f, 10.0f);
	}
}

void drawLightning(pfVec3 src, pfVec3 dst)
{
	pfVec3	v2;
	float	t, wiggle = 0.3f;
	int		i;

	glColor4f(1.0f, 0.8f, 0.0f, 1.0f);
	for (i = 0; i < 5; i++) {
		glBegin(GL_LINE_STRIP);
		t = 0.0f;
		while (t < 1.0f) {
			pfScaleVec3(v2, t, dst);
			pfAddScaledVec3(v2, v2, 1.0f-t, src);
			v2[PF_X] += RANDOM0TO1 * wiggle;
			v2[PF_Y] += RANDOM0TO1 * wiggle;
			v2[PF_Z] += RANDOM0TO1 * wiggle;
			glVertex3fv(v2);
			t += 0.1f;
		}
		glEnd();
	}
}


void Glider::DrawWeapon(int32 weapon)
{
	float	weaponHandleDists[PROJECTILE_TYPE_HOWMANY] = {
		0.0f,
		0.0f,
		0.0f,
		0.25f,
		0.25f,
		0.0f,
		0.0f,
		0.5f,
		0.0f,
	};
	pfVec3	weaponOffsets[PROJECTILE_TYPE_HOWMANY] = {
		{0.0f, 0.0f, 0.0f},
		{0.4f,-0.6f, 0.6f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.7f, 1.7f},
		{0.0f, 0.7f, 1.7f},
		{0.4f,-0.6f, 0.6f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.7f, 1.7f},
		{0.0f, 0.0f, 0.0f},
	};
	int32	weaponTextures[PROJECTILE_TYPE_HOWMANY] = {
		0,
		TEXID_WEAPON_MISSILE,
		TEXID_WEAPON_BLASTER,
		TEXID_WEAPON_SLINGSHOT,
		TEXID_WEAPON_SWORD,
		TEXID_WEAPON_MISSILE,
		TEXID_WEAPON_BALLISTIC,
		TEXID_WEAPON_CROSSBOW,
		TEXID_WEAPON_PARTICLE_ACCELERATOR,
	};
	int32	i;
	bool	glow = false;
	bool	glow2 = false;
	float	glow2Brightness = 0.0f;
	pfVec3	glowPos;
	pfVec3	glow2Pos;
	pfVec3	up = {0, 0, 1};
	pfVec3	fwd = {0, 1, 0};
	pfVec3	right = {1, 0, 0};
	pfVec3	verts[4], extrusion;
	float	len, wd;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	switch (weapon) {
	case PROJECTILE_TYPE_SLINGSHOT:
	case PROJECTILE_TYPE_CROSSBOW:
	case PROJECTILE_TYPE_SWORD:
		glow = true;
		UseLibTexture(weaponTextures[weapon]);
		pfXformPt3(glowPos, weaponOffsets[weapon], mDrawMatrix);
		len = 2.0f; wd = 2.0f;
		glColor4f(1, 1, 1, 1);
		pfSetVec3(verts[0], -wd*0.5f, -len*weaponHandleDists[weapon], 0.0f);
		pfSetVec3(verts[1],  wd*0.5f, -len*weaponHandleDists[weapon], 0.0f);
		pfSetVec3(verts[2],  wd*0.5f,  len*(1.0f-weaponHandleDists[weapon]), 0.0f);
		pfSetVec3(verts[3], -wd*0.5f,  len*(1.0f-weaponHandleDists[weapon]), 0.0f);
		for (i = 0; i < 4; i++) {
			pfAddVec3(verts[i], verts[i], weaponOffsets[weapon]);
		}
		pfCopyVec3(extrusion, up);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		if (weapon == PROJECTILE_TYPE_SWORD) {
			pfVec3	center;
			float	dot = pfDotVec3(mVelocity, mMatrix[PF_Y]);
			if (dot > MIN_SWORD_GLOW_SPEED) {
				pfCopyVec3(center, weaponOffsets[weapon]);
				center[PF_Y] += 1.0f;
				pfXformPt3(glow2Pos, center, mDrawMatrix);
				glow2 = true;
				glow2Brightness = SWORD_GLOW_FACTOR * (dot - MIN_SWORD_GLOW_SPEED);
				if (glow2Brightness < 0.0f) glow2Brightness = 0.0f;
				if (glow2Brightness > 0.5f) glow2Brightness = 0.5f;
			}
		}

		break;
#if 1
	case PROJECTILE_TYPE_MISSILE:
	case PROJECTILE_TYPE_SPLIT:
		UseLibTexture(weaponTextures[weapon]);
		len = 2.0f; wd = 2.0f;
		glColor4f(1, 1, 1, 0.7f);
		pfSetVec3(verts[0], -wd*0.5f, -len*weaponHandleDists[weapon], 0.0f);
		pfSetVec3(verts[1],  wd*0.5f, -len*weaponHandleDists[weapon], 0.0f);
		pfSetVec3(verts[2],  wd*0.5f,  len*(1.0f-weaponHandleDists[weapon]), 0.0f);
		pfSetVec3(verts[3], -wd*0.5f,  len*(1.0f-weaponHandleDists[weapon]), 0.0f);
		for (i = 0; i < 4; i++) {
			pfAddVec3(verts[i], verts[i], weaponOffsets[weapon]);
		}
		pfCopyVec3(extrusion, up);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		break;
#endif
	default: break;
	}

	if (glow && mWeaponGlowSpark) {
		SETFLAG(mWeaponGlowSpark->mFlags, GLOWSPARK_FLAG_ACTIVE);
		pfCopyVec3(mWeaponGlowSpark->mPosition, glowPos);
	}
	if (glow2 && mWeaponGlowSpark2) {
		SETFLAG(mWeaponGlowSpark2->mFlags, GLOWSPARK_FLAG_ACTIVE);
		pfCopyVec3(mWeaponGlowSpark2->mPosition, glow2Pos);
		mWeaponGlowSpark2->mGlowColor[3] = glow2Brightness;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Glider::DrawDude(void)
{
	float	guySize = 1.0f;
	float	boardSize = 1.0f;
	int		i, layers = 8;
	float	x, y, z;
	float	boardThickness = 0.05f;
	float	guyThickness = 0.1f;
	pfVec3	verts[4], extrusion;

	glEnable(GL_TEXTURE_2D);
//glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glDepthMask(true);
	
	/**** draw the board's shadow ****/
	if (gGameMode == GAME_MODE_PLAY || gGameMode == GAME_MODE_SLIDES) {
		UseLibTexture(TEXID_BOARD1 + mPlayerNum);
		glColor4f(0, 0, 0, 0.2);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f( boardSize, -boardSize, 0.05f);
		glTexCoord2f(0, 1);
		glVertex3f( boardSize,  boardSize, 0.05f);
		glTexCoord2f(1, 1);
		glVertex3f(-boardSize,  boardSize, 0.05f);
		glTexCoord2f(1, 0);
		glVertex3f(-boardSize, -boardSize, 0.05f);
		glEnd();
	}

	/**** draw the board ****/
	UseLibTexture(TEXID_BOARD1 + mPlayerNum);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	z = -boardThickness;
	for (i = 0; i < layers; i++) {
		glTexCoord2f(0, 0);
		glVertex3f( boardSize, -boardSize, mHoverHeight + z);
		glTexCoord2f(0, 1);
		glVertex3f( boardSize,  boardSize, mHoverHeight + z);
		glTexCoord2f(1, 1);
		glVertex3f(-boardSize,  boardSize, mHoverHeight + z);
		glTexCoord2f(1, 0);
		glVertex3f(-boardSize, -boardSize, mHoverHeight + z);
		z += boardThickness / layers;
	}
	glEnd();

	/**** Draw the guy ****/
	UseLibTexture(TEXID_CHARACTER1 + mPlayerNum);
	glColor4f(1, 1, 1, 1);
//	glBegin(GL_QUADS);
	pfSetVec3(verts[0], -guyThickness*0.5f, -guySize, mHoverHeight);
	pfSetVec3(verts[1], -guyThickness*0.5f, -guySize, mHoverHeight + 2*guySize);
	pfSetVec3(verts[2], -guyThickness*0.5f,  guySize, mHoverHeight + 2*guySize);
	pfSetVec3(verts[3], -guyThickness*0.5f,  guySize, mHoverHeight);
	pfSetVec3(extrusion, guyThickness, 0.0f, 0.0f);
	DrawChiaQuad(verts, extrusion, 16, 0, 1.0f, CHIA_FLAG_DIAGONALS);

	CLRFLAG(mWeaponGlowSpark->mFlags, GLOWSPARK_FLAG_ACTIVE);
	CLRFLAG(mWeaponGlowSpark2->mFlags, GLOWSPARK_FLAG_ACTIVE);
	if (gGameMode == GAME_MODE_PLAY) {
		DrawWeapon(mCurrentWeapon[0]);
		DrawWeapon(mCurrentWeapon[1]);
	}
}

void Glider::Draw(void)
{
	bool	drawTriangleShip = false;

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	DrawTrail();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf((float*)mDrawMatrix);

	/**** Draw the thrust jet ****/
	if (mThrusting > 0) {
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, RANDOM_IN_RANGE(0.25f, 0.75f));
		glVertex3f(0.0f, -1.2f, mHoverHeight - 0.05f);
		
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex3f( 0.0f, -1.8f, mHoverHeight - 0.05f);
		glVertex3f(-0.3f, -0.6f, mHoverHeight - 0.05f);
		glVertex3f( 0.3f, -0.6f, mHoverHeight - 0.05f);
		glVertex3f( 0.0f, -1.8f, mHoverHeight - 0.05f);
		glEnd();
	}

	if (drawTriangleShip) {
		glBegin(GL_TRIANGLES);
		glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
		glVertex3f( 0.0f,  1.0f, 0.05f);
		glVertex3f(-0.5f, -0.5f, 0.05f);
		glVertex3f( 0.5f, -0.5f, 0.05f);

		glColor4f(0.8f, 1.0f, 1.0f, 0.7f);
		glVertex3f( 0.0f,  1.0f, mHoverHeight);
		glVertex3f(-0.5f, -0.5f, mHoverHeight);
		glVertex3f( 0.5f, -0.5f, mHoverHeight);
		glEnd();
	} else {
		DrawDude();
	}

	glPopMatrix();

	if (0) {
		pfVec3	v1, v2;
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_ALPHA_TEST);
		pfSetVec3(v1, mMatrix[PF_T][PF_X], mMatrix[PF_T][PF_Y], mMatrix[PF_T][PF_Z] + mHoverHeight);
		pfSetVec3(v2, tempVec[PF_X], tempVec[PF_Y], tempVec[PF_Z] + 0*mHoverHeight);
		drawLightning(v1, v2);
	}

	if (0 && mLightningIndex >= 0) {
		pfVec3	v1, v2;
		gCells->GetPosFromIndex(mLightningIndex, v1);
		pfCopyVec3(v2, mMatrix[PF_T]);
		v2[PF_Z] += mHoverHeight;
		drawLightning(v1, v2);
	}
}

void Glider::ManageTripleWeapons(void)
{
	int			i;
	pfVec3		pos;
	float		r1, r2, dist, dot;
	pfMatrix	mat;
	static float	quickie[2] = {0,0};

	if (gTripleRepeaterTimer[mPlayerNum] > 0.0f) {
		gTripleRepeaterTimer[mPlayerNum] -= DeltaTime;
		quickie[mPlayerNum] -= DeltaTime;
		if (quickie[mPlayerNum] < 0.0f) {
			quickie[mPlayerNum] += 0.01f;
			pfCopyMat(mat, mMatrix);
			mat[PF_T][PF_Z] = mHoverHeight;
			pfAddScaledVec3(mat[PF_T], mat[PF_T], 1.0f, mat[PF_Y]);
			Projectile::Launch(PROJECTILE_TYPE_SLINGSHOT, mat, this);
		}
	}
	if (gTripleCrossbowTimer[mPlayerNum] > 0.0f) {
		r1 = 100.0f * (1.0f -  gTripleCrossbowTimer[mPlayerNum]);
		gTripleCrossbowTimer[mPlayerNum] -= DeltaTime;
		r2 = 100.0f * (1.0f -  gTripleCrossbowTimer[mPlayerNum]);
		r1 *= r1;
		r2 *= r2;
		for (i = 0; i < gCells->mNumCells; i++) {
			gCells->GetPosFromIndex(i, pos);
			pfSubVec3(pos, pos, gTripleCrossbowPos[mPlayerNum]);
			dist = pfDotVec3(pos, pos);
			if (dist >= r1 && dist < r2) {
				if (RANDOM0TO1 < 0.5f) {
					gCells->mCells[i].goodTimer = 1.0f;
					gCells->mCells[i].flags |= CELL_FLAG_GOOD;
					if (RANDOM0TO1 < 0.1f) {
						gCells->mCells[i].flags |= CELL_FLAG_SHOT;
					}
				}
			}
		}
	}
	if (gTripleSwordTimer[mPlayerNum] > 0.0f) {
		r1 = 100.0f * (1.0f -  gTripleSwordTimer[mPlayerNum]);
		gTripleSwordTimer[mPlayerNum] -= DeltaTime;
		r2 = 100.0f * (1.0f -  gTripleSwordTimer[mPlayerNum]);
		r1 *= r1;
		r2 *= r2;
		for (i = 0; i < gCells->mNumCells; i++) {
			gCells->GetPosFromIndex(i, pos);
			pfSubVec3(pos, pos, gTripleSwordPos[mPlayerNum]);
			dist = pfDotVec3(pos, pos);
			if (dist >= r1 && dist < r2) {
				pfNormalizeVec3(pos);
				dot = pfDotVec3(pos, mDrawMatrix[PF_Y]);
				if (dot > 0.7f) {
					if (RANDOM0TO1 < 0.2f) {
						gCells->mCells[i].flags |= CELL_FLAG_SHOT;
					}
				}
			}
		}
	}
}



