
/************************************************************\
	cellport.cpp
	Files for controlling the port in Ben's project,
	This is the "hub" of the game.
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
#include "cellport.h"
#include "glider.h"
#include "texture.h"
#include "hud.h"
#include "boss.h"
#include "spline.h"
#include "sound.h"
#include "draw.h"
#include "projectile.h"

int32		gGameMode = GAME_MODE_SLIDES;
CellPort	*gPort = NULL;
SplinedObj	*gIntroCameraPath = NULL;
SplinedObj	*gHubPath[SHIELD_HOWMANY] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
float		gWinTimer = 0.0f;
float		gZoomTimer = -1.0f;
int32		gZoomPipe = -1;

LevelStruct	gLevels[SHIELD_HOWMANY];
int			gCurrentLevel = 0;
int			gNextLevelNumber = 0;

CellPort::CellPort()
{
	int32		i, j;
	pfMatrix	mat;

	mRadius = 200.0f;
	pfSetVec3(mCenter, 0.0f, 0.0f, 0.0f);
	for (i = 0; i < kPortCircleSegments; i++) {
		pfSinCos(i * (360.0f / kPortCircleSegments), &mSVals[i], &mCVals[i]);
	}
	if (!gIntroCameraPath) {
		static pfVec3	knots[] =	{	{   0.0f, 200.0f, 400.0f},
									//	{2000.0f,    0.0f, 4000.0f},
									//	{   0.0f,   10.0f, 4000.0f},
										{ 100.0f, 100.0f, 450.0f},
									//	{4000.0f,    0.0f, 2000.0f},
										{ 100.0f,    0.0f,  500.0f},
										{  10.0f,    0.0f,   20.0f},
									};
		gIntroCameraPath = BuildBasicSplinePath(knots, sizeof(knots)/sizeof(knots[0]), 0);
		SmoothBasicSplinePath(gIntroCameraPath);
		gIntroCameraPath->SplineFactor = 0.5f;
	}
	if (!gHubPath[0]) {
		static pfVec3	knots[] =	{
										{   0.0f,	30.0f,	4.0f},
										{   0.0f,	50.0f,	0.0f},
										{	0*20.0f,	60.0f,	-40.0f},
										{   0.0f,	100.0f,	-40.0f},
										{   0.0f,	140.0f,	-20.0f},
										{	0*40.0f,	200.0f,	-20.0f},
										{	0*100.0f,	300.0f,	-50.0f},
									   {	0*10.0f,	400.0f,	-20.0f},
									   {	0*10.0f,	500.0f,	-20.0f},
									   {	0*10.0f,	600.0f,	-20.0f},
									};
		static pfVec3	aknots[sizeof(knots)/sizeof(knots[0])];
		for (i = 0; i < 7; i++) {
			for (j = 0; j < sizeof(knots)/sizeof(knots[0]); j++) {
				pfMakeEulerMat(mat, 360.0f * (float)i / 7.0f, 0.0f, 0.0f);
				pfXformVec3(aknots[j], knots[j], mat);
				if (j > 1) {
					aknots[j][PF_X] += RANDOM_IN_RANGE(-20.0f, 20.0f);
					aknots[j][PF_Y] += RANDOM_IN_RANGE(-20.0f, 20.0f);
					aknots[j][PF_Z] += RANDOM_IN_RANGE(-10.0f, 10.0f);
				}
			}
			gHubPath[i] = BuildBasicSplinePath(aknots, sizeof(aknots)/sizeof(aknots[0]), 0);
			SmoothBasicSplinePath(gHubPath[i]);
			gHubPath[i]->SplineFactor = 0.5f;
		}
	}
	mIntroCameraTimer = 0.0f;
}

CellPort::~CellPort()
{
}

void CellPort::Clear(void)
{
}

void CellPort::IntroCamera(void)
{
	pfMatrix	mat;
	float		oldHeight, newHeight, flashHeight;

	flashHeight = 3.0f * 20.0f;
	FetchSplinePath(gIntroCameraPath, mat);
	oldHeight = GlobalCameraMatrix[PF_T][PF_Z];
	pfCopyVec3(GlobalCameraMatrix[PF_T], mat[PF_T]);
	newHeight = GlobalCameraMatrix[PF_T][PF_Z];
	if (gIntroCameraPath->AtSplineEnd) {
		gGameMode = GAME_MODE_PORT;
		gCells->mNumWalls = 0;
	}

	if (oldHeight > flashHeight && newHeight <= flashHeight) {
		if (PF_ABS(GlobalCameraMatrix[PF_T][PF_X]) < 300.0f) {
			if (PF_ABS(GlobalCameraMatrix[PF_T][PF_Y]) < 300.0f) {
				gScreenFlashSpeed = 2.0f;
				gScreenFlashTimer = 1.0f;
				pfSetVec4(gScreenFlashColor, 1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
	}


	CameraLookAtTarget();
	LevelCamera();

	if (0) {
		Glider	*g;
		static float	tumble1 = 0.0f;
		static float	tumble2 = 0.0f;
		static float	tumble3 = 0.0f;
		tumble1 += DeltaTime * 90.0f;
		tumble2 += DeltaTime * 100.0f;
		tumble3 += DeltaTime * 110.0f;
		if (tumble1 > 360.0f) tumble1 -= 360.0f;
		if (tumble2 > 360.0f) tumble2 -= 360.0f;
		if (tumble3 > 360.0f) tumble3 -= 360.0f;

		for (g = gActiveGliderList; g; g = g->mNext) {

			pfCopyMat(mat, GlobalCameraMatrix);
			pfPreRotMat(mat, tumble1, 0, 1, 0, mat);
			pfPreRotMat(mat, tumble2, 1, 0, 0, mat);
			pfPreRotMat(mat, tumble3, 0, 0, 1, mat);
			pfAddScaledVec3(mat[PF_T], mat[PF_T], 10.0f, GlobalCameraMatrix[PF_Z]);
			pfCopyMat(g->mMatrix, mat);
			pfCopyMat(g->mDrawMatrix, g->mMatrix);
		}
	}
}

void CellPort::Think(void)
{
	float	activateRadius = 50.0f;
	float	tubeActivateRadius = 20.0f;
	float	away, maxAway = 0.0f;
	int32	i;
	Glider	*g;
	int		pick = -1;
	
	if (gZoomPipe >= 0) return;
	for (i = 0; i < SHIELD_HOWMANY; i++) {
		/**** check distance to shield mouth ****/
		for (g = gActiveGliderList; g; g = g->mNext) {
			away = pfDistancePt3(g->mMatrix[PF_T], gHubPath[i]->SData->SplineList->Loc);
			if (away < tubeActivateRadius) {
				if (!gLevels[i].mComplete) {
					pick = i;
				}
			}
		}
	}

	for (g = gActiveGliderList; g; g = g->mNext) {
		away = pfLengthVec2(g->mMatrix[PF_T]);
		if (away > maxAway) maxAway = away;
	}

	if ((pick >= 0) || maxAway > activateRadius) {
		gZoomPipe = pick;
		gZoomTimer = -1.0f;
	} else {
		/**** impose a leash ****/
		float	max = 45.0f;
		pfVec3	origin = {0,0,0};
		for (g = gActiveGliderList; g; g = g->mNext) {
			away = pfDistancePt3(g->mMatrix[PF_T], origin);
			if (away > max) {
				pfNormalizeVec3(g->mMatrix[PF_T]);
				float dot = pfDotVec3(g->mVelocity, g->mMatrix[PF_T]);
				if (dot > 0.0f) {
					pfAddScaledVec3(g->mVelocity, g->mVelocity, -dot, g->mMatrix[PF_T]);
				}
				pfScaleVec3(g->mMatrix[PF_T], max, g->mMatrix[PF_T]);
			}
		}
	}
}

