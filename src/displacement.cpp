
#include <stdio.h>
#include <glut.h>
#include "genincludes.h"
#include "umath.h"
#include "upmath.h"
#include "timer.h"
#include "texture.h"
#include "displacement.h"

/**** arrays for sub-cubes ****/
#define MAX_DIVISIONS 16
pfVec2	arrayTV[MAX_DIVISIONS+1][MAX_DIVISIONS+1];
pfVec3	 arrayV[MAX_DIVISIONS+1][MAX_DIVISIONS+1];
pfVec3	arrayNV[MAX_DIVISIONS+1][MAX_DIVISIONS+1];
pfVec4	arrayCV[MAX_DIVISIONS+1][MAX_DIVISIONS+1];

long	CurveTest = 0;
long	CurveColors = 0;
int CurveDiv = 2;

long	DCubeMaterial = 0;
long	DCubeLayers = 64;
float	DCubeDepth = 0.15f;
float	DCubeScale = 2.0f;
long	DCubeTexID = 0;
long	DCubeFlags = 0;
long	DCubeSubdivisions = 1;
long	DCubeForceExtraFlags = 0;
Boolean	DCubeClockRunning = TRUE;
Boolean	DCubeMouseDisturb = FALSE;
pfVec3	DCubeDisturbMouseLoc = {0,0,0};
double	DCubeClock = 0.0f;

//Boolean	DCubeDisturbOn = FALSE;
//pfVec3	DCubeDisturbLoc = {0,0,0};
//float	DCubeDisturbStrength = 1.0f;
//float	DCubeDisturbSpringFreq = 1.0f;
//float	DCubeDisturbSpringDecay = 1.0f;

void interp2(pfVec2 vout, pfVec2 v1, pfVec2 v2, float t)
{
	PFSUB_VEC2(vout, v2, v1);
	PFSCALE_VEC2(vout, t, vout);
	PFADD_VEC2(vout, vout, v1);
}

void interp3(pfVec3 vout, pfVec3 v1, pfVec3 v2, float t)
{
	PFSUB_VEC3(vout, v2, v1);
	PFSCALE_VEC3(vout, t, vout);
	PFADD_VEC3(vout, vout, v1);
}

void interp4(pfVec4 vout, pfVec4 v1, pfVec4 v2, float t)
{
	PFSUB_VEC4(vout, v2, v1);
	PFSCALE_VEC4(vout, t, vout);
	PFADD_VEC4(vout, vout, v1);
}


void UpCurveDiv(void)
{
	if (CurveDiv <= 0) CurveDiv = 1;
	else CurveDiv *= 2;

	if (CurveDiv > 64) CurveDiv = 64;
}

void DownCurveDiv(void)
{
	CurveDiv /= 2;
}


