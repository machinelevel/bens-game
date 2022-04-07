
/************************************************************\
	cellgrid.cpp
	Files for controlling the grid of cells in Ben's project
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <string.h>
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
#include "slides.h"
#include "sound.h"

#include "wallmaps.h"

CellGrid	*gCells = NULL;
const float	hexContract = 0.866f; /**** sqrt(3)/2 ****/
float	gHalfPipeRadius = 10.0f;
int		gSizzleCycle = 0;

CellGrid::CellGrid(int32 hSize, int32 vSize)
{
hSize = vSize = 80;
	mLifeUpdateTimer = 0.0f;
	mLifeUpdateTimeInterval = 0.1f;
mLifeUpdateTimeInterval = 0.5f;
	mHSize = hSize;
	mVSize = vSize;
	mCellSize = 2.0f;
	mNumCells = hSize * vSize;
	mCells = new OneCell[mNumCells];
	pfSetVec3(mCenter, 0.0f, 0.0f, 0.0f);
	Clear();
	mNumWalls = 0;
}

void CellGrid::Clear(void)
{
	extern int32	gBadHistorySamples;
	int32	i;

	gBadHistorySamples = 0;
	for (i = 0; i < mNumCells; i++) {
		mCells[i].shift = 0;
		mCells[i].flags = 0;
		mCells[i].goodTimer = 0;
	}
}

#define MAP_WD	39
#define MAP_HT	19