void CellPort::HandleZoom(void)
{
	int					i;
	static SplinedObj	zoomRabbit;
	static float		zoomAngle[2] = {0,0};
	static float		zoomAngleVel[2] = {0,0};
	pfMatrix			mat, matRabbit[2];
	Glider				*g;
	float				zoomAcceleration = 0.5f;

	if (gZoomPipe < 0) return;
	if (gZoomTimer < 0.0f) {
		gZoomTimer = 0.0f;
		zoomRabbit = *(gHubPath[gZoomPipe]);
		zoomRabbit.SplineTime = 0.0f;
		zoomRabbit.AtSplineEnd = false;
		zoomRabbit.AtSplineStart = true;
		zoomRabbit.SplineFactor = 1.0f;
		zoomAngle[0] = zoomAngle[1] = 0.0f;
		zoomAngleVel[0] = zoomAngleVel[1] = 0.0f;

		gCurrentLevel = gZoomPipe;
		if (gLevels[gCurrentLevel].mLevelNumber < 0) {
			gLevels[gCurrentLevel].mLevelNumber = gNextLevelNumber++;
		}
		gCells->SetupWalls();
	}

	gZoomTimer += DeltaTime;
	zoomRabbit.SplineFactor += zoomAcceleration * DeltaTime;
	FetchSplinePath(&zoomRabbit, matRabbit[1]);
	FetchSplinePathForward(&zoomRabbit, matRabbit[0], 0.1f);
	for (g = gActiveGliderList; g; g = g->mNext) {

		if (KeysDown[gGliderKeys[g->mPlayerNum][GLIDER_KEY_TURN_LEFT]]) zoomAngle[g->mPlayerNum] += 500.0f * DeltaTime;
		if (KeysDown[gGliderKeys[g->mPlayerNum][GLIDER_KEY_TURN_RIGHT]]) zoomAngle[g->mPlayerNum] -= 500.0f * DeltaTime;
//		zoomAngle[g->mPlayerNum] += zoomAngleVel[g->mPlayerNum] * DeltaTime;

		pfCopyMat(mat, matRabbit[g->mPlayerNum]);
		pfPreRotMat(mat, zoomAngle[g->mPlayerNum], 0, 1, 0, mat);
		pfAddScaledVec3(mat[PF_T], mat[PF_T], -5.0f, mat[PF_Z]);

		if (gZoomTimer < 1.0f) {
			pfCombineVec3(mat[PF_T], gZoomTimer, mat[PF_T], 1.0f-gZoomTimer, g->mMatrix[PF_T]);
		}
		pfCopyMat(g->mMatrix, mat);
		pfCopyMat(g->mDrawMatrix, g->mMatrix);
	}
	if (gZoomTimer > 4.0f) {
		gZoomPipe = -1;
		gZoomTimer = 0.0f;

		if (1) {
			/**** Pick a boss ****/
			gBoss->Reset(gLevels[gCurrentLevel].mBossID);

			gGameMode = GAME_MODE_PLAY;
			gScreenFlashSpeed = 2.0f;
			gScreenFlashTimer = 1.0f;
			pfSetVec4(gScreenFlashColor, 1.0f, 1.0f, 1.0f, 1.0f);

			gCells->Clear();
			for (i = 0; i < 3; i++) {
				gCells->Mutate();
			}
			for (g = gActiveGliderList; g; g = g->mNext) {
				g->mTrailNext = -1;
				pfMakeEulerMat(g->mMatrix, 90.0f, 0.0f, 0.0f);
				pfSetVec3(g->mMatrix[PF_T], 2.0f*g->mPlayerNum, 0, 0);
				pfSetVec3(g->mVelocity, 0, 0, 0);
				pfCopyMat(g->mDrawMatrix, g->mMatrix);
				pfCopyMat(g->mLastMatrix, g->mMatrix);
				g->mCurrentWeapon[0] = PROJECTILE_TYPE_SMALL;
				hudGetTripleWeapon(g, 0);
				hudGetTripleWeapon(g, 0);
				hudGetTripleWeapon(g, 0);
			}
			gHealthLevel[HEALTH_TYPE_HEALTH] = 1.0f;
			gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
			gHealthLevel[HEALTH_TYPE_ATTITUDE] = 1.0f;

	//		gCells->mLifeUpdateTimeInterval = RANDOM_IN_RANGE(0.05f, 2.0f);
			gCells->mLifeUpdateTimeInterval = 0.5f;

			playSound2D(VO_BOTH_LEVEL1+(gLevels[gCurrentLevel].mLevelNumber), 0.9f, RANDOM_IN_RANGE(0.9f, 1.1f));
		}

	}
}

