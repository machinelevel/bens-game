/******************************************************************************\

   spline.c - engine spline stuff

   LucasArts Proprietary and Confidential.

   If found, drop in any mailbox. Postage is pre-paid.
   Do not submerge or take internally.
   No user-serviceable parts inside.
   Any similarity to actual persons, living or dead, is entirely coincidental.
   This end up.
   Do not stack more than four high.
   Dry clean only.
   One size fits all.

\******************************************************************************/


#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "genincludes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <malloc.h>


#include "umath.h"
#include "upmath.h"

#include "timer.h"
#include "spline.h"




static pfMatrix SplineMatPoint = {
		{-1.0f/6.0f, 3.0f/6.0f, -3.0f/6.0f, 1.0f/6.0f},
		{ 3.0f/6.0f,-6.0f/6.0f,  3.0f/6.0f, 0.0f/6.0f},
		{-3.0f/6.0f, 0.0f/6.0f,  3.0f/6.0f, 0.0f/6.0f},
		{ 1.0f/6.0f, 4.0f/6.0f,  1.0f/6.0f, 0.0f/6.0f},
	};

static pfMatrix SplineMatTangn = {
		{ 0.0f/6.0f, 0.0f/6.0f,  0.0f/6.0f, 0.0f/6.0f},
		{-3.0f/6.0f, 9.0f/6.0f, -9.0f/6.0f, 3.0f/6.0f},
		{ 6.0f/6.0f,-12.0f/6.0f, 6.0f/6.0f, 0.0f/6.0f},
		{-3.0f/6.0f, 0.0f/6.0f,  3.0f/6.0f, 0.0f/6.0f},
	};

static pfMatrix SplineMatSlope = {
		{ 0.0f/6.0f, 0.0f/6.0f,  0.0f/6.0f, 0.0f/6.0f},
		{ 0.0f/6.0f, 0.0f/6.0f,  0.0f/6.0f, 0.0f/6.0f},
		{-6.0f/6.0f, 18.0f/6.0f,-18.0f/6.0f,6.0f/6.0f},
		{ 6.0f/6.0f,-12.0f/6.0f, 6.0f/6.0f, 0.0f/6.0f},
	};