void CellGrid::DrawBackdrop(void)
{
	int		i;
	pfVec4	hue;
	static float	ctime[3] = {0,0,0};
	float			cfreq[3] = {0.01f,0.02f,0.03f};
	float	sval, cval;
	float	hSize = 200.0f;
	float	vSize = 200.0f;

	for (i = 0; i < 3; i++) {
		ctime[i] += 360.0f * DeltaTime * cfreq[i];
		if (ctime[i] > 360.0f) ctime[i] -= 360.0f;
		pfSinCos(ctime[i], &sval, &cval);
		hue[i] = (sval+1.0f)*0.15f;
	}
	hue[3] = 1.0f;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDepthMask(false);
	glBegin(GL_TRIANGLE_STRIP);
	glColor4fv(hue);    glVertex3f( hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize,  hSize, vSize);
	glColor4fv(hue);    glVertex3f(-hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f(-hSize,  hSize, vSize);
	glColor4fv(hue);    glVertex3f(-hSize, -hSize, 0);
	glColor4f(0,0,0,1); glVertex3f(-hSize, -hSize, vSize);
	glColor4fv(hue);    glVertex3f( hSize, -hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize, -hSize, vSize);
	glColor4fv(hue);    glVertex3f( hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize,  hSize, vSize);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glColor4fv(hue);    glVertex3f( hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize,  hSize, -vSize);
	glColor4fv(hue);    glVertex3f(-hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f(-hSize,  hSize, -vSize);
	glColor4fv(hue);    glVertex3f(-hSize, -hSize, 0);
	glColor4f(0,0,0,1); glVertex3f(-hSize, -hSize, -vSize);
	glColor4fv(hue);    glVertex3f( hSize, -hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize, -hSize, -vSize);
	glColor4fv(hue);    glVertex3f( hSize,  hSize, 0);
	glColor4f(0,0,0,1); glVertex3f( hSize,  hSize, -vSize);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(true);
}

void CellGrid::SetupWalls(void)
{
	char	map[MAP_HT*MAP_WD+1], tag;
	int32	i, j, wall = 0;
	float	xsize = 70, ysize = 60, x1, y1, x2, y2;
	float	xscale = xsize / (MAP_WD / 2);
	float	yscale = ysize / (MAP_HT / 2);
	bool	found;
	int		mapID;

	mapID = 7 * gDifficultySetting;
	mapID += gLevels[gCurrentLevel].mLevelNumber;
	strcpy(map, gWallMaps[mapID]);
	for (i = 0; i < MAP_HT*MAP_WD; i++) {
		tag = map[i];
		found = false;
		if (tag != '.' && tag != '=' && tag != '|') {
			for (j = i+1; j < MAP_HT*MAP_WD && !found; j++) {
				if (map[j] == tag) {
					x1 = ((i % MAP_WD) - (MAP_WD / 2)) * xscale;
					y1 = ((i / MAP_WD) - (MAP_HT / 2)) * yscale;
					x2 = ((j % MAP_WD) - (MAP_WD / 2)) * xscale;
					y2 = ((j / MAP_WD) - (MAP_HT / 2)) * yscale;
					pfSetVec3(mWalls[wall][0], x1, y1, 0);
					pfSetVec3(mWalls[wall][1], x2, y2, 0);
					wall++;
					//map[i] = map[j] = '.';
					found = true;
				}
			}
		}
	}
	mNumWalls = wall;
}

void CellGrid::DrawWalls(void)
{
	int32	i;
	pfVec4	hue;
	float	*f1, *f2;
	float	zmin = 0.1f, zmax = 2.0f;
	static float	ttimer = 0.0f, toff = 0.0f;
	float	toffset1, toffset2;
	float	stretch;

	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	UseLibTexture(TEXID_ZAP);

	ttimer += 20.0f * DeltaTime;
	if (ttimer > 1.0f) {
		ttimer -= 1.0f;
		toff += 0.25f;
		if (toff >= 1.0f) toff = 0.0f;
	}

	glBegin(GL_QUADS);
	for (i = 0; i < mNumWalls; i++) {
		stretch = RANDOM_IN_RANGE(-2.0f, 1.0f);
		toffset1 = toff;
		toffset2 = toff + 0.25f;

		f1 = (float*)mWalls[i][0];
		f2 = (float*)mWalls[i][1];
//		PFCOPY_VEC4(hue, healthFillColor[mWallTypes[i]]);
		PFSET_VEC4(hue, 1.0f, 1.0f, 1.0f, 1.0f);
		hue[3] = RANDOM_IN_RANGE(0.5f, 1.0f);
		glColor4fv(hue);
		glTexCoord2f(0.0f, toffset1);
		glVertex3f(f1[PF_X], f1[PF_Y], zmin - stretch);
		hue[3] = RANDOM_IN_RANGE(0.5f, 1.0f);
		glColor4fv(hue);
		glTexCoord2f(0.0f, toffset2);
		glVertex3f(f1[PF_X], f1[PF_Y], zmax + stretch);
		hue[3] = RANDOM_IN_RANGE(0.5f, 1.0f);
		glColor4fv(hue);
		glTexCoord2f(1.0f, toffset2);
		glVertex3f(f2[PF_X], f2[PF_Y], zmax + stretch);
		hue[3] = RANDOM_IN_RANGE(0.5f, 1.0f);
		glColor4fv(hue);
		glTexCoord2f(1.0f, toffset1);
		glVertex3f(f2[PF_X], f2[PF_Y], zmin - stretch);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	for (i = 0; i < mNumWalls; i++) {
		f1 = (float*)mWalls[i][0];
		f2 = (float*)mWalls[i][1];
		PFSET_VEC4(hue, 1.0f, 1.0f, 1.0f, 1.0f);
		glColor4fv(hue);
		glVertex3f(f1[PF_X], f1[PF_Y], zmin);
		glVertex3f(f1[PF_X], f1[PF_Y], zmax);
		glVertex3f(f2[PF_X], f2[PF_Y], zmax);
		glVertex3f(f2[PF_X], f2[PF_Y], zmin);
	}
	glEnd();
}

void CellGrid::DrawHalfPipe(void)
{
	int32	i;
	pfVec4	hue = {1.0f, 1.0f, 1.0f, 0.5f};
	float	up, out, angle, sval, cval;
	float	lineAngle = 5.0f;
	float	xmin, xmax;
	float	ymin, ymax;
	float	zmin = 0.0f;
	float	u1 = 30.0f, v1 = 1.0f;
	float	lastUp, lastOut;

	GetBounds(&xmin, &xmax, &ymin, &ymax);

	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	UseLibTexture(TEXID_GRID);

	for (angle = 0.0f; angle <= 90.0f; angle += lineAngle) {
		pfSinCos(angle, &sval, &cval);
		up = gHalfPipeRadius * (1.0f - cval);
		out = gHalfPipeRadius * sval;

		if (angle > 0.0f) {
			glColor4fv(hue);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xmin - out, ymin - out, zmin + up);
			glTexCoord2f(0.0f, v1);
			glVertex3f(xmin - lastOut, ymin - lastOut, zmin + lastUp);
			glTexCoord2f(u1, 0.0f);
			glVertex3f(xmax + out, ymin - out, zmin + up);
			glTexCoord2f(u1, v1);
			glVertex3f(xmax + lastOut, ymin - lastOut, zmin + lastUp);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xmax + out, ymax + out, zmin + up);
			glTexCoord2f(0.0f, v1);
			glVertex3f(xmax + lastOut, ymax + lastOut, zmin + lastUp);
			glTexCoord2f(u1, 0.0f);
			glVertex3f(xmin - out, ymax + out, zmin + up);
			glTexCoord2f(u1, v1);
			glVertex3f(xmin - lastOut, ymax + lastOut, zmin + lastUp);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xmin - out, ymin - out, zmin + up);
			glTexCoord2f(0.0f, v1);
			glVertex3f(xmin - lastOut, ymin - lastOut, zmin + lastUp);
			glEnd();
		}
		lastUp = up;
		lastOut = out;
	}
	glDepthMask(true);
}

