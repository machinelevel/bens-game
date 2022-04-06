
/************************************************************\
	camera.c
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
#include "glider.h"
#include "cellport.h"
#include "boss.h"

pfVec3		GlobalCameraTarget;
pfMatrix	GlobalCameraMatrix;

void MoveCamera(void)
{
	CameraSwingAroundDegrees(CameraSwingVeloc[PF_X] * UnscaledDeltaTime, CameraSwingVeloc[PF_Y] * UnscaledDeltaTime, CameraSwingVeloc[PF_Z] * UnscaledDeltaTime);
}

void CameraLookAtTarget(void)
{
	pfVec3	diff;

	pfSubVec3(diff, GlobalCameraTarget, GlobalCameraMatrix[PF_T]);
	pfCopyVec3(GlobalCameraMatrix[PF_Y], diff);
	pfCrossVec3(GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y], GlobalCameraMatrix[PF_Z]);
	pfCrossVec3(GlobalCameraMatrix[PF_Z], GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_X]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Y]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Z]);
}



void GliderCam(void)
{
	float	azimuth = 0.0f;
	float	elevation = -30.0f;
	pfVec3	diff;
	float	away;

//	if (0) {
//		pfMakeIdentMat(GlobalCameraMatrix);
//		pfPreRotMat(GlobalCameraMatrix, azimuth, 0.0f, 0.0f, 1.0f, GlobalCameraMatrix);
//		pfPreRotMat(GlobalCameraMatrix, elevation, 1.0f, 0.0f, 0.0f, GlobalCameraMatrix);
//		pfCopyVec3(GlobalCameraMatrix[PF_T], gGlider->mDrawMatrix[PF_T]);
//		pfAddScaledVec3(GlobalCameraMatrix[PF_T], GlobalCameraMatrix[PF_T], -gGlider->mGliderCamDist, GlobalCameraMatrix[PF_Y]);
//	}
	if (1) {
		int		numPlayers = 0;
		float	vmax = 0.8f * gActiveGliderList->mGliderCamDist;
		float	vmin = 0.4f * gActiveGliderList->mGliderCamDist;
		float	vert;
		Glider	*g;
		pfVec3	center, sideDiff;
		float	dot, dist, maxDist;
		float	zoomOutDistMultiplier = 3.0f;
		float	autoZoomInSpeed = 3.0f;
		float	autoZoomInMinDist = 8.0f;

		pfSetVec3(center, 0.0f, 0.0f, 0.0f);
		maxDist = 0.0f;
		for (g = gActiveGliderList; g; g = g->mNext) {
			numPlayers++;
			pfAddVec3(center, center, g->mDrawMatrix[PF_T]);
		}
		pfScaleVec3(center, 1.0f/numPlayers, center);

		for (g = gActiveGliderList; g; g = g->mNext) {
//			pfSubVec3(sideDiff, center, g->mDrawMatrix[PF_T]);
//			dot = pfDotVec3(sideDiff, GlobalCameraMatrix[PF_Y]);
//			pfAddScaledVec3(sideDiff, sideDiff, -dot, GlobalCameraMatrix[PF_Y]);
//			dist = pfLengthVec3(sideDiff);
			dist = pfDistancePt3(center, g->mDrawMatrix[PF_T]);
			if (dist > maxDist) {
				maxDist = dist;
			}
		}
		if (numPlayers > 1) {
			if (gActiveGliderList->mGliderCamDist > autoZoomInMinDist) {
				gActiveGliderList->mGliderCamDist -= autoZoomInSpeed * UnscaledDeltaTime;
			}
			if (gActiveGliderList->mGliderCamDist < (zoomOutDistMultiplier * maxDist)) {
				gActiveGliderList->mGliderCamDist = (zoomOutDistMultiplier * maxDist);
			}
		}


		pfSubVec3(diff, center, GlobalCameraMatrix[PF_T]);
		vert = diff[PF_Z];
		away = pfNormalizeVec3(diff);
//		if (away > gGlider->mGliderCamDist) {
			pfAddScaledVec3(GlobalCameraMatrix[PF_T], center, -gActiveGliderList->mGliderCamDist, diff);
//		}
		if (GlobalCameraMatrix[PF_T][PF_Z] < center[PF_Z] + vmin) {
			GlobalCameraMatrix[PF_T][PF_Z] = center[PF_Z] + vmin;
		}
		if (GlobalCameraMatrix[PF_T][PF_Z] > center[PF_Z] + vmax) {
			GlobalCameraMatrix[PF_T][PF_Z] = center[PF_Z] + vmax;
		}
		if (gGameMode == GAME_MODE_PORT && gZoomTimer <= 0.0f) {
			/**** apply leash to center ****/
			pfVec3 v, nv;
			float len, max;
			
			if (gGameMode == GAME_MODE_PORT) max = 50.0f;
			else max = 500.0f;

			pfCopyVec3(v, GlobalCameraMatrix[PF_T]);
			v[PF_Z] = 0.0f;
			len = pfNormalizeVec3(v);
			if (len > max) {
				GlobalCameraMatrix[PF_T][PF_X] = max * v[PF_X];
				GlobalCameraMatrix[PF_T][PF_Y] = max * v[PF_Y];
			}
		}
		pfCopyVec3(GlobalCameraTarget, center);
		GlobalCameraTarget[PF_Z] += 1.75f;	/**** don't focus on the shoes ****/