void curveTriangle(int flags, int v0, int v1, int v2, pfVec3 *pv, pfVec2 *tv, pfVec4 *cv, pfVec3 *nv)
{
	int	CurveDiv0;
	int	CurveDiv1;
	int	CurveDiv2;
	int	SubdivLevel;
	int	ring, s1, s2, div, i, d, side;
	float	ringT, ringTStep, t, tstep, radius;
	pfVec3	p0, p1, p2, p3, pA, pB, pCenter, pOuter[6], pop, pop0, pop1, mp0, mp1;
	pfVec3	n0, n1, n2, n3, nA, nB, nCenter;
	pfVec2	t0, t1, t2, t3, tA, tB, tCenter;
	pfVec4	c0, c1, c2, c3, cA, cB, cCenter;

	CurveDiv0 = CurveDiv1 = CurveDiv2 = CurveDiv;

	SubdivLevel = CurveDiv0;
	if (CurveDiv1 > SubdivLevel) SubdivLevel = CurveDiv1;
	if (CurveDiv2 > SubdivLevel) SubdivLevel = CurveDiv2;
	
	if (SubdivLevel == 0) {
		if (flags & DIS_FLAG_WIREFRAME) glBegin(GL_LINE_STRIP);
		else glBegin(GL_TRIANGLE_STRIP);

		glColor4fv(cv[v0]);
		glTexCoord2fv(tv[v0]);
		glVertex3fv(pv[v0]);

		glColor4fv(cv[v1]);
		glTexCoord2fv(tv[v1]);
		glVertex3fv(pv[v1]);

		glColor4fv(cv[v2]);
		glTexCoord2fv(tv[v2]);
		glVertex3fv(pv[v2]);

		glEnd();
	}
	else {
		interp3(pOuter[0], pv[v0], pv[v2], 1.0f/3.0f);
		interp3(pOuter[1], pv[v0], pv[v1], 1.0f/3.0f);
		interp3(pOuter[2], pv[v1], pv[v0], 1.0f/3.0f);
		interp3(pOuter[3], pv[v1], pv[v2], 1.0f/3.0f);
		interp3(pOuter[4], pv[v2], pv[v1], 1.0f/3.0f);
		interp3(pOuter[5], pv[v2], pv[v0], 1.0f/3.0f);

		pfSetVec3(pCenter, 0.0f, 0.0f, 0.0f);
		for (i = 0; i < 6; i++) {
			pfAddVec3(pCenter, pCenter, pOuter[i]);
		}
		pfScaleVec3(pCenter, 1.0f/6.0f, pCenter);
		
		
		glColor4fv(cv[v0]);
		glTexCoord2fv(tv[v0]);

		if (flags & DIS_FLAG_WIREFRAME) {
			glBegin(GL_LINE_LOOP);
			glVertex3fv(pOuter[1]);
			glVertex3fv(pv[v0]);
			glVertex3fv(pOuter[0]);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3fv(pOuter[2]);
			glVertex3fv(pOuter[1]);
			glVertex3fv(pCenter);
			glVertex3fv(pOuter[0]);
			glVertex3fv(pOuter[5]);
			glVertex3fv(pCenter);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3fv(pv[v1]);
			glVertex3fv(pOuter[2]);
			glVertex3fv(pOuter[3]);
			glVertex3fv(pCenter);
			glVertex3fv(pOuter[4]);
			glVertex3fv(pOuter[5]);
			glVertex3fv(pv[v2]);
			glVertex3fv(pOuter[4]);
			glVertex3fv(pOuter[3]);
			glEnd();
		}
		else {
			glBegin(GL_TRIANGLE_STRIP);
			glVertex3fv(pOuter[1]);
			glVertex3fv(pv[v0]);
			glVertex3fv(pOuter[0]);
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
			glVertex3fv(pOuter[2]);
			glVertex3fv(pOuter[1]);
			glVertex3fv(pCenter);
			glVertex3fv(pOuter[0]);
			glVertex3fv(pOuter[5]);
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
			glVertex3fv(pv[v1]);
			glVertex3fv(pOuter[2]);
			glVertex3fv(pOuter[3]);
			glVertex3fv(pCenter);
			glVertex3fv(pOuter[4]);
			glVertex3fv(pOuter[5]);
			glVertex3fv(pv[v2]);
			glEnd();
		}
	}
}


