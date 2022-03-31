
/************************************************************\
	projectile.cpp
	Files for controlling the projectiles in Ben's project
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
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
#include "hud.h"
#include "sound.h"

Projectile	*Projectile::msFirst = NULL;

float	gTripleCrossbowTimer[2] = {0, 0};
pfVec3	gTripleCrossbowPos[2];
float	gTripleSwordTimer[2] = {0, 0};
pfVec3	gTripleSwordPos[2];
float	gTripleRepeaterTimer[2] = {0, 0};

Projectile::Projectile(int32 type)
{
	/**** add to beginning of list ****/
	mPrev = NULL;
	mNext = msFirst;
	if (mNext) mNext->mPrev = this;
	msFirst = this;

	mType = type;
	pfMakeIdentMat(mMatrix);
	pfSetVec3(mVelocity, 0, 0, 0);
	mSpeed = 0.0f;
	mOwner = NULL;
	mLifeTimer = 0.0f;
	mFlags = 0;
	mGlowSpark = NULL;
	mGeneration = 0;

	switch (mType) {
	case PROJECTILE_TYPE_SMALL:
		mGlowSpark = new GlowSpark(GLOWSPARK_TYPE_NORMAL, 64);
		pfSetVec4(mGlowSpark->mGlowColor, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case PROJECTILE_TYPE_SPLIT:
		mGlowSpark = new GlowSpark(GLOWSPARK_TYPE_NORMAL, 64);
		pfSetVec4(mGlowSpark->mGlowColor, 1.0f, 1.0f, 1.0f, 1.0f);
		mGlowSpark->mGlowSize *= 2.0f;
		break;
	case PROJECTILE_TYPE_CROSSBOW:
		mGlowSpark = new GlowSpark(GLOWSPARK_TYPE_NORMAL, 64);
		pfSetVec4(mGlowSpark->mGlowColor, 0.0f, 1.0f, 1.0f, 1.0f);
		break;
	case PROJECTILE_TYPE_SWORD:
		mGlowSpark = new GlowSpark(GLOWSPARK_TYPE_SWORD, 64);
		pfSetVec4(mGlowSpark->mGlowColor, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	default:
		mGlowSpark = new GlowSpark(GLOWSPARK_TYPE_NORMAL, 64);
		pfSetVec4(mGlowSpark->mGlowColor, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
}

Projectile::~Projectile()
{
	/**** remove from list ****/
	if (msFirst == this) msFirst = mNext;
	if (mPrev) mPrev->mNext = mNext;
	if (mNext) mNext->mPrev = mPrev;
	if (mGlowSpark) delete mGlowSpark;
}

Projectile *Projectile::Launch(int32 type, pfMatrix mat, Glider *owner)
{
	Projectile *pro;
	pro = new Projectile(type);
	
	pro->mOwner = owner;
	pro->mSpeed = 50.0f;
	pro->mLifeTimer = 1.0f;
	pfCopyMat(pro->mMatrix, mat);
	pfScaleVec3(pro->mVelocity, pro->mSpeed, mat[PF_Y]);
	if (owner) {
		pfAddVec3(pro->mVelocity, pro->mVelocity, owner->mVelocity);
	}

	switch (pro->mType) {
	case PROJECTILE_TYPE_SPLIT:
		pro->mSpeed = 15.0f;
		pfScaleVec3(pro->mVelocity, pro->mSpeed, mat[PF_Y]);
		pro->mLifeTimer = 3.0f;
		if (gTripleWeaponReady[owner->mPlayerNum]) {
			playSound2D(SOUND_BUBBLE_POP, 1.0f, RANDOM_IN_RANGE(0.2f, 0.25f));
			pro->mFlags |= PROJECTILE_FLAG_TRIPLE;
			gTripleWeaponReady[owner->mPlayerNum] = false;
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
		}
		break;
	case PROJECTILE_TYPE_SMALL:
		break;
	case PROJECTILE_TYPE_CROSSBOW:
		if (gTripleWeaponReady[owner->mPlayerNum]) {
			playSound2D(SOUND_BUBBLE_POP, 1.0f, RANDOM_IN_RANGE(0.2f, 0.25f));
			gTripleCrossbowTimer[owner->mPlayerNum] = 1.0f;
			pfCopyVec3(gTripleCrossbowPos[owner->mPlayerNum], owner->mDrawMatrix[PF_T]);
			gTripleCrossbowPos[owner->mPlayerNum][PF_T] = 0.0f;
			gTripleWeaponReady[owner->mPlayerNum] = false;
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
		}
		break;
	case PROJECTILE_TYPE_SLINGSHOT:
		if (gTripleWeaponReady[owner->mPlayerNum]) {
			playSound2D(SOUND_BUBBLE_POP, 1.0f, RANDOM_IN_RANGE(0.2f, 0.25f));
			gTripleRepeaterTimer[owner->mPlayerNum] = 3.0f;
			gTripleWeaponReady[owner->mPlayerNum] = false;
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
		}
		break;
	case PROJECTILE_TYPE_SWORD:
		pro->mLifeTimer = 0.25f;
		if (gTripleWeaponReady[owner->mPlayerNum]) {
			playSound2D(SOUND_BUBBLE_POP, 1.0f, RANDOM_IN_RANGE(0.2f, 0.25f));
			gTripleSwordTimer[owner->mPlayerNum] = 1.0f;
			pfCopyVec3(gTripleSwordPos[owner->mPlayerNum], owner->mDrawMatrix[PF_T]);
			gTripleSwordPos[owner->mPlayerNum][PF_T] = 0.0f;
			gTripleWeaponReady[owner->mPlayerNum] = false;
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
			hudGetTripleWeapon(owner, 0);
		}
		break;
	default:
		break;
	}
	return(pro);
}

void Projectile::Detonate(void)
{
	SETFLAG(mFlags, PROJECTILE_FLAG_DEAD);
}

void Projectile::Think(void)
{
	OneCell		*pcell;
	bool		good = false;
	pfMatrix	mat2;
	Projectile	*pro;

	mLifeTimer -= DeltaTime;
	if (mLifeTimer <= 0.0f) {
		Detonate();
	}

	switch (mType) {
//	case PROJECTILE_TYPE_SMALL:
	case PROJECTILE_TYPE_CROSSBOW:
		/**** detect a hit ****/
		pcell = gCells->GetClosestCell(mMatrix[PF_T]);
		if (pcell) {
			if ((pcell->flags & CELL_FLAG_BOSS) || (pcell->shift & (CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT))) {
				SETFLAG(pcell->flags, CELL_FLAG_GOOD);
				pcell->goodTimer = 8.0f;	/**** cells freeze for this long ****/
//				if (pcell->flags & CELL_FLAG_BOSS) {
					SETFLAG(pcell->flags, CELL_FLAG_SHOT);
//				}
				Detonate();
			}
		}
		break;
	case PROJECTILE_TYPE_SPLIT:
		/**** detect a hit ****/
		pcell = gCells->GetClosestCell(mMatrix[PF_T]);
		if (pcell) {
			float	triple = false;
			if (mFlags & PROJECTILE_FLAG_TRIPLE) triple = true;
			if (!(pcell->flags & CELL_FLAG_SHOT)) {
				if ((pcell->flags & CELL_FLAG_BOSS) || (pcell->shift & (CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT))) {
					CLRFLAG(pcell->shift, CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT);
					SETFLAG(pcell->flags, CELL_FLAG_SHOT);
					Detonate();
					if (mGeneration < 3 || triple) {
						int divider = 1 << (mGeneration+1);
						if (triple) divider = 1.0f;
						pfPreRotMat(mat2,  30, 0, 0, 1, mMatrix);
						pro = Launch(mType, mat2, mOwner);
						if (pro) {
							pro->mGeneration = mGeneration+1;
							pro->mLifeTimer = 1.0 / (float)divider;
							pro->mSpeed = 50.0f;
							pfScaleVec3(pro->mVelocity, pro->mSpeed, pro->mMatrix[PF_Y]);
							if (triple) pro->mFlags |= PROJECTILE_FLAG_TRIPLE;
						}
						pfPreRotMat(mat2, -30, 0, 0, 1, mMatrix);
						pro = Launch(mType, mat2, mOwner);
						if (pro) {
							pro->mGeneration = mGeneration+1;
							pro->mLifeTimer = 1.0 / (float)divider;
							pro->mSpeed = 50.0f;
							pfScaleVec3(pro->mVelocity, pro->mSpeed, pro->mMatrix[PF_Y]);
							if (triple) pro->mFlags |= PROJECTILE_FLAG_TRIPLE;
						}
					}
				}
			}
		}
		break;
	case PROJECTILE_TYPE_SWORD:
		/**** detect a hit ****/
		pcell = gCells->GetClosestCell(mMatrix[PF_T]);
		if (pcell) {
			if ((pcell->flags & CELL_FLAG_BOSS) || (pcell->shift & (CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT))) {
				CLRFLAG(pcell->shift, CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT);
				SETFLAG(pcell->flags, CELL_FLAG_SHOT);
				if (pcell->flags & CELL_FLAG_BOSS) {
					Detonate();
				}
			}
		}
		break;
	default:
		/**** detect a hit ****/
		pcell = gCells->GetClosestCell(mMatrix[PF_T]);
		if (pcell) {
			if ((pcell->flags & CELL_FLAG_BOSS) || (pcell->shift & (CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT))) {
				CLRFLAG(pcell->shift, CELL_SHIFT_ON | CELL_SHIFT_ON_NEXT);
				SETFLAG(pcell->flags, CELL_FLAG_SHOT);
				Detonate();
			}
		}
		break;
	}
}

void Projectile::Move(void)
{
	float	floor = gCells->mCenter[PF_Z];

	pfAddScaledVec3(mMatrix[PF_T], mMatrix[PF_T], DeltaTime, mVelocity);
}

void Projectile::React(void)
{
	if (mGlowSpark) {
		SETFLAG(mGlowSpark->mFlags, GLOWSPARK_FLAG_ACTIVE);
		pfCopyVec3(mGlowSpark->mPosition, mMatrix[PF_T]);
	}
}

void Projectile::Draw(void)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf((float*)mMatrix);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);

//	glBegin(GL_QUADS);
//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//	glVertex3f(-0.1f, -0.1f, 0.0f);
//	glVertex3f(-0.1f,  0.1f, 0.0f);
//	glVertex3f( 0.1f,  0.1f, 0.0f);
//	glVertex3f( 0.1f, -0.1f, 0.0f);
//	glEnd();

	switch (mType) {
	case PROJECTILE_TYPE_SMALL:
		break;
	case PROJECTILE_TYPE_SPLIT:
		glScalef(1.5f,1.5f,1.5f);
		glEnable(GL_TEXTURE_2D);
		UseLibTexture(TEXID_WEAPON_MISSILE);
		glColor4f(1,1,1,1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex3f(0,0, 0.5f);
		glTexCoord2f(1,0); glVertex3f(0,1, 0.5f);
		glTexCoord2f(1,1); glVertex3f(0,1,-0.5f);
		glTexCoord2f(0,1); glVertex3f(0,0,-0.5f);
		glTexCoord2f(0,0); glVertex3f( 0.5f,0,0);
		glTexCoord2f(1,0); glVertex3f( 0.5f,1,0);
		glTexCoord2f(1,1); glVertex3f(-0.5f,1,0);
		glTexCoord2f(0,1); glVertex3f(-0.5f,0,0);
		glEnd();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case PROJECTILE_TYPE_CROSSBOW:
		break;
	case PROJECTILE_TYPE_SWORD:
		break;
	default:
		break;
	}

	glPopMatrix();
}

void Projectile::RemoveDead(void)
{
	Projectile	*pro, *next;

	pro = msFirst;
	while (pro) {
		next = pro->mNext;
		if (pro->mFlags & PROJECTILE_FLAG_DEAD) {
			delete pro;
		}
		pro = next;
	}
}

void Projectile::ThinkAll(void)
{
	Projectile	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Think();
	}
}

void Projectile::MoveAll(void)
{
	Projectile	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Move();
	}
}

void Projectile::ReactAll(void)
{
	Projectile	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->React();
	}
}

void Projectile::DrawAll(void)
{
	Projectile	*pro;
	for (pro = msFirst; pro; pro = pro->mNext) {
		pro->Draw();
	}
}