void CellPort::DrawShields(void)
{
	int32	i, h, v;
	int32	index;
	static float xShift = 0.0f;
	static float yShift = 0.0f;
	static float tShift = 0.0f;

	/**** Draw the tubes ****/
	if (1 || gGameMode == GAME_MODE_PORT) {
		xShift += 1.0f * DeltaTime;
		yShift -= 1.0f * DeltaTime;
//		tShift += 0.2f * DeltaTime;
		while (xShift > 1.0f) xShift -= 1.0f;
		while (yShift < 0.0f) yShift += 1.0f;
//		while (tShift > 1.0f) tShift -= 1.0f;
//		pfSinCos(360.0f * tShift, &xShift, &yShift);
//		xShift += 0.5f;
//		yShift += 0.5f;
		glEnable(GL_TEXTURE_2D);
		UseLibTexture(TEXID_GRID);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);
		glAlphaFunc(GL_GREATER, 0.0f);
		glDepthMask(true);

		for (i = 0; i < 7; i++) {
			float	firstKnot = 0;
			float	numKnots = 2;
			if (gLevels[i].mComplete) {
				glColor4f(0.5f, 1.0f, 0.5f, 1.0f);
				DrawSplineTube(gHubPath[i], 5.0f, 10, 20, 0*xShift, 0*yShift, firstKnot, numKnots, 0);
			} else {
				glColor4f(1.0f, 0.7f, 0.5f, 1.0f);
				DrawSplineTube(gHubPath[i], 5.0f, 10, 20, xShift, yShift, firstKnot, numKnots, 0);
			}
		}
	}

	/**** draw the actual shields ****/
	if (1) {
		static float	angle = 0.0f;
		angle += 90.0f * DeltaTime;
		if (angle > 360.0f) angle -= 360.0f;

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);
		glAlphaFunc(GL_GREATER, 0.0f);
		glDepthMask(true);

		for (i = 0; i < SHIELD_HOWMANY; i++) {
			pfVec3	pos;
			pfVec3	verts[4] = {{1,0,-1},{1,0,1},{-1,0,1},{-1,0,-1}};
			pfVec3	extrusion = {0,1,0};
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			pfCopyVec3(pos, gHubPath[i]->SData->SplineList->Loc);
			glTranslatef(pos[PF_X], pos[PF_Y], pos[PF_Z]);
			glRotatef(angle, 0.0f, 0.0f, 1.0f);
			glScalef(3.0f, 3.0f, 3.0f);

			if (!gLevels[i].mComplete) {
				UseLibTexture(gLevels[i].mShieldTexID);
				glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
				DrawChiaQuad(verts, extrusion, 1, 0, false, 0);
			}

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}
}