void old_radius_curveTriangle(int flags, int v0, int v1, int v2, pfVec3 *pv, pfVec2 *tv, pfVec4 *cv, pfVec3 *nv)
{
	int	CurveDiv0;
	int	CurveDiv1;
	int	CurveDiv2;
	int	Rings;
	int	ring, s1, s2, div, i, d, side;
	float	ringT, ringTStep, t, tstep, radius;
	pfVec3	p0, p1, p2, p3, pA, pB, pcenter, pop, pop0, pop1, mp0, mp1;
	pfVec3	n0, n1, n2, n3, nA, nB, ncenter;
	pfVec2	t0, t1, t2, t3, tA, tB, tcenter;
	pfVec4	c0, c1, c2, c3, cA, cB, ccenter;

	CurveDiv0 = CurveDiv1 = CurveDiv2 = CurveDiv;

	Rings = CurveDiv0;
	if (CurveDiv1 > Rings) Rings = CurveDiv1;
	if (CurveDiv2 > Rings) Rings = CurveDiv2;
	
	if (Rings == 0) {
		if (flags & DIS_FLAG_WIREFRAME) glBegin(GL_LINE_STRIP);
		else glBegin(GL_TRIANGLE_STRIP);

		glColor4fv(cv[v0]);
		glTexCoord2fv(tv[v0]);
		glVertex3fv(pv[v0]);

		glColor4fv(cv[v1]);
		glTexCoord2fv(tv[v1]);
		glVertex3fv(pv[v1]);

		glColor4fv(cv[v2]);
		glTexCoord2fv(tv[v2]);
		glVertex3fv(pv[v2]);

		glEnd();
	}
	else {
		if (CurveDiv0 < 1) CurveDiv0 = 1;
		if (CurveDiv1 < 1) CurveDiv1 = 1;
		if (CurveDiv2 < 1) CurveDiv2 = 1;
		
		radius = pfLengthVec3(pv[v0]);	/**** so we can cheat later ****/

		PFADD_VEC3(ncenter, nv[v0], nv[v1]);
		PFADD_VEC3(ncenter, ncenter, nv[v2]);
		PFSCALE_VEC3(ncenter, 1.0f/3.0f, ncenter);
//		pfNormalizeVec3(ncenter);
		
		PFADD_VEC2(tcenter, tv[v0], tv[v1]);
		PFADD_VEC2(tcenter, tcenter, tv[v2]);
		PFSCALE_VEC2(tcenter, 1.0f/3.0f, tcenter);
if (CurveColors) {
	pfSetVec4(cv[v0], 1, 0, 0, 1);
	pfSetVec4(cv[v1], 0, 1, 0, 1);
	pfSetVec4(cv[v2], 0, 0, 1, 1);
}
		PFADD_VEC3(ccenter, cv[v0], cv[v1]);
		PFADD_VEC3(ccenter, ccenter, cv[v2]);
		PFSCALE_VEC3(ccenter, 1.0f/3.0f, ccenter);

		PFADD_VEC3(pcenter, pv[v0], pv[v1]);
		PFADD_VEC3(pcenter, pcenter, pv[v2]);
		PFSCALE_VEC3(pcenter, 1.0f/3.0f, pcenter);

if (0) {
		pfSetVec3(pcenter, 0.0f, 0.0f, 0.0f);
		pfAddScaledVec3(pop, pv[v0], -(pfDotVec3(pv[v0], nv[v1]) - pfDotVec3(pv[v1], nv[v1])), nv[v1]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
		pfAddScaledVec3(pop, pv[v0], -(pfDotVec3(pv[v0], nv[v2]) - pfDotVec3(pv[v2], nv[v2])), nv[v2]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
		pfAddScaledVec3(pop, pv[v1], -(pfDotVec3(pv[v1], nv[v0]) - pfDotVec3(pv[v0], nv[v0])), nv[v0]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
		pfAddScaledVec3(pop, pv[v1], -(pfDotVec3(pv[v1], nv[v2]) - pfDotVec3(pv[v2], nv[v2])), nv[v2]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
		pfAddScaledVec3(pop, pv[v2], -(pfDotVec3(pv[v2], nv[v0]) - pfDotVec3(pv[v0], nv[v0])), nv[v0]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
		pfAddScaledVec3(pop, pv[v2], -(pfDotVec3(pv[v2], nv[v1]) - pfDotVec3(pv[v1], nv[v1])), nv[v1]);
		pfAddScaledVec3(pcenter, pcenter, 1.0f/6.0f, pop);
}		
//		pfAddScaledVec3(pcenter, pcenter, 0.95f, ncenter);

if (1) {	/**** cheat and use the known sphere ****/
	pfNormalizeVec3(pcenter);
	pfCopyVec3(ncenter, pcenter);
	pfScaleVec3(pcenter, radius, pcenter);
}

		ringT = 0.0f;
		ringTStep = 1.0f / (float)Rings;
		for (ring = 0; ring < Rings; ring++) {
			for (side = 0; side < 3; side++) {
				if (side == 0) {
					s1 = v0;
					s2 = v1;
					div = CurveDiv0;
				}
				else if (side == 1) {
					s1 = v1;
					s2 = v2;
					div = CurveDiv1;
				}
				else if (side == 2) {
					s1 = v2;
					s2 = v0;
					div = CurveDiv2;
				}
				if (flags & DIS_FLAG_WIREFRAME) glBegin(GL_LINE_STRIP);
				else glBegin(GL_TRIANGLE_STRIP);

				interp3(n0, nv[s1], ncenter, ringT+ringTStep);
				interp2(t0, tv[s1], tcenter, ringT+ringTStep);
				interp4(c0, cv[s1], ccenter, ringT+ringTStep);
				interp3(p0, pv[s1], pcenter, ringT+ringTStep);
				interp3(n1, nv[s1], ncenter, ringT);
				interp2(t1, tv[s1], tcenter, ringT);
				interp4(c1, cv[s1], ccenter, ringT);
				interp3(p1, pv[s1], pcenter, ringT);
				interp3(n2, nv[s2], ncenter, ringT+ringTStep);
				interp2(t2, tv[s2], tcenter, ringT+ringTStep);
				interp4(c2, cv[s2], ccenter, ringT+ringTStep);
				interp3(p2, pv[s2], pcenter, ringT+ringTStep);
				interp3(n3, nv[s2], ncenter, ringT);
				interp2(t3, tv[s2], tcenter, ringT);
				interp4(c3, cv[s2], ccenter, ringT);
				interp3(p3, pv[s2], pcenter, ringT);

//pfCopyVec3(n0, nv[s1]);
//pfCopyVec3(n1, nv[s1]);
//pfCopyVec3(n2, nv[s2]);
//pfCopyVec3(n3, nv[s2]);

				pfAddScaledVec3(pop0, pv[s1], -(pfDotVec3(pv[s1], ncenter) - pfDotVec3(pcenter, ncenter)), ncenter);
				pfAddScaledVec3(pop1, pcenter, -(pfDotVec3(pcenter, nv[s1]) - pfDotVec3(pv[s1], nv[s1])), nv[s1]);
				interp3(mp0, pv[s1], pop0, ringT);
				interp3(mp1, pop1, pcenter, ringT);
				interp3(p1, mp0, mp1, ringT);
				interp3(mp0, pv[s1], pop0, ringT+ringTStep);
				interp3(mp1, pop1, pcenter, ringT+ringTStep);
				interp3(p0, mp0, mp1, ringT+ringTStep);
				
				pfAddScaledVec3(pop0, pv[s2], -(pfDotVec3(pv[s2], ncenter) - pfDotVec3(pcenter, ncenter)), ncenter);
				pfAddScaledVec3(pop1, pcenter, -(pfDotVec3(pcenter, nv[s2]) - pfDotVec3(pv[s2], nv[s2])), nv[s2]);
				interp3(mp0, pv[s2], pop0, ringT);
				interp3(mp1, pop1, pcenter, ringT);
				interp3(p3, mp0, mp1, ringT);
				interp3(mp0, pv[s2], pop0, ringT+ringTStep);
				interp3(mp1, pop1, pcenter, ringT+ringTStep);
				interp3(p2, mp0, mp1, ringT+ringTStep);
				
//				pfNormalizeVec3(n0);
//				pfNormalizeVec3(n1);
//				pfNormalizeVec3(n2);
//				pfNormalizeVec3(n3);
if (1) {	/**** cheat and use the known sphere ****/
	pfCopyVec3(n0, p0);
	pfCopyVec3(n1, p1);
	pfCopyVec3(n2, p2);
	pfCopyVec3(n3, p3);
	pfNormalizeVec3(n0);
	pfNormalizeVec3(n1);
	pfNormalizeVec3(n2);
	pfNormalizeVec3(n3);
}
if (1) {
	pfScaleVec3(p0, radius, n0);
	pfScaleVec3(p1, radius, n1);
	pfScaleVec3(p2, radius, n2);
	pfScaleVec3(p3, radius, n3);
}

				glColor4fv(c0);
				glTexCoord2fv(t0);
				glVertex3fv(p0);

				glColor4fv(c1);
				glTexCoord2fv(t1);
				glVertex3fv(p1);
								
				t = 0.0f;
				tstep = 1.0f / (float)div;
				for (d = 0; d < div; d++) {
					t += tstep;

//					interp3(nA, n0, n2, t);
					interp2(tA, t0, t2, t);
					interp4(cA, c0, c2, t);
					interp3(pA, p0, p2, t);

//					interp3(nB, n1, n3, t);
					interp2(tB, t1, t3, t);
					interp4(cB, c1, c3, t);
					interp3(pB, p1, p3, t);

					pfAddScaledVec3(pop0, p0, -(pfDotVec3(p0, n2) - pfDotVec3(p2, n2)), n2);
					pfAddScaledVec3(pop1, p2, -(pfDotVec3(p2, n0) - pfDotVec3(p0, n0)), n0);
					interp3(mp0, p0, pop0, t);
					interp3(mp1, pop1, p2, t);
					interp3(pA, mp0, mp1, t);

					pfAddScaledVec3(pop0, p1, -(pfDotVec3(p1, n3) - pfDotVec3(p3, n3)), n3);
					pfAddScaledVec3(pop1, p3, -(pfDotVec3(p3, n1) - pfDotVec3(p1, n1)), n1);
					interp3(mp0, p1, pop0, t);
					interp3(mp1, pop1, p3, t);
					interp3(pB, mp0, mp1, t);

//pfNormalizeVec3(pA);
//pfScaleVec3(pA, radius, pA);
//pfNormalizeVec3(pB);
//pfScaleVec3(pB, radius, pB);

					glColor4fv(cA);
					glTexCoord2fv(tA);
					glVertex3fv(pA);

					glColor4fv(cB);
					glTexCoord2fv(tB);
					glVertex3fv(pB);
					
				}
				glEnd();
			}
			ringT += ringTStep;
		}
	}
}


void DrawCurveCube(long textureID, long flags, long layers, float depth, long divisions, pfMatrix mat)
{
	long	i, j, face, v, layer;
	long	numFaces = 6;
	float	depthStep, d, t, bright, sval, cval;
	pfVec3	tempVerts[4];
	pfVec3	tempNVerts[4];
	pfVec4	tempCVerts[4];
	pfVec3	avgNorm;
	
	pfVec3	verts[] = {		{-1, -1, -1},
							{-1,  1, -1},
							{ 1,  1, -1},
							{ 1, -1, -1},
							{-1, -1,  1},
							{-1,  1,  1},
							{ 1,  1,  1},
							{ 1, -1,  1}};
	
	pfVec3	nVerts[] = {	{-1, -1, -1},
							{-1,  1, -1},
							{ 1,  1, -1},
							{ 1, -1, -1},
							{-1, -1,  1},
							{-1,  1,  1},
							{ 1,  1,  1},
							{ 1, -1,  1}};
	
	pfVec4	cVerts[] = {	{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1}};
	
	pfVec2	tVerts[] = {	{0, 0},
							{0, 1},
							{1, 1},
							{1, 0}};
	
	long polys[][4] = {	{ 0, 1, 2, 3 },
						{ 4, 7, 6, 5 },
						{ 4, 5, 1, 0 },
						{ 6, 7, 3, 2 },
						{ 4, 0, 3, 7 },
						{ 5, 6, 2, 1 }};
	
	pfVec3	lightVec = {1, 2, 5};					
	
	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
   	glDisable(GL_BLEND);
	glPushMatrix();
	glMultMatrixf((float *)mat);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
   	glDisable(GL_BLEND);
   	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UseLibTexture(textureID);
//	UseLibTexture(NULL);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL, 0);

	if (flags & DIS_FLAG_SHOW_BACKFACE) {
		glDisable(GL_CULL_FACE);
	}
	if (flags & DIS_FLAG_BLEND) {
		glEnable(GL_BLEND);
	}
	if (flags & DIS_FLAG_NO_ZBUFFER) {
		glDisable(GL_DEPTH_TEST);
	}

	if (divisions < 1) divisions = 1;
	if (divisions > MAX_DIVISIONS) divisions = MAX_DIVISIONS;
	
	if (depth == 0.0f) {
		layers = 1;
		depth = 1.0f;
	}
	if (layers < 1) layers = 1;
	if (layers == 1) depthStep = 0.0f;
	else depthStep = depth / (layers - 1);
	if (flags & DIS_FLAG_FLAT) {
		layers = 1;
	}
	/**** normalize the normals. duh. ****/
	for (i = 0; i < sizeof(nVerts) / sizeof(nVerts[0]); i++) {
		pfNormalizeVec3(nVerts[i]);
	}
	pfNormalizeVec3(lightVec);
	
	for (layer = 0; layer < layers; layer++) {
		d = depthStep * layer;
		t = d / depth;
		
		glAlphaFunc(GL_GEQUAL, t);
//		if (!(flags & DIS_FLAG_WIREFRAME)) glBegin(GL_QUADS);
		
		for (face = 0; face < numFaces; face++) {
			/**** build the average normal ****/
			pfSetVec3(avgNorm, 0, 0, 0);
			for (v = 0; v < 4; v++) {
				pfAddVec3(avgNorm, avgNorm, nVerts[polys[face][v]]);
			}
			pfNormalizeVec3(avgNorm);
			
			for (v = 0; v < 4; v++) {
				/**** copy the corner position ****/
				pfCopyVec3(tempVerts[v], verts[polys[face][v]]);
				
				/**** set the normal ****/
				if (flags & DIS_FLAG_AVERAGE_NORM) pfCopyVec3(tempNVerts[v], avgNorm);
				else pfCopyVec3(tempNVerts[v], nVerts[polys[face][v]]);
				
				/**** set the color based on the normal. (fake lighting) ****/
				PFCOPY_VEC4(tempCVerts[v], cVerts[polys[face][v]]);
				bright = pfDotVec3(tempNVerts[v], lightVec);
				bright = (bright * 0.25f) + 0.75f;
				pfScaleVec3(tempCVerts[v], bright, tempCVerts[v]);	
			}
			if (1) {
				if (flags & DIS_FLAG_SHAKE) pfSinCos((DCubeClock + 0.0f) * 360.0f, &sval, &cval);
				
				/**** extrude along the normal ****/
				for (v = 0; v < 4; v++) {
					if (flags & DIS_FLAG_SHAKE) tempNVerts[v][PF_X] += 0.5f * sval;
					pfAddScaledVec3(tempVerts[v], tempVerts[v], d, tempNVerts[v]);
				}
				/**** draw the face ****/
				curveTriangle(flags, 0, 1, 2, tempVerts, tVerts, tempCVerts, tempNVerts);
				curveTriangle(flags, 0, 2, 3, tempVerts, tVerts, tempCVerts, tempNVerts);
			}
		}
//		if (!(flags & DIS_FLAG_WIREFRAME)) glEnd();
	}
	
	glPopMatrix();
	glPopAttrib();
}

void DrawDisplacementCube(long textureID, long flags, long layers, float depth, long divisions, pfMatrix mat)
{
	long	i, j, face, v, layer;
	long	numFaces = 6;
	float	depthStep, d, t, bright, sval, cval;
	pfVec3	tempVerts[4];
	pfVec3	tempNVerts[4];
	pfVec4	tempCVerts[4];
	pfVec3	avgNorm;
	
	pfVec3	verts[] = {		{-1, -1, -1},
							{-1,  1, -1},
							{ 1,  1, -1},
							{ 1, -1, -1},
							{-1, -1,  1},
							{-1,  1,  1},
							{ 1,  1,  1},
							{ 1, -1,  1}};
	
	pfVec3	nVerts[] = {	{-1, -1, -1},
							{-1,  1, -1},
							{ 1,  1, -1},
							{ 1, -1, -1},
							{-1, -1,  1},
							{-1,  1,  1},
							{ 1,  1,  1},
							{ 1, -1,  1}};
	
	pfVec4	cVerts[] = {	{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1},
							{1, 1, 1, 1}};
	
	pfVec2	tVerts[] = {	{0, 0},
							{0, 1},
							{1, 1},
							{1, 0}};
	
	long polys[][4] = {	{ 0, 1, 2, 3 },
						{ 4, 7, 6, 5 },
						{ 4, 5, 1, 0 },
						{ 6, 7, 3, 2 },
						{ 4, 0, 3, 7 },
						{ 5, 6, 2, 1 }};
	
	pfVec3	lightVec = {1, 2, 5};					
	
	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
    glDisable(GL_LIGHTING);
   	glDisable(GL_BLEND);
	glPushMatrix();
	glMultMatrixf((float *)mat);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
   	glDisable(GL_BLEND);
   	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UseLibTexture(textureID);
//	UseLibTexture(NULL);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL, 0);

	if (flags & DIS_FLAG_SHOW_BACKFACE) {
		glDisable(GL_CULL_FACE);
	}
	if (flags & DIS_FLAG_BLEND) {
		glEnable(GL_BLEND);
	}
	if (flags & DIS_FLAG_NO_ZBUFFER) {
		glDisable(GL_DEPTH_TEST);
	}

	if (divisions < 1) divisions = 1;
	if (divisions > MAX_DIVISIONS) divisions = MAX_DIVISIONS;
	
	if (depth == 0.0f) {
		layers = 1;
		depth = 1.0f;
	}
	if (layers < 1) layers = 1;
	if (layers == 1) depthStep = 0.0f;
	else depthStep = depth / (layers - 1);
	if (flags & DIS_FLAG_FLAT) {
		layers = 1;
	}
	/**** normalize the normals. duh. ****/
	for (i = 0; i < sizeof(nVerts) / sizeof(nVerts[0]); i++) {
		pfNormalizeVec3(nVerts[i]);
	}
	pfNormalizeVec3(lightVec);
	
	for (layer = 0; layer < layers; layer++) {
		d = depthStep * layer;
		t = d / depth;
		
		glAlphaFunc(GL_GEQUAL, t);
		if (!(flags & DIS_FLAG_WIREFRAME)) glBegin(GL_QUADS);
		
		for (face = 0; face < numFaces; face++) {
			/**** build the average normal ****/
			pfSetVec3(avgNorm, 0, 0, 0);
			for (v = 0; v < 4; v++) {
				pfAddVec3(avgNorm, avgNorm, nVerts[polys[face][v]]);
			}
			pfNormalizeVec3(avgNorm);
			
			for (v = 0; v < 4; v++) {
				/**** copy the corner position ****/
				pfCopyVec3(tempVerts[v], verts[polys[face][v]]);
				
				/**** set the normal ****/
				if (flags & DIS_FLAG_AVERAGE_NORM) pfCopyVec3(tempNVerts[v], avgNorm);
				else pfCopyVec3(tempNVerts[v], nVerts[polys[face][v]]);
				
				/**** set the color based on the normal. (fake lighting) ****/
				PFCOPY_VEC4(tempCVerts[v], cVerts[polys[face][v]]);
				bright = pfDotVec3(tempNVerts[v], lightVec);
				bright = (bright * 0.25f) + 0.75f;
				pfScaleVec3(tempCVerts[v], bright, tempCVerts[v]);	
			}
			if (divisions == 1) {
				if (flags & DIS_FLAG_SHAKE) pfSinCos((DCubeClock + 0.0f) * 360.0f, &sval, &cval);
				
				/**** extrude along the normal ****/
				for (v = 0; v < 4; v++) {
					if (flags & DIS_FLAG_SHAKE) tempNVerts[v][PF_X] += 0.5f * sval;
					pfAddScaledVec3(tempVerts[v], tempVerts[v], d, tempNVerts[v]);
				}
				if (flags & DIS_FLAG_WIREFRAME) glBegin(GL_LINE_LOOP);
				/**** draw the face ****/
				for (v = 0; v < 4; v++) {
					glTexCoord2fv(tVerts[v]);
					glColor4fv(tempCVerts[v]);
					glVertex3fv(tempVerts[v]);
				}
				if (flags & DIS_FLAG_WIREFRAME) glEnd();
			}
			else {
				/**** divide into sub-cubes ****/
				pfVec2	dTVecH, dTVecV;
				pfVec3	dVecH, dVecV, dNVecH, dNVecV;
				pfVec4	dCVecH, dCVecV;
				long	dh, dv;
				float	th1, th2, tv1, tv2, dStrength;
				pfVec2	tv, r_tv;
				pfVec3	v, nv, r_v, r_nv, disturbVec;
				pfVec4	cv, r_cv;
				
				PFSUB_VEC3(dVecH,   tempVerts[1],  tempVerts[0]); pfScaleVec3(dVecH, 1.0f / divisions, dVecH);
				PFSUB_VEC3(dVecV,   tempVerts[3],  tempVerts[0]); pfScaleVec3(dVecV, 1.0f / divisions, dVecV);
				PFSUB_VEC3(dNVecH, tempNVerts[1], tempNVerts[0]); pfScaleVec3(dNVecH, 1.0f / divisions, dNVecH);
				PFSUB_VEC3(dNVecV, tempNVerts[3], tempNVerts[0]); pfScaleVec3(dNVecV, 1.0f / divisions, dNVecV);
				PFSUB_VEC4(dCVecH, tempCVerts[1], tempCVerts[0]); PFSCALE_VEC4(dCVecH, 1.0f / divisions, dCVecH);
				PFSUB_VEC4(dCVecV, tempCVerts[3], tempCVerts[0]); PFSCALE_VEC4(dCVecV, 1.0f / divisions, dCVecV);
				PFSUB_VEC2(dTVecH,     tVerts[1],     tVerts[0]); PFSCALE_VEC2(dTVecH, 1.0f / divisions, dTVecH);
				PFSUB_VEC2(dTVecV,     tVerts[3],     tVerts[0]); PFSCALE_VEC2(dTVecV, 1.0f / divisions, dTVecV);
				PFCOPY_VEC3(r_v,  tempVerts[0]);
				PFCOPY_VEC3(r_nv, tempNVerts[0]);
				PFCOPY_VEC4(r_cv, tempCVerts[0]);
				PFCOPY_VEC2(r_tv, tVerts[0]);
				for (dh = 0; dh <= divisions; dh++) {
					th1 = (float)dh / (float)divisions;
					PFCOPY_VEC3(v, r_v);
					PFCOPY_VEC3(nv, r_nv);
					PFCOPY_VEC4(cv, r_cv);
					PFCOPY_VEC2(tv, r_tv);
					if ((flags & DIS_FLAG_SHAKE) || DCubeMouseDisturb) pfSinCos((DCubeClock + th1) * 360.0f, &sval, &cval);
					
					for (dv = 0; dv <= divisions; dv++) {
						tv1 = (float)dv / (float)divisions;
						
						/**** copy them into the grid ****/
						PFCOPY_VEC3(arrayV[dh][dv], v);
						PFCOPY_VEC3(arrayNV[dh][dv], nv);
						PFCOPY_VEC4(arrayCV[dh][dv], cv);
						PFCOPY_VEC2(arrayTV[dh][dv], tv);
						
						/**** clean up ****/
						pfNormalizeVec3(arrayNV[dh][dv]);

						/**** set the disturb stringth, if active ****/
						dStrength = 1.0f;
						if (DCubeMouseDisturb) {
							float dist;
							
							PFSUB_VEC2(disturbVec, arrayV[dh][dv], DCubeDisturbMouseLoc);
							disturbVec[PF_Z] = 0.0f;
							dist = pfNormalizeVec3(disturbVec);
//							dist = pfDistancePt2(arrayV[dh][dv], DCubeDisturbMouseLoc);
							dStrength = 1.0f - (dist * 3.0f);
							dStrength *= 2.0f;
							if (dStrength < 0.0f) dStrength = 0.0f;
			//				dStrength = 1.0f;
						}
						
						/**** alter as necessary ****/
						if (flags & DIS_FLAG_SHAKE) arrayNV[dh][dv][PF_X] += 0.5f * sval;
						if (DCubeMouseDisturb) pfAddScaledVec3(arrayNV[dh][dv], arrayNV[dh][dv], dStrength, disturbVec);
						
						/**** extrude along the normal ****/
						pfAddScaledVec3(arrayV[dh][dv], arrayV[dh][dv], d, arrayNV[dh][dv]);
						
						PFADD_VEC3(v, v, dVecV);
						PFADD_VEC3(nv, nv, dNVecV);
						PFADD_VEC4(cv, cv, dCVecV);
						PFADD_VEC2(tv, tv, dTVecV);
					}
					PFADD_VEC3(r_v, r_v, dVecH);
					PFADD_VEC3(r_nv, r_nv, dNVecH);
					PFADD_VEC4(r_cv, r_cv, dCVecH);
					PFADD_VEC2(r_tv, r_tv, dTVecH);
				}
				/**** draw the faces ****/
				for (dh = 0; dh < divisions; dh++) {
					for (dv = 0; dv < divisions; dv++) {
						if (flags & DIS_FLAG_WIREFRAME) glBegin(GL_LINE_LOOP);						

						glTexCoord2fv(arrayTV[dh+0][dv+0]);
						glColor4fv(arrayCV[dh+0][dv+0]);
						glVertex3fv(arrayV[dh+0][dv+0]);
						
						glTexCoord2fv(arrayTV[dh+1][dv+0]);
						glColor4fv(arrayCV[dh+1][dv+0]);
						glVertex3fv(arrayV[dh+1][dv+0]);
						
						glTexCoord2fv(arrayTV[dh+1][dv+1]);
						glColor4fv(arrayCV[dh+1][dv+1]);
						glVertex3fv(arrayV[dh+1][dv+1]);
						
						glTexCoord2fv(arrayTV[dh+0][dv+1]);
						glColor4fv(arrayCV[dh+0][dv+1]);
						glVertex3fv(arrayV[dh+0][dv+1]);

						if (flags & DIS_FLAG_WIREFRAME) glEnd();
					}
				}
			}
		}
		if (!(flags & DIS_FLAG_WIREFRAME)) glEnd();
	}
	
	glPopMatrix();
	glPopAttrib();
}

typedef struct displacementMaterial {
	char	*texName;
	long	layers;
	float	depth;
	float	cubeScale;
	long	flags;
} displacementMaterial;

displacementMaterial DCubeTestMaterials[] = {
	{ "NULL",	1, 0.15f, 2.0f, DIS_FLAG_AVERAGE_NORM },
//	{ "sphere",	64, 2*0.732f, 1.0f, DIS_FLAG_SHOW_BACKFACE },
	{ "lawn",	32, 0.05f, 2.0f, DIS_FLAG_AVERAGE_NORM },
	{ "lawn",	32, 0.15f, 2.0f, 0 },
	{ "lego2",	32, 0.05f, 2.0f, DIS_FLAG_AVERAGE_NORM },
	{ "lego4",	64, 0.15f, 2.0f, DIS_FLAG_AVERAGE_NORM },
	{ "NULL",	3, 0.15f, 2.0f, DIS_FLAG_AVERAGE_NORM },
	{ "NULL",	3, 0.15f, 2.0f, 0 },
};

void DCubeToggleFlat(void)
{
	DCubeForceExtraFlags ^= DIS_FLAG_FLAT;
}

void DCubeToggleBackface(void)
{
	DCubeForceExtraFlags ^= DIS_FLAG_SHOW_BACKFACE;
}

void DCubeToggleWireframe(void)
{
	DCubeForceExtraFlags ^= DIS_FLAG_WIREFRAME;
}

void DCubeToggleShake(void)
{
	DCubeForceExtraFlags ^= DIS_FLAG_SHAKE;
}

void DCubeToggleMouseDisturb(void)
{
	DCubeMouseDisturb = !DCubeMouseDisturb;
}

void DCubeToggleClock(void)
{
	DCubeClockRunning = !DCubeClockRunning;
}

void DCubeSubdivide(long jump)
{
	DCubeSubdivisions += jump;
	if (DCubeSubdivisions < 1) DCubeSubdivisions = 1;
	if (DCubeSubdivisions > MAX_DIVISIONS) DCubeSubdivisions = MAX_DIVISIONS;
}

void DCubeChangeDepth(float delta)
{
	DCubeDepth += delta;
}

void DCubeChangeLayers(long jump)
{
	if (jump > 0) DCubeLayers <<= 1;
	else if (jump < 0) DCubeLayers >>= 1;
	if (DCubeLayers < 1) DCubeLayers = 1;
	if (DCubeLayers > 256) DCubeLayers = 256;
}

void SwitchDCubeMaterial(long jump)
{
	long	count;
	displacementMaterial	*dm;
	
	count = sizeof(DCubeTestMaterials) / sizeof(DCubeTestMaterials[0]);
	DCubeMaterial += jump;
	while (DCubeMaterial < 0) DCubeMaterial += count;
	while (DCubeMaterial >= count) DCubeMaterial -= count;
	
	dm = &(DCubeTestMaterials[DCubeMaterial]);
	DCubeLayers = dm->layers;
	DCubeDepth = dm->depth;
	DCubeScale = dm->cubeScale;
	DCubeTexID = GetTextureID(dm->texName);
	DCubeFlags = dm->flags;
}

void ToggleCurve(void)
{
	CurveTest = !CurveTest;
}

void ToggleCurveColors(void)
{
	CurveColors = !CurveColors;
}

void TestDisplacementCube(void)
{
	pfMatrix		mat;
	long			flags;
	
	if (DCubeClockRunning) DCubeClock += DeltaTime;
//	if (DCubeClockRunning && DCubeMouseDisturb) DCubeClock += DeltaTime;	/**** add it again to speed it up ****/
	flags = DCubeFlags | DCubeForceExtraFlags;
	pfMakeIdentMat(mat);
	pfMakeScaleMat(mat, DCubeScale, DCubeScale, DCubeScale);

	if (CurveTest) DrawCurveCube(DCubeTexID, flags, DCubeLayers, DCubeDepth, DCubeSubdivisions, mat);
	else DrawDisplacementCube(DCubeTexID, flags, DCubeLayers, DCubeDepth, DCubeSubdivisions, mat);
}