void CellGrid::DrawCorners(void)
{
	int32	i;
	pfVec4	hue;
	float	*f1, *f2;
	float	xmin, xmax;
	float	ymin, ymax;
	float	zmin = 0.0f, zmax = 8.0f;
	float	x, y;
	float	close = 10.0f, dist;
	float	width = 2.0f;

	static bool	strobe = true;

	strobe = !strobe;

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	UseLibTexture(TEXID_ZAP);

	
	GetBounds(&xmin, &xmax, &ymin, &ymax);

	glBegin(GL_TRIANGLES);

//	gx = gGlider->mMatrix[PF_T][PF_X];
//	gy = gGlider->mMatrix[PF_T][PF_Y];

	x = xmin;
	y = ymin;
	PFCOPY_VEC4(hue, healthFillColor[HEALTH_TYPE_ATTITUDE]);
	hue[3] = 1.0f;
	glColor4fv(hue);
	glVertex3f(x, y, zmin);
	hue[3] = 0.0f;
//	dist = (x-gx)*(x-gx)+(y-gy)*(y-gy);
//	if (dist < close*close) {
//		gHealthLevel[HEALTH_TYPE_ATTITUDE] = 1.0f;
//		hue[3] = 0.25f;
//	}
	glColor4fv(hue);
	glVertex3f(x-width, y+width, zmax);
	glVertex3f(x+width, y-width, zmax);

	x = xmax;
	y = ymin;
	PFCOPY_VEC4(hue, healthFillColor[HEALTH_TYPE_HEALTH]);
	hue[3] = 1.0f;
	glColor4fv(hue);
	glVertex3f(x, y, zmin);
	hue[3] = 0.0f;
//	dist = (x-gx)*(x-gx)+(y-gy)*(y-gy);
//	if (dist < close*close) {
//		gHealthLevel[HEALTH_TYPE_HEALTH] = 1.0f;
//		hue[3] = 0.25f;
//	}
	glColor4fv(hue);
	glVertex3f(x+width, y+width, zmax);
	glVertex3f(x-width, y-width, zmax);

	x = xmin;
	y = ymax;
	PFCOPY_VEC4(hue, healthFillColor[HEALTH_TYPE_AMMO]);
	hue[3] = 1.0f;
	glColor4fv(hue);
	glVertex3f(x, y, zmin);
	hue[3] = 0.0f;
//	dist = (x-gx)*(x-gx)+(y-gy)*(y-gy);
//	if (dist < close*close) {
//		gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f;
//		hue[3] = 0.25f;
//	}
	glColor4fv(hue);
	glVertex3f(x-width, y-width, zmax);
	glVertex3f(x+width, y+width, zmax);
	
	glEnd();

}

void CellGrid::Mutate(void)
{
	int32	h, v;
	OneCell	*pcell;

	for (v = 1; v < mVSize - 1; v++) {
		for (h = 1; h < mHSize - 1; h++) {
			pcell = &(mCells[h + (v * mHSize)]);
			if (RANDOM0TO1 < 0.05f) SETFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
		}
	}
}

OneCell *CellGrid::GetHVCell(int32 h, int32 v, int32 *pIndex)
{
	int32	index;
	if (pIndex) *pIndex = -1;
	if (h < 0 || h >= mHSize) return(NULL);
	if (v < 0 || v >= mVSize) return(NULL);
	index = (v * mHSize) + h;
	if (pIndex) *pIndex = index;
	return(mCells + index);
}

/**********************************************************\
	CellGrid::GetClosestIndex
	Given a position, return a pointer to the closest cell.
	Also, fill pRow and pCol, for convenience, if they're
	not NULL.
\**********************************************************/
OneCell *CellGrid::GetClosestCell(pfVec3 pos, int32 *pCol, int32 *pRow, int32 *pIndex)
{
	float	frow, fcol;
	int32	row, col, index;
	OneCell	*pcell;
	pfVec3	v;

	pfSubVec3(v, pos, mCenter);
	frow = ( v[PF_Y] / (mCellSize * hexContract)) + (mVSize / 2);
	fcol = ( v[PF_X] / mCellSize) + (mHSize / 2);
	frow += 0.5f;
	fcol += 0.5f;

	row = (int32)frow;
	if (row & 1) fcol -= 0.5f;
	col = (int32)fcol;

	if (row < 0) row = 0;
	if (col < 0) col = 0;
	if (row > mVSize-1) row = mVSize-1;
	if (col > mHSize-1) col = mHSize-1;
	index = col + (mHSize * row);
	pcell = &(mCells[index]);

	if (pRow) *pRow = row;
	if (pCol) *pCol = col;
	if (pIndex) *pIndex = index;
	return(pcell);
}

bool CellGrid::KillCellsInRange(pfVec3 center, int32 range, int32 flags)
{
	bool	retval = false;
	int32	h, v, hc, vc, ic;
	OneCell	*pcell, *pcell2;

	/**** Infect the nearby cells ****/
	pcell = gCells->GetClosestCell(center, &hc, &vc, &ic);
	if (pcell) {
		for (v = vc - range; v <= vc + range; v++) {
			for (h = hc - range; h <= hc + range; h++) {
				pcell2 = gCells->GetHVCell(h, v, &ic);
				if (pcell2) {
					if (pcell2->shift & (CELL_SHIFT_ON|CELL_SHIFT_ON_NEXT)) {
						retval = true;
						SETFLAG(pcell2->flags, CELL_FLAG_SHOT);
						CLRFLAG(pcell2->shift, CELL_SHIFT_ON|CELL_SHIFT_ON_NEXT);
					}
				}
			}
		}
	}
	return(retval);
}

