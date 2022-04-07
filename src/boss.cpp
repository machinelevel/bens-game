
/************************************************************\
	boss.cpp
	Files for controlling the boss in Ben's project
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
#include "draw.h"
#include "boss.h"
#include "slides.h"
#include "sound.h"

Boss	*gBoss = NULL;

Boss::Boss()
{
	mBossType = BOSS_TYPE_ROBARF;
	pfMakeIdentMat(mMatrix);
	pfCopyMat(mLastMatrix, mMatrix);
	pfSetVec3(mTargetPos, 0, 0, 0);
	mInnerRange = 1.0f;
	mOuterRange = 1.0f;
	mDead = true;
}

Boss::~Boss()
{
}

void Boss::Reset(int32 type)
{
	int		i;
	float	xmin, xmax, ymin, ymax;

	mBossType = type;
	mDead = false;
	mRelocate = false;
	mHealth = 1.0f;
	mInnerRange = 6.0f;
	mOuterRange = 8.0f;
	gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);
	mMatrix[PF_T][PF_X] = RANDOM_IN_RANGE(xmin, xmax);
	mMatrix[PF_T][PF_Y] = RANDOM_IN_RANGE(ymin, ymax);
	mMatrix[PF_T][PF_Z] = 0.0f;
	mTargetPos[PF_X] = RANDOM_IN_RANGE(xmin, xmax);
	mTargetPos[PF_Y] = RANDOM_IN_RANGE(ymin, ymax);
	mTargetPos[PF_Z] = 0.0f;
	mDesiredFacing = 0.0f;
	mCurrentFacing = 0.0f;
	mVoiceTimer = 0.0f;

	mNextLaunchTimer = 20.0f;

	mLaunchTravelTime = 5.0f;
	mLaunchGravity = -7.0f;
	mLaunchFromHeight = 5.0f;
	mLaunchUpSpeed = -(mLaunchFromHeight/mLaunchTravelTime + 0.5f*mLaunchGravity*mLaunchTravelTime);
	for (i = 0; i < BOSS_MAX_LAUNCH; i++) {
		mLaunchStage[i] = BOSS_LAUNCH_STATE_OFF;
		mLaunchLifeTimer[i] = 0.0f;
	}
	switch (type) {
		case BOSS_TYPE_ICEMAN:
			mLaunchTexID = TEXID_LAUNCH_SNOW;
			break;
		case BOSS_TYPE_ROBARF:
			mLaunchTexID = TEXID_LAUNCH_BARF;
			break;
		case BOSS_TYPE_VAMPIRE:
			mLaunchTexID = TEXID_LAUNCH_BATS;
			break;
		case BOSS_TYPE_EVILCHICKEN:
			mLaunchTexID = TEXID_LAUNCH_POX;
			break;
		case BOSS_TYPE_TORNADO:
			mLaunchTexID = TEXID_LAUNCH_DUST;
			break;
		case BOSS_TYPE_QBALL:
			mLaunchTexID = TEXID_LAUNCH_BALL;
			break;
		case BOSS_TYPE_FIREMAN:
		default:
			mLaunchTexID = TEXID_LAUNCH_FIRE;
			break;
	}
}

void Boss::ManageLaunches(void)
{
	int			i;
	static int	playerTarget = 0;
	Glider		*g;

	for (i = 0; i < BOSS_MAX_LAUNCH; i++) {
		if (mDead) {
			mLaunchStage[i] = BOSS_LAUNCH_STATE_OFF;
		} else if (mLaunchStage[i] == BOSS_LAUNCH_STATE_AIRBORNE) {
			mLaunchVel[i][PF_Z] += mLaunchGravity * DeltaTime;
			pfAddScaledVec3(mLaunchMat[i][PF_T], mLaunchMat[i][PF_T], DeltaTime, mLaunchVel[i]);

			pfPreRotMat(mLaunchMat[i], 180.0f*DeltaTime, 0, 0, 1, mLaunchMat[i]);
			pfPreRotMat(mLaunchMat[i], 180.0f*DeltaTime, 1, 0, 0, mLaunchMat[i]);
				
			if (mLaunchMat[i][PF_T][PF_Z] < 0.0f) {
				mLaunchStage[i] = BOSS_LAUNCH_STATE_LANDED;
				pfSetVec3(mLaunchMat[i][PF_X], 1, 0, 0);
				pfSetVec3(mLaunchMat[i][PF_Y], 0, 1, 0);
				pfSetVec3(mLaunchMat[i][PF_Z], 0, 0, 1);
				pfPreRotMat(mLaunchMat[i], RANDOM_IN_RANGE(0.0f,360.0f), 0, 0, 1, mLaunchMat[i]);
				mLaunchMat[i][PF_T][PF_Z] = RANDOM_IN_RANGE(0.0f, 0.1f);
				pfSetVec3(mLaunchVel[i], 0, 0, 0);
				playSound2D(SOUND_BARF_SPLAT, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
				for (g = gActiveGliderList; g; g = g->mNext) {
				}
			}
		} else if (mLaunchStage[i] == BOSS_LAUNCH_STATE_CHASE) {
			mLaunchLifeTimer[i] -= DeltaTime;

			switch (mLaunchTexID) {
				case TEXID_LAUNCH_BATS:
					pfAddScaledVec3(mLaunchMat[i][PF_T], mLaunchMat[i][PF_T], DeltaTime, mLaunchVel[i]);
					mLaunchMat[i][PF_T][PF_Z] = 0.0f;
				//	pfPreRotMat(mLaunchMat[i], 360.0f*DeltaTime, 0, 0, 1, mLaunchMat[i]);
					break;
				case TEXID_LAUNCH_DUST:
				case TEXID_LAUNCH_BALL:
					pfAddScaledVec3(mLaunchMat[i][PF_T], mLaunchMat[i][PF_T], DeltaTime, mLaunchVel[i]);
					mLaunchMat[i][PF_T][PF_Z] = 0.0f;
					pfPreRotMat(mLaunchMat[i], 360.0f*DeltaTime, 0, 0, 1, mLaunchMat[i]);
					//mLaunchRadius[i] += 100.0f * DeltaTime;
					//if (mLaunchRadius[i] > 6.0f) mLaunchRadius[i] = 6.0f;
					//mLaunchThickness[i] -= 1.0f * DeltaTime;
					//if (mLaunchThickness[i] < 0.5f) mLaunchThickness[i] = 0.5f;
					break;
			}
			if (mLaunchLifeTimer[i] < 0.0f) {
				mLaunchStage[i] = BOSS_LAUNCH_STATE_OFF;
			}
		} else if (mLaunchStage[i] == BOSS_LAUNCH_STATE_LANDED) {
			mLaunchLifeTimer[i] -= DeltaTime;

			switch (mLaunchTexID) {
				case TEXID_LAUNCH_POX:
					mLaunchRadius[i] += 100.0f * DeltaTime;
					if (mLaunchRadius[i] > 6.0f) mLaunchRadius[i] = 6.0f;
					mLaunchThickness[i] -= 1.0f * DeltaTime;
					if (mLaunchThickness[i] < 0.5f) mLaunchThickness[i] = 0.5f;
					break;
				case TEXID_LAUNCH_FIRE:
				case TEXID_LAUNCH_SNOW:
					mLaunchRadius[i] += 100.0f * DeltaTime;
					if (mLaunchRadius[i] > 6.0f) mLaunchRadius[i] = 6.0f;
					mLaunchThickness[i] -= 1.0f * DeltaTime;
					if (mLaunchThickness[i] < 0.5f) mLaunchThickness[i] = 0.5f;
					break;
				case TEXID_LAUNCH_BARF:
				default:
					mLaunchRadius[i] += 1.0f * DeltaTime;
					if (mLaunchRadius[i] > 6.0f) mLaunchRadius[i] = 6.0f;
					mLaunchThickness[i] -= 0.2f * DeltaTime;
					if (mLaunchThickness[i] < 1.0f) mLaunchThickness[i] = 1.0f;
					break;
			}
			if (mLaunchLifeTimer[i] < 0.0f) {
				mLaunchStage[i] = BOSS_LAUNCH_STATE_OFF;
			}
		}
	}
}

void Boss::DrawLaunches(void)
{
	int			i, j;
	static int	playerTarget = 0;
	Glider		*g;
	pfVec3		verts[4], extrusion;
	float		alpha = 1.0f, size = 2.0f, thick = 1.0f;
	static float flap = 0;
	float		sval, cval, zz;

	UseLibTexture(mLaunchTexID);

	flap += 2.0f * 360.0f * DeltaTime;
	if (flap > 360.0f) flap -= 360.0f;
	pfSinCos(flap, &sval, &cval);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glDepthMask(true);

	for (i = 0; i < BOSS_MAX_LAUNCH; i++) {
		size = mLaunchRadius[i];
		thick = mLaunchThickness[i];
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf((float*)mLaunchMat[i]);

		if (mLaunchStage[i] == BOSS_LAUNCH_STATE_AIRBORNE) {
			switch (mLaunchTexID) {
				case TEXID_LAUNCH_FIRE:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					glDepthMask(false);
					alpha = 0.25f;
					
					glColor4f(1, 0.3f, 0, alpha);

					pfSetVec3(verts[0], -size,  size, 0);
					pfSetVec3(verts[1], -size, -size, 0);
					pfSetVec3(verts[2],  size, -size, 0);
					pfSetVec3(verts[3],  size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);

					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
					pfScaleVec3(extrusion, -1.0f, extrusion);
					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
					break;
				case TEXID_LAUNCH_POX:
				case TEXID_LAUNCH_SNOW:
				case TEXID_LAUNCH_BARF:
				default:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					glDepthMask(false);
					alpha = 0.25f;
					
					glColor4f(1, 1, 1, alpha);

					pfSetVec3(verts[0], -size,  size, 0);
					pfSetVec3(verts[1], -size, -size, 0);
					pfSetVec3(verts[2],  size, -size, 0);
					pfSetVec3(verts[3],  size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);

					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
					pfScaleVec3(extrusion, -1.0f, extrusion);
					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
					break;
			}
		} else if (mLaunchStage[i] == BOSS_LAUNCH_STATE_CHASE) {
			switch (mLaunchTexID) {
				case TEXID_LAUNCH_DUST:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(false);
					
					alpha = 1.0f * mLaunchLifeTimer[i];
					if (alpha > 1.0f) alpha = 1.0f;
					
					alpha *= 0.5f;
					glColor4f(1, 1, 1, alpha);

					pfSetVec3(verts[0], -size,  size, 0);
					pfSetVec3(verts[1], -size, -size, 0);
					pfSetVec3(verts[2],  size, -size, 0);
					pfSetVec3(verts[3],  size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);

					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
					break;
				case TEXID_LAUNCH_BATS:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(false);
					
					alpha = 1.0f * mLaunchLifeTimer[i];
					if (alpha > 1.0f) alpha = 1.0f;
					zz = 1.5f;
					alpha *= 0.5f;
					glColor4f(1, 1, 1, alpha);

					pfSetVec3(verts[0],  2*size,  size, zz+0.5f*sval);
					pfSetVec3(verts[1],  2*size, -size, zz+0.5f*sval);
					pfSetVec3(verts[2],  0*size, -size, zz+0);
					pfSetVec3(verts[3],  0*size,  size, zz+0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);

					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
					for (j = 0; j < 4; j++) {
						pfAddVec3(verts[j], verts[j], extrusion);
					}
					DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);

					pfSetVec3(verts[0], -2*size,  size, zz+0.5f*sval);
					pfSetVec3(verts[1], -2*size, -size, zz+0.5f*sval);
					pfSetVec3(verts[2],  0*size, -size, zz+0);
					pfSetVec3(verts[3],  0*size,  size, zz+0);

					DrawChiaQuad(verts, extrusion, 8, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
					for (j = 0; j < 4; j++) {
						pfAddVec3(verts[j], verts[j], extrusion);
					}
					DrawChiaQuad(verts, extrusion, 8, 0, alpha, CHIA_FLAG_TAPER_ALPHA);

					/**** shadow ****/
					if (1) {
						zz = 0.0f;
						alpha = 0.5f;
						glColor4f(0,0,0,alpha);
						pfSetVec3(verts[0], -2*size,  size, zz);
						pfSetVec3(verts[1], -2*size, -size, zz);
						pfSetVec3(verts[2],  0*size, -size, zz);
						pfSetVec3(verts[3],  0*size,  size, zz);
						DrawChiaQuad(verts, extrusion, 1, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
						pfSetVec3(verts[0],  2*size,  size, zz);
						pfSetVec3(verts[1],  2*size, -size, zz);
						pfSetVec3(verts[2],  0*size, -size, zz);
						pfSetVec3(verts[3],  0*size,  size, zz);
						DrawChiaQuad(verts, extrusion, 1, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
					}
					break;
				case TEXID_LAUNCH_BALL:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(false);
					
					alpha = 1.0f * mLaunchLifeTimer[i];
					if (alpha > 1.0f) alpha = 1.0f;
					
					alpha *= 0.5f;
					glColor4f(1, 1, 1, alpha);

					pfSetVec3(verts[0],  -size,  size, 0);
					pfSetVec3(verts[1],  -size, -size, 0);
					pfSetVec3(verts[2],   size, -size, 0);
					pfSetVec3(verts[3],   size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);

					DrawChiaQuad(verts, extrusion, 8, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_REVERSE_ORDER);
					for (j = 0; j < 4; j++) {
						pfAddVec3(verts[j], verts[j], extrusion);
					}
					DrawChiaQuad(verts, extrusion, 8, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
					break;
				case TEXID_LAUNCH_POX:
				case TEXID_LAUNCH_SNOW:
				case TEXID_LAUNCH_BARF:
				default:
					break;
					break;
			}
		} else if (mLaunchStage[i] == BOSS_LAUNCH_STATE_LANDED) {
			switch (mLaunchTexID) {
				case TEXID_LAUNCH_FIRE:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(true);

					alpha = 1.0f * mLaunchLifeTimer[i];
					if (alpha > 1.0f) alpha = 1.0f;
					glColor4f(1, RANDOM_IN_RANGE(0,1), 0, alpha);
					pfSetVec3(verts[0], -size,  size, 0);
					pfSetVec3(verts[1], -size, -size, 0);
					pfSetVec3(verts[2],  size, -size, 0);
					pfSetVec3(verts[3],  size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);
					DrawChiaQuad(verts, extrusion, 16, 0, 1.0f, CHIA_FLAG_TAPER_ALPHA);
					break;
				case TEXID_LAUNCH_POX:
				case TEXID_LAUNCH_SNOW:
				case TEXID_LAUNCH_BARF:
				default:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(true);

					alpha = 1.0f * mLaunchLifeTimer[i];
					if (alpha > 1.0f) alpha = 1.0f;
					glColor4f(1, 1, 1, alpha);
					pfSetVec3(verts[0], -size,  size, 0);
					pfSetVec3(verts[1], -size, -size, 0);
					pfSetVec3(verts[2],  size, -size, 0);
					pfSetVec3(verts[3],  size,  size, 0);
					pfSetVec3(extrusion, 0.0f, 0.0f, thick);
					DrawChiaQuad(verts, extrusion, 16, 0, 1.0f, CHIA_FLAG_TAPER_ALPHA);
					break;
			}
		}
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Boss::Fire(int32 weapon)
{
	int			i;
	static int	playerTarget = 0;
	Glider		*g;
	float		aim = 6.0f;

//if (mNextLaunchTimer > 1) mNextLaunchTimer = 1;
	mNextLaunchTimer -= DeltaTime;
	if (mNextLaunchTimer > 0.0f) return;

	if (gDifficultySetting == DIFFICULTY_EASY) {
		mNextLaunchTimer = RANDOM_IN_RANGE(20.0f, 30.0f);
	} else if (gDifficultySetting == DIFFICULTY_MEDIUM) {
		mNextLaunchTimer = RANDOM_IN_RANGE(2.0f, 10.0f);
	} else {
		mNextLaunchTimer = RANDOM_IN_RANGE(0.5f, 2.0f);
	}

	for (i = 0; i < BOSS_MAX_LAUNCH; i++) {
		if (mLaunchStage[i] == BOSS_LAUNCH_STATE_OFF) {
			playerTarget = (playerTarget + 1) % gNumPlayers;
			g = gGliders[playerTarget];
			SimpleVoice();
			if (g) {
				switch (mLaunchTexID) {
					case TEXID_LAUNCH_DUST:
						playSound2D(SOUND_BARF_LAUNCH, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
						mLaunchStage[i] = BOSS_LAUNCH_STATE_CHASE;
						mLaunchRadius[i] = 1.0f;
						mLaunchThickness[i] = 2.0f;
						mLaunchLifeTimer[i] = 10.0f;
						pfCopyMat(mLaunchMat[i], mMatrix);
						pfPreRotMat(mLaunchMat[i], RANDOM_IN_RANGE(0.0f,360.0f), 0, 0, 1, mLaunchMat[i]);
						mLaunchMat[i][PF_T][PF_Z] = mLaunchFromHeight;
						pfSubVec3(mLaunchVel[i], g->mMatrix[PF_T], mMatrix[PF_T]);
						mLaunchVel[i][PF_X] += RANDOM_IN_RANGE(-aim, aim);
						mLaunchVel[i][PF_Y] += RANDOM_IN_RANGE(-aim, aim);
						pfScaleVec3(mLaunchVel[i], 1.0f/mLaunchTravelTime, mLaunchVel[i]);
						mLaunchVel[i][PF_Z] = mLaunchUpSpeed;
						break;
					case TEXID_LAUNCH_BATS:
						playSound2D(SOUND_BARF_LAUNCH, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
						mLaunchStage[i] = BOSS_LAUNCH_STATE_CHASE;
						mLaunchRadius[i] = 2.0f;
						mLaunchThickness[i] = 0.3f;
						mLaunchLifeTimer[i] = 10.0f;
						pfCopyMat(mLaunchMat[i], mMatrix);
						pfPreRotMat(mLaunchMat[i], 90.0f, 0, 0, 1, mLaunchMat[i]);
						mLaunchMat[i][PF_T][PF_Z] = mLaunchFromHeight;
						pfSubVec3(mLaunchVel[i], g->mMatrix[PF_T], mMatrix[PF_T]);
						mLaunchVel[i][PF_X] += RANDOM_IN_RANGE(-aim, aim);
						mLaunchVel[i][PF_Y] += RANDOM_IN_RANGE(-aim, aim);
						pfScaleVec3(mLaunchVel[i], 1.0f/mLaunchTravelTime, mLaunchVel[i]);
						mLaunchVel[i][PF_Z] = mLaunchUpSpeed;
						break;
					case TEXID_LAUNCH_BALL:
						playSound2D(SOUND_BARF_LAUNCH, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
						mLaunchStage[i] = BOSS_LAUNCH_STATE_CHASE;
						mLaunchRadius[i] = 1.0f;
						mLaunchThickness[i] = 0.7f;
						mLaunchLifeTimer[i] = 10.0f;
						pfCopyMat(mLaunchMat[i], mMatrix);
						pfPreRotMat(mLaunchMat[i], RANDOM_IN_RANGE(0.0f,360.0f), 0, 0, 1, mLaunchMat[i]);
						mLaunchMat[i][PF_T][PF_Z] = mLaunchFromHeight;
						pfSubVec3(mLaunchVel[i], g->mMatrix[PF_T], mMatrix[PF_T]);
						mLaunchVel[i][PF_X] += RANDOM_IN_RANGE(-aim, aim);
						mLaunchVel[i][PF_Y] += RANDOM_IN_RANGE(-aim, aim);
						pfScaleVec3(mLaunchVel[i], 1.0f/mLaunchTravelTime, mLaunchVel[i]);
						mLaunchVel[i][PF_Z] = mLaunchUpSpeed;
						break;
					case TEXID_LAUNCH_POX:
						playSound2D(SOUND_BARF_LAUNCH, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
						mLaunchStage[i] = BOSS_LAUNCH_STATE_AIRBORNE;
						mLaunchRadius[i] = 2.0f;
						mLaunchThickness[i] = 0.5f;
						mLaunchLifeTimer[i] = 10.0f;
						pfCopyMat(mLaunchMat[i], mMatrix);
						pfPreRotMat(mLaunchMat[i], RANDOM_IN_RANGE(0.0f,360.0f), 0, 0, 1, mLaunchMat[i]);
						mLaunchMat[i][PF_T][PF_Z] = mLaunchFromHeight;
						pfSubVec3(mLaunchVel[i], g->mMatrix[PF_T], mMatrix[PF_T]);
						mLaunchVel[i][PF_X] += RANDOM_IN_RANGE(-aim, aim);
						mLaunchVel[i][PF_Y] += RANDOM_IN_RANGE(-aim, aim);
						pfScaleVec3(mLaunchVel[i], 1.0f/mLaunchTravelTime, mLaunchVel[i]);
						mLaunchVel[i][PF_Z] = mLaunchUpSpeed;
						break;
					case TEXID_LAUNCH_SNOW:
					case TEXID_LAUNCH_BARF:
					case TEXID_LAUNCH_FIRE:
					default:
						playSound2D(SOUND_BARF_LAUNCH, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
						mLaunchStage[i] = BOSS_LAUNCH_STATE_AIRBORNE;
						mLaunchRadius[i] = 2.0f;
						mLaunchThickness[i] = 1.0f;
						mLaunchLifeTimer[i] = 10.0f;
						pfCopyMat(mLaunchMat[i], mMatrix);
						pfPreRotMat(mLaunchMat[i], RANDOM_IN_RANGE(0.0f,360.0f), 0, 0, 1, mLaunchMat[i]);
						mLaunchMat[i][PF_T][PF_Z] = mLaunchFromHeight;
						pfSubVec3(mLaunchVel[i], g->mMatrix[PF_T], mMatrix[PF_T]);
						mLaunchVel[i][PF_X] += RANDOM_IN_RANGE(-aim, aim);
						mLaunchVel[i][PF_Y] += RANDOM_IN_RANGE(-aim, aim);
						pfScaleVec3(mLaunchVel[i], 1.0f/mLaunchTravelTime, mLaunchVel[i]);
						mLaunchVel[i][PF_Z] = mLaunchUpSpeed;
						break;
				}
			}
			return;
		}
	}
}

void Boss::Damage(float amount)
{
	mHealth -= amount;
	/**** if he gets too small, it's over. ****/
	if (mHealth < 0.2f) {
		mHealth = 0.0f;
	}
	SimpleVoice();
	if (mHealth <= 0.0f) {
		mDead = true;
		gCells->mLifeUpdateTimeInterval = 0.1f;	/**** speed them up! ****/
	} else {
		mRelocate = true;
	}
}

void Boss::Think(void)
{
	OneCell	*pcell, *pcell2;
	int32	hc, vc, ic, h, v, d;
	static float	mutateTimer = gCells->mLifeUpdateTimeInterval;
	float			mutateSeconds = 0.5f;
	bool			mutate = false;
	float			mutationChance = 0.1f;
	bool			damaged = false;

	if (mDead) return;

	if (gDifficultySetting == DIFFICULTY_EASY) {
		if (mHealth > 0.5f) mHealth = 0.5f;
	} else if (gDifficultySetting == DIFFICULTY_MEDIUM) {
	} else {
	}

	mVoiceTimer -= DeltaTime;
	mutateTimer += DeltaTime;
	if (mutateTimer > mutateSeconds) {
		mutate = true;
		mutateTimer -= mutateSeconds;
	}


	/**** Calculate the desired facing ****/
	if (1) {
		Glider	*g, *gClose;
		pfVec3	vToPlayer, vToCamera;
		float	aToPlayer, aToCamera, dist, distClose;

		gClose = NULL;
		distClose = -1.0f;
		for (g = gActiveGliderList; g; g = g->mNext) {
			dist = pfSqrDistancePt3(g->mMatrix[PF_T], mMatrix[PF_T]);
			if (dist < distClose || gClose == NULL) {
				gClose = g;
				distClose = dist;
			}
		}

		if (gClose) {
			pfSubVec3(vToPlayer, gClose->mMatrix[PF_T], mMatrix[PF_T]);
			aToPlayer = pfArcTan2(vToPlayer[PF_Y], vToPlayer[PF_X]);

			pfSubVec3(vToCamera, GlobalCameraMatrix[PF_T], mMatrix[PF_T]);
			aToCamera = pfArcTan2(vToCamera[PF_Y], vToCamera[PF_X]);

			mDesiredFacing = aToPlayer;

			mCurrentFacing = mDesiredFacing;
		}
	}

	Fire(0);

	/**** Infect the nearby cells ****/
	pcell = gCells->GetClosestCell(mMatrix[PF_T], &hc, &vc, &ic);
	if (pcell) {
		for (v = vc - mOuterRange; v <= vc + mOuterRange; v++) {
			for (h = hc - mOuterRange; h <= hc + mOuterRange; h++) {
				pcell2 = gCells->GetHVCell(h, v, &ic);
				if (pcell2) {
					d = (h-hc)*(h-hc)+(v-vc)*(v-vc);
					if (d <= (mInnerRange*mHealth)*(mInnerRange*mHealth)) {
						SETFLAG(pcell2->flags, CELL_FLAG_BOSS);
//						SETFLAG(pcell2->shift, CELL_SHIFT_ON|CELL_SHIFT_ON_NEXT);
						if (pcell2->flags & CELL_FLAG_SHOT) {
							if (gHealthLevel[HEALTH_TYPE_HEALTH] > 0.0f
								&& gHealthLevel[HEALTH_TYPE_AMMO] > 0.0f
								&& gHealthLevel[HEALTH_TYPE_ATTITUDE] > 0.0f) {
								CLRFLAG(pcell2->flags, CELL_FLAG_SHOT);
								damaged = true;
							}
						}
					}
					if (mutate && RANDOM0TO1 < mutationChance) {
						SETFLAG(pcell2->shift, CELL_SHIFT_ON_NEXT);
					}
				}
			}
		}
	}

	if (damaged) {
		Damage(0.1f);
	}

	/**** time to move? ****/
	if (mRelocate) {
		float	xmin, xmax, ymin, ymax;
		gCells->GetBounds(&xmin, &xmax, &ymin, &ymax);
		mTargetPos[PF_X] = RANDOM_IN_RANGE(xmin, xmax);
		mTargetPos[PF_Y] = RANDOM_IN_RANGE(ymin, ymax);
		mTargetPos[PF_Z] = 0.0f;
		mRelocate = false;
	}
}

void Boss::Move(void)
{
	float		speed = 25.0f;
	float		dist;
	pfMatrix	m;

	ManageLaunches();

	if (mDead) return;


	/**** Set the facing ****/
	pfMakeEulerMat(m, mCurrentFacing, 0.0f, 0.0f);
	pfCopyVec3(m[PF_T], mMatrix[PF_T]);
	pfCopyMat(mMatrix, m);

	pfSubVec3(mVelocity, mTargetPos, mMatrix[PF_T]);
	dist = pfNormalizeVec3(mVelocity);
	if (dist > speed) {
		pfScaleVec3(mVelocity, speed, mVelocity);
	} else {
		pfScaleVec3(mVelocity, dist, mVelocity);
	}
	pfAddScaledVec3(mMatrix[PF_T], mMatrix[PF_T], DeltaTime, mVelocity);
}

void Boss::React(void)
{
	if (mDead) return;
}

pfVec3		vList_roBarf[] =	{
								{ 1, 3, 4},
								{-1, 3, 4},
								{-3, 1, 4},
								{-3,-1, 4},
								{-1,-3, 4},
								{ 1,-3, 4},
								{ 3,-1, 4},
								{ 3, 1, 4},
								{ 1*3, 3*3, 0},
								{-1*3, 3*3, 0},
								{-3*3, 1*3, 0},
								{-3*3,-1*3, 0},
								{-1*3,-3*3, 0},
								{ 1*3,-3*3, 0},
								{ 3*3,-1*3, 0},
								{ 3*3, 1*3, 0},
								};

pfVec4		cList_roBarf[] =	{
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								{1,1,1,1},
								};

pfVec2		tList_roBarf[] =	{
								{0,0},
								{1,0},
								{0,0},
								{1,0},
								{0,0},
								{1,0},
								{0,0},
								{1,0},
								
								{0,1},
								{1,1},
								{0,1},
								{1,1},
								{0,1},
								{1,1},
								{0,1},
								{1,1},
								};

uint16		iList_roBarf[] =	{
								0, 1, 1+8, 0+8,
								1, 2, 2+8, 1+8,
								2, 3, 3+8, 2+8,
								3, 4, 4+8, 3+8,
								4, 5, 5+8, 4+8,
								5, 6, 6+8, 5+8,
								6, 7, 7+8, 6+8,
								7, 0, 0+8, 7+8,
								};

quickModel model_roBarf_body = {
	{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}},
	{1,1,1},
	vList_roBarf,
	NULL,
	cList_roBarf,
	tList_roBarf,
	iList_roBarf,
	sizeof(iList_roBarf)/sizeof(iList_roBarf[0]),
	0,
	QUICKMODEL_FLAG_QUADS | QUICKMODEL_FLAG_COLOR_PER_VERT,
};

void Boss::Draw(void)
{
	pfMatrix	mat;
	float		alpha;

	DrawLaunches();

	if (mDead) return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf((float*)mMatrix);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	pfVec3 verts[4], extrusion;
	float bossThickness = 10.0f * mHealth;
	float bossSize = 10.0f * mHealth;
	float bossHeight = 0.0f;

	switch (mBossType) {
	case BOSS_TYPE_QBALL:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS1A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS1F);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.5f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.5f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.5f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.5f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_EVILCHICKEN:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS4A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.5f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS4F);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.25f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.25f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.25f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.25f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_TORNADO:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS6A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.5f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_TORNADO_EFFECT);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS6F);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.25f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.25f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.25f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.25f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_VAMPIRE:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS7A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.3f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS7F);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.25f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.25f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.25f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.25f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_ICEMAN:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS5A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.5f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS5F);

//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.25f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.25f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.25f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.25f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
//		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_FIREMAN:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		UseLibTexture(TEXID_BOSS2A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.5f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA|CHIA_FLAG_FIRE_EFFECT);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS2F);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.5f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.5f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.5f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.5f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	case BOSS_TYPE_ROBARF:
		/**** Draw the guy ****/
		alpha = 0.3f;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);

		glDepthMask(false);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		UseLibTexture(TEXID_BOSS3A);
		glColor4f(1, 1, 1, alpha);
		pfSetVec3(verts[0], 0.0f, -bossSize, bossHeight);
		pfSetVec3(verts[1], 0.0f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], 0.0f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], 0.0f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness*0.5f, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 16, 0, alpha, CHIA_FLAG_TAPER_ALPHA);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glDepthMask(true);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		UseLibTexture(TEXID_BOSS3F);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1, 1, 1, 0.5f);
		pfSetVec3(verts[0], bossThickness*0.25f, -bossSize, bossHeight);
		pfSetVec3(verts[1], bossThickness*0.25f, -bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[2], bossThickness*0.25f,  bossSize, bossHeight + 2*bossSize);
		pfSetVec3(verts[3], bossThickness*0.25f,  bossSize, bossHeight);
		pfSetVec3(extrusion, bossThickness, 0.0f, 0.0f);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);
		DrawChiaQuad(verts, extrusion, 1, 0, 1.0f, 0);

		break;
	default:
		pfSetVec3(model_roBarf_body.scale, 1.0f * mHealth, 1.0f * mHealth, 1.0f * mHealth);
		DrawQuickModel(&model_roBarf_body);
		break;
	}


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void Boss::SimpleVoice(void)
{
	int id;
	float pitch;

	if (mVoiceTimer <= 0.0f) {
		if (mBossType == BOSS_TYPE_EVILCHICKEN) {
			id = SOUND_CHICKEN_VOICE;
			pitch = RANDOM_IN_RANGE(0.5f, 0.7f);
		} else {
			id = SOUND_BOSS_VOICE;
			pitch = RANDOM_IN_RANGE(0.1f, 0.2f);
		}
		playSound2D(id, 1.0f, pitch);
		mVoiceTimer = RANDOM_IN_RANGE(5.0f, 10.0f);
	}
}


