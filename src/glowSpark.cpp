
/************************************************************\
	GlowSpark.cpp
	Files for controlling the GlowSparks in Ben's project
\************************************************************/

#include <stdio.h>
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
#include "sound.h"

GlowSpark	*GlowSpark::msFirst = NULL;
MiniSparks	*gFireFlies = NULL;
MiniSparks *gFireworks[NUM_FIREWORKS];
float		gFireworksTimer = 0.0f;

GlowSpark::GlowSpark(int32 type, int32 numSparks)
{
	int32	i;

	/**** add to beginning of list ****/
	mPrev = NULL;
	mNext = msFirst;
	if (mNext) mNext->mPrev = this;
	msFirst = this;

	mType = type;
	mGlowSize = 1.0f;
	pfSetVec3(mPosition, 0, 0, 0);
	mLifeTimer = 0.0f;
	mFlags = 0;
	mNumSparks = numSparks;
	mSparks = new Spark[numSparks];
	for (i = 0; i < mNumSparks; i++) {
		mSparks[i].mFlags = 0;
	}
	mGlowTexture = TEXID_GLOWSPARK;

	switch (mType) {
	case GLOWSPARK_TYPE_NORMAL:
		mGlowSize = 1.0f;
		pfSetVec4(mGlowColor, 1,1,1,1);
		mGlowTexture = TEXID_GLOWSPARK;
		break;
	case GLOWSPARK_TYPE_BIGSOFT:
		mGlowSize = 3.0f;
		pfSetVec4(mGlowColor, 1,1,1,0.25f);
		mGlowTexture = TEXID_GLOWSPARK;
		break;
	case GLOWSPARK_TYPE_SWORD:
		mGlowSize = 3.0f;
		pfSetVec4(mGlowColor, 1,1,1,0.25f);
		mGlowTexture = TEXID_GLOWSWEEP;
		break;
	default:
		break;
	}
}

GlowSpark::~GlowSpark()
{
	/**** remove from list ****/
	if (msFirst == this) msFirst = mNext;
	if (mPrev) mPrev->mNext = mNext;
	if (mNext) mNext->mPrev = mPrev;

	if (mSparks) delete[] mSparks;
}

void GlowSpark::Think(void)
{
	if (!(mFlags & GLOWSPARK_FLAG_ACTIVE)) return;
}

void GlowSpark::Move(void)
{
	float	floor = gCells->mCenter[PF_Z];
	int32	i;
	pfVec3	force = {0,0,0};

	if (!(mFlags & GLOWSPARK_FLAG_ACTIVE)) return;

	for (i = 0; i < mNumSparks; i++) {
		Spark	*spark = &(mSparks[i]);
		pfAddScaledVec3(spark->mPosition, spark->mPosition, DeltaTime, spark->mVelocity);
	}

	pfSubVec3(mVelocity, mPosition, mLastPosition);
	if (DeltaTime > 0.0f) pfScaleVec3(mVelocity, 1.0f/DeltaTime, mVelocity);
	pfCopyVec3(mLastPosition, mPosition);

	switch (mType) {
	case GLOWSPARK_TYPE_SWORD:
		mGlowSize += 100.0f * DeltaTime;
		mGlowColor[3] -= 10.0f * DeltaTime;
		if (mGlowColor[3] < 0.0f) mGlowColor[3] = 0.0f;
		break;
	case GLOWSPARK_TYPE_NORMAL:
	case GLOWSPARK_TYPE_BIGSOFT:
	default:
		break;
	}
}

void GlowSpark::React(void)
{
}