void CellPort::Draw(void)
{
	int32	h, v;
	uint8	shift, flags;
	pfVec3	pos, cpos, vertex, toCam, up, over;
	OneCell	*pcell;
	float	t;
	float	*c1, *c2;
	float	gridSquares = 80.0f;
	pfVec4	color, mix1, mix2;
//	pfVec4	hueBack = {250.0/255.0, 192.0/255.0, 135.0/255.0, 1.0};
	pfVec4	huePatient = {0.0/255.0, 220.0/255.0, 0.0/255.0, 1.0};
	pfVec4	hueBack = {0.0/255.0, 255.0/255.0, 0.0/255.0, 1.0};
	int				i;
	float			cx, cy, cz, hw, hh, sval, cval;

	if (0) {
		/**** draw the patient map ****/
		glEnable(GL_TEXTURE_2D);
//		UseLibTexture(TEXID_PATIENT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(true);

		glColor4fv(huePatient);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(mCenter[PF_X] - (mRadius * 20.0f), mCenter[PF_Y] - (mRadius * 16.0f), mCenter[PF_Z] - 0.1f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(mCenter[PF_X] + (mRadius * 20.0f), mCenter[PF_Y] - (mRadius * 16.0f), mCenter[PF_Z] - 0.1f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(mCenter[PF_X] + (mRadius * 20.0f), mCenter[PF_Y] + (mRadius * 24.0f), mCenter[PF_Z] - 0.1f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(mCenter[PF_X] - (mRadius * 20.0f), mCenter[PF_Y] + (mRadius * 24.0f), mCenter[PF_Z] - 0.1f);
		glEnd();
	}
	if (1) {
		/**** draw the underlay ****/
		glEnable(GL_TEXTURE_2D);
		UseLibTexture(TEXID_GRID);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(true);

		hueBack[2] = hueBack[3] = RANDOM_IN_RANGE(0.8f, 1.0f);
		glColor4fv(hueBack);
		glBegin(GL_QUADS);
		glTexCoord2f(-0.5f * gridSquares, -0.5f * gridSquares);
		glVertex3f(mCenter[PF_X] - (2*mRadius), mCenter[PF_Y] - (2*mRadius), mCenter[PF_Z] - 0.1f);
		glTexCoord2f(-0.5f * gridSquares,  0.5f * gridSquares);
		glVertex3f(mCenter[PF_X] + (2*mRadius), mCenter[PF_Y] - (2*mRadius), mCenter[PF_Z] - 0.1f);
		glTexCoord2f( 0.5f * gridSquares,  0.5f * gridSquares);
		glVertex3f(mCenter[PF_X] + (2*mRadius), mCenter[PF_Y] + (2*mRadius), mCenter[PF_Z] - 0.1f);
		glTexCoord2f( 0.5f * gridSquares, -0.5f * gridSquares);
		glVertex3f(mCenter[PF_X] - (2*mRadius), mCenter[PF_Y] + (2*mRadius), mCenter[PF_Z] - 0.1f);
		glEnd();
	}
	/**** draw the port itself ****/
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(true);

	float	portScale = 20.0f;
	float	ht = 3.0f * portScale, thick = 0.7f;
	float	rad = 5.0f * portScale;
	float	bright;
	pfVec3	norm;
	pfVec3	lightDir = {0.707f, 0.0f, 0.707f};
	int32	seg;

	cx = mCenter[PF_X];
	cy = mCenter[PF_Y];
	cz = mCenter[PF_Z];

	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= kPortCircleSegments; i++) {
		seg = i % kPortCircleSegments;
		pfSetVec3(norm, mSVals[seg], mCVals[seg], 0.0f);
		bright = 0.5f + (0.5f * pfDotVec3(norm, lightDir));
		bright = 0.4f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (rad * mSVals[seg]), cy + (rad * mCVals[seg]), cz);
		pfSetVec3(norm, mSVals[seg] * 0.707f, mCVals[seg] * 0.707f, 0.707f);
		bright = 0.5f + (0.5f * pfDotVec3(norm, lightDir));
		bright = 0.4f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (rad * mSVals[seg]), cy + (rad * mCVals[seg]), cz + ht);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= kPortCircleSegments; i++) {
		seg = i % kPortCircleSegments;
		pfSetVec3(norm, mSVals[seg], mCVals[seg], 0.0f);
		bright = 0.5f + (0.5f * -pfDotVec3(norm, lightDir));
		bright = 0.25f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (thick * rad * mSVals[seg]), cy + (thick * rad * mCVals[seg]), cz);
		pfSetVec3(norm, mSVals[seg] * 0.707f, mCVals[seg] * 0.707f, 0.707f);
		bright = 0.5f + (0.5f * -pfDotVec3(norm, lightDir));
		bright = 0.25f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (thick * rad * mSVals[seg]), cy + (thick * rad * mCVals[seg]), cz + (ht * 1.1f));
	}
	glEnd();

	/**** topper circle ****/
	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= kPortCircleSegments; i++) {
		seg = i % kPortCircleSegments;
		pfSetVec3(norm, mSVals[seg] * 0.707f, mCVals[seg] * 0.707f, 0.707f);
		bright = 0.5f + (0.5f * pfDotVec3(norm, lightDir));
		bright = 0.4f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (rad * mSVals[seg]), cy + (rad * mCVals[seg]), cz + ht);
		pfSetVec3(norm, 0.0f, 0.0f, 1.0f);
		bright = 0.5f + (0.5f * pfDotVec3(norm, lightDir));
		bright = 0.4f + (0.5f * bright);
		glColor4f(bright, bright, bright, 1.0f);
		glNormal3fv(norm);
		glVertex3f(cx + (thick * rad * mSVals[seg]), cy + (thick * rad * mCVals[seg]), cz + (ht * 1.1f));
	}
	glEnd();

	/**** clear dome ****/
	float	theta, sphrad = 5.0f * portScale, thstep = 5.0f;
	float	h1, h2, r1, r2;
	for (theta = 0.0f; theta < 50.0f; theta += thstep) {
		pfSinCos(theta, &sval, &cval);
		h1 = (cval * sphrad);
		r1 = sval * sphrad;
		pfSinCos(theta + thstep, &sval, &cval);
		h2 = (cval * sphrad);
		r2 = sval * sphrad;

		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= kPortCircleSegments; i++) {
			seg = i % kPortCircleSegments;
			glColor4f(1.0f, 1.0f, 1.0f, RANDOM_IN_RANGE(0.7f, 0.8f));
			glVertex3f(cx + (r1 * mSVals[seg]), cy + (r1 * mCVals[seg]), cz + h1);
			glColor4f(1.0f, 1.0f, 1.0f, RANDOM_IN_RANGE(0.7f, 0.8f));
			glVertex3f(cx + (r2 * mSVals[seg]), cy + (r2 * mCVals[seg]), cz + h2);
		}
		glEnd();
	}

	DrawShields();

	/**** draw the chooser ****/
}