void CellGrid::GetBounds(float *pXMin, float *pXMax, float *pYMin, float *pYMax)
{
	if (pXMin) *pXMin = mCenter[PF_X] - (mCellSize * mHSize * 0.5f);
	if (pXMax) *pXMax = mCenter[PF_X] + (mCellSize * mHSize * 0.5f);
	if (pYMin) *pYMin = mCenter[PF_Y] - (mCellSize * mHSize * 0.5f * hexContract);
	if (pYMax) *pYMax = mCenter[PF_Y] + (mCellSize * mHSize * 0.5f * hexContract);
}

void CellGrid::GetPosFromIndex(int32 index, pfVec3 dest)
{
	int32	h, v;
	float	fieldH = mHSize * mCellSize;
	float	fieldV = mVSize * mCellSize * hexContract;

	h = index % mHSize;
	v = index / mHSize;

	pfCopyVec3(dest, mCenter);
	dest[PF_X] -= fieldH * 0.5f;
	dest[PF_Y] -= fieldV * 0.5f;
//	dest[PF_X] += x * mCellSize;
//	dest[PF_Y] += y * mCellSize * hexContract;
	if (1) {	//hex grid
		pfSetVec3(dest, dest[PF_X] + (h * mCellSize), dest[PF_Y] + (v * mCellSize * hexContract), dest[PF_Z]);
		if (v & 1) dest[PF_X] += mCellSize * 0.5f;
	} else {	//rect grid
		pfSetVec3(dest, dest[PF_X] + (h * mCellSize), dest[PF_Y] + (v * mCellSize), dest[PF_Z]);
	}
}

void CellGrid::Think(void)
{
	if (KeysDown['1']) mLifeUpdateTimeInterval += DeltaTime * 1.0f;
	if (KeysDown['2']) mLifeUpdateTimeInterval -= DeltaTime * 1.0f;
	if (mLifeUpdateTimeInterval < 0.05f) mLifeUpdateTimeInterval = 0.05f;
	if (mLifeUpdateTimeInterval > 10.0f) mLifeUpdateTimeInterval = 10.0f;

	LifeUpdate();
}

CellGrid::~CellGrid()
{
	if (mCells) {
		delete[] mCells;
		mCells = NULL;
	}
}

