
/************************************************************\
	hud.h
	Functions for drawing the HUD in Ben's project
\************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <stdarg.h>
#include <time.h>
#include "genincludes.h"
#include "camera.h"
#include "timer.h"
#include "guideway.h"
#include "texture.h"
#include "draw.h"
//#include "displacement.h"
#include "cellgrid.h"
#include "glider.h"
#include "projectile.h"
#include "random.h"
#include "hud.h"
#include "cellport.h"
#include "font.h"
#include "slides.h"
#include "boss.h"
#include "sound.h"

/***********************************************************\
 For convenience, all HUD items are drawn in 640x480 space
	
	----------------
	| 0,0          |
	|              |
	|              |
	|              |
	|      640,480 |
	----------------

  They will be scaled as necessary to fit any display.
\***********************************************************/

float	gScreenFlashSpeed = 1.0f;
float	gScreenFlashTimer = 0.0f;
pfVec4	gScreenFlashColor = {1.0f, 1.0f, 1.0f, 1.0f};

bool	gPaused = false;
float	gPauseFader = 0.0f;

int		gTripleWeaponTexIDs[2][3] = {{0,0,0},{0,0,0}};
float	gTripleWeaponTimer[2] = {0,0};
bool	gTripleWeaponReady[2] = {false,false};
int		gTripleWeaponBleeps[2] = {0,0};

Shield	gShields[7] = {
	{ TEXID_SHIELD_POX,		true },
	{ TEXID_SHIELD_COLDS,	true },
	{ TEXID_SHIELD_RASH,	true },
	{ TEXID_SHIELD_BARF,	true },
	{ TEXID_SHIELD_FEVER,	true },
	{ TEXID_SHIELD_HAIR,	true },
	{ TEXID_SHIELD_FOOT,	true },
};

float	gHealthLevel[HEALTH_TYPE_HOWMANY] = { 0.7, 0.5, 0.9 };

void getShield(int texID)
{
	int	i;

	gLevels[gCurrentLevel].mComplete = true;
	for (i = 0; i < 7; i++) {
		if (!gShields[i].have) {
			gShields[i].have = true;
			gShields[i].icon = texID;
			return;
		}
	}
}

void hudQuickRect(float x, float y, float w, float h)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(x, y, 0);
	glTexCoord2f(0, 0);
	glVertex3f(x, y+h, 0);
	glTexCoord2f(1, 0);
	glVertex3f(x+w, y+h, 0);
	glTexCoord2f(1, 1);
	glVertex3f(x+w, y, 0);
	glEnd();
}

void hudQuickCircle(float x, float y, float w, float h)
{
	const int kCircleSegments = 32;
	static float	svals[kCircleSegments];
	static float	cvals[kCircleSegments];
	static bool		inited = false;
	int				i;
	float			cx, cy, hw, hh;

	if (!inited) {
		for (i = 0; i < kCircleSegments; i++) {
			pfSinCos(i * (360.0f / kCircleSegments), &svals[i], &cvals[i]);
		}
	}

	hw = w * 0.5f;
	hh = h * 0.5f;
	cx = x + hw;
	cy = y + hh;

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(cx, cy, 0);
	for (i = 0; i < kCircleSegments; i++) {
		glTexCoord2f(0.5f + (svals[i] * 0.5f), 0.5f + (cvals[i] * 0.5f));
		glVertex3f(cx + (hw * svals[i]), cy + (hh * cvals[i]), 0);
	}
	i = 0;
	glTexCoord2f(0.5f + (svals[i] * 0.5f), 0.5f + (cvals[i] * 0.5f));
	glVertex3f(cx + (hw * svals[i]), cy + (hh * cvals[i]), 0);
	glEnd();
}