void GlowSpark::Draw(void)
{
	int32		i;
	pfMatrix	mat;
	pfVec3		up, over, camPos, toCam, camUp, zVec;
	pfVec3		verts[4];
	float		scale;

	if (!(mFlags & GLOWSPARK_FLAG_ACTIVE)) return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(true);

	UseLibTexture(mGlowTexture);
	
	pfCopyVec3(camPos, GlobalCameraMatrix[PF_T]);

	/**** Now construct our up and over vectors ****/
	switch (mType) {
	case GLOWSPARK_TYPE_SWORD:
		pfScaleVec3(up, -1.0f, mVelocity);
		up[PF_Z] = 0.0f;
//pfSetVec3(up, 1.0f, 0.0f, 0.0f);
		pfSetVec3(zVec, 0.0f, 0.0f, 1.0f);
		pfCrossVec3(over, zVec, up);
		pfNormalizeVec3(up);
		pfNormalizeVec3(over);
		break;
	case GLOWSPARK_TYPE_NORMAL:
	case GLOWSPARK_TYPE_BIGSOFT:
	default:
		pfSubVec3(toCam, camPos, mPosition);
		pfCopyVec3(camUp, GlobalCameraMatrix[PF_Z]);
		pfAddScaledVec3(camUp, camUp, RANDOM_IN_RANGE(-1.0f, 1.0f), GlobalCameraMatrix[PF_X]);
		pfCrossVec3(over, toCam, camUp);
		pfCrossVec3(up, over, toCam);
		pfNormalizeVec3(up);
		pfNormalizeVec3(over);
		pfNormalizeVec3(toCam);
		break;
	}

	scale = 0.5f * mGlowSize;
	pfAddScaledVec3(verts[0], mPosition,  scale, up);
	pfAddScaledVec3(verts[1], mPosition,  scale, up);
	pfAddScaledVec3(verts[2], mPosition, -scale, up);
	pfAddScaledVec3(verts[3], mPosition, -scale, up);
	pfAddScaledVec3(verts[0], verts[0],  scale, over);
	pfAddScaledVec3(verts[1], verts[1], -scale, over);
	pfAddScaledVec3(verts[2], verts[2], -scale, over);
	pfAddScaledVec3(verts[3], verts[3],  scale, over);
	glBegin(GL_QUADS);
	glColor4fv(mGlowColor);
	glTexCoord2f(0, 0);
	glVertex3fv(verts[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(verts[1]);
	glTexCoord2f(1, 1);
	glVertex3fv(verts[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(verts[3]);
	glEnd();

	glPopMatrix();
}

void GlowSpark::ThinkAll(void)
{
	GlowSpark	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Think();
	}
}

void GlowSpark::MoveAll(void)
{
	GlowSpark	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Move();
	}
}

void GlowSpark::ReactAll(void)
{
	GlowSpark	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->React();
	}
}

void GlowSpark::DrawAll(void)
{
	GlowSpark	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Draw();
	}
}







/********************************************************************************/
/********************************************************************************/
/********************************************************************************/





MiniSparks::MiniSparks(int32 type, int32 numSparks)
{
	int32	i;

	mType = type;
	pfSetVec3(mCenter, 0, 0, 0);
	mSpread = 30.0f;
	mLifeTimer = 0.0f;
	mFlags = 0;
	mBurst = false;
	mDrag = 1.0f;
	mAcc = 1000.0f;
	mBuddyAcc = 100.0f;
	mNumSparks = numSparks;
	mSparks = new Spark[numSparks];
	for (i = 0; i < mNumSparks; i++) {
		Spark	*sp = &(mSparks[i]);
		sp->mFlags = 0;
		sp->mLifeTimer = 1.0f;
		sp->mSize = RANDOM_IN_RANGE(5.0f, 10.0f);
		sp->mPosition[PF_X] = mCenter[PF_X] + RANDOM_IN_RANGE(-2000, 2000);
		sp->mPosition[PF_Y] = mCenter[PF_Y] + RANDOM_IN_RANGE(-2000, 2000);
		sp->mPosition[PF_Z] = 0.0f;
		pfCopyVec3(sp->mLastPosition, sp->mPosition);
		sp->mVelocity[PF_X] = RANDOM_IN_RANGE(-mSpread, mSpread);
		sp->mVelocity[PF_Y] = RANDOM_IN_RANGE(-mSpread, mSpread);
		sp->mVelocity[PF_Z] = 0.0f;
		pfSetVec4(sp->mColor, 1, 1, 1, 1);
	}
	mSparkTexture = TEXID_GLOWSPARK;

	switch (mType) {
	case MINISPARK_TYPE_FIREFLY:
		break;
	default:
		break;
	}
}