// Use this to get pics of the bosses.
//if (gGameMode != GAME_MODE_PORT) {
//	pfCopyVec3(GlobalCameraTarget, gBoss->mMatrix[PF_T]);
//	GlobalCameraTarget[PF_T] += 8.0f;
//	GlobalCameraMatrix[PF_T][PF_Z] = 8.0f;
//}
		CameraLookAtTarget();
		LevelCamera();
	}
	if (0) {
		pfCopyVec3(GlobalCameraTarget, gActiveGliderList->mDrawMatrix[PF_T]);
		CameraLookAtTarget();
		LevelCamera();
	}
}

void CameraSwingAroundDegrees(float x, float y, float z)
{
	pfVec3	diff;
	float	dist;

	pfSubVec3(diff, GlobalCameraTarget, GlobalCameraMatrix[PF_T]);
	dist = pfLengthVec3(diff);
	pfAddScaledVec3(GlobalCameraMatrix[PF_T], GlobalCameraMatrix[PF_T], dist, GlobalCameraMatrix[PF_Y]);
	pfPreRotMat(GlobalCameraMatrix, x, 0, 0, 1, GlobalCameraMatrix);
	pfPreRotMat(GlobalCameraMatrix, z, 1, 0, 0, GlobalCameraMatrix);
	dist -= y;
	if (dist < 1.0f) dist = 1.0f;
	pfAddScaledVec3(GlobalCameraMatrix[PF_T], GlobalCameraMatrix[PF_T], -dist, GlobalCameraMatrix[PF_Y]);
	CameraLookAtTarget();
}

void SetCameraOnAxis(long axis, long sign)
{
	pfVec3	diff, setdiff;
	float	dist;

	pfSubVec3(diff, GlobalCameraTarget, GlobalCameraMatrix[PF_T]);
	dist = pfLengthVec3(diff);

	pfMakeIdentMat(GlobalCameraMatrix);
	PFSET_VEC3(setdiff, 0, 0, 0);
	setdiff[axis] = dist * sign;

	pfAddScaledVec3(GlobalCameraMatrix[PF_T], GlobalCameraTarget, 1.0f, setdiff);
	pfAddScaledVec3(GlobalCameraMatrix[PF_Y], GlobalCameraTarget, -1.0f, setdiff);

	if (axis == PF_Z) PFSET_VEC3(GlobalCameraMatrix[PF_Z], 0.0f, 1.0f, 0.0f);
	else PFSET_VEC3(GlobalCameraMatrix[PF_Z], 0.0f, 0.0f, 1.0f);

	pfCrossVec3(GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y], GlobalCameraMatrix[PF_Z]);
	pfCrossVec3(GlobalCameraMatrix[PF_Z], GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y]);

	pfNormalizeVec3(GlobalCameraMatrix[PF_X]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Y]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Z]);
}

void LevelCamera(void)
{
	if (PF_ABS(GlobalCameraMatrix[PF_Y][PF_Z]) > 0.99f) PFSET_VEC3(GlobalCameraMatrix[PF_Z], 0.0f, 1.0f, 0.0f);
	else PFSET_VEC3(GlobalCameraMatrix[PF_Z], 0.0f, 0.0f, 1.0f);

	pfCrossVec3(GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y], GlobalCameraMatrix[PF_Z]);
	pfCrossVec3(GlobalCameraMatrix[PF_Z], GlobalCameraMatrix[PF_X], GlobalCameraMatrix[PF_Y]);

	pfNormalizeVec3(GlobalCameraMatrix[PF_X]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Y]);
	pfNormalizeVec3(GlobalCameraMatrix[PF_Z]);
}

void InitCamera(void)
{
	pfSetVec3(GlobalCameraTarget, 0.0f, 0.0f, 0.5f*4);
	pfMakeIdentMat(GlobalCameraMatrix);
	pfSetVec3(GlobalCameraMatrix[PF_T], 0.0f, -2.0f*4, 1.0f*4);
	GlobalCameraMatrix[PF_T][PF_X] -= 50;
	CameraLookAtTarget();
}


void SetGLViewToCamera(void)
{
	pfMatrix	m;
	pfMatrix	BasicCorrectionMat;

	pfMakeIdentMat(BasicCorrectionMat);
	pfSetVec3(BasicCorrectionMat[PF_Y], 0.0f, 0.0f,-1.0f);
	pfSetVec3(BasicCorrectionMat[PF_Z], 0.0f, 1.0f, 0.0f);
	pfInvertOrthoMat(m, GlobalCameraMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((const float *)BasicCorrectionMat);
	glMultMatrixf((const float *)m);
}