void CellGrid::Draw(void)
{
	int32	h, v;
	uint8	shift, flags;
	pfVec3	pos, cpos, vertex, toCam, up, over;
	OneCell	*pcell;
	float	t, alt, alt1, alt2;
	float	*c1, *c2;
	pfVec4	color, mix1, mix2;
	pfVec4	hueGood	  = {0.0f, 1.0f, 1.0f, 1.0f};
	pfVec4	hueDarkGood	  = {0.0f, 0.7f, 0.7f, 1.0f};
	pfVec4	hueCancer  = {1.0f, 0.0f, 0.0f, 1.0f};
	pfVec4	hueHealthy = {1.0f, 1.0f, 1.0f, 0.2f};
	pfVec4	hueDark    = {0.7f, 0.0f, 0.0f, 1.0f};
	pfVec4	hueBack    = {0.3f, 0.3f, 0.8f, 1.0f};
//	pfVec4	hueDark    = {1.0f, 1.0f, 1.0f, 0.5f};
	pfVec4	BhueCancer  = {1.0f, 0.3f, 0.0f, 0.4f};
	pfVec4	BhueHealthy = {1.0f, 1.0f, 1.0f, 0.0f};
	pfVec4	BhueDark    = {0.7f, 0.0f, 0.0f, 0.2f};
	float	bright;

	float	fieldH = mHSize * mCellSize;
	float	fieldV = mVSize * mCellSize * hexContract;
	float	sval, cval;
	static float	waveTime = 0.0f;
//	int32	hClose, vClose, iClose;
	pfVec3	camPos, posClose;

	pfCopyVec3(camPos, GlobalCameraMatrix[PF_T]);
//	GetClosestCell(gGlider->mMatrix[PF_T], &hClose, &vClose, &iClose);
//	GetPosFromIndex(iClose, posClose);

	waveTime += 0.1f * DeltaTime;
	if (waveTime > 1.0f) waveTime -= 1.0f;

	pfCopyVec3(pos, mCenter);
	pos[PF_X] -= fieldH * 0.5f;
	pos[PF_Y] -= fieldV * 0.5f;
	pcell = mCells;
	t = mLifeUpdateTimer / mLifeUpdateTimeInterval;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;


	if (1) {
		/**** draw the underlay ****/
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(true);

		glColor4fv(hueBack);
		glBegin(GL_QUADS);
		glVertex3f(mCenter[PF_X] - (0.5f * fieldH), mCenter[PF_Y] - (0.5f * fieldV), mCenter[PF_Z] - 0.1f);
		glVertex3f(mCenter[PF_X] + (0.5f * fieldH), mCenter[PF_Y] - (0.5f * fieldV), mCenter[PF_Z] - 0.1f);
		glVertex3f(mCenter[PF_X] + (0.5f * fieldH), mCenter[PF_Y] + (0.5f * fieldV), mCenter[PF_Z] - 0.1f);
		glVertex3f(mCenter[PF_X] - (0.5f * fieldH), mCenter[PF_Y] + (0.5f * fieldV), mCenter[PF_Z] - 0.1f);
		glEnd();
	}
	DrawCorners();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(true);
	UseLibTexture(TEXID_CELL);
	
	glBegin(GL_QUADS);

	for (v = 0; v < mVSize; v++) {
		pfSinCos((360.0f * waveTime) + (720.0f * (float)v/(float)mVSize), &sval, &cval);
		if (!gOptionWaves) sval = cval = 0.0f;
		for (h = 0; h < mHSize; h++) {
			shift = pcell->shift;
			flags = pcell->flags;

			if (1) {	//hex grid
				pfSetVec3(cpos, pos[PF_X] + (h * mCellSize), pos[PF_Y] + (v * mCellSize * hexContract), pos[PF_Z]);
				if (v & 1) cpos[PF_X] += mCellSize * 0.5f;
			} else {	//rect grid
				pfSetVec3(cpos, pos[PF_X] + (h * mCellSize), pos[PF_Y] + (v * mCellSize), pos[PF_Z]);
			}

			cpos[PF_Z] += 0.6f * sval;
//			if (h == hClose && v == vClose) {
//				cpos[PF_Z] += 1.0f;
//			}

#if 1
			/******************\
				L N
				000	h-h
				001 h-c
				010 c-d
				011 c-c
				100 d-h
				101 d-c
				110 c-d
				111 c-c
			\******************/

//			if (flags & CELL_FLAG_GOOD) {
//				switch (shift & 0x07) {
//				case 0: c1 = hueHealthy;	c2 = hueHealthy;	alt1 = 0.0f;	alt2 = 0.0f;	break;
//				case 1: c1 = hueHealthy;	c2 = hueGood;		alt1 = 0.0f;	alt2 = 1.0f;	break;
//				case 2: c1 = hueGood;		c2 = hueDarkGood;	alt1 = 1.0f;	alt2 = 1.0f;	break;
//				case 3: c1 = hueGood;		c2 = hueGood;		alt1 = 1.0f;	alt2 = 1.0f;	break;
//				case 4: c1 = hueDarkGood;	c2 = hueHealthy;	alt1 = 1.0f;	alt2 = 0.0f;	break;
//				case 5: c1 = hueDarkGood;	c2 = hueGood;		alt1 = 1.0f;	alt2 = 1.0f;	break;
//				case 6: c1 = hueGood;		c2 = hueDarkGood;	alt1 = 1.0f;	alt2 = 1.0f;	break;
//				case 7: c1 = hueGood;		c2 = hueGood;		alt1 = 1.0f;	alt2 = 1.0f;	break;
//				default: c1 = c2 = hueHealthy; break;
//				}
//			} else {
				switch (shift & 0x07) {
				case 0: c1 = hueHealthy;	c2 = hueHealthy;	alt1 = 0.0f;	alt2 = 0.0f;	break;
				case 1: c1 = hueHealthy;	c2 = hueCancer;		alt1 = 0.0f;	alt2 = 1.0f;	break;
				case 2: c1 = hueCancer;		c2 = hueDark;		alt1 = 1.0f;	alt2 = 1.0f;	break;
				case 3: c1 = hueCancer;		c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
				case 4: c1 = hueDark;		c2 = hueHealthy;	alt1 = 1.0f;	alt2 = 0.0f;	break;
				case 5: c1 = hueDark;		c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
				case 6: c1 = hueCancer;		c2 = hueDark;		alt1 = 1.0f;	alt2 = 1.0f;	break;
				case 7: c1 = hueCancer;		c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
				default: c1 = c2 = hueHealthy; break;
				}
		
//			}
#else
			/******************\
				L N
				000	h-h
				001 h-c
				010 c-h
				011 c-c
				100 h-h
				101 h-c
				110 c-h
				111 c-c
			\******************/

			switch (shift & 0x07) {
			case 0: c1 = hueHealthy;	c2 = hueHealthy;	alt1 = 0.0f;	alt2 = 0.0f;	break;
			case 1: c1 = hueHealthy;	c2 = hueCancer;		alt1 = 0.0f;	alt2 = 1.0f;	break;
			case 2: c1 = hueCancer;		c2 = hueHealthy;	alt1 = 1.0f;	alt2 = 1.0f;	break;
			case 3: c1 = hueCancer;		c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
			case 4: c1 = hueHealthy;	c2 = hueHealthy;	alt1 = 1.0f;	alt2 = 0.0f;	break;
			case 5: c1 = hueHealthy;	c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
			case 6: c1 = hueCancer;		c2 = hueHealthy;	alt1 = 1.0f;	alt2 = 1.0f;	break;
			case 7: c1 = hueCancer;		c2 = hueCancer;		alt1 = 1.0f;	alt2 = 1.0f;	break;
			default: c1 = c2 = hueHealthy; break;
			}
#endif
			/**** move bad cells up for a cool effect ****/
			alt = ((1.0f - t) * alt1) + (t * alt2);
			cpos[PF_Z] += alt;

			pfScaleVec4(mix1, 1.0f - t, c1);
			pfScaleVec4(mix2, t, c2);
			PFADD_VEC4(color, mix1, mix2);

			color[3] += 0.1f * sval;
			if (color[3] < 0.0f) color[3] = 0.0f;
			if (color[3] > 1.0f) color[3] = 1.0f;

			if (gCells->mTotalBadCells == 0) {
				if (RANDOM0TO1 < 0.005f) {
					pfSetVec4(color, 1.0f, 1.0f, 1.0f, 1.0f);
				}
			}

			if (flags & CELL_FLAG_BOSS) {
				bright = RANDOM_IN_RANGE(0.4f, 0.5f);
				color[0] = bright;
				color[1] = color[2] = bright * 0.5f;
				color[3] = 1.0f;
			}

			if (flags & CELL_FLAG_GOOD) {
				float f = pcell->goodTimer;
				if (f > 1.0f) f = 1.0f;
				pfScaleVec4(mix1, 1.0f - f, color);
				pfScaleVec4(mix2, f, hueGood);
				PFADD_VEC4(color, mix1, mix2);
			}

			if (flags & CELL_FLAG_SHOT) {
				bright = RANDOM_IN_RANGE(0.7f, 1.0f);
				color[0] = color[1] = color[2] = bright;
				color[3] = 1.0f;
			}

			glColor4fv(color);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(cpos[PF_X]-(0.5f*mCellSize), cpos[PF_Y]-(0.5f*mCellSize), cpos[PF_Z]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(cpos[PF_X]+(0.5f*mCellSize), cpos[PF_Y]-(0.5f*mCellSize), cpos[PF_Z]);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(cpos[PF_X]+(0.5f*mCellSize), cpos[PF_Y]+(0.5f*mCellSize), cpos[PF_Z]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(cpos[PF_X]-(0.5f*mCellSize), cpos[PF_Y]+(0.5f*mCellSize), cpos[PF_Z]);
			pcell++;
		}
	}
	glEnd();

	if (1) {
		/**** Draw the bubbles ****/
		pcell = mCells;
		glEnable(GL_DEPTH_TEST);
		glDepthMask(false);
		UseLibTexture(TEXID_BUBBLE);
	
		glBegin(GL_QUADS);

		for (v = 0; v < mVSize; v++) {
			pfSinCos((360.0f * waveTime) + (720.0f * (float)v/(float)mVSize), &sval, &cval);

			for (h = 0; h < mHSize; h++) {
				shift = pcell->shift;
				flags = pcell->flags;

				if (/*(shift & 7) || */(flags & (CELL_FLAG_SHOT|CELL_FLAG_BOUNCE))) {	/**** Only if cancer ****/
					if (1) {	//hex grid
						pfSetVec3(cpos, pos[PF_X] + (h * mCellSize), pos[PF_Y] + (v * mCellSize * hexContract), pos[PF_Z]);
						if (v & 1) cpos[PF_X] += mCellSize * 0.5f;
					} else {	//rect grid
						pfSetVec3(cpos, pos[PF_X] + (h * mCellSize), pos[PF_Y] + (v * mCellSize), pos[PF_Z]);
					}

					cpos[PF_Z] += 0.3f * sval;
//					cpos[PF_Z] += 0.25f * mCellSize;
					
					switch (shift & 0x07) {
					case 0: c1 = BhueHealthy;	c2 = BhueHealthy;	alt1 = 0.0f;	alt2 = 0.0f;		break;
					case 1: c1 = BhueHealthy;	c2 = BhueCancer;	alt1 = 0.0f;	alt2 = 1.0f;		break;
					case 2: c1 = BhueCancer;	c2 = BhueDark;		alt1 = 1.0f;	alt2 = 1.0f;		break;
					case 3: c1 = BhueCancer;	c2 = BhueCancer;	alt1 = 1.0f;	alt2 = 1.0f;		break;
					case 4: c1 = BhueDark;		c2 = BhueHealthy;	alt1 = 1.0f;	alt2 = 0.0f;		break;
					case 5: c1 = BhueDark;		c2 = BhueCancer;	alt1 = 1.0f;	alt2 = 1.0f;		break;
					case 6: c1 = BhueCancer;	c2 = BhueDark;		alt1 = 1.0f;	alt2 = 1.0f;		break;
					case 7: c1 = BhueCancer;	c2 = BhueCancer;	alt1 = 1.0f;	alt2 = 1.0f;		break;
					default: c1 = c2 = BhueHealthy; break;
					}

					/**** move bad cells up for a cool effect ****/
					alt = ((1.0f - t) * alt1) + (t * alt2);
					cpos[PF_Z] += alt;

					pfScaleVec4(mix1, 1.0f - t, c1);
					pfScaleVec4(mix2, t, c2);
					PFADD_VEC4(color, mix1, mix2);

					color[3] += 0.1f * sval;
					if (color[3] < 0.0f) color[3] = 0.0f;
					if (color[3] > 1.0f) color[3] = 1.0f;

	//				pfSetVec4(color, 1.0f, 1.0f, 1.0f, 0.5f);
					if (flags & CELL_FLAG_SHOT) {
						bright = RANDOM_IN_RANGE(0.7f, 1.0f);
						color[0] = color[1] = color[2] = bright;
						color[3] = 0.4f * (1.0f - t);
					} else if (flags & CELL_FLAG_BOUNCE) {
						bright = RANDOM_IN_RANGE(0.7f, 1.0f);
						color[0] = color[1] = color[2] = bright;
						color[3] = 0.4f;
					}

					glColor4fv(color);

					/**** Now construct our up and over vectors ****/
					pfSubVec3(toCam, camPos, cpos);
					pfCrossVec3(over, toCam, GlobalCameraMatrix[PF_Z]);
					pfCrossVec3(up, over, toCam);
					pfNormalizeVec3(up);
					pfNormalizeVec3(over);
					if (flags & CELL_FLAG_SHOT) {
						pfScaleVec3(up, (0.95f + t*5.0f) * mCellSize, up);
						pfScaleVec3(over, (0.95f + t*5.0f) * mCellSize, over);
					} else {
						pfScaleVec3(up, (0.95f) * mCellSize, up);
						pfScaleVec3(over, (0.95f) * mCellSize, over);
					}
					pfAddScaledVec3(vertex, cpos, -0.5f, up);
					pfAddScaledVec3(vertex, vertex, -0.5f, over);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3fv(vertex);
					pfAddVec3(vertex, vertex, over);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3fv(vertex);
					pfAddVec3(vertex, vertex, up);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3fv(vertex);
					pfSubVec3(vertex, vertex, over);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3fv(vertex);
				}
				pcell++;
			}
		}
		glEnd();
	}
}


void CellGrid::LifeUpdate(void)
{
	int32	i, h, v, count, nugcount, wasShot = 0;
	OneCell	*pcell, *phi, *plo, *plolo;

	for (i = 0; i < mNumCells; i++) {
		CLRFLAG(mCells[i].flags, CELL_FLAG_BOSS);
		wasShot |= (mCells[i].flags & CELL_FLAG_SHOT);
//		if (mCells[i].shift & (CELL_SHIFT_ON|CELL_SHIFT_ON_NEXT)) {
//		} else {
//			CLRFLAG(mCells[i].flags, CELL_FLAG_GOOD);
//		}
	}

	if (wasShot && (gSizzleCycle < 1)) {
		playSound2D(SOUND_SIZZLE_1, 0.85f, RANDOM_IN_RANGE(0.9f, 1.1f));
		gSizzleCycle++;
	}

	/**** count up the bad cells ****/
	mTotalBadCells = 0;
	for (i = 0; i < mNumCells; i++) {
		pcell = &(mCells[i]);
		if (pcell->shift & (CELL_SHIFT_ON|CELL_SHIFT_ON_NEXT)) {
			mTotalBadCells++;
		}
		if (pcell->flags & CELL_FLAG_GOOD) {
			pcell->goodTimer -= DeltaTime;
			if (pcell->goodTimer <= 0.0f) {
				pcell->goodTimer = 0.0f;
				CLRFLAG(pcell->flags, CELL_FLAG_GOOD);
			} else {
				CLRFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
			}
		}
	}

	mLifeUpdateTimer += DeltaTime;
	while (1) {
		if (mLifeUpdateTimer < mLifeUpdateTimeInterval) return;
		mLifeUpdateTimer -= mLifeUpdateTimeInterval;

		for (i = 0; i < mNumCells; i++) {
			mCells[i].shift <<= 1;
			CLRFLAG(mCells[i].flags, CELL_FLAG_SHOT|CELL_FLAG_BOUNCE|CELL_FLAG_BOSS);
		}

		gSizzleCycle = 0;

		for (v = 1; v < mVSize - 1; v++) {
			for (h = 1; h < mHSize - 1; h++) {
				pcell = &(mCells[h + (v * mHSize)]);
				phi = pcell - mHSize;
				plo = pcell + mHSize;
				plolo = plo + mHSize;

				count = 0;
				count += phi[-1].shift & CELL_SHIFT_ON;
				count += phi[ 0].shift & CELL_SHIFT_ON;
				count += phi[ 1].shift & CELL_SHIFT_ON;
				count += pcell[-1].shift & CELL_SHIFT_ON;
				count += pcell[ 1].shift & CELL_SHIFT_ON;
				count += plo[-1].shift & CELL_SHIFT_ON;
				count += plo[ 0].shift & CELL_SHIFT_ON;
				count += plo[ 1].shift & CELL_SHIFT_ON;
				count >>= 1;	/**** because CELL_SHIFT_ON is actually bit 2 ****/

				if (pcell->shift & CELL_SHIFT_ON) {
					if (count == 2 || count == 3) {
						SETFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
					}
				} else {
					if (count == 3) {
						SETFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
					}
				}

				/**** Detect 2x2 "nuggets" and turn them into gliders ****/
				if (pcell->shift & CELL_SHIFT_ON) {
					if (h < mHSize-2 && v < mVSize-2) {
						nugcount = 0;
						nugcount += pcell[ 0].shift & CELL_SHIFT_ON;
						nugcount += pcell[ 1].shift & CELL_SHIFT_ON;
						nugcount += plo[ 0].shift & CELL_SHIFT_ON;
						nugcount += plo[ 1].shift & CELL_SHIFT_ON;
						nugcount >>= 1;	/**** because CELL_SHIFT_ON is actually bit 2 ****/
						if (nugcount == 4) {
							nugcount = 0;
							nugcount += phi[-1].shift & CELL_SHIFT_ON;
							nugcount += phi[ 0].shift & CELL_SHIFT_ON;
							nugcount += phi[ 1].shift & CELL_SHIFT_ON;
							nugcount += phi[ 2].shift & CELL_SHIFT_ON;
							nugcount += pcell[-1].shift & CELL_SHIFT_ON;
							nugcount += pcell[ 2].shift & CELL_SHIFT_ON;
							nugcount += plo[-1].shift & CELL_SHIFT_ON;
							nugcount += plo[ 2].shift & CELL_SHIFT_ON;
							nugcount += plolo[-1].shift & CELL_SHIFT_ON;
							nugcount += plolo[ 0].shift & CELL_SHIFT_ON;
							nugcount += plolo[ 1].shift & CELL_SHIFT_ON;
							nugcount += plolo[ 2].shift & CELL_SHIFT_ON;
							nugcount >>= 1;	/**** because CELL_SHIFT_ON is actually bit 2 ****/
							if (nugcount == 0) {
								/**** okay, it's a nugget. ****/
								if (h < mHSize/2) {
									if (v < mVSize/2) {
										CLRFLAG(pcell[0].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(phi[0].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(plo[-1].shift, CELL_SHIFT_ON_NEXT);
									} else {
										CLRFLAG(plo[0].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(plolo[0].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(pcell[-1].shift, CELL_SHIFT_ON_NEXT);
									}
								} else {
									if (v < mVSize/2) {
										CLRFLAG(pcell[1].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(phi[1].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(plo[2].shift, CELL_SHIFT_ON_NEXT);
									} else {
										CLRFLAG(plo[1].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(plolo[1].shift, CELL_SHIFT_ON_NEXT);
										SETFLAG(pcell[2].shift, CELL_SHIFT_ON_NEXT);
									}
								}
							}
						}
					}
				}

				/**** if it's surrounded by "good" mutated cells, make it good. ****/
//				if (pcell->shift & (CELL_SHIFT_ON_NEXT | CELL_SHIFT_ON)) {
//					int goodCount = 0;
//					goodCount += phi[-1].flags & CELL_FLAG_GOOD;
//					goodCount += phi[ 0].flags & CELL_FLAG_GOOD;
//					goodCount += phi[ 1].flags & CELL_FLAG_GOOD;
//					goodCount += pcell[-1].flags & CELL_FLAG_GOOD;
//					goodCount += pcell[ 1].flags & CELL_FLAG_GOOD;
//					goodCount += plo[-1].flags & CELL_FLAG_GOOD;
//					goodCount += plo[ 0].flags & CELL_FLAG_GOOD;
//					goodCount += plo[ 1].flags & CELL_FLAG_GOOD;
//					goodCount /= CELL_FLAG_GOOD;
//
//					if (goodCount > 1) {
//						SETFLAG(pcell->flags, CELL_FLAG_GOOD);
//					}
//					if (goodCount > 1) {
//						CLRFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
//					}
//				}
//				if (pcell->flags & CELL_FLAG_GOOD) {
//					SETFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
//				}

				/**** some mutation ****/
//				if (count > 0) {
//					if (RANDOM0TO1 < 0.0005f) {
//						SETFLAG(pcell->shift, CELL_SHIFT_ON_NEXT);
//					}
//				}
			}
		}
	}
}