MiniSparks::~MiniSparks()
{
	if (mSparks) delete[] mSparks;
}

void MiniSparks::Think(void)
{
	mLifeTimer -= UnscaledDeltaTime;
	if (mLifeTimer <= 0.0f) return;
}

void MiniSparks::Move(void)
{
	float	floor = gCells->mCenter[PF_Z];
	int32	i;
	pfVec3	force = {0,0,0};
	pfVec3	dirToCenter, dir;
	float	distToCenter;
	pfVec3	dirToBuddy;
	float	distToBuddy;

	if (mLifeTimer <= 0.0f) return;

	for (i = 0; i < mNumSparks; i++) {
		Spark	*sp = &(mSparks[i]);
		if (RANDOM0TO1 < 0.1f) {
			sp->mColor[3] = 1.0f;
		} else {
			sp->mColor[3] = RANDOM_IN_RANGE(0.0f, 0.4f);
		}
//		sp->mColor[3] = 1.0f;

		pfCopyVec3(dir, sp->mVelocity);
		pfNormalizeVec3(dir);
		pfSubVec3(dirToCenter, mCenter, sp->mPosition);
		distToCenter = pfNormalizeVec3(dirToCenter);
		if (!mBurst) {
			if (distToCenter > mSpread) {
//				pfScaleVec3(sp->mVelocity, mDrag, sp->mVelocity);
				pfAddScaledVec3(sp->mVelocity, sp->mVelocity, -mDrag*UnscaledDeltaTime, sp->mVelocity);
			}
			pfAddScaledVec3(sp->mVelocity, sp->mVelocity, mAcc * UnscaledDeltaTime, dirToCenter);
		}
		if (i > 0) {
			pfSubVec3(dirToBuddy, mSparks[i-1].mPosition, sp->mPosition);
			distToBuddy = pfNormalizeVec3(dirToBuddy);
			pfAddScaledVec3(sp->mVelocity, sp->mVelocity, -mBuddyAcc * UnscaledDeltaTime, dirToBuddy);
		}
		sp->mVelocity[PF_Z] = 0.0f;
		sp->mPosition[PF_Z] = 0.0f;
		pfCopyVec3(sp->mLastPosition, sp->mPosition);
		pfAddScaledVec3(sp->mPosition, sp->mPosition, UnscaledDeltaTime, sp->mVelocity);

//pfSetVec3(sp->mPosition, 100, 100, 0);
//spfSetVec3(sp->mLastPosition, 110, 110, 0);

	}

	switch (mType) {
	case MINISPARK_TYPE_FIREFLY:
		break;
	case MINISPARK_TYPE_FIREWORKS:
		for (i = 0; i < mNumSparks; i++) {
			Spark	*sp = &(mSparks[i]);
			sp->mVelocity[PF_Y] += UnscaledDeltaTime * 800.0f;
		}
		break;
	default:
		break;
	}
}

void MiniSparks::React(void)
{
}

