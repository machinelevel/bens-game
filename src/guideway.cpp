
/************************************************************\
	guideway.c
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "genincludes.h"
#include "upmath.h"
#include "timer.h"
#include "texture.h"

#include "guideway.h"

#define MAX_ROUNDSIDES	64
pfVec3		crossSection[MAX_ROUNDSIDES];
pfVec4		hueCrossSection[MAX_ROUNDSIDES];

float	birdSpeed = 0.0f;
float	desiredBirdSpeed = 0.0f;
float	birdAccel = 60.0f;
float	birdSpeedMult = 15.0f;
float	birdSpeedLevel = 1;
float	maxBirdSpeedLevel = 2;
guideArc	*birdTrack = NULL;
float	birdTrackT = 0.0f;
pfMatrix	birdPos;
float	birdTextureVOffset = 0;


guideArc	testGA1[] = {
	{10.0f, 0.0f, { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{gwARC_LENGTH(180.0f, 2.0f), gwRADIUS_TO_CURVATURE(2.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{10.0f, 0.0f, { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{gwARC_LENGTH(180.0f, 2.0f), gwRADIUS_TO_CURVATURE(2.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{-1}	/**** terminator ****/
};

guideArc	testGA2[] = {
	{gwARC_LENGTH(360.0f, 3.0f), gwRADIUS_TO_CURVATURE(3.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{-1}	/**** terminator ****/
};

guideArc	testGA3[] = {
	{gwARC_LENGTH(360.0f, 3.0f), gwRADIUS_TO_CURVATURE(3.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{10.0f, 0.0f, { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{gwARC_LENGTH(180.0f, 2.0f), gwRADIUS_TO_CURVATURE(2.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{10.0f, 0.0f, { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{gwARC_LENGTH(180.0f, 2.0f), gwRADIUS_TO_CURVATURE(2.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{-1}	/**** terminator ****/
};

guideArc	testGA4[] = {
	{gwARC_LENGTH(360.0f, 3.0f), gwRADIUS_TO_CURVATURE(3.0f), { 0.0f, 0.0f, 1.0f}, {0, 0, 1, 1}},
	{-1}	/**** terminator ****/
};

guideArc	*MasterGuidewayList[] = {
//	testGA3,
//	NULL,
	testGA1,
	testGA2,
//	testGA4,
	NULL
};

pfVec3	MasterGuidewayPosList[] = {
	{ 0,  0,  0},
//	{ 0,1.5,  0},
//	{10,  0,  7},
	{ 0,  0,  0},
	{ 0,  0,  0},
	{ 0,  0,  0},
	{ 0,  0,  0},
	{ 0,  0,  0},
};

pfVec3	MasterGuidewayDirList[] = {
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
	{ 1,  0,  0},
};



float		testGAPipeThickness = 0.25f;
long		roundSides = 16;		/**** how many sides the tube gets ****/
float		degreeChop = 5.0f;	/**** make a new segment every x degrees ****/

void guideInterp(pfMatrix outMat, guideArc *ga, float t)
{
	pfMatrix	tm;
	pfVec3		saveCenterPos;
	float		ang, radius;
	
	PFMAKE_IDENT_MAT(tm);
	PFCOPY_VEC3(tm[PF_X], ga->curveAxis);
	PFCOPY_VEC3(tm[PF_Y], ga->endMat[0][PF_Y]);
	pfCrossVec3(tm[PF_Z], tm[PF_X], tm[PF_Y]);
/**** these are guaranteed to be normalized already, because ****/
/**** of prep work done to make the two source vec's norm'd & perp. ****/
//	pfCrossVec3(tm[PF_Y], tm[PF_Z], tm[PF_X]);
//	pfNormalizeVec3(tm[PF_X]);
//	pfNormalizeVec3(tm[PF_Y]);
//	pfNormalizeVec3(tm[PF_Z]);
	radius = gwCURVATURE_TO_RADIUS(ga->curvature);
	ang = gwARC_DEGREES(ga->length, ga->curvature);

	PFCOPY_MAT(outMat, ga->endMat[0]);

	if (ga->curvature == 0.0f) {
		pfAddScaledVec3(outMat[PF_T], outMat[PF_T], ga->length * t, tm[PF_Y]);
	}
	else {
		pfAddScaledVec3(saveCenterPos, outMat[PF_T], radius, tm[PF_Z]);
		pfPostRotMat(outMat, outMat, ang * t, tm[PF_X][PF_X], tm[PF_X][PF_Y], tm[PF_X][PF_Z]);
		pfPostRotMat(tm, tm, ang * t, tm[PF_X][PF_X], tm[PF_X][PF_Y], tm[PF_X][PF_Z]);
		pfAddScaledVec3(outMat[PF_T], saveCenterPos, -radius, tm[PF_Z]);
	}
}

float findGuidewayPos(pfMatrix outMat, guideArc *ga, float pos)
{
	long	i;
	float	t, len, totalLength = 0;

	for (i = 0; ga[i].length >= 0.0f; i++) totalLength += ga[i].length;
	while (pos < 0.0f) pos += totalLength;
	while (pos > totalLength) pos -= totalLength;
	len = 0;
	for (i = 0; ga[i].length >= 0.0f; i++) {
		if (pos >= len && pos <= len + ga[i].length) {
			t = (pos - len) / ga[i].length;
			guideInterp(outMat, &(ga[i]), t);
			return(pos);
		}
		else {
			len += ga[i].length;
		}
	}
	return(pos);
}

void PrepGuideway(guideArc *ga, pfVec3 startPos, pfVec3 startDir)
{
	long		i;
	pfMatrix	currentMat;
	pfVec3		vec, up, fw, rt;

	PFCOPY_VEC3(fw, startDir);
	PFSET_VEC3(up, 0, 0, 1);
	pfCrossVec3(rt, fw, up);
	if (PFDOT_VEC3(rt, rt) < 0.001f) {
		PFSET_VEC3(up, 0, 1, 0);
		pfCrossVec3(rt, fw, up);
	}
	pfCrossVec3(up, rt, fw);
	pfNormalizeVec3(up);
	pfNormalizeVec3(rt);
	pfNormalizeVec3(fw);

	PFMAKE_IDENT_MAT(currentMat);
	PFCOPY_VEC3(currentMat[PF_X], rt);
	PFCOPY_VEC3(currentMat[PF_Y], fw);
	PFCOPY_VEC3(currentMat[PF_Z], up);
	PFCOPY_VEC3(currentMat[PF_T], startPos);

//	pfPreRotMat(currentMat, 90.0f, 1, 0, 0, currentMat);

	for (i = 0; ga[i].length > 0.0f; i++) {
		PFCOPY_MAT(ga[i].endMat[0], currentMat);

		/**** make sure the curve axis is normalized and perpendicular ****/
		pfCrossVec3(vec, ga[i].endMat[0][PF_Y], ga[i].curveAxis);
		pfCrossVec3(ga[i].curveAxis, vec, ga[i].endMat[0][PF_Y]);
		pfNormalizeVec3(ga[i].curveAxis);

		guideInterp(ga[i].endMat[1], &(ga[i]), 1.0f);
		PFCOPY_MAT(currentMat, ga[i].endMat[1]);
	}
}

void InitGuideways(void)
{
	long	gi;

	for (gi = 0; MasterGuidewayList[gi] != NULL; gi++) {
		PrepGuideway(MasterGuidewayList[gi], MasterGuidewayPosList[gi], MasterGuidewayDirList[gi]);
	}

}

void CountGuidewayVerts(guideArc *guide, long *pv, long *pi)
{
	long		degrees, sections;
	guideArc	*ga;

	*pv = 0;
	*pi = 0;
	for (ga = guide; ga->length > 0; ga++) {
		if (ga->curvature == 0) {
			degrees = 0.0f;
			sections = 1;
		}
		else {
			degrees = gwARC_DEGREES(ga->length, ga->curvature);
			sections = 1 + ((long)(degrees / degreeChop));
		}
		*pv += roundSides * sections;
		*pi += roundSides * 2 * sections * 3;	/**** 2 is because they're quads. ****/
	}
}

void InitCrossSection(void)
{
	static Boolean	inited = FALSE;
	long		i;
	float		bright, ang, sval[MAX_ROUNDSIDES], cval[MAX_ROUNDSIDES];
	pfVec3		vec;
	pfMatrix	m;

	if (inited) return;
	inited = TRUE;
	pfMakeIdentMat(m);
	for (i = 0; i < roundSides; i++) {
		ang = (360.0f * i) / roundSides;
		sval[i] = sinf(ang * PF_PI / 180.0f);
		cval[i] = cosf(ang * PF_PI / 180.0f);
		PFCOPY_VEC3(vec, m[PF_T]);
		PFADD_SCALED_VEC3(vec, vec,  sval[i] * testGAPipeThickness, m[PF_X]);
		PFADD_SCALED_VEC3(vec, vec, -cval[i] * testGAPipeThickness, m[PF_Z]);
		PFCOPY_VEC3(crossSection[i], vec);
		bright = 0.0 + (1.0*PF_ABS(cval[i]*cval[i]*cval[i]));
		PFSET_VEC4(hueCrossSection[i], bright, bright, bright, 1.0f);
	}
}

void DrawGLGuideway(long flags)
{
	long		i, j, k, vold, vnew, gi, num, sections, index, voffset;
	float		degrees, radius;
	guideArc	*ga;
	float		sec_length, sec_degrees;
	float		r, g, b, a;
	pfVec3		vec, xfCrossSection[2][MAX_ROUNDSIDES];
	pfVec4		xfHueCrossSection[2][MAX_ROUNDSIDES];
	pfMatrix	m;
	long		oldVerts = 0, oldIndices = 0;
	long		newVerts, newIndices;

	InitCrossSection();
	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
   	glDisable(GL_BLEND);
	glPushMatrix();

	glCullFace(GL_BACK);
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/**** now build the geometry ****/
	for (gi = 0; MasterGuidewayList[gi] != NULL; gi++) {
		for (ga = MasterGuidewayList[gi]; ga->length > 0; ga++) {
			if (ga->curvature == 0) {
				degrees = 0.0f;
				sections = 1;
			}
			else {
				degrees = gwARC_DEGREES(ga->length, ga->curvature);
				sections = 1 + ((long)(degrees / degreeChop));
			}

			sec_length = ga->length / sections;
			sec_degrees = degrees / sections;
			if (ga->curvature == 0.0f) radius = 0.0f;
			else radius = 1.0f / ga->curvature;

			for (j = 0; j <= sections; j++) {
				guideInterp(m, ga, (float)j / (float)sections);
				vold = j & 1;
				vnew = 1 - vold;
				for (k = 0; k < roundSides; k++) {
					pfXformPt3(xfCrossSection[vnew][k], crossSection[k], m);
					PFCOPY_VEC4(xfHueCrossSection[vnew][k], hueCrossSection[k]);
					if (xfHueCrossSection[vnew][k][0] < ga->color[0]) xfHueCrossSection[vnew][k][0] = ga->color[0];
					if (xfHueCrossSection[vnew][k][1] < ga->color[1]) xfHueCrossSection[vnew][k][1] = ga->color[1];
					if (xfHueCrossSection[vnew][k][2] < ga->color[2]) xfHueCrossSection[vnew][k][2] = ga->color[2];
				}
				
				if (j != 0) {
					glBegin(GL_QUAD_STRIP);
					glColor4fv(xfHueCrossSection[vold][0]);
					glVertex3fv(xfCrossSection[vold][0]);
					glColor4fv(xfHueCrossSection[vnew][0]);
					glVertex3fv(xfCrossSection[vnew][0]);
					for (k = 0; k < roundSides; k++) {
						glColor4fv(xfHueCrossSection[vold][(k+1)%roundSides]);
						glVertex3fv(xfCrossSection[vold][(k+1)%roundSides]);
						glColor4fv(xfHueCrossSection[vnew][(k+1)%roundSides]);
						glVertex3fv(xfCrossSection[vnew][(k+1)%roundSides]);
					}
					glEnd();
				}
			}
		}
	}
	glPopMatrix();
    glPopAttrib();
}

void DrawBirdTail(long flags, long sections, float start, float end)
{
	long		i, j, k, vold, vnew, gi, num, index, voffset;
	float		degrees, radius;
	guideArc	*ga;
	float		tval, t1, t2, talpha, tincrement, sec_length, sec_degrees;
	float		r, g, b, a;
	pfVec3		vec, xfCrossSection[2][MAX_ROUNDSIDES];
	pfVec4		xfHueCrossSection[2][MAX_ROUNDSIDES];
	pfMatrix	m;
	long		oldVerts = 0, oldIndices = 0;
	long		newVerts, newIndices;
	float		gridscale_u = 3.0f, gridscale_v = 2.0f;
	float		texture_v[2];

	if (birdTrack == NULL) return;
	if (start == end) return;
	InitCrossSection();

	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
   	glDisable(GL_BLEND);
	glPushMatrix();

    glDisable(GL_DEPTH_TEST);

//	glCullFace(GL_BACK);
//	glEnable( GL_CULL_FACE );
	glDisable( GL_CULL_FACE );
	glShadeModel(GL_SMOOTH);

	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); //GL_ONE_MINUS_SRC_ALPHA);

	UseLibTexture(GetTextureID("noise2"));
//	UseLibTexture(0);

	/**** now build the geometry ****/

	ga = birdTrack;
	t1 = birdTrackT + start;
	t2 = birdTrackT + end;
	tincrement = (t2 - t1) / sections;
	tval = t1;

	if (t1 > t2) glCullFace(GL_FRONT);
	else glCullFace(GL_BACK);

	for (j = 0; j <= sections; j++) {
		findGuidewayPos(m, birdTrack, tval);
		vold = j & 1;
		vnew = 1 - vold;
		
		texture_v[vnew] = gridscale_v * (birdTextureVOffset + tval - birdTrackT);

		for (k = 0; k < roundSides; k++) {
			pfXformPt3(xfCrossSection[vnew][k], crossSection[k], m);
			PFCOPY_VEC4(xfHueCrossSection[vnew][k], hueCrossSection[k]);
			talpha = 1.0f - ((tval - t1) / (t2 - t1));
xfHueCrossSection[vnew][k][0] = 120.0f/255.0f;
xfHueCrossSection[vnew][k][1] = 207.0f/255.0f;
xfHueCrossSection[vnew][k][2] = 183.0f/255.0f;
			xfHueCrossSection[vnew][k][3] = talpha;
		}
		
		if (j != 0) {
			glBegin(GL_QUAD_STRIP);
			glColor4fv(xfHueCrossSection[vold][0]); glTexCoord2f(0.0f, texture_v[vold]);
			glVertex3fv(xfCrossSection[vold][0]);
			glColor4fv(xfHueCrossSection[vnew][0]); glTexCoord2f(0.0f, texture_v[vnew]);
			glVertex3fv(xfCrossSection[vnew][0]);
			for (k = 0; k < roundSides; k++) {
				glColor4fv(xfHueCrossSection[vold][(k+1)%roundSides]); glTexCoord2f(gridscale_u * ((float)(k+1) / (float)roundSides), texture_v[vold]);
				glVertex3fv(xfCrossSection[vold][(k+1)%roundSides]);
				glColor4fv(xfHueCrossSection[vnew][(k+1)%roundSides]); glTexCoord2f(gridscale_u * ((float)(k+1) / (float)roundSides), texture_v[vnew]);
				glVertex3fv(xfCrossSection[vnew][(k+1)%roundSides]);
			}
			glEnd();
		}
		tval += tincrement;
	}
	UseLibTexture(0);
	glPopMatrix();
	glPopAttrib();
}

void DrawGLGuidewayNearBird(long flags, long sections, float nearDist, float farDist)
{
	float	speedMag, speedSign;
	float	backTail, minBackTail = 1.5f, backTailMult = 0.5f;
	float	frontTail, minFrontTail = 1.5f, frontTailMult = 0.2f;

	speedMag  = PF_ABS(birdSpeed);
	speedSign = MY_SGN(birdSpeed);

	backTail = speedMag * backTailMult;
	if (backTail < minBackTail) backTail = minBackTail;
	frontTail = speedMag * frontTailMult;
	if (frontTail < minFrontTail) frontTail = minFrontTail;

	DrawBirdTail(0, 10, 0.0f, backTail * -speedSign);
	DrawBirdTail(0, 10, 0.0f, frontTail * speedSign);
}


/**********************************************************************/
/**** bird controls are below. This should be in a different file. ****/
/**********************************************************************/

void DrawGLBird(long flags)
{
	pfMatrix	m;

	if (birdTrack == NULL) return;
	findGuidewayPos(m, birdTrack, birdTrackT);
	/**** fake some rotation ****/
	pfPreRotMat(m, m[PF_T][PF_X]*10.0f + m[PF_T][PF_Y] * 5.0f, 1, 0, 0, m);
	pfPreRotMat(m, m[PF_T][PF_X]*5.0f + m[PF_T][PF_Y] * 10.0f, 0, 1, 0, m);

	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
   	glDisable(GL_BLEND);
	glPushMatrix();
	glMultMatrixf((float *)m);

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

// @@ convert to SDL, is this used?
//	glutWireTetrahedron();

	glPopMatrix();
    glPopAttrib();
}

void AdjustBirdSpeed(long amount)
{
	long	adjust = 0;
	Boolean	noStop = TRUE;

	if (noStop) {
		if (amount > 0) {	/**** accel ****/
			if (birdSpeedLevel > 0) adjust = 1;
			else adjust = -1;
		}
		else {
			if (birdSpeedLevel > 0) adjust = -1;
			else adjust = 1;
		}
	}
	else {
		adjust = amount;
	}
	birdSpeedLevel += adjust;
	if (noStop) if (birdSpeedLevel == 0) birdSpeedLevel += adjust;	/**** can't stop! ****/
	if (birdSpeedLevel >  maxBirdSpeedLevel) birdSpeedLevel =  maxBirdSpeedLevel;
	if (birdSpeedLevel < -maxBirdSpeedLevel) birdSpeedLevel = -maxBirdSpeedLevel;
}

void MoveBird(void)
{
	pfMatrix	m;
	long		gi;

	if (birdTrack == NULL) {
		birdTrack = MasterGuidewayList[0];
		birdTrackT = 0.0f;
	}

	desiredBirdSpeed = birdSpeedLevel * birdSpeedMult;
	if (desiredBirdSpeed > birdSpeed) {
		birdSpeed += birdAccel * DeltaTime;
		if (birdSpeed > desiredBirdSpeed) birdSpeed = desiredBirdSpeed;
	}
	if (desiredBirdSpeed < birdSpeed) {
		birdSpeed -= birdAccel * DeltaTime;
		if (birdSpeed < desiredBirdSpeed) birdSpeed = desiredBirdSpeed;
	}
	birdTrackT += birdSpeed * DeltaTime;
	birdTrackT = findGuidewayPos(m, birdTrack, birdTrackT);

	birdTextureVOffset += 1.1f * birdSpeed * DeltaTime;
	while (birdTextureVOffset > 2.0f) birdTextureVOffset -= 2.0f;
	while (birdTextureVOffset < -2.0f) birdTextureVOffset += 2.0f;

	PFCOPY_MAT(birdPos, m);
}

float closestPointOnGuideway(guideArc *thisGA, pfVec3 testPoint, float *closeT)
{
	float		len, closeDistSq, dist, distSq, radius, angleToEnd, angleToPt;
	float		dot, xdot, t, tTotal, angleCutoff, centerDist;
	long		closeGI, gi;
	pfVec3		vec, center, radiusVec, centerToStart, centerToEnd, ringPt;
	pfVec3		centerToPlanePt, planePt;
	pfPlane		plane;
	guideArc	*ga;

	closeDistSq = -1.0f;
	if (closeT != NULL) *closeT = 0;
	closeGI = -1;
	tTotal = 0.0f;

	for (gi = 0; thisGA[gi].length > 0.0f; gi++) {
		ga = &(thisGA[gi]);
		if (ga->curvature == 0.0f) {	/**** straight segs ****/
			foxClosestPtOnSeg(testPoint, ga->endMat[0][PF_T], ga->endMat[1][PF_T], vec);
			distSq = PFSQR_DISTANCE_PT3(testPoint, vec);
			if (closeDistSq < 0.0f || distSq < closeDistSq) {
				t = pfDistancePt3(vec, ga->endMat[0][PF_T]);
				closeDistSq = distSq;
				closeGI = gi;
				if (closeT != NULL) *closeT = tTotal + t;
			}
		}
		else {	/**** curved segs ****/
			pfCrossVec3(radiusVec, ga->curveAxis, ga->endMat[0][PF_Y]);
			radius = gwCURVATURE_TO_RADIUS(ga->curvature);
			pfAddScaledVec3(center, ga->endMat[0][PF_T], radius, radiusVec);
			PFCOPY_VEC3(plane.normal, ga->curveAxis);

			centerDist = pfDistancePt3(center, testPoint);
			dist = PF_ABS(centerDist - radius);
			distSq = dist * dist;
			if (distSq < closeDistSq || closeDistSq < 0.0f) {
				plane.offset = PFDOT_VEC3(center, plane.normal);
				foxClosestPtOnPlane(&plane, testPoint, planePt);

				/**** could do more trivial-reject ****/
				pfSubVec3(centerToPlanePt, planePt, center);
				pfNormalizeVec3(centerToPlanePt);

				pfSubVec3(centerToStart, ga->endMat[0][PF_T], center);
				pfScaleVec3(centerToStart, 1.0f / radius, centerToStart);
				pfSubVec3(centerToEnd, ga->endMat[1][PF_T], center);
				pfScaleVec3(centerToEnd, 1.0f / radius, centerToEnd);

				angleToEnd = gwARC_DEGREES(ga->length, ga->curvature);
				angleCutoff = ((360.0f - angleToEnd) / 2.0f) + angleToEnd;
				pfCrossVec3(vec, centerToStart, centerToPlanePt);
				dot = PFDOT_VEC3(centerToStart, centerToPlanePt);
				xdot = PFDOT_VEC3(vec, ga->curveAxis);
				angleToPt = pfArcSin(xdot);
				if (dot < 0.0f) {
					if (angleToPt > 0.0f) angleToPt = 180.0f - angleToPt;
					else angleToPt = -180.0f - angleToPt;
				}
				if (angleToPt < 0.0f) angleToPt += 360.0f;

				if (angleToPt > angleToEnd) {
					if (angleToPt > angleCutoff) {
						PFCOPY_VEC3(ringPt, ga->endMat[0][PF_T]);
						t = 0;
					}
					else {
						PFCOPY_VEC3(ringPt, ga->endMat[1][PF_T]);
						t = ga->length;
					}
				}
				else {
					pfAddScaledVec3(ringPt, center, radius, centerToPlanePt);
					t = ga->length * (angleToPt / angleToEnd);
				}

				distSq = PFSQR_DISTANCE_PT3(testPoint, ringPt);
				if (closeDistSq < 0.0f || distSq < closeDistSq) {
					closeDistSq = distSq;
					closeGI = gi;
					if (closeT != NULL) *closeT = tTotal + t;
				}
			}
		}
		tTotal += ga->length;
	}

	if (closeDistSq >= 0.0f) return(pfSqrt(closeDistSq));
	else return(-1.0f);
}

float findClosestTrackPos(guideArc *thisGA, float thisT, guideArc **closeGA, float *closeT)
{
	long		i;
	float		t, dist, closeDist = -1.0f;
	pfVec3		testPoint;
	pfMatrix	m;

	if (closeGA != NULL) *closeGA = NULL;
	if (closeT  != NULL) *closeT = 0;

	findGuidewayPos(m, thisGA, thisT);
	PFCOPY_VEC3(testPoint, m[PF_T]);

	for (i = 0; MasterGuidewayList[i] != NULL; i++) {
		if (MasterGuidewayList[i] != thisGA) {
			dist = closestPointOnGuideway(MasterGuidewayList[i], testPoint, &t);
			if (dist >= 0.0f) {
				if (closeDist < 0.0f || dist < closeDist) {
					closeDist = dist;
					if (closeGA != NULL) *closeGA = MasterGuidewayList[i];
					if (closeT  != NULL) *closeT = t;
				}
			}
		}
	}
	return(closeDist);
}

void BirdJump(long type)
{
	guideArc	*closeGA;
	float		closeT, dist, dot;
	float		maxJumpDist = 2.0f;
	pfMatrix	fromMat, toMat;

	dist = findClosestTrackPos(birdTrack, birdTrackT, &closeGA, &closeT);
	if (dist > 0.0f && dist < maxJumpDist && closeGA != NULL) {
		findGuidewayPos(fromMat, birdTrack, birdTrackT);

		birdTrack = closeGA;
		birdTrackT = closeT;

		findGuidewayPos(toMat, closeGA, birdTrackT);
		
		/**** adjust the speed too ****/
		dot = PFDOT_VEC3(fromMat[PF_Y], toMat[PF_Y]);
		birdSpeed *= dot;
		if (dot < 0) birdSpeedLevel = -birdSpeedLevel;
	}
}