void hudDrawCompass(void)
{
	pfMatrix	cmat, mat;
	static float flash = 0;
	float		sval, cval;

	flash += 360.0f * DeltaTime;
	if (flash > 360.0f) flash -= 360.0f;
	pfSinCos(flash, &sval, &cval);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	pfCopyMat(cmat, GlobalCameraMatrix);
	pfMakeTransMat(mat,0,0,0);
	pfAddScaledVec3(mat[PF_T], cmat[PF_T],  5.0f, cmat[PF_Y]);
	pfAddScaledVec3(mat[PF_T],  mat[PF_T],  1.5f, cmat[PF_X]);
	pfAddScaledVec3(mat[PF_T],  mat[PF_T], -1.1f, cmat[PF_Z]);
	glMultMatrixf((float*)mat);
	glScalef(0.5f,0.5f,0.5f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	UseLibTexture(TEXID_COMPASS);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	/**** additive ****/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(1,1); glVertex3f(-1,-1,0);
	glTexCoord2f(0,1); glVertex3f( 1,-1,0);
	glTexCoord2f(0,0); glVertex3f( 1, 1,0);
	glTexCoord2f(1,0); glVertex3f(-1, 1,0);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBegin(GL_QUADS);
	glColor4f(1,1,1,0.25f*(sval+1.0f));
	glTexCoord2f(1,1); glVertex3f(-1,-1,0);
	glColor4f(1,1,1,0.25f*(sval+1.0f));
	glTexCoord2f(0,1); glVertex3f( 1,-1,0);
	glColor4f(1,1,1,0.25f*(sval+1.0f));
	glTexCoord2f(0,0); glVertex3f( 1, 1,0);
	glColor4f(1,1,1,0.25f*(sval+1.0f));
	glTexCoord2f(1,0); glVertex3f(-1, 1,0);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDisable(GL_TEXTURE_2D);

	if (gCells->mTotalBadCells == 0) {
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1,1,1,0.5f*(sval+1.0f));
		glVertex3f( 1,   1,   0);
		glColor4f(0,0,0,0);
		glVertex3f(-1,   1,   0);
		glVertex3f( 1,  -1,   0);
		glVertex3f( 1.5f,1.5f,0);
		glVertex3f(-1,   1,   0);
		glEnd();
	}
	if (gHealthLevel[HEALTH_TYPE_AMMO] <= 0.0f) {
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(healthFillColor[HEALTH_TYPE_AMMO][0], healthFillColor[HEALTH_TYPE_AMMO][1], healthFillColor[HEALTH_TYPE_AMMO][2], 0.5f*(sval+1.0f));
		glVertex3f(-1,   1,   0);
		glColor4f(0,0,0,0);
		glVertex3f( 1,   1,   0);
		glVertex3f(-1,  -1,   0);
		glVertex3f(-1.5f,1.5f,0);
		glVertex3f( 1,   1,   0);
		glEnd();
	}
	if (gHealthLevel[HEALTH_TYPE_HEALTH] <= 0.0f) {
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(healthFillColor[HEALTH_TYPE_HEALTH][0], healthFillColor[HEALTH_TYPE_HEALTH][1], healthFillColor[HEALTH_TYPE_HEALTH][2], 0.5f*(sval+1.0f));
		glVertex3f( 1,  -1,   0);
		glColor4f(0,0,0,0);
		glVertex3f( 1,   1,   0);
		glVertex3f(-1,  -1,   0);
		glVertex3f( 1.5f,-1.5f,0);
		glVertex3f( 1,   1,   0);
		glEnd();
	}
	if (gHealthLevel[HEALTH_TYPE_ATTITUDE] <= 0.0f) {
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(healthFillColor[HEALTH_TYPE_ATTITUDE][0], healthFillColor[HEALTH_TYPE_ATTITUDE][1], healthFillColor[HEALTH_TYPE_ATTITUDE][2], 0.5f*(sval+1.0f));
		glVertex3f(-1,  -1,   0);
		glColor4f(0,0,0,0);
		glVertex3f(-1,   1,   0);
		glVertex3f( 1,  -1,   0);
		glVertex3f(-1.5f,-1.5f,0);
		glVertex3f(-1,   1,   0);
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void hudSwapTripleWeapon(Glider *player)
{
	int	current = gTripleWeaponTexIDs[player->mPlayerNum][2];

	gTripleWeaponTexIDs[player->mPlayerNum][2] = gTripleWeaponTexIDs[player->mPlayerNum][1];
	gTripleWeaponTexIDs[player->mPlayerNum][1] = gTripleWeaponTexIDs[player->mPlayerNum][0];
	gTripleWeaponTexIDs[player->mPlayerNum][0] = current;
	gTripleWeaponTimer[player->mPlayerNum] = 1.0f;
	switch (current) {
 		case TEXID_WEAPON_MISSILE: player->mCurrentWeapon[0] = PROJECTILE_TYPE_SPLIT; break;
		case TEXID_WEAPON_SLINGSHOT: player->mCurrentWeapon[0] = PROJECTILE_TYPE_SLINGSHOT; break;
		case TEXID_WEAPON_CROSSBOW: player->mCurrentWeapon[0] = PROJECTILE_TYPE_CROSSBOW; break;
		case TEXID_WEAPON_SWORD: player->mCurrentWeapon[0] = PROJECTILE_TYPE_SWORD; break;
		default: player->mCurrentWeapon[0] = PROJECTILE_TYPE_SMALL; break;
	}
}

void hudGetTripleWeapon(Glider *player, int texID)
{
	gTripleWeaponTexIDs[player->mPlayerNum][2] = gTripleWeaponTexIDs[player->mPlayerNum][1];
	gTripleWeaponTexIDs[player->mPlayerNum][1] = gTripleWeaponTexIDs[player->mPlayerNum][0];
	gTripleWeaponTexIDs[player->mPlayerNum][0] = texID;
	gTripleWeaponTimer[player->mPlayerNum] = 1.0f;
	gTripleWeaponBleeps[player->mPlayerNum] = 0;
}

void hudDrawTripleWeapon(void)
{
	float	basex[2] = {320, 220}, basey[2] = {65,65};
	float	size = 40;
	float	angle, sval, cval;
	float	x, y;
	int		i;
	static float	blink[2] = {0.0f, 0.0f};
	float			max, blinkRate;
	int				matchCount = 0;
	int				matchTex = 0;
	float			alpha;
	int				tex;
	int				player;

	for (player = 0; player < gNumPlayers; player++) {
		gTripleWeaponTimer[player] -= 4.0f * DeltaTime;
		if (gTripleWeaponTimer[player] < 0.0f) gTripleWeaponTimer[player] = 0.0f;
		angle = -gTripleWeaponTimer[player] * 120.0f;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_TEXTURE_2D);

		matchCount = 0;
		if (gTripleWeaponTexIDs[player][0] == gTripleWeaponTexIDs[player][1]) {
			matchTex = gTripleWeaponTexIDs[player][0];
			matchCount = 2;
			if (gTripleWeaponTexIDs[player][0] == gTripleWeaponTexIDs[player][2]) matchCount = 3;
		} else if (gTripleWeaponTexIDs[player][1] == gTripleWeaponTexIDs[player][2]) {
			matchTex = gTripleWeaponTexIDs[player][1];
			matchCount = 2;
			if (gTripleWeaponTexIDs[player][0] == gTripleWeaponTexIDs[player][2]) matchCount = 3;
		} else if (gTripleWeaponTexIDs[player][0] == gTripleWeaponTexIDs[player][2]) {
			matchTex = gTripleWeaponTexIDs[player][0];
			matchCount = 2;
			if (gTripleWeaponTexIDs[player][1] == gTripleWeaponTexIDs[player][2]) matchCount = 3;
		}
		if (matchTex == 0) matchCount = 0;

		gTripleWeaponReady[player] = false;
		if (matchCount == 3) {
			gTripleWeaponReady[player] = true;
		}

		max = matchCount * 0.33f;
		blinkRate = matchCount * 1.0f;
		blink[player] += DeltaTime * blinkRate;
		if (blink[player] > 1.0f && matchCount > 1) {
			if (gTripleWeaponBleeps[player] < 5) {
				playSound2D(SOUND_BEEP_1, 0.8f, 0.5f * matchCount);
				gTripleWeaponBleeps[player]++;
			}
		}
		while (blink[player] > 1.0f) blink[player] -= 1.0f;
		alpha = 0.2f + (max * blink[player]);
		if (alpha > 1.0f) alpha = 1.0f;
		if (alpha < 0.0f) alpha = 0.0f;

		for (i = 0; i < 3; i++) {
			pfSinCos(angle, &sval, &cval);
			angle += 360.0f / 3.0f;
			x = basex[player] + (0.6f * size * sval);
			y = basey[player] - (0.6f * size * cval);

			tex = gTripleWeaponTexIDs[player][i];
			UseLibTexture(TEXID_BUTTON_BLANK);
			if (tex == matchTex) {
				glColor4f(1,1,1,alpha);
			} else {
				glColor4f(1,1,1,0.2f);
			}
			hudQuickRect(x-size/2, y-size/2, size, size);
			if (gTripleWeaponTexIDs[player][i]) {
				if (tex == matchTex) {
					glColor4f(1,1,1,alpha);
				} else {
					glColor4f(1,1,1,0.2f);
				}
				UseLibTexture(gTripleWeaponTexIDs[player][i]);
				hudQuickRect(x-size/2, y-size/2, size, size);
			}
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void hudDrawHealthBars(void)
{
	int32	x = 32, y = 340;
	int32	w = 32, h = 128;
	int32	bx = 32+9, by = 440, bw = 32-20, bh = 128-40, bby;
	int32	cx = 36, cy = 439, cw = 23, ch = 23;
	int32	ix = 32, iy = 430, iw = 32, ih = 32;
	int32	i;
	float	twinkle, sval, cval;
	static float	timer = 0.0f;
	pfVec4	hue;
	pfVec4	iconColor[] = {
		{ 0.5, 0.5, 0.5, 1.0 },
		{ 0.5, 0.5, 0.5, 1.0 },
		{ 0.5, 0.5, 0.5, 1.0 },
	};
	static float	flash = 0.0f;

	flash -= 1.0f * DeltaTime;
	if (flash < 0.0f) {
		flash += 1.0f;
		if (gHealthLevel[0] <= 0.0f || gHealthLevel[1] <= 0.0f || gHealthLevel[2] <= 0.0f) {
			playSound2D(SOUND_BEEP_1, 0.9f, 0.5f);
		}
	}

	timer += 0.2f * DeltaTime;
	if (timer > 1.0f) timer -= 1.0f;


	if (gCells->mTotalBadCells == 0 || gBoss->mDead) {
		for (i = 0; i < 3; i++) {
			gHealthLevel[i] += 0.2f * DeltaTime;
			if (gHealthLevel[i] > 1.0f) gHealthLevel[i] = 1.0f;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	/**** normal ****/
	UseLibTexture(TEXID_THERMOMETER);
	
	x -= 10;
	bx -= 10;
	cx -= 10;
	ix -= 10;

	y -= 62;
	by -= 62;
	cy -= 62;
	iy -= 20;

	for (i = 0; i < 3; i++) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	/**** normal ****/
		glDisable(GL_TEXTURE_2D);
		PFCOPY_VEC4(hue, healthFillColor[i]);
		if (gHealthLevel[i] <= 0.0f) {
			hue[0] += (1.0f - hue[0]) * flash;
			hue[1] += (1.0f - hue[1]) * flash;
			hue[2] += (1.0f - hue[2]) * flash;
			hue[3] += (1.0f - hue[3]) * flash;
//			PFSCALE_VEC4(hue, flicker, hue);
		}
		glColor4fv(hue);
		bh = (gHealthLevel[i] * (440 - 352));
		bby = (by) - bh;
		hudQuickRect(bx, bby, bw, bh);
		hudQuickCircle(cx, cy, cw, ch);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);	/**** additive ****/
		glEnable(GL_TEXTURE_2D);
		glColor4f(1, 1, 1, 0.5f);
		hudQuickRect(x, y, w, h);
		x += w;
		bx += w;
		cx += w;
		y += 10;
		by += 10;
		cy += 10;
	}

	for (i = 0; i < 3; i++) {
		UseLibTexture(gHealthIcons[i]);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	/**** normal ****/
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);	/**** additive ****/
		glEnable(GL_TEXTURE_2D);
		PFCOPY_VEC4(hue, iconColor[i]);
		if (gHealthLevel[i] <= 0.0f) {
			hue[0] = hue[1] = hue[2] = hue[3] = flash;
		}
		glColor4fv(hue);
		hudQuickRect(ix, iy, iw, ih);
		ix += w;
		iy += 10;
	}
}

void hudDrawShields(void)
{
	int32	x, y;
	int32	sx = 30, sy = 30;
	int32	w = 32, h = 32;
	int32	i;
	float	twinkle, sval, cval;
	static float	timer = 0.0f;

	timer += 0.2f * DeltaTime;
	if (timer > 1.0f) timer -= 1.0f;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	x = sx;
	y = sy;
	for (i = 0; i < 7; i++) {
		pfSinCos(i * 60.0f + (timer * 360.0f), &sval, &cval);
		twinkle = 0.8f;
		if (sval > 0.0f) {
			twinkle += sval * sval * (1.0f - twinkle);
		}

		if (gShields[i].have) {
			glColor4f(twinkle, twinkle, twinkle, 1);
		} else {
			glColor4f(1, 1, 1, 0.35f);
		}
		UseLibTexture(gShields[i].icon);
		hudQuickRect(x, y, w, h);
		x += w;
		if (i == 3) {
			x -= w * 3.5f;
			y += h * 1.0f;
		}
	}
}

void hudDrawMessages(void)
{
	int32	x = 320-200, y = 100;
	int32	w = 400, h = 100;
	int32	i;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (gWinTimer > 0.0f) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(RANDOM0TO1, RANDOM0TO1, RANDOM0TO1, RANDOM_IN_RANGE(0.5f, 1.0f));
		UseLibTexture(TEXID_TEXT_COMPLETE);
		hudQuickRect(x, y, w, h);
		if (gWinTimer > 2.0f) x += (gWinTimer - 2.0f) * 300.0f;
		hudQuickRect(x, y, w, h);
		if (gWinTimer > 2.0f) x -= (gWinTimer - 2.0f) * 600.0f;
		hudQuickRect(x, y, w, h);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

#define NUM_BADCOUNT	60
int32	gBadCountHistory[NUM_BADCOUNT];
int32	gBadCountNext = 0;
float	gBadHistoryTimer = 0.0f;
int32	gBadHistorySamples = 0;

void hudDrawGraph(void)
{
	float	x, xmin = 450, y = 50, maxy = 50, yscale = 0.05f;
	float	w = 3, h = 30, d;
	int32	i, index, val, stripe;
	float	wScale, hScale;
	float	win_width, win_height;
	
	win_width  = glutGet(GLUT_WINDOW_WIDTH);
	win_height = glutGet(GLUT_WINDOW_HEIGHT);
	

	if (1) {
		gBadHistoryTimer += 1.0f * DeltaTime;
		if (gBadHistoryTimer > 1.0f || (gBadHistorySamples < NUM_BADCOUNT)) {
			gBadCountHistory[gBadCountNext] = gCells->mTotalBadCells;
			gBadCountNext = (gBadCountNext+1) % NUM_BADCOUNT;
			gBadHistorySamples++;
			if (gBadHistoryTimer > 1.0f) {
				gBadHistoryTimer -= 1.0f;
			}
		}
	}
	
	stripe = gBadHistoryTimer * NUM_BADCOUNT;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	x = xmin;
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	hudQuickRect(x, y, w*NUM_BADCOUNT, maxy);

	glEnable(GL_TEXTURE_2D);
	UseLibTexture(TEXID_HUD_GRAPH);
	x = xmin;
	index = gBadCountNext;
	glBegin(GL_QUADS);
	for (i = 0; i < NUM_BADCOUNT; i++) {
		val = gBadCountHistory[index];
		val *= yscale;
		if (val > maxy) val = maxy;
		index = (index+1) % NUM_BADCOUNT;
		if (stripe == i) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		} else {
			glColor4f(0.7f, 0.0f, 0.0f, 1.0f);
		}
		glTexCoord2f(0, 1);
		glVertex3f(x, y+maxy-val, 0);
		glTexCoord2f(0, 0);
		glVertex3f(x, y+maxy, 0);
		glTexCoord2f(1, 0);
		glVertex3f(x+w, y+maxy, 0);
		glTexCoord2f(1, 1);
		glVertex3f(x+w, y+maxy-val, 0);
		
		x += w;
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

	x = xmin + (0.5f * w);
	glBegin(GL_LINE_STRIP);
	index = gBadCountNext;
	for (i = 0; i < NUM_BADCOUNT; i++) {
		val = gBadCountHistory[index];
		val *= yscale;
		if (val > maxy) val = maxy;
		index = (index+1) % NUM_BADCOUNT;
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(x, y + maxy - val, 0.0f);
		x += w;
	}
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (1) {
		pfVec4	fc1 = {1,1,0,1};
		pfVec4	fc2 = {1,0,0,1};
		gFontSys->BeginDraw(FONT_ID_MAIN);

		gFontSys->SetColor(fc1, fc2);
		gFontSys->SetScale(1, 1);
		gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);
		gFontSys->SetPos(470, 70);
		gFontSys->DrawString("%d", gCells->mTotalBadCells);
	}
	if (0) {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		AppDrawText(300, 300, 20, "%d", gCells->mTotalBadCells);
	}
}

void hudDrawPatient(void)
{
	int32	x = 480, y = 100;
	int32	w = 64, h = 64;
	pfVec4	hue1 = {0.0, 0.0, 0.0, 0.25};
	pfVec4	hue2 = {0.0, 1.0, 0.0, 0.5};
	pfVec4	hue3 = {0.0, 1.0, 0.0, 0.5};
	static float	pulse = 0.0f, lastBright = 0.0f;
	float			bright;
return;
	pulse -= DeltaTime;
	if (pulse < 0.0f) pulse += 1.0f;
	bright = (pulse + lastBright) * 0.5f;
	lastBright = bright;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
//	UseLibTexture(TEXID_PATIENT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4fv(hue1);
	hudQuickRect(x, y, w, h);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	/**** additive ****/
	glColor4fv(hue2);
	hudQuickRect(x, y, w, h);

	pfSetVec4(hue3, 0.0f, 0.25f, 0.0f, bright);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	/**** additive ****/
	glColor4fv(hue3);
	hudQuickRect(x, y, w, h);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void hudDrawScreenFlash(void)
{
	pfVec4	hue;

	if (gScreenFlashTimer <= 0.0f) return;
	
	gScreenFlashTimer -= DeltaTime * gScreenFlashSpeed;
	PFCOPY_VEC4(hue, gScreenFlashColor);
	hue[3] *= gScreenFlashTimer;
	glColor4fv(hue);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	hudQuickRect(0, 0, 640, 480);
}

void hudDraw(void)
{
	long	win_width, win_height;
	
//	win_width  = glutGet(GLUT_WINDOW_WIDTH);
//	win_height = glutGet(GLUT_WINDOW_HEIGHT);

	if (gGameMode != GAME_MODE_PORT) {
		hudDrawCompass();
	}


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glDepthMask(false);
	glEnable(GL_BLEND);

	hudDrawShields();
	if (gGameMode != GAME_MODE_PORT) {
		hudDrawHealthBars();
		hudDrawPatient();
		hudDrawGraph();
		hudDrawTripleWeapon();
		hudDrawMessages();
		hudDrawScreenFlash();
	}

	if (gPaused) {
		gPauseFader += 1.0f * UnscaledDeltaTime;
		if (gPauseFader > 1.0f) gPauseFader = 1.0f;
		gTimeScale = 0.01f;
		gSlides->mCurrentSlide = SLIDE_PAUSE;
		gSlides->Think();
		gSlides->Draw();
	} else {
		gPauseFader = 0.0f;
		gTimeScale = 1.0f;
		gSlides->TendButtonSize(BUTTON_ID_MAW,		0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_OPTIONS,	0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_QUIT,		0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_THANKS,	0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_HELP,		0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_FORTUNE,	0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_PLAY,		0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_1PLAYER,	0, 0, 200);
		gSlides->TendButtonSize(BUTTON_ID_2PLAYER,	0, 0, 200);
		
		gSlides->TendButtonPos(BUTTON_ID_MAW,		320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_THANKS,	320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_FORTUNE,	320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_HELP,		320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_OPTIONS,	320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_QUIT,		320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_PLAY,		320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_1PLAYER,	320, 240, 500);
		gSlides->TendButtonPos(BUTTON_ID_2PLAYER,	320, 240, 500);
	}
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
}