void MiniSparks::Draw(void)
{
	int32		i;
	pfMatrix	mat;
	pfVec3		up, over, zVec = {0,0,1}, dir;
	pfVec3		verts[4], tail;
	float		scale;
	Spark		*sp;
	float		sqDist1, sqDist2;

	if (mLifeTimer <= 0.0f) return;

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	//additive

	UseLibTexture(mSparkTexture);
	
	/**** Now construct our up and over vectors ****/
	switch (mType) {
	case MINISPARK_TYPE_FIREFLY:
	case MINISPARK_TYPE_FIREWORKS:
		glBegin(GL_TRIANGLES);
		for (i = 0; i < mNumSparks; i++) {
			sp = &(mSparks[i]);
			pfCopyVec3(dir, sp->mVelocity);
			pfNormalizeVec3(dir);
			pfScaleVec3(up, sp->mSize, dir);
			pfCrossVec3(over, up, zVec);
			pfAddScaledVec3(verts[0], sp->mPosition,  1, over);
			pfAddScaledVec3(verts[1], sp->mPosition,  1, up);
			pfAddScaledVec3(verts[2], sp->mPosition, -1, over);
			pfAddScaledVec3(verts[3], sp->mPosition, -1, up);

//			pfSubVec3(tail, sp->mLastPosition, sp->mPosition);
			pfScaleVec3(tail, -0.1f, sp->mVelocity);
			pfScaleVec3(tail, 1.0f, tail);
			pfAddVec3(tail, tail, sp->mPosition);

			sqDist1 = pfSqrDistancePt3(sp->mPosition, verts[3]);
			sqDist2 = pfSqrDistancePt3(sp->mPosition, tail);
			if (sqDist2 > sqDist1) {
				pfCopyVec3(verts[3], tail);
			}

			glColor4fv(sp->mColor);

			glTexCoord2f(0, 0);
			glVertex3fv(verts[0]);
			glTexCoord2f(0, 1);
			glVertex3fv(verts[1]);
			glTexCoord2f(1, 1);
			glVertex3fv(verts[2]);

			glTexCoord2f(1, 1);
			glVertex3fv(verts[2]);
			glTexCoord2f(1, 0);
			glVertex3fv(verts[3]);
			glTexCoord2f(0, 0);
			glVertex3fv(verts[0]);
		}
		glEnd();
	default:
		break;
	}

	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MiniSparks::UpdateFireworks(void)
{
	int			i;
	MiniSparks	*f;


//	for (i = 0; i < NUM_FIREWORKS; i++) {
//		f = gFireworks[i];
//		f->Think();
//		f->Move();
//		f->Draw();
//	}

	gFireworksTimer -= UnscaledDeltaTime;
	if (gFireworksTimer < 0.0f) {
		gFireworksTimer = RANDOM_IN_RANGE(0.05f, 0.3f);
		for (i = 0; i < NUM_FIREWORKS; i++) {
			f = gFireworks[i];
			if (f->mLifeTimer <= 0.0f) {
				playSound2D(SOUND_MORTAR_1, 1.0f, RANDOM_IN_RANGE(0.8f, 1.2f));

				pfSetVec3(f->mCenter, RANDOM_IN_RANGE(40, 600), RANDOM_IN_RANGE(40, 440), 0);
				f->mSpread = 500.0f;
				f->mLifeTimer = RANDOM_IN_RANGE(0.6f, 1.8f);
				f->mFlags = 0;
				f->mBurst = true;
				f->mDrag = 1.0f;
				f->mAcc = 1000.0f;
				f->mBuddyAcc = 100.0f;

				pfVec4	hue;
				pfSetVec4(hue, RANDOM_IN_RANGE(0.01f, 1.0f), RANDOM_IN_RANGE(0.01f, 1.0f), RANDOM_IN_RANGE(0.01f, 1.0f), 1);
				pfNormalizeVec3(hue);

				for (i = 0; i < f->mNumSparks; i++) {
					Spark	*sp = &(f->mSparks[i]);
					sp->mFlags = 0;
					sp->mLifeTimer = 1.0f;
					sp->mSize = RANDOM_IN_RANGE(5.0f, 10.0f);
					pfSetVec3(sp->mPosition, f->mCenter[PF_X] + RANDOM_IN_RANGE(-5, 5), f->mCenter[PF_Y] + RANDOM_IN_RANGE(-5, 5), 0);
					pfCopyVec3(sp->mLastPosition, sp->mPosition);
					sp->mVelocity[PF_X] = RANDOM_IN_RANGE(-f->mSpread, f->mSpread);
					sp->mVelocity[PF_Y] = RANDOM_IN_RANGE(-f->mSpread, f->mSpread);
					sp->mVelocity[PF_Z] = 0.0f;
					pfCopyVec4(sp->mColor, hue);
				}

				return;
			}
		}
	}
}