static pfMatrix BezierMatPoint = {
		{-1.0f/1.0f, 3.0f/1.0f, -3.0f/1.0f, 1.0f/1.0f},
		{ 3.0f/1.0f,-6.0f/1.0f,  3.0f/1.0f, 0.0f/1.0f},
		{-3.0f/1.0f, 3.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
		{ 1.0f/1.0f, 0.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
	};

static pfMatrix BezierMatTangn = {
		{ 0.0f/1.0f, 0.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
		{-3.0f/1.0f, 9.0f/1.0f, -9.0f/1.0f, 3.0f/1.0f},
		{ 6.0f/1.0f,-12.0f/1.0f, 6.0f/1.0f, 0.0f/1.0f},
		{-3.0f/1.0f, 3.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
	};
static pfMatrix BezierMatSlope = {
		{ 0.0f/1.0f, 0.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
		{ 0.0f/1.0f, 0.0f/1.0f,  0.0f/1.0f, 0.0f/1.0f},
		{-6.0f/1.0f, 18.0f/1.0f,-18.0f/1.0f,6.0f/1.0f},
		{ 6.0f/1.0f,-12.0f/1.0f, 6.0f/1.0f, 0.0f/1.0f},
	};




void GetKnotLoc(SplineSet *SData, long i, pfVec3 Loc)
{
	PFCOPY_VEC3(Loc,SData->SplineList[i].Loc);
}

long GetKnotOrder(SplineSet *SData, long i)
{
	return(SData->SplineList[i].Order);
}

long GetNthKnot(SplineSet *SData, long i, long StartingAt)
{
	long j;

	for (j=StartingAt;j<SData->NumSplines;j++) {
		if (SData->SplineList[j].Order==i) return(j);
	}
	return(-1);
}

long GetSplineObjKnotID(SplinedObj *ThisSpline, long WhichIndex)
{
	long SplineNum;
	long IDNum;
	
	if (ThisSpline->CurrentIDSetting==0) return(ThisSpline->SplineIndex[WhichIndex]);
	 else {
		SplineNum=ThisSpline->SplineIndex[WhichIndex];
		if (WhichIndex==0)IDNum=ThisSpline->CurrentIDSetting;
		 else IDNum=(ThisSpline->CurrentIDSetting>>WhichIndex);
		return(ThisSpline->SData->SplineList[SplineNum].OtherIDs[IDNum]);
	}
}

void GetSplineInfo(SplineSet *SData, long ReturnFlags, float UVal, long SI[4], SplineReturn *Results)
{
	pfVec4	UVec,UTrans;
	pfVec3  *P0,*P1,*P2,*P3,*N0,*N1,*N2,*N3;
		
	UVec[3]=1;
	UVec[2]=UVec[3]*UVal;	  /* u		    */
	UVec[1]=UVec[2]*UVal;	  /* u squared  */
	UVec[0]=UVec[1]*UVal;	  /* u cubed    */
	
	if (SData->Type==SPL_SPLINE) {
		pfXformVec4(UTrans,UVec,SplineMatPoint);

		P0=&(SData->SplineList[SI[0]].Loc);
		P1=&(SData->SplineList[SI[1]].Loc);
		P2=&(SData->SplineList[SI[2]].Loc);
		P3=&(SData->SplineList[SI[3]].Loc);
	} else {
		pfXformVec4(UTrans,UVec,BezierMatPoint);

		P0=&(SData->SplineList[SI[0]].Loc);
		P1=&(SData->SplineList[SI[0]].OutVec);
		P2=&(SData->SplineList[SI[1]].InVec);
		P3=&(SData->SplineList[SI[1]].Loc);
	}

	if (ReturnFlags & SPLGETPOINT) {
		Results->RetPoint[PF_X]=(*P0)[PF_X]*UTrans[0]+(*P1)[PF_X]*UTrans[1]+(*P2)[PF_X]*UTrans[2]+(*P3)[PF_X]*UTrans[3];
		Results->RetPoint[PF_Y]=(*P0)[PF_Y]*UTrans[0]+(*P1)[PF_Y]*UTrans[1]+(*P2)[PF_Y]*UTrans[2]+(*P3)[PF_Y]*UTrans[3];
		Results->RetPoint[PF_Z]=(*P0)[PF_Z]*UTrans[0]+(*P1)[PF_Z]*UTrans[1]+(*P2)[PF_Z]*UTrans[2]+(*P3)[PF_Z]*UTrans[3];
	}


	if(ReturnFlags & SPLGETNORMAL) {
		if (SData->Type==SPL_BEZIER) {
			Results->RetNormal[PF_X]=0.0f;
			Results->RetNormal[PF_Y]=0.0f;
			Results->RetNormal[PF_Z]=1.0f;
		} else {
			N0=&(SData->SplineList[SI[0]].Norm);
			N1=&(SData->SplineList[SI[1]].Norm);
			N2=&(SData->SplineList[SI[2]].Norm);
			N3=&(SData->SplineList[SI[3]].Norm);
			Results->RetNormal[PF_X]=(*N0)[PF_X]*UTrans[0]+(*N1)[PF_X]*UTrans[1]+(*N2)[PF_X]*UTrans[2]+(*N3)[PF_X]*UTrans[3];
			Results->RetNormal[PF_Y]=(*N0)[PF_Y]*UTrans[0]+(*N1)[PF_Y]*UTrans[1]+(*N2)[PF_Y]*UTrans[2]+(*N3)[PF_Y]*UTrans[3];
			Results->RetNormal[PF_Z]=(*N0)[PF_Z]*UTrans[0]+(*N1)[PF_Z]*UTrans[1]+(*N2)[PF_Z]*UTrans[2]+(*N3)[PF_Z]*UTrans[3];
		}
	}
		
	if(ReturnFlags & SPLGETTAN) {
		if (SData->Type==SPL_SPLINE) pfXformVec4(UTrans,UVec,SplineMatTangn);
		 else pfXformVec4(UTrans,UVec,BezierMatTangn);
		Results->RetTan[PF_X]=(*P0)[PF_X]*UTrans[0]+(*P1)[PF_X]*UTrans[1]+(*P2)[PF_X]*UTrans[2]+(*P3)[PF_X]*UTrans[3];
		Results->RetTan[PF_Y]=(*P0)[PF_Y]*UTrans[0]+(*P1)[PF_Y]*UTrans[1]+(*P2)[PF_Y]*UTrans[2]+(*P3)[PF_Y]*UTrans[3];
		Results->RetTan[PF_Z]=(*P0)[PF_Z]*UTrans[0]+(*P1)[PF_Z]*UTrans[1]+(*P2)[PF_Z]*UTrans[2]+(*P3)[PF_Z]*UTrans[3];
	}

	if(ReturnFlags & SPLGETSLOPE) {
		if (SData->Type==SPL_SPLINE) pfXformVec4(UTrans,UVec,SplineMatSlope);
		 else pfXformVec4(UTrans,UVec,BezierMatSlope);
		Results->RetSlope[PF_X]=(*P0)[PF_X]*UTrans[0]+(*P1)[PF_X]*UTrans[1]+(*P2)[PF_X]*UTrans[2]+(*P3)[PF_X]*UTrans[3];
		Results->RetSlope[PF_Y]=(*P0)[PF_Y]*UTrans[0]+(*P1)[PF_Y]*UTrans[1]+(*P2)[PF_Y]*UTrans[2]+(*P3)[PF_Y]*UTrans[3];
		Results->RetSlope[PF_Z]=(*P0)[PF_Z]*UTrans[0]+(*P1)[PF_Z]*UTrans[1]+(*P2)[PF_Z]*UTrans[2]+(*P3)[PF_Z]*UTrans[3];
	}
}


void FetchNewSpline(SplineSet *SData, short Direction, SplinedObjPtr ThisSO)
{
	SplinePtr	SplineStart;
	long		NewSplineIndex;
	short       BranchNum;

	if (Direction==SPLFORWARD) {
		ThisSO->AtSplineStart=FALSE;
		if (!ThisSO->AtSplineEnd) {
			if (SData->Type==SPL_SPLINE) SplineStart=&(SData->SplineList[ThisSO->SplineIndex[3]]);
			 else SplineStart=&(SData->SplineList[ThisSO->SplineIndex[1]]);
			if (SplineStart->NumNexts == 0) {
				NewSplineIndex=-1;
				ThisSO->AtSplineEnd=TRUE;
				ThisSO->SplineTime=1.0f;
			} else {
				/* ThisSO->DesiredSwitch=1; */
				if (ThisSO->DesiredSwitch < SplineStart->NumNexts) BranchNum=ThisSO->DesiredSwitch;
				 else BranchNum=ThisSO->DesiredSwitch%SplineStart->NumNexts;
				NewSplineIndex=SplineStart->NextSpline[BranchNum];

				if (SData->Type==SPL_SPLINE) ThisSO->CurrentIDSetting = (ThisSO->CurrentIDSetting >> 1) | (BranchNum << 2);
				 else ThisSO->CurrentIDSetting = (ThisSO->CurrentIDSetting >> 1) | (BranchNum);
			}
		}
		if (!ThisSO->AtSplineEnd) {
			ThisSO->SplineIndex[0]=ThisSO->SplineIndex[1];
			if (SData->Type==SPL_SPLINE) {
				ThisSO->SplineIndex[1]=ThisSO->SplineIndex[2];
				ThisSO->SplineIndex[2]=ThisSO->SplineIndex[3];
				ThisSO->SplineIndex[3]=NewSplineIndex;
			} else {
				ThisSO->SplineIndex[1]=NewSplineIndex;
			}
		}
	} else {
		ThisSO->AtSplineEnd=FALSE;
		if (!ThisSO->AtSplineStart) {
			SplineStart=&(SData->SplineList[ThisSO->SplineIndex[0]]);
			if (SplineStart->NumPrevs == 0) {
				NewSplineIndex=-1;
				ThisSO->AtSplineStart=TRUE;
				ThisSO->SplineTime=0.0f;
			} else {
				if (ThisSO->DesiredSwitch < SplineStart->NumPrevs) NewSplineIndex=SplineStart->PrevSpline[ThisSO->DesiredSwitch];
				 else NewSplineIndex=SplineStart->PrevSpline[ThisSO->DesiredSwitch%SplineStart->NumPrevs];

				if (SData->Type==SPL_SPLINE) {
					ThisSO->CurrentIDSetting = ((ThisSO->CurrentIDSetting << 1) & 0x0007);
				} else {
					ThisSO->CurrentIDSetting = ((ThisSO->CurrentIDSetting << 1) & 0x0001);
				}
				
				if (ThisSO->SplineIndex[0]!=SData->SplineList[NewSplineIndex].NextSpline[0]) ThisSO->CurrentIDSetting |= 0x0001;
			}
			
		}
		if (!ThisSO->AtSplineStart) {
			if (SData->Type==SPL_SPLINE) {
				ThisSO->SplineIndex[3]=ThisSO->SplineIndex[2];
				ThisSO->SplineIndex[2]=ThisSO->SplineIndex[1];
			}
			ThisSO->SplineIndex[1]=ThisSO->SplineIndex[0];
			ThisSO->SplineIndex[0]=NewSplineIndex;
		}

	}
}






/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/



void GetBasicSplineInfo(SplinedObjPtr ThisSO, SplineReturn *Results)
{
	float	SplineChange,LastSplineTime;
	Boolean ReturnVal;
	
	long	LastKnotID,ThisKnotID;
	pfCoord TempCoord;
	
	ReturnVal=TRUE;

	LastSplineTime=ThisSO->SplineTime;

	if (ThisSO->SplineFactor > 0.0f && !ThisSO->AtSplineEnd || ThisSO->SplineFactor < 0.0f && !ThisSO->AtSplineStart) {
		SplineChange=((DeltaTime*ThisSO->SplineFactor));
#if 0		
		if (ThisSO->Speed<0.01f) {
			if (ThisSO->Speed < 0.0f) /* We are starting */ SplineChange=0.0f;
			 else {
				if (ThisSO->SplineFactor > 0.0f) SplineChange=1.0f;
				 else SplineChange=-1.0f;
			}
		} else SplineChange= SplineChange*(200.0f/ThisSO->Speed);
#endif		
		ThisSO->SplineTime += SplineChange;

		if (SplineChange < 0.0f) ThisSO->AtSplineEnd=FALSE;
		else if (SplineChange > 0.0f) ThisSO->AtSplineStart=FALSE;
	}

	while (ThisSO->SplineTime>=1.0f && !ThisSO->AtSplineEnd) {   /* WOLFY:Changed 14Jan98.  Used to be > 1.0 rather than >= */
		ThisSO->SplineTime -= 1.0f; 		
		FetchNewSpline(ThisSO->SData,SPLFORWARD,ThisSO);
	}
	while (ThisSO->SplineTime<0.0f && !ThisSO->AtSplineStart) {
		ThisSO->SplineTime += 1.0f; 		
		FetchNewSpline(ThisSO->SData,SPLBACKWARD,ThisSO);
	}

	if (ThisSO->SplineTime<0.0f) ThisSO->SplineTime=0.0f;
	if (ThisSO->SplineTime>1.0f) ThisSO->SplineTime=1.0f;

	GetSplineInfo(ThisSO->SData,SPLGETBASIC,ThisSO->SplineTime,ThisSO->SplineIndex,Results);
	ThisSO->Speed=PFLENGTH_VEC3(Results->RetTan);

}





void FetchSplinePath(SplinedObjPtr ThisSO, pfMatrix TransMat)
{
	pfVec3	NewUp,NewRight,CurPos;
	Boolean ReturnVal;
	SplineReturn Results;

	GetBasicSplineInfo(ThisSO,&Results);

	if (PFLENGTH_VEC3(Results.RetTan) < 0.0001f) {
		if (ThisSO->SplineTime < 0.5f) GetSplineInfo(ThisSO->SData,SPLGETTAN,0.001f,ThisSO->SplineIndex,&Results);
		 else GetSplineInfo(ThisSO->SData,SPLGETTAN,0.999f,ThisSO->SplineIndex,&Results);
	}

	pfCrossVec3(NewRight,Results.RetTan,Results.RetNormal);
	pfCrossVec3(NewUp,NewRight,Results.RetTan);
	pfNormalizeVec3(NewRight);
	pfNormalizeVec3(NewUp);
	pfNormalizeVec3(Results.RetTan);

	TransMat[0][3]=0.0f;
	TransMat[1][3]=0.0f;
	TransMat[2][3]=0.0f;
	TransMat[3][3]=1.0f;
	
	pfSetMatRowVec3(TransMat,PF_X,NewRight);
	pfSetMatRowVec3(TransMat,PF_Y,Results.RetTan);
	pfSetMatRowVec3(TransMat,PF_Z,NewUp);

	pfGetMatRowVec3(TransMat,PF_T,CurPos);
	pfSetMatRowVec3(TransMat,PF_T,Results.RetPoint);
	
}



void FetchSplinePathForward(SplinedObjPtr ThisSO, pfMatrix TransMat, float Offset)
{
	SplinedObj NewSO;


	NewSO=*ThisSO;
	NewSO.SplineTime += Offset;
	NewSO.SplineFactor = 0.0f; /* So we don't creep forward */
	FetchSplinePath(&NewSO,TransMat);
}




void GetSplineMat(SplineSet *SData, pfMatrix TMat, long Knot0, long Knot1, long Knot2, long Knot3, float Time)
{
	SplinedObj		TSpline;
		
	TSpline.SData = SData;
	TSpline.SplineFactor = 0;
	TSpline.SplineTime = Time;
	TSpline.Speed = 0;
	TSpline.SplineIndex[0]=Knot0;
	TSpline.SplineIndex[1]=Knot1;
	TSpline.SplineIndex[2]=Knot2;
	TSpline.SplineIndex[3]=Knot3;
	TSpline.AtSplineEnd = FALSE;
	TSpline.AtSplineStart = FALSE;
	TSpline.DesiredSwitch = 0;
		
	FetchSplinePath(&TSpline, TMat);
}


#ifdef UNUSED_FUNCTIONS
void GetSplineMatOneKnot(SplineSet *SData, pfMatrix TMat, long Knot0, float Time)
{
	SplinedObj		TSpline;
		
	TSpline.SData = SData;
	TSpline.SplineFactor = 0;
	TSpline.SplineTime = Time;
	TSpline.Speed = 0;
	TSpline.SplineIndex[0]=Knot0;

	if (SData->SplineList[TSpline.SplineIndex[0]].NumNexts==0) return;
	TSpline.SplineIndex[1]=SData->SplineList[TSpline.SplineIndex[0]].NextSpline[0];
	if (SData->Type==SPL_SPLINE) {
		if (SData->SplineList[TSpline.SplineIndex[1]].NumNexts==0) return;
		TSpline.SplineIndex[2]=SData->SplineList[TSpline.SplineIndex[1]].NextSpline[0];
		if (SData->SplineList[TSpline.SplineIndex[2]].NumNexts==0) return;
		TSpline.SplineIndex[3]=SData->SplineList[TSpline.SplineIndex[2]].NextSpline[0];
	}

	TSpline.AtSplineEnd = FALSE;
	TSpline.AtSplineStart = FALSE;
	TSpline.DesiredSwitch = 0;
		
	FetchSplinePath(&TSpline, TMat);
}
#endif

void SetStartingKnots(SplinedObjPtr ThisSO, long StartKnot)
{
	ThisSO->SplineIndex[0]=StartKnot;
	ThisSO->SplineIndex[1]=StartKnot;
	ThisSO->SplineIndex[2]=StartKnot;
	ThisSO->SplineIndex[3]=StartKnot;

	if (ThisSO->SData->SplineList[ThisSO->SplineIndex[0]].NumNexts==0) return;
	ThisSO->SplineIndex[1]=ThisSO->SData->SplineList[ThisSO->SplineIndex[0]].NextSpline[0];
	if (ThisSO->SData->Type==SPL_SPLINE) {
		if (ThisSO->SData->SplineList[ThisSO->SplineIndex[1]].NumNexts==0) return;
		ThisSO->SplineIndex[2]=ThisSO->SData->SplineList[ThisSO->SplineIndex[1]].NextSpline[0];
		if (ThisSO->SData->SplineList[ThisSO->SplineIndex[2]].NumNexts==0) return;
		ThisSO->SplineIndex[3]=ThisSO->SData->SplineList[ThisSO->SplineIndex[2]].NextSpline[0];
	}
}



SplinedObj *BuildBasicSplinePath(pfVec3 *knotList, uint32 numKnots, uint32 flags)
{
	SplinedObj	*sobj;
	SplinePtr	spline;
	int32		i;

	if (knotList == NULL || numKnots == 0) return(NULL);

	/**** allocate and initialize ****/
	sobj = new SplinedObj;
	memset(sobj, 0, sizeof(SplinedObj));	/**** zero out these guys so they don't start with bad data ****/
	sobj->SData = new SplineSet;
	memset(sobj->SData, 0, sizeof(SplineSet));
	sobj->SData->SplineList = new Spline[numKnots];
	memset(sobj->SData->SplineList, 0, numKnots*sizeof(Spline));
	sobj->SData->NumVirtualKnots = 0;
	sobj->SData->NumSplines = 0;
	sobj->SData->Type = SPL_BEZIER;
	sobj->SplineTime = -1.0f;
	sobj->SplineFactor = 1.0f;
	sobj->Speed = 0.0f;
	sobj->DesiredSwitch=0;
	sobj->CurrentIDSetting=0;
	sobj->AtSplineEnd = FALSE;
	sobj->AtSplineStart = TRUE;

	spline = sobj->SData->SplineList;


	for (i = 0; i < numKnots; i++) {
		pfCopyVec3 (spline[i].Loc, knotList[i]);
		spline[i].NumNexts = 1;
		spline[i].NumPrevs = 1;
		spline[i].NextSpline[0] = i + 1;
		spline[i].Order = i;
		pfSetVec3 (spline[i].Norm, 0.0f,0.0f,1.0f);
	}
	spline[0].NumPrevs = 0;
	spline[numKnots-1].NumNexts = 0;
	sobj->SData->NumSplines = numKnots;

//	printf ("spline path: %d knots\n", numKnots);
	return(sobj);
}


void SmoothBasicSplinePath(SplinedObj *sobj)
{
	SplinePtr	spline = sobj->SData->SplineList;
	int32		i;

	// O_o  auto generate in/out points and normals.
	pfCopyVec3 (spline[0].OutVec, spline[0].Loc);
	for (i = 1; i < sobj->SData->NumSplines-1; i++)
	{
		pfVec3 direct;
		pfVec3 halfpoint;
		pfVec3 toThis, toNext;
		float  toThisDot, toNextDot;
		float  dist, totaldot;

		pfSubVec3 (direct, spline[i+1].Loc, spline[i-1].Loc);
		pfAddScaledVec3 (halfpoint, spline[i-1].Loc, 0.5f, direct);
		dist = pfNormalizeVec3 (direct);

		pfSubVec3 (toThis, spline[i].Loc, spline[i-1].Loc);
		pfSubVec3 (toNext, spline[i+1].Loc, spline[i].Loc);
		toThisDot = PFDOT_VEC3 (toThis, direct);
		toNextDot = PFDOT_VEC3 (toNext, direct);

		pfAddScaledVec3(spline[i].InVec, spline[i].Loc, -toThisDot * 0.4f, direct);
		pfAddScaledVec3(spline[i].OutVec, spline[i].Loc, toNextDot * 0.4f, direct);
	}

	pfCopyVec3 (spline[sobj->SData->NumSplines-1].InVec, spline[sobj->SData->NumSplines-1].Loc);

	SetStartingKnots (sobj, 0);
}

void DrawSplineTube(SplinedObj *sobj, float radius, float samplesPerKnot, int32 tubeSides, float uOffset, float vOffset, float firstKnot, float numKnots, uint32 flags)
{
	SplinedObj	drawObj = *sobj;
	pfMatrix	mat1, mat2;
	pfVec3		circleVerts[256], v1, v2;
	int			flipx = 0, flipy = 0;
	int			i;
	pfVec2		texVerts[2][2] = {	{{0.0f+uOffset, 1.0f+vOffset}, {0.0f+uOffset, 0.0f+vOffset}},
									{{1.0f+uOffset, 1.0f+vOffset}, {1.0f+uOffset, 0.0f+vOffset}}};
	pfVec2		uv[2];

	if (tubeSides > 256) tubeSides = 256;
	for (i = 0; i < tubeSides; i++) {
		pfSinCos(360.0f * (float)i/(float)tubeSides, &(circleVerts[i][PF_X]), &(circleVerts[i][PF_Z]));
		circleVerts[i][PF_Y] = 0.0f;
		pfScaleVec3(circleVerts[i], radius, circleVerts[i]);
	}
	drawObj.SplineTime = 0.0f;
	drawObj.SplineFactor = 0.0f;
	drawObj.Speed = 0.0f;
	drawObj.DesiredSwitch=0;
	drawObj.CurrentIDSetting=0;
	drawObj.AtSplineEnd = FALSE;
	drawObj.AtSplineStart = TRUE;

	FetchSplinePath(&drawObj, mat2);

	while (!(drawObj.AtSplineEnd)) {
		PFSET_VEC2(uv[0], uOffset, vOffset);
		PFSET_VEC2(uv[1], uOffset, vOffset+1.0f);
		drawObj.SplineTime += 1.0f/samplesPerKnot;
		pfCopyMat(mat1, mat2);
		FetchSplinePath(&drawObj, mat2);
		glBegin(GL_TRIANGLE_STRIP);
		flipy = !flipy;
//		if (drawObj.SplineTime >= firstKnot && drawObj.SplineTime <= (firstKnot+numKnots)) {
			for (i = 0; i < tubeSides; i++) {
				pfXformPt3(v1, circleVerts[i], mat1);
				pfXformPt3(v2, circleVerts[i], mat2);
				flipx = !flipx;
	//			glTexCoord2fv(texVerts[flipx][flipy]);
				glTexCoord2fv(uv[0]);
				glVertex3fv(v1);
	//			glTexCoord2fv(texVerts[flipx][!flipy]);
				glTexCoord2fv(uv[1]);
				glVertex3fv(v2);

				uv[0][PF_X] += 1.0f;
				uv[1][PF_X] += 1.0f;
			}
//		}
		/**** and one more to complete the circle ****/
		i = 0;
		pfXformPt3(v1, circleVerts[i], mat1);
		pfXformPt3(v2, circleVerts[i], mat2);
		flipx = !flipx;
//		glTexCoord2fv(texVerts[flipx][flipy]);
		glTexCoord2fv(uv[0]);
		glVertex3fv(v1);
//		glTexCoord2fv(texVerts[flipx][!flipy]);
		glTexCoord2fv(uv[1]);
		glVertex3fv(v2);

		glEnd();
	}
}

//void DrawPointTube(pfVec3 from, pfVec3 to, float radius, float vscale, int32 tubeSides, float uOffset, float vOffset, uint32 flags)
//{
//	pfMatrix	mat1, mat2, mat3;
//	pfVec3		circleVerts[256], v1, v2;
//	int			flipx = 0, flipy = 0;
//	int			i;
//	pfVec2		texVerts[2][2] = {	{{0.0f+uOffset, 1.0f+vOffset}, {0.0f+uOffset, 0.0f+vOffset}},
//									{{1.0f+uOffset, 1.0f+vOffset}, {1.0f+uOffset, 0.0f+vOffset}}};
//	pfVec2		uv[2];
//
//	if (tubeSides > 256) tubeSides = 256;
//	for (i = 0; i < tubeSides; i++) {
//		pfSinCos(360.0f * (float)i/(float)tubeSides, &(circleVerts[i][PF_X]), &(circleVerts[i][PF_Z]));
//		circleVerts[i][PF_Y] = 0.0f;
//		pfScaleVec3(circleVerts[i], radius, circleVerts[i]);
//	}
//
//	while (!(drawObj.AtSplineEnd)) {
//		PFSET_VEC2(uv[0], uOffset, vOffset);
//		PFSET_VEC2(uv[1], uOffset, vOffset+1.0f);
//		drawObj.SplineTime += 1.0f/samplesPerKnot;
//		pfCopyMat(mat1, mat2);
//		FetchSplinePath(&drawObj, mat2);
//		glBegin(GL_TRIANGLE_STRIP);
//		flipy = !flipy;
////		if (drawObj.SplineTime >= firstKnot && drawObj.SplineTime <= (firstKnot+numKnots)) {
//			for (i = 0; i < tubeSides; i++) {
//				pfXformPt3(v1, circleVerts[i], mat1);
//				pfXformPt3(v2, circleVerts[i], mat2);
//				flipx = !flipx;
//	//			glTexCoord2fv(texVerts[flipx][flipy]);
//				glTexCoord2fv(uv[0]);
//				glVertex3fv(v1);
//	//			glTexCoord2fv(texVerts[flipx][!flipy]);
//				glTexCoord2fv(uv[1]);
//				glVertex3fv(v2);
//
//				uv[0][PF_X] += 1.0f;
//				uv[1][PF_X] += 1.0f;
//			}
////		}
//		/**** and one more to complete the circle ****/
//		i = 0;
//		pfXformPt3(v1, circleVerts[i], mat1);
//		pfXformPt3(v2, circleVerts[i], mat2);
//		flipx = !flipx;
////		glTexCoord2fv(texVerts[flipx][flipy]);
//		glTexCoord2fv(uv[0]);
//		glVertex3fv(v1);
////		glTexCoord2fv(texVerts[flipx][!flipy]);
//		glTexCoord2fv(uv[1]);
//		glVertex3fv(v2);
//
//		glEnd();
//	}
//}
//
//
//
