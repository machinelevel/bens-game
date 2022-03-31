

/**** I'm just using this file for reference ****/
#if 0

#include "genincludes.h"

#include "upmath.h"
#include "unodes.h"



#include "isect.h"
#include "isectnotshared.h"



#if USE_SHORTVEC3

typedef short ShortVec3[3];
#define VertData ShortVec3

#else

#define VertData pfVec3

#endif



#define POLY_TRI 0
#define POLY_TRI2 1 /* Second triangle of a tristrip */
#define POLY_QUAD 2

#define SEARCH_Z		0
#define SEARCH_SEG		1
#define SEARCH_SPHERE	2
#define SEARCH_LOZENGE	3
#define SEARCH_MULTISEG	4

Boolean SegISectBothfacesOverride=FALSE;

Boolean SegISectFrontfaces=TRUE;
Boolean SegISectBackfaces=FALSE;
Boolean SphereISectBackfaces=TRUE;

float CloseDistance;
pfVec3 ClosePoint;
pfVec3 CloseNormal;
float CloseZHeight;
pfGeoSet *CloseGSet;

float SeekRadiusSquared;
float SeekRadius;
pfSeg SeekSeg;
pfVec3 SeekPoint;
pfVec3 SeekSegMax;
pfVec3 SeekSegMin;
short SeekType;

pfGeoSet *SeekGSet;

static pfGeoSet *CollisionResultGSet = NULL;


void (*SeekFunc)(VertData* SearchVerts, short PolyType);
void (*SeekIndexedFunc)(VertData* SearchVerts, short PolyType, ushort *SearchIndices);


pfVec3 SeekLozengeAxis;       /* Needs to be normalized */
float SeekLozengePosLimit;    /* Part of the plane equation */
float   SeekLozengeNegLimit;  /* Part of the plane equation */

pfVec3 SeekLozengeAxisOrig;   
float   SeekLozengePosLimitOrig;  
float   SeekLozengeNegLimitOrig;


/**** multi-segment intersections ****/
#define MAXMULTISEGS 16
pfSeg 	*SeekMultiSegOrig;
pfSeg	SeekMultiSeg[MAXMULTISEGS];
long	SeekMultiSegNum;
pfSeg	*SeekMultiBoundingCylinderSeg;
float	SeekMultiBoundingCylinderRadius;
float	*SeekMultiResultDists;
pfVec3	*SeekMultiResultPoints;
pfVec3	*SeekMultiResultNormals;


short SeekZMode;

#define HIT_TEST_LIST_MAX	64
short	HitTestListLength = 0;
pfVec3	HitTestList[HIT_TEST_LIST_MAX];
Boolean	HitTestListActive = FALSE;



/**** Transform Updates ****/
#define TSTATE_MATWITHTRANS 0x0001
#define TSTATE_MATWITHROT   0x0002

Boolean State_NewMat;
long	 State_NewCloseData;



	 





long ISectTravMaskAll=0x00000000;
long ISectTravMaskAny=0xFFFFFFFF;

#define NODE_INTERSECTION_ENABLED(_n)	( \
						((pfGetNodeTravMask(_n,PFTRAV_ISECT) & ISectTravMaskAll)==ISectTravMaskAll) && \
						((pfGetNodeTravMask(_n,PFTRAV_ISECT) & ISectTravMaskAny))    )

#define NODE_INTERSECTION_DISABLED(_node)	(!(NODE_INTERSECTION_ENABLED(_node)))
											 

void IMStackPushMat(pfMatrix TMat);
void IMStackPushMultMat(pfMatrix TMat);
void IMStackGetMat(pfMatrix TMat);
void IMStackPopMat(void);
void IMStackInit(void);










void SetSegISectBothFaceOverride(Boolean Val)
{
	SegISectBothfacesOverride=Val;
}

void ClearCollisionResultGSet(void)
{
	CollisionResultGSet = NULL;
}

pfGeoSet *GetCollisionResultGSet(void)
{
	return(CollisionResultGSet);
}



Boolean DebugIsectDump=FALSE;

#define USE_GEODE_BBOXES 1

#define IDEBUG 1
#define IDEBUGDI 0

#if IDEBUG
long IDTransformsPushed;
long IDTransformsUsed;
long IDNodesEncountered;
long IDNodesSearched;
long IDGeodesEncountered;
long IDGeodesSearched;
long IDGSetsEncountered;
long IDGSetsSearched;
long IDPolys;


void IDebugReset(void)
{
	IDTransformsPushed=0;
	IDTransformsUsed=0;
	IDNodesEncountered=0;
	IDNodesSearched=0;
	IDGeodesEncountered=0;
	IDGeodesSearched=0;
	IDGSetsEncountered=0;
	IDGSetsSearched=0;
	IDPolys=0;
}

void IDebugPrint(char *Comment)
{
#if 0	
	printf("%s\n",Comment);
	printf("   %d %s\n",	IDTransformsPushed,"TransformsPushed");
	printf("   %d %s\n",	IDTransformsUsed,"TransformsUsed");
	printf("   %d %s\n",	IDNodesEncountered,"NodesEncountered");
	printf("   %d %s\n",	IDNodesSearched,"NodesSearched");
	printf("   %d %s\n",	IDGeodesEncountered,"GeodesEncountered");
	printf("   %d %s\n",	IDGeodesSearched,"GeodesSearched");
	printf("   %d %s\n",	IDGSetsEncountered,"GSetsEncountered");
	printf("   %d %s\n",	IDGSetsSearched,"GSetsSearched");
	printf("   %d %s\n",	IDPolys,"Polys");
#endif	
}

void IDebugFetch(long *DataList, long Len)
{
	long i;

	
	for (i=0;i<Len;i++) {
		DataList[i]=0;
	}

	if (i>0) DataList[0]=IDTransformsPushed;
	if (i>1) DataList[1]=IDTransformsUsed;
	if (i>2) DataList[2]=IDNodesEncountered;
	if (i>3) DataList[3]=IDNodesSearched;
	if (i>4) DataList[4]=IDGeodesEncountered;
	if (i>5) DataList[5]=IDGeodesSearched;
	if (i>6) DataList[6]=IDGSetsEncountered;
	if (i>7) DataList[7]=IDGSetsSearched;
	if (i>8) DataList[8]=IDPolys;
}


#define IDebugIncrement(_var) _var++;

#else

#define IDebugIncrement(_var) 
void IDebugReset(void)
{
}
void IDebugPrint(char *Comment)
{
}

void IDebugFetch(long *DataList, long Len)
{
	long i;

	for (i=0;i<Len;i++) {
		DataList[i]=0;
	}
}
#endif






#if IDEBUGDI

long DebugIsectDepth=0;
long DebugIsectHoldVal=0;

#define DIDepthIncr DebugIsectDepth++
#define DIDepthDecr DebugIsectDepth--


#define	DISetHold(_val) DebugIsectHoldVal=_val

#define DIDump0(_Str) \
if (DebugIsectDump) { \
	long i; \
 \
	for (i=0;i<DebugIsectDepth;i++) printf(" "); \
	printf(_Str); \
} 

#define DIDump1(_Str,_x) \
if (DebugIsectDump) { \
	long i; \
 \
	for (i=0;i<DebugIsectDepth;i++) printf(" "); \
	printf(_Str,_x); \
} \

#define DIDump3(_Str,_x1,_x2,_x3) \
if (DebugIsectDump) { \
	long i; \
 \
	for (i=0;i<DebugIsectDepth;i++) printf(" "); \
	printf(_Str,_x1,_x2,_x3); \
} \



#else


#define DIDepthIncr 
#define DIDepthDecr 
#define	DISetHold(_val) 
#define DIDump0(_Str) 
#define DIDump1(_Str,_x) 
#define DIDump3(_Str,_x1,_x2,_x3) 



#endif

/***********************************************************************************\


       Wall stuff below here


\***********************************************************************************/



Boolean PlanePointInTriangle(pfVec3 PtInPlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 C12, pfVec3 C23, pfVec3 C31)
{
	pfVec3 Temp1,Temp2,Temp3;
	pfVec3	Cross1,Cross2,Cross3;
	float	Check1,Check2,Check3;
	long	UseAxis;
	
		
	PFSUB_VEC3(Temp1,C1,PtInPlane);
	PFSUB_VEC3(Temp2,C2,PtInPlane);
	PFSUB_VEC3(Temp3,C3,PtInPlane);
	
	pfCrossVec3(Cross1,C12,C23);
	if (Cross1[PF_X]==0.0f && Cross1[PF_Y]==0.0f && Cross1[PF_Z]==0.0f) return(FALSE); /* Triangle has no area */
	
	pfCrossVec3(Cross1,Temp1,C12);
	pfCrossVec3(Cross2,Temp2,C23);
	pfCrossVec3(Cross3,Temp3,C31);


	Check1=PF_ABS(Cross1[PF_X]);
	Check2=PF_ABS(Cross1[PF_Y]);
	Check3=PF_ABS(Cross1[PF_Z]);

	if (Check1+Check2+Check3 > 0.0001f) {  /* We're not on the first line */

		/* Find the axis with the greatest magnitude */

		if ((Check2) > (Check1)) {
			if ((Check3) > (Check2)) {
				UseAxis=PF_Z;
				/* 3 */
			} else {
				UseAxis=PF_Y;
				/* 2 */
			}
		} else {
			if ((Check3) > (Check1)) {
				UseAxis=PF_Z;
				/* 3 */
			} else {
				UseAxis=PF_X;
				/* 1 */
			}
		}
	


		/* Check if point is inside the triangle */

		if (Cross1[UseAxis]<0.0) {			/* Better to do these as if-else instead of one compound if so */
											/* We don't have to check Y and Z axes if X is sufficient to reject */
			 if (Cross2[UseAxis] <= 0 && Cross3[UseAxis] <= 0) return(TRUE);
		} else {
			 if (Cross2[UseAxis] >= 0 && Cross3[UseAxis] >= 0) return(TRUE);
			 
		}

	} else {
		/* we're on the first line */

		Check1=PF_ABS(Cross2[PF_X]);
		Check2=PF_ABS(Cross2[PF_Y]);
		Check3=PF_ABS(Cross2[PF_Z]);

		if (Check1+Check2+Check3 < 0.001f) return(TRUE);  /* At the intersection of two lines */
		
		/* Find the axis with the greatest magnitude */
		
		if ((Check2) > (Check1)) {
			if ((Check3) > (Check2)) {
				UseAxis=PF_Z;
				/* 3 */
			} else {
				UseAxis=PF_Y;
				/* 2 */
			}
		} else {
			if ((Check3) > (Check1)) {
				UseAxis=PF_Z;
				/* 3 */
			} else {
				UseAxis=PF_X;
				/* 1 */
			}
		}

		if (Cross2[UseAxis]<0.0) {
			 if (Cross3[UseAxis] <= 0) return(TRUE);
		} else {
			 if (Cross3[UseAxis] >= 0) return(TRUE);
			 
		}
	}
	
	return(FALSE);
}


void LozengeNormalCheck(float Distance, pfVec3 TransSeekPoint, pfVec3 LocalPoint, pfVec3 PlaneNormal)
{
	pfVec3 DiffVec;
	float  DiffLength;

	/* Distance is actually a squared distance */
	
	if (Distance > CloseDistance - 0.01f) {
					/* the close point is near one we already have (as opposed to much closer) */
			/* Need to worry about normals. Accept as closer a normal that is close to the hit direction */
		PFSUB_VEC3(DiffVec, TransSeekPoint,LocalPoint);
			/* Typically, we would normalize DiffVec here to do the DOT comparisons. But since DiffVec is used
			     in both DOT items, its length acts as a constant and can be ignored */
		if (PFDOT_VEC3(DiffVec,PlaneNormal) > PFDOT_VEC3(DiffVec,CloseNormal)) {
			CloseDistance=Distance;
			PFCOPY_VEC3(ClosePoint,LocalPoint);
			PFCOPY_VEC3(CloseNormal,PlaneNormal);
			CloseGSet=SeekGSet;
			State_NewCloseData=0x01;
		}
	} else {
		CloseDistance=Distance;
		PFCOPY_VEC3(ClosePoint,LocalPoint);
		PFCOPY_VEC3(CloseNormal,PlaneNormal);
		CloseGSet=SeekGSet;
		State_NewCloseData=0x01;
	}
}




void FinishLozengeCheck(pfVec3 PtInPlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 TransSeekPoint, pfVec3 PlaneNormal)
{
	float	TempDistance;
	float Distance;

	pfVec3 C12,C23,C31;

	pfVec3 LocalPoint;

	/* Constructed from the parts of the sphere check */

	PFSUB_VEC3(C12,C2,C1);
	PFSUB_VEC3(C23,C3,C2);
	PFSUB_VEC3(C31,C1,C3);

	/* We know the PtInPlane doesn't lie in the triangle. Otherwise it would have been caught earlier */
		
	foxClosestPtOnSeg(PtInPlane,C1,C2,C12);
	foxClosestPtOnSeg(PtInPlane,C2,C3,C23);
	foxClosestPtOnSeg(PtInPlane,C3,C1,C31);
		
	Distance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C12));
			
	PFCOPY_VEC3(LocalPoint,C12);
	TempDistance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C23));
	if (TempDistance < Distance) {
		PFCOPY_VEC3(LocalPoint,C23);
		Distance=TempDistance;
	}
	
	TempDistance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C31));
	
	if (TempDistance < Distance) {
		PFCOPY_VEC3(LocalPoint,C31);
		Distance=TempDistance;
	}
	
    if (Distance <= CloseDistance) {
		LozengeNormalCheck(Distance,TransSeekPoint,LocalPoint,PlaneNormal);
	}
}


void ClipSeg(pfVec3 Dest, pfVec3 P1, pfVec3 P2, pfVec3 Norm, float KVal)
{
	float k,NDotA,NDotB;
	pfVec3 PointDir;

	/* Code borrowed from SegIsectPlane */

	PFSUB_VEC3(PointDir,P2,P1);
	pfNormalizeVec3(PointDir);

	NDotA=PFDOT_VEC3(P1,Norm);  /* p=pos+dir*k for segment and (p dot N) - offset = 0 for plane */
	NDotB=PFDOT_VEC3(PointDir,Norm);	 /* And yes, I'm sure that it's -offset, and not +offset like the pf docs say */
	
/*	if (NDotB >= -0.0001 && NDotB <= 0.0001) return(-1.0f); */
/*  P1 and P2 supposedly lie on oposite sides of the plane. So don't worry about it */
	
	k=((KVal)-NDotA)/NDotB;
	
	PFSCALE_VEC3(Dest,k,PointDir);
	PFADD_VEC3(Dest,P1,Dest);	
}










/*


+2        /\
          --              ----                       --------------
                      /\  \  /                |\     \            /   /\
----------------------------------------------------------------------------------
                     /  \  \/                 | \     \          /   /  \
   /\                ----                     |  \     \        /   /    \
   --                                         |   \     \      /   /      \
                                       ----   |   /      \    /   /        \
+0                                 /\  \  /   |  /        \  /   /          \
-----------------------------------------------------------------------------------
                /\                /  \  \/    | /          \/   /            \
-1              --                ----        |/                --------------


  Triv    6     -3    2    4       -2   -1     1          3             0
  Acc.
*/




void LozengeDealWithTriangle(pfVec3 PtInPlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 TransSeekPoint, float Distance, pfVec3 PlaneNormal)
{
	float Dot1,Dot2,Dot3;
	short InVal1,InVal2,InVal3;
	pfVec3 P1,P2,P3,P4,P5;
	float LimitVal;
	float NDotS;
	

	NDotS=PFDOT_VEC3(PtInPlane,SeekLozengeAxis);
	if (NDotS > SeekLozengeNegLimit && NDotS < SeekLozengePosLimit) {
		/* It's between the two limit planes */
		/* Trivial accept */
		LozengeNormalCheck(Distance,TransSeekPoint,PtInPlane,PlaneNormal);

	} else {
		Dot1=PFDOT_VEC3(C1,SeekLozengeAxis);
		Dot2=PFDOT_VEC3(C2,SeekLozengeAxis);
		Dot3=PFDOT_VEC3(C3,SeekLozengeAxis);

		InVal1=0;
		InVal2=0;
		InVal3=0;

		if (Dot1<SeekLozengeNegLimit) InVal1=-1;
		 else if (Dot1>SeekLozengePosLimit) InVal1=2;

		if (Dot2<SeekLozengeNegLimit) InVal2=-1;
		 else if (Dot2>SeekLozengePosLimit) InVal2=2;

		if (Dot3<SeekLozengeNegLimit) InVal3=-1;
		 else if (Dot3>SeekLozengePosLimit) InVal3=2;


		LimitVal=SeekLozengePosLimit;

		/***** WOLF:  Caution. The ordering of the case statements below is very sensitive. LimitVal
		    defaults to the PosLimit, and is set to negative in appropriate cases */
		
		switch (InVal1+InVal2+InVal3) {
			case 6:
			case -3:
				/* Trivial rejects. Above and Below */
				break;

			case -1:
				/* Two In. One Down. */
				LimitVal=SeekLozengeNegLimit;
			case 2:
				/* One Up. Two In. */
  
				if (InVal1!=0) {
					ClipSeg(P1,C1,C2,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C1,C3,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,P1,C2,P2,TransSeekPoint,PlaneNormal);
					FinishLozengeCheck(PtInPlane,P2,C2,C3,TransSeekPoint,PlaneNormal);
				} else if (InVal2!=0) {
					ClipSeg(P1,C2,C3,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C2,C1,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,P1,C3,P2,TransSeekPoint,PlaneNormal);
					FinishLozengeCheck(PtInPlane,P2,C3,C1,TransSeekPoint,PlaneNormal);
				} else {
					ClipSeg(P1,C3,C1,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C3,C2,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,P1,C1,P2,TransSeekPoint,PlaneNormal);
					FinishLozengeCheck(PtInPlane,P2,C1,C2,TransSeekPoint,PlaneNormal);
				}
				break;

			case -2:
				/* One In. Two Down. */
				LimitVal=SeekLozengeNegLimit;
			case 4:
				/* Two Up. One In. */

				if (InVal1==0) {
					ClipSeg(P1,C1,C2,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C1,C3,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,C1,P1,P2,TransSeekPoint,PlaneNormal);
				} else if (InVal2==0) {
					ClipSeg(P1,C2,C3,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C2,C1,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,C2,P1,P2,TransSeekPoint,PlaneNormal);
				} else {
					ClipSeg(P1,C3,C1,SeekLozengeAxis,LimitVal);
					ClipSeg(P2,C3,C2,SeekLozengeAxis,LimitVal);
					FinishLozengeCheck(PtInPlane,C3,P1,P2,TransSeekPoint,PlaneNormal);
				}
				
				break;


			case 1:
				/* One Up. One In. One Down. */

				if (InVal1==0) {
					PFCOPY_VEC3(P3,C1);
					ClipSeg(P1,C2,C3,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P5,C2,C3,SeekLozengeAxis,SeekLozengeNegLimit);

					if (InVal2==2) {
						ClipSeg(P2,C1,C2,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C1,C3,SeekLozengeAxis,SeekLozengeNegLimit);
					} else {
						ClipSeg(P2,C1,C3,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C1,C2,SeekLozengeAxis,SeekLozengeNegLimit);
					}
				} else if (InVal2==0) {
					PFCOPY_VEC3(P3,C2);
					ClipSeg(P1,C3,C1,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P5,C3,C1,SeekLozengeAxis,SeekLozengeNegLimit);

					if (InVal3==2) {
						ClipSeg(P2,C2,C3,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C2,C1,SeekLozengeAxis,SeekLozengeNegLimit);
					} else {
						ClipSeg(P2,C2,C1,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C2,C3,SeekLozengeAxis,SeekLozengeNegLimit);
					}
				} else {
					PFCOPY_VEC3(P3,C3);
					ClipSeg(P1,C1,C2,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P5,C1,C2,SeekLozengeAxis,SeekLozengeNegLimit);

					if (InVal1==2) {
						ClipSeg(P2,C3,C1,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C3,C2,SeekLozengeAxis,SeekLozengeNegLimit);
					} else {
						ClipSeg(P2,C3,C2,SeekLozengeAxis,SeekLozengePosLimit);
						ClipSeg(P4,C3,C1,SeekLozengeAxis,SeekLozengeNegLimit);
					}
				}

				FinishLozengeCheck(PtInPlane,P1,P2,P3,TransSeekPoint,PlaneNormal);
				FinishLozengeCheck(PtInPlane,P1,P3,P5,TransSeekPoint,PlaneNormal);
				FinishLozengeCheck(PtInPlane,P3,P4,P5,TransSeekPoint,PlaneNormal);
				
				break;

			case 3:
				/* Two Up. One Down. */
			case 0:
				/* One Up. Two Down. */
				if (InVal2==InVal3) {
					ClipSeg(P1,C1,C2,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P2,C1,C3,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P3,C1,C3,SeekLozengeAxis,SeekLozengeNegLimit);
					ClipSeg(P4,C1,C2,SeekLozengeAxis,SeekLozengeNegLimit);
				} else if (InVal3==InVal1) {
					ClipSeg(P1,C2,C3,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P2,C2,C1,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P3,C2,C1,SeekLozengeAxis,SeekLozengeNegLimit);
					ClipSeg(P4,C2,C3,SeekLozengeAxis,SeekLozengeNegLimit);
				} else {
					ClipSeg(P1,C3,C1,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P2,C3,C2,SeekLozengeAxis,SeekLozengePosLimit);
					ClipSeg(P3,C3,C2,SeekLozengeAxis,SeekLozengeNegLimit);
					ClipSeg(P4,C3,C1,SeekLozengeAxis,SeekLozengeNegLimit);
				}
				
				FinishLozengeCheck(PtInPlane,P1,P2,P3,TransSeekPoint,PlaneNormal);
				FinishLozengeCheck(PtInPlane,P1,P3,P4,TransSeekPoint,PlaneNormal);
			
				break;


			default: ;

		}
	}
}


void SphereDealWithTriangle(pfPlane *ThePlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 TransSeekPoint)
{
	pfVec3 C12,C23,C31,Diff;
	pfVec3 PtInPlane;

	float	TempDistance;
	float Distance;
	Boolean ReverseNormal=FALSE;

	PFSUB_VEC3(Diff,TransSeekPoint,C1);
	if (PFDOT_VEC3(Diff,ThePlane->normal)<0) {		  /* Plane is facing away from us */
		if (!SphereISectBackfaces) return;
		ReverseNormal=TRUE;
	}
	
	foxClosestPtOnPlane(ThePlane,TransSeekPoint,PtInPlane); 
	Distance=(PFSQR_DISTANCE_PT3(TransSeekPoint,PtInPlane));

	if (Distance <= SeekRadiusSquared)  {
	
		PFSUB_VEC3(C12,C2,C1);
		PFSUB_VEC3(C23,C3,C2);
		PFSUB_VEC3(C31,C1,C3); 
		
		if (!PlanePointInTriangle(PtInPlane,C1,C2,C3,C12,C23,C31)) {
			foxClosestPtOnSeg(PtInPlane,C1,C2,C12);
			foxClosestPtOnSeg(PtInPlane,C2,C3,C23);
			foxClosestPtOnSeg(PtInPlane,C3,C1,C31);
			
			Distance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C12));
				
			PFCOPY_VEC3(PtInPlane,C12);
			TempDistance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C23));
			if (TempDistance < Distance) {
				PFCOPY_VEC3(PtInPlane,C23);
				Distance=TempDistance;
			}
			TempDistance=(PFSQR_DISTANCE_PT3(TransSeekPoint,C31));
			if (TempDistance < Distance) {
				PFCOPY_VEC3(PtInPlane,C31);
				Distance=TempDistance;
			}
		}
		if (HitTestListActive && (HitTestListLength < (HIT_TEST_LIST_MAX - 1))) {
			PFCOPY_VEC3(HitTestList[HitTestListLength], PtInPlane);
			HitTestListLength++;
		}
	    if (Distance <= CloseDistance) {
			if (SeekType==SEARCH_LOZENGE) {
				if (ReverseNormal) {
					PFNEGATE_VEC3(ThePlane->normal,ThePlane->normal);
					LozengeDealWithTriangle(PtInPlane,C1, C3, C2, TransSeekPoint, Distance,ThePlane->normal);
				} else {
					LozengeDealWithTriangle(PtInPlane,C1, C2, C3, TransSeekPoint, Distance,ThePlane->normal);
				}
			} else {
				State_NewCloseData=0x01;
				CloseDistance=Distance;
				CloseGSet=SeekGSet;
				PFCOPY_VEC3(ClosePoint,PtInPlane);
			}
		}
	}
}





#if 0
Boolean TestBBox(pfBox *BBox, pfVec3 Src, float Rad)
{
	if (Src[PF_X] > BBox->max[PF_X]+Rad) return (FALSE);
	if (Src[PF_X] < BBox->min[PF_X]-Rad) return (FALSE);
	if (Src[PF_Y] > BBox->max[PF_Y]+Rad) return (FALSE);
	if (Src[PF_Y] < BBox->min[PF_Y]-Rad) return (FALSE);
	if (Src[PF_Z] > BBox->max[PF_Z]+Rad) return (FALSE);
	if (Src[PF_Z] < BBox->min[PF_Z]-Rad) return (FALSE);
	return(TRUE);
}

#else

#define TestBBox(_BBox, _Src, _Rad) \
  (!(((_Src)[PF_X] > (_BBox)->max[PF_X]+_Rad) ||\
	 ((_Src)[PF_X] < (_BBox)->min[PF_X]-_Rad) ||\
	 ((_Src)[PF_Y] > (_BBox)->max[PF_Y]+_Rad) ||\
	 ((_Src)[PF_Y] < (_BBox)->min[PF_Y]-_Rad) ||\
	 ((_Src)[PF_Z] > (_BBox)->max[PF_Z]+_Rad) ||\
	 ((_Src)[PF_Z] < (_BBox)->min[PF_Z]-_Rad)))
#endif

#define TestBBox2(_BBox, _Up, _Down) \
  (!(((_Down)[PF_X] > (_BBox)->max[PF_X]) ||\
	 ((_Up)[PF_X] < (_BBox)->min[PF_X]) ||\
	 ((_Down)[PF_Y] > (_BBox)->max[PF_Y]) ||\
	 ((_Up)[PF_Y] < (_BBox)->min[PF_Y]) ||\
	 ((_Down)[PF_Z] > (_BBox)->max[PF_Z]) ||\
	 ((_Up)[PF_Z] < (_BBox)->min[PF_Z])))



#define TestSegBBox(_SegMax,_SegMin,_BBoxPtr) \
		 (!( \
				  (_SegMax[PF_X]<_BBoxPtr->min[PF_X]) ||  \
				  (_SegMin[PF_X]>_BBoxPtr->max[PF_X]) ||  \
				  (_SegMax[PF_Y]<_BBoxPtr->min[PF_Y]) ||  \
				  (_SegMin[PF_Y]>_BBoxPtr->max[PF_Y]) ||  \
				  (_SegMax[PF_Z]<_BBoxPtr->min[PF_Z]) ||  \
				  (_SegMin[PF_Z]>_BBoxPtr->max[PF_Z])     \
				  ))





/***********************************************************************************\


       Seg stuff below here


\***********************************************************************************/




float SegIntersectsPlane(pfPlane *ThePlane, pfSeg *TheSeg, pfVec3 Dst) /* return the distance to intersection */
{
	float k,NDotA,NDotB;

	NDotA=PFDOT_VEC3(TheSeg->pos,ThePlane->normal);  /* p=pos+dir*k for segment and (p dot N) - offset = 0 for plane */
	NDotB=PFDOT_VEC3(TheSeg->dir,ThePlane->normal);	 /* And yes, I'm sure that it's -offset, and not +offset like the pf docs say */
	if (NDotB >= -0.0001 && NDotB <= 0.0001) return(-1.0f);
	
	k=((ThePlane->offset)-NDotA)/NDotB;
	if (k < 0.0f) return(-1.0f);
	if (k > TheSeg->length) return(-1.0f);
	
	PFSCALE_VEC3(Dst,k,TheSeg->dir);
	PFADD_VEC3(Dst,TheSeg->pos,Dst);	
	return (k);
	
}


void SegDealWithTri(pfPlane *ThePlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfSeg *ThisSeg)
{
	pfVec3 C12,C23,C31;
	pfVec3 PtInPlane;
	float	TempDistance;
	float Distance;
	float DirDotNormal;
	Boolean ReverseNormal=FALSE;

	DirDotNormal=PFDOT_VEC3(ThisSeg->dir,ThePlane->normal);

	if (DirDotNormal > 0) {
		if (!SegISectBackfaces) return; /* Plane is facing away from us (right-handed system) */
		 else ReverseNormal=TRUE;
	} else {
#if 1
		if (!SegISectFrontfaces) return; /* Plane is facing away from us (left-handed system) */
#endif		
	}
	Distance=SegIntersectsPlane(ThePlane, ThisSeg, PtInPlane);
	if (Distance < 0.0f) return;  /* Must have been out of bounds of the segment */
	
	if (Distance < CloseDistance) {  /* See if this is closer than previous stuff before doing more calculation */
		PFSUB_VEC3(C12,C2,C1);
		PFSUB_VEC3(C23,C3,C2);
		PFSUB_VEC3(C31,C1,C3); 
			
		if (PlanePointInTriangle(PtInPlane,C1,C2,C3,C12,C23,C31)) {

			State_NewCloseData=0x01;
			CloseDistance=Distance;
			PFCOPY_VEC3(ClosePoint,PtInPlane);
			CloseGSet=SeekGSet;
			
			if (ReverseNormal) PFNEGATE_VEC3(CloseNormal,ThePlane->normal);
			 else PFCOPY_VEC3(CloseNormal,ThePlane->normal); 
		}
	}
}

/**** This was added by ej, who needs to check with foxwere about transformed seghits. ****/

Boolean SimpleSegHitTri(pfPlane *ThePlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfSeg *ThisSeg)
{
	pfVec3	C12,C23,C31;
	pfVec3	PtInPlane;
	float	Distance;

#if 0
	/**** assume backfaces don't obscure line-of-sight ****/
	/**** This is taken out because the check is done elsewhere now. ****/
	if (PFDOT_VEC3(ThisSeg->dir,ThePlane->normal)>0) return(TRUE);
#endif

	Distance=SegIntersectsPlane(ThePlane, ThisSeg, PtInPlane);
	if (Distance < 0.0f) return(FALSE);  /* Must have been out of bounds of the segment */
	
	PFSUB_VEC3(C12,C2,C1);
	PFSUB_VEC3(C23,C3,C2);
	PFSUB_VEC3(C31,C1,C3); 
	if (PlanePointInTriangle(PtInPlane,C1,C2,C3,C12,C23,C31)) return(TRUE);
	return(FALSE);
}


void MultiSegDealWithTri(pfPlane *ThePlane, pfVec3 C1, pfVec3 C2, pfVec3 C3)
{
	pfVec3 C12,C23,C31;
	pfVec3 PtInPlane;
	float	TempDistance;
	float Distance;
	float DirDotNormal;
	Boolean ReverseNormal=FALSE;
	long	i;
	
	/**** note: the segs are parallel, so we only need to check backfacing for one. ****/
	
	DirDotNormal=PFDOT_VEC3(SeekMultiSeg[0].dir,ThePlane->normal);

	if (DirDotNormal > 0) {
		if (!SegISectBackfaces) return; /* Plane is facing away from us (right-handed system) */
		 else ReverseNormal=TRUE;
	} else {
		if (!SegISectFrontfaces) return; /* Plane is facing away from us (left-handed system) */
	}

	for (i = 0; i < SeekMultiSegNum; i++) {
		Distance=SegIntersectsPlane(ThePlane, &SeekMultiSeg[i], PtInPlane);
		if (Distance < 0.0f) { /* Must have been out of bounds of the segment */

		} else 	if (Distance < SeekMultiResultDists[i]) {  /* See if this is closer than previous stuff before doing more calculation */
			PFSUB_VEC3(C12,C2,C1);
			PFSUB_VEC3(C23,C3,C2);
			PFSUB_VEC3(C31,C1,C3); 
				
			if (PlanePointInTriangle(PtInPlane,C1,C2,C3,C12,C23,C31)) {
	
				State_NewCloseData |= (0x01 << i);
			
				SeekMultiResultDists[i]=Distance;
				PFCOPY_VEC3(SeekMultiResultPoints[i],PtInPlane);
				
				if (ReverseNormal) PFNEGATE_VEC3(SeekMultiResultNormals[i],ThePlane->normal);
				 else PFCOPY_VEC3(SeekMultiResultNormals[i],ThePlane->normal); 
			}
		}
	}
}



























/************************************************************************************\
 ************************************************************************************

   Start of Seek Functions

 ************************************************************************************
\************************************************************************************/

#if 1 || USE_SHORTVEC3

#define UxCV1(_D,_S,_i) _D[_i][0]=_S[_i][0]; _D[_i][1]=_S[_i][1]; _D[_i][2]=_S[_i][2]; 
#define IxCV1(_D,_S,_i) _D[_i][0]=_S[SearchIndices[_i]][0]; _D[_i][1]=_S[SearchIndices[_i]][1]; _D[_i][2]=_S[SearchIndices[_i]][2]; 

#define UxCVT_SHORT3() UxCV1(UxTV,SearchVerts,0); UxCV1(UxTV,SearchVerts,1); UxCV1(UxTV,SearchVerts,2);
#define UxCVT_SHORT4() UxCV1(UxTV,SearchVerts,3);
#define IxCVT_SHORT3() IxCV1(IxTV,SearchVerts,0); IxCV1(IxTV,SearchVerts,1); IxCV1(IxTV,SearchVerts,2);
#define IxCVT_SHORT4() IxCV1(IxTV,SearchVerts,3);


#define IxSV(_idx) IxTV[_idx]
#define UxSV(_idx) UxTV[_idx]

#else

/* The other set of macros seems to run fractionally faster. Hence the 1 ||
   WOLF (21Oct98) */

#define UxCVT_SHORT3() 
#define UxCVT_SHORT4() 
#define IxCVT_SHORT3() 
#define IxCVT_SHORT4() 

/* Indexed SearchVert */
#define IxSV(_idx) SearchVerts[SearchIndices[_idx]]
/* Unindexed SearchVert */
#define UxSV(_idx) SearchVerts[_idx]

#endif



void PolyZSearch(VertData* SearchVerts, short PolyType)
{
#if 0	
	Boolean InPoly;
	pfPlane ThisPlane;
	float ThisZ;

	if (PolyType==POLY_QUAD) {
		InPoly=ZPointInQuad(SeekPoint,SearchVerts[0],SearchVerts[1],SearchVerts[2],SearchVerts[3]);
	} else {
		InPoly=ZPointInTriangle(SeekPoint,SearchVerts[0],SearchVerts[1],SearchVerts[2]);
	}

	if (InPoly) {
		if (PolyType==POLY_QUAD) {
			pfMakePtsPlane(&ThisPlane,SearchVerts[0],SearchVerts[1],SearchVerts[3]); 
			if (ThisPlane.normal[PF_Z] < 0.001f && ThisPlane.normal[PF_Z] > -0.001f) {
				pfMakePtsPlane(&ThisPlane,SearchVerts[1],SearchVerts[2],SearchVerts[3]); /* In case it's degenerate */
			}
		} else if (PolyType==POLY_TRI2) pfMakePtsPlane(&ThisPlane,SearchVerts[0],SearchVerts[2],SearchVerts[1]);
		 else pfMakePtsPlane(&ThisPlane,SearchVerts[0],SearchVerts[1],SearchVerts[2]);

		if (ThisPlane.normal[PF_Z] < 0.001f && ThisPlane.normal[PF_Z] > -0.001f) {
			/* CloseZHeight=-1000.0f; */
		} else {
			ThisZ=ZPtOnPlane(&ThisPlane,SeekPoint);

			switch (SeekZMode) {
				case ZMODE_ANY:
					CloseZHeight=ThisZ;
					break;
				case ZMODE_UP:
					if (ThisZ > SeekPoint[PF_Z] && ThisZ < CloseZHeight) CloseZHeight=ThisZ;
					break;
				case ZMODE_DOWN:
					if (ThisZ < SeekPoint[PF_Z] && ThisZ > CloseZHeight) CloseZHeight=ThisZ;
					break;
				case ZMODE_CLOSEST:
					if (PF_ABS(ThisZ - SeekPoint[PF_Z]) < PF_ABS(CloseZHeight - SeekPoint[PF_Z])) CloseZHeight=ThisZ; 
					break;
				default: ;
			}
		}
	}
#endif	
}





void PolyIndexZSearch(VertData* SearchVerts, short PolyType, ushort *SearchIndices)
{
#if 0	
	Boolean InPoly;
	pfPlane ThisPlane;
	float ThisZ;

	if (PolyType==POLY_QUAD) {
		InPoly=ZPointInQuad(SeekPoint,SearchVerts[SearchIndices[0]],SearchVerts[SearchIndices[1]],SearchVerts[SearchIndices[2]],SearchVerts[SearchIndices[3]]);
	} else {
		InPoly=ZPointInTriangle(SeekPoint,SearchVerts[SearchIndices[0]],SearchVerts[SearchIndices[1]],SearchVerts[SearchIndices[2]]);
	}

	if (InPoly) {
		if (PolyType==POLY_QUAD) {
			pfMakePtsPlane(&ThisPlane,SearchVerts[SearchIndices[0]],
									  SearchVerts[SearchIndices[1]],
									  SearchVerts[SearchIndices[3]]); 
			if (ThisPlane.normal[PF_Z] < 0.001f && ThisPlane.normal[PF_Z] > -0.001f) {
				pfMakePtsPlane(&ThisPlane,SearchVerts[SearchIndices[1]],
										  SearchVerts[SearchIndices[2]],
										  SearchVerts[SearchIndices[3]]); /* In case it's degenerate */
			}
		} else if (PolyType==POLY_TRI2) pfMakePtsPlane(&ThisPlane,SearchVerts[SearchIndices[0]],
																  SearchVerts[SearchIndices[2]],
																  SearchVerts[SearchIndices[1]]);
		 else pfMakePtsPlane(&ThisPlane,SearchVerts[SearchIndices[0]],
										SearchVerts[SearchIndices[1]],
										SearchVerts[SearchIndices[2]]);
		

		if (ThisPlane.normal[PF_Z] < 0.001f && ThisPlane.normal[PF_Z] > -0.001f) {
			/* CloseZHeight=-1000.0f; */
		} else {
			ThisZ=ZPtOnPlane(&ThisPlane,SeekPoint);

			switch (SeekZMode) {
				case ZMODE_ANY:
					CloseZHeight=ThisZ;
					break;
				case ZMODE_UP:
					if (ThisZ > SeekPoint[PF_Z] && ThisZ < CloseZHeight) CloseZHeight=ThisZ;
					break;
				case ZMODE_DOWN:
					if (ThisZ < SeekPoint[PF_Z] && ThisZ > CloseZHeight) CloseZHeight=ThisZ;
					break;
				case ZMODE_CLOSEST:
					if (PF_ABS(ThisZ - SeekPoint[PF_Z]) < PF_ABS(CloseZHeight - SeekPoint[PF_Z])) CloseZHeight=ThisZ; 
					break;
				default: ;
			}
		}
	}
#endif	
}















/********************************************************************************\

   Segment intersection set up. Do bounding box checks on each poly

\********************************************************************************/


#define TestBSegPoly4(_V1, _V2, _V3, _V4) \
	(!( \
	   (_V1[PF_X]<SeekSegMin[PF_X] && _V2[PF_X]<SeekSegMin[PF_X] && _V3[PF_X]<SeekSegMin[PF_X] && _V4[PF_X]<SeekSegMin[PF_X]) || \
	   (_V1[PF_X]>SeekSegMax[PF_X] && _V2[PF_X]>SeekSegMax[PF_X] && _V3[PF_X]>SeekSegMax[PF_X] && _V4[PF_X]>SeekSegMax[PF_X]) || \
	   (_V1[PF_Y]<SeekSegMin[PF_Y] && _V2[PF_Y]<SeekSegMin[PF_Y] && _V3[PF_Y]<SeekSegMin[PF_Y] && _V4[PF_Y]<SeekSegMin[PF_Y]) || \
	   (_V1[PF_Y]>SeekSegMax[PF_Y] && _V2[PF_Y]>SeekSegMax[PF_Y] && _V3[PF_Y]>SeekSegMax[PF_Y] && _V4[PF_Y]>SeekSegMax[PF_Y]) || \
	   (_V1[PF_Z]<SeekSegMin[PF_Z] && _V2[PF_Z]<SeekSegMin[PF_Z] && _V3[PF_Z]<SeekSegMin[PF_Z] && _V4[PF_Z]<SeekSegMin[PF_Z]) || \
	   (_V1[PF_Z]>SeekSegMax[PF_Z] && _V2[PF_Z]>SeekSegMax[PF_Z] && _V3[PF_Z]>SeekSegMax[PF_Z] && _V4[PF_Z]>SeekSegMax[PF_Z]) \
	  )) 

#define TestBSegPoly3(_V1, _V2, _V3) \
	(!( \
	   (_V1[PF_X] < SeekSegMin[PF_X] && _V2[PF_X] < SeekSegMin[PF_X] && _V3[PF_X] < SeekSegMin[PF_X]) || \
	   (_V1[PF_X] > SeekSegMax[PF_X] && _V2[PF_X] > SeekSegMax[PF_X] && _V3[PF_X] > SeekSegMax[PF_X]) ||  \
	   (_V1[PF_Y] < SeekSegMin[PF_Y] && _V2[PF_Y] < SeekSegMin[PF_Y] && _V3[PF_Y] < SeekSegMin[PF_Y]) ||  \
	   (_V1[PF_Y] > SeekSegMax[PF_Y] && _V2[PF_Y] > SeekSegMax[PF_Y] && _V3[PF_Y] > SeekSegMax[PF_Y]) ||  \
	   (_V1[PF_Z] < SeekSegMin[PF_Z] && _V2[PF_Z] < SeekSegMin[PF_Z] && _V3[PF_Z] < SeekSegMin[PF_Z]) ||  \
	   (_V1[PF_Z] > SeekSegMax[PF_Z] && _V2[PF_Z] > SeekSegMax[PF_Z] && _V3[PF_Z] > SeekSegMax[PF_Z]) \
	  )) 



void PolyIndexSegSearch(VertData* SearchVerts, short PolyType, ushort *SearchIndices)
{
	pfPlane ThisPlane;
	pfVec3 IxTV[4];
	
	IxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		IxCVT_SHORT4();
		if (TestBSegPoly4(IxSV(0),IxSV(1),IxSV(2),IxSV(3))) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(3));
			SegDealWithTri(&ThisPlane,IxSV(0),IxSV(1),IxSV(3),&SeekSeg);
			pfMakePtsPlane(&ThisPlane,IxSV(1),IxSV(2),IxSV(3));
			SegDealWithTri(&ThisPlane,IxSV(1),IxSV(2),IxSV(3),&SeekSeg);
		}
 	} else if (TestBSegPoly3(IxSV(0),IxSV(1),IxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(2),IxSV(1));
			SegDealWithTri(&ThisPlane,IxSV(0),IxSV(2),IxSV(1),&SeekSeg);
		} else {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(2));
			SegDealWithTri(&ThisPlane,IxSV(0),IxSV(1),IxSV(2),&SeekSeg);
		}
	} 
}



void PolySegSearch(VertData* SearchVerts, short PolyType)
{
	pfPlane ThisPlane;
	pfVec3 UxTV[4];
	
	UxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		UxCVT_SHORT4();
		if (TestBSegPoly4(UxSV(0),UxSV(1),UxSV(2),UxSV(3))) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(3));
			SegDealWithTri(&ThisPlane,UxSV(0),UxSV(1),UxSV(3),&SeekSeg);
			pfMakePtsPlane(&ThisPlane,UxSV(1),UxSV(2),UxSV(3));
			SegDealWithTri(&ThisPlane,UxSV(1),UxSV(2),UxSV(3),&SeekSeg);
		}
 	} else if (TestBSegPoly3(UxSV(0),UxSV(1),UxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(2),UxSV(1));
			SegDealWithTri(&ThisPlane,UxSV(0),UxSV(2),UxSV(1),&SeekSeg);
		} else {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(2));
			SegDealWithTri(&ThisPlane,UxSV(0),UxSV(1),UxSV(2),&SeekSeg);
		}
	} 
}


void PolyIndexMultiSegSearch(VertData* SearchVerts, short PolyType, ushort *SearchIndices)
{
	pfPlane ThisPlane;
	pfVec3 IxTV[4];

	IxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		IxCVT_SHORT4();
		if (TestBSegPoly4(IxSV(0),IxSV(1),IxSV(2),IxSV(3))) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(3));
			MultiSegDealWithTri(&ThisPlane,IxSV(0),IxSV(1),IxSV(3));
			pfMakePtsPlane(&ThisPlane,IxSV(1),IxSV(2),IxSV(3));
			MultiSegDealWithTri(&ThisPlane,IxSV(1),IxSV(2),IxSV(3));
		}
 	} else if (TestBSegPoly3(IxSV(0),IxSV(1),IxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(2),IxSV(1));
			MultiSegDealWithTri(&ThisPlane,IxSV(0),IxSV(2),IxSV(1));
		} else {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(2));
			MultiSegDealWithTri(&ThisPlane,IxSV(0),IxSV(1),IxSV(2));
		}
	} 
}


void PolyMultiSegSearch(VertData* SearchVerts, short PolyType)
{
	pfPlane ThisPlane;
	pfVec3 UxTV[4];

	UxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		UxCVT_SHORT4();
		if (TestBSegPoly4(UxSV(0),UxSV(1),UxSV(2),UxSV(3))) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(3));
			MultiSegDealWithTri(&ThisPlane,UxSV(0),UxSV(1),UxSV(3));
			pfMakePtsPlane(&ThisPlane,UxSV(1),UxSV(2),UxSV(3));
			MultiSegDealWithTri(&ThisPlane,UxSV(1),UxSV(2),UxSV(3));
		}
 	} else if (TestBSegPoly3(UxSV(0),UxSV(1),UxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(2),UxSV(1));
			MultiSegDealWithTri(&ThisPlane,UxSV(0),UxSV(2),UxSV(1));
		} else {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(2));
			MultiSegDealWithTri(&ThisPlane,UxSV(0),UxSV(1),UxSV(2));
		}
	} 
}









/********************************************************************************\

   Sphere intersection set up. Do bounding box checks on each poly

\********************************************************************************/

Boolean TestBPoly4(pfVec3 V0, pfVec3 V1, pfVec3 V2, pfVec3 V3)
{
	pfVec3 T1,T2,T3,T4;
	short Flag1,Flag2,Flag3,Flag4;
	short F1,F2,F3,F4,F5,F6;

	PFSUB_VEC3(T1,V0,SeekPoint);
	PFSUB_VEC3(T2,V1,SeekPoint);
	PFSUB_VEC3(T3,V2,SeekPoint);
	PFSUB_VEC3(T4,V3,SeekPoint);
#if 1
	/* This is faster when optimization is on */

	if (
	   (T1[PF_X] < -SeekRadius && T2[PF_X] < -SeekRadius && T3[PF_X] < -SeekRadius && T4[PF_X] < -SeekRadius) ||
	   (T1[PF_X] >  SeekRadius && T2[PF_X] >  SeekRadius && T3[PF_X] >  SeekRadius && T4[PF_X] >  SeekRadius) || 
	   (T1[PF_Y] < -SeekRadius && T2[PF_Y] < -SeekRadius && T3[PF_Y] < -SeekRadius && T4[PF_Y] < -SeekRadius) || 
	   (T1[PF_Y] >  SeekRadius && T2[PF_Y] >  SeekRadius && T3[PF_Y] >  SeekRadius && T4[PF_Y] >  SeekRadius) || 
	   (T1[PF_Z] < -SeekRadius && T2[PF_Z] < -SeekRadius && T3[PF_Z] < -SeekRadius && T4[PF_Z] < -SeekRadius) || 
	   (T1[PF_Z] >  SeekRadius && T2[PF_Z] >  SeekRadius && T3[PF_Z] >  SeekRadius && T4[PF_Z] >  SeekRadius)
	  ) return(FALSE);
	else return(TRUE);
	
#else

	/* This is faster when optimization is off */
	
	F1 = (T1[PF_X] < -SeekRadius && T2[PF_X] < -SeekRadius && T3[PF_X] < -SeekRadius && T4[PF_X] < -SeekRadius);
	F2 = (T1[PF_X] >  SeekRadius && T2[PF_X] >  SeekRadius && T3[PF_X] >  SeekRadius && T4[PF_X] >  SeekRadius);
	F3 = (T1[PF_Y] < -SeekRadius && T2[PF_Y] < -SeekRadius && T3[PF_Y] < -SeekRadius && T4[PF_Y] < -SeekRadius);
	F4 = (T1[PF_Y] >  SeekRadius && T2[PF_Y] >  SeekRadius && T3[PF_Y] >  SeekRadius && T4[PF_Y] >  SeekRadius);
	F5 = (T1[PF_Z] < -SeekRadius && T2[PF_Z] < -SeekRadius && T3[PF_Z] < -SeekRadius && T4[PF_Z] < -SeekRadius);
	F6 = (T1[PF_Z] >  SeekRadius && T2[PF_Z] >  SeekRadius && T3[PF_Z] >  SeekRadius && T3[PF_Z] >  SeekRadius);

	return(!(F1 || F2 || F3 || F4 || F5 || F6));
#endif	
}


Boolean TestBPoly3(pfVec3 V0, pfVec3 V1, pfVec3 V2)
{
	pfVec3 T1,T2,T3;
	short Flag1,Flag2,Flag3;
	short F1,F2,F3,F4,F5,F6;

	PFSUB_VEC3(T1,V0,SeekPoint);
	PFSUB_VEC3(T2,V1,SeekPoint);
	PFSUB_VEC3(T3,V2,SeekPoint);

#if 1
	/* This is faster when optimization is on */

	if (
	   (T1[PF_X] < -SeekRadius && T2[PF_X] < -SeekRadius && T3[PF_X] < -SeekRadius) ||
	   (T1[PF_X] >  SeekRadius && T2[PF_X] >  SeekRadius && T3[PF_X] >  SeekRadius) || 
	   (T1[PF_Y] < -SeekRadius && T2[PF_Y] < -SeekRadius && T3[PF_Y] < -SeekRadius) || 
	   (T1[PF_Y] >  SeekRadius && T2[PF_Y] >  SeekRadius && T3[PF_Y] >  SeekRadius) || 
	   (T1[PF_Z] < -SeekRadius && T2[PF_Z] < -SeekRadius && T3[PF_Z] < -SeekRadius) || 
	   (T1[PF_Z] >  SeekRadius && T2[PF_Z] >  SeekRadius && T3[PF_Z] >  SeekRadius)
	  ) return(FALSE);
	else return(TRUE);
	
#else

	/* This is faster when optimization is off */
	
	F1 = (T1[PF_X] < -SeekRadius && T2[PF_X] < -SeekRadius && T3[PF_X] < -SeekRadius);
	F2 = (T1[PF_X] >  SeekRadius && T2[PF_X] >  SeekRadius && T3[PF_X] >  SeekRadius);
	F3 = (T1[PF_Y] < -SeekRadius && T2[PF_Y] < -SeekRadius && T3[PF_Y] < -SeekRadius);
	F4 = (T1[PF_Y] >  SeekRadius && T2[PF_Y] >  SeekRadius && T3[PF_Y] >  SeekRadius);
	F5 = (T1[PF_Z] < -SeekRadius && T2[PF_Z] < -SeekRadius && T3[PF_Z] < -SeekRadius);
	F6 = (T1[PF_Z] >  SeekRadius && T2[PF_Z] >  SeekRadius && T3[PF_Z] >  SeekRadius);

	return(!(F1 || F2 || F3 || F4 || F5 || F6));
#endif	
}



void PolyIndexSphereSearch(VertData* SearchVerts, short PolyType, ushort *SearchIndices)
{
	pfPlane ThisPlane;
	pfVec3 IxTV[4];
	
	IxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		IxCVT_SHORT4();
		if (TestBPoly4(IxSV(0),IxSV(1),IxSV(2),IxSV(3))) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(3));
			SphereDealWithTriangle(&ThisPlane,IxSV(0),IxSV(1),IxSV(3),SeekPoint);
			pfMakePtsPlane(&ThisPlane,IxSV(1),IxSV(2),IxSV(3));
			SphereDealWithTriangle(&ThisPlane,IxSV(1),IxSV(2),IxSV(3),SeekPoint);
		}
 	} else if (TestBPoly3(IxSV(0),IxSV(1),IxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(2),IxSV(1));
			SphereDealWithTriangle(&ThisPlane,IxSV(0),IxSV(2),IxSV(1),SeekPoint);
		} else {
			pfMakePtsPlane(&ThisPlane,IxSV(0),IxSV(1),IxSV(2));
			SphereDealWithTriangle(&ThisPlane,IxSV(0),IxSV(1),IxSV(2),SeekPoint);
		}
	} 
}


	

void PolySphereSearch(VertData* SearchVerts, short PolyType)
{
	pfPlane ThisPlane;
	pfVec3 UxTV[4];
	
	UxCVT_SHORT3();
	if (PolyType==POLY_QUAD) {
		UxCVT_SHORT4();
		if (TestBPoly4(UxSV(0),UxSV(1),UxSV(2),UxSV(3))) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(3));
			SphereDealWithTriangle(&ThisPlane,UxSV(0),UxSV(1),UxSV(3),SeekPoint);
			pfMakePtsPlane(&ThisPlane,UxSV(1),UxSV(2),UxSV(3));
			SphereDealWithTriangle(&ThisPlane,UxSV(1),UxSV(2),UxSV(3),SeekPoint);
		}
 	} else if (TestBPoly3(UxSV(0),UxSV(1),UxSV(2))) {
		if (PolyType==POLY_TRI2) {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(2),UxSV(1));
			SphereDealWithTriangle(&ThisPlane,UxSV(0),UxSV(2),UxSV(1),SeekPoint);
		} else {
			pfMakePtsPlane(&ThisPlane,UxSV(0),UxSV(1),UxSV(2));
			SphereDealWithTriangle(&ThisPlane,UxSV(0),UxSV(1),UxSV(2),SeekPoint);
		}
	} 
}



/************************************************************************************\
 ************************************************************************************

   End of Seek Functions

 ************************************************************************************
\************************************************************************************/













#if 0
/* This code could come out, but it runs faster when it's still in! Caching??? WOLF(19Dec97) */
void PolyGeneralSearch(pfVec3* SearchVerts, short PolyType, ushort *SearchIndices)
{
	if (SearchIndices == NULL) {
		switch(SeekType) {
			case SEARCH_Z: PolyZSearch(SearchVerts,PolyType); break;
			case SEARCH_SEG: PolySegSearch(SearchVerts,PolyType); break;
			case SEARCH_MULTISEG: PolyMultiSegSearch(SearchVerts,PolyType); break;
			case SEARCH_SPHERE: PolySphereSearch(SearchVerts,PolyType); break;
			case SEARCH_LOZENGE: PolySphereSearch(SearchVerts,PolyType); break;
			default: ;
		}
	}
	else {
		switch(SeekType) {
			case SEARCH_Z: PolyIndexZSearch(SearchVerts,PolyType,SearchIndices); break;
			case SEARCH_SEG: PolyIndexSegSearch(SearchVerts,PolyType,SearchIndices); break;
			case SEARCH_MULTISEG: PolyIndexMultiSegSearch(SearchVerts,PolyType,SearchIndices); break;
			case SEARCH_SPHERE: PolyIndexSphereSearch(SearchVerts,PolyType,SearchIndices); break;
			case SEARCH_LOZENGE: PolyIndexSphereSearch(SearchVerts,PolyType,SearchIndices); break;
			default: ;
		}
	}
}
#endif


float SearchGeoSet(pfGeoSet *ThisGSet)
{

    VertData  *vertList;

    ushort  *polyList;
    long    i,j,k;
    long    primType, primNum;
	long	*primLengths;
	short	TriStripParity;

	SeekGSet=ThisGSet;
	
	DISetHold(IDPolys);
	IDebugIncrement(IDGSetsSearched);
	
    primType = pfGetGSetPrimType(ThisGSet);

    primNum = pfGetGSetNumPrims(ThisGSet);
    pfGetGSetAttrLists(ThisGSet, PFGS_COORD3, (void **)&vertList, &polyList);

	if (vertList == NULL) printf("WPoly Error: vertList is NULL\n");
	 else if (polyList != NULL) {
		/* printf("WPoly Error: Indexed GeoSet\n"); */
		/* pfPrint(ThisGSet, PFTRAV_SELF, PFPRINT_VB_ON, NULL); */

		k=0;
		switch (primType) {
			case PFGS_TRISTRIPS:
			case PFGS_FLAT_TRISTRIPS:
				primLengths = pfGetGSetPrimLengths(ThisGSet);
				for (i=0; i<primNum; i++) {
					TriStripParity=POLY_TRI;
					for (j=0;j<primLengths[i]-2;j++) {
						IDebugIncrement(IDPolys);
						SeekIndexedFunc(vertList,TriStripParity, (&polyList[k]));
						TriStripParity=1-TriStripParity;
						k+=1;
					}
					k+=2;
				}
				break;
			case PFGS_QUADS:
				for (i=0; i<primNum; i++) {
					IDebugIncrement(IDPolys);
					IDebugIncrement(IDPolys);
					SeekIndexedFunc(vertList,POLY_QUAD, (&polyList[k]));
					k+=4;
				}
				break;
			case PFGS_TRIS:
				for (i=0; i<primNum; i++) {
					IDebugIncrement(IDPolys);
					SeekIndexedFunc(vertList,POLY_TRI, (&polyList[k]));
					k+=3;
				}
				break;
			default: ;
		}
	}
	 else {
		k=0;
		switch (primType) {
			case PFGS_TRISTRIPS:
			case PFGS_FLAT_TRISTRIPS:
				primLengths = pfGetGSetPrimLengths(ThisGSet);
				for (i=0; i<primNum; i++) {
					TriStripParity=POLY_TRI;
					for (j=0;j<primLengths[i]-2;j++) {
						IDebugIncrement(IDPolys);
						SeekFunc((&vertList[k]),TriStripParity);
						TriStripParity=1-TriStripParity;
						k+=1;
					}
					k+=2;
				}
				break;
			case PFGS_QUADS:
				for (i=0; i<primNum; i++) {
					IDebugIncrement(IDPolys);
					IDebugIncrement(IDPolys);
					SeekFunc((&vertList[k]),POLY_QUAD);
					k+=4;
				}
				break;
			case PFGS_TRIS:
				for (i=0; i<primNum; i++) {
					IDebugIncrement(IDPolys);
					SeekFunc((&vertList[k]),POLY_TRI);
					k+=3;
				}
				break;
			default: ;
		}
	}
	DIDump1("GSet Searched (%d Polys)\n",IDPolys-DebugIsectHoldVal);
	return(0);
}











/***************************************************************\


     		Root level routines for Z,Seg,Sphere


\***************************************************************/


void ZCollisionSearchGeode(pfGeode *ThisGeode)
{
	long NumSets,i;
	Boolean Done;
	pfGeoSet *ThisGSet;

	if (ThisGeode != NULL) {
		NumSets=pfGetNumGSets(ThisGeode);
		i=0;
		Done=FALSE;
		while (!Done) {
			if (i>=NumSets) Done=TRUE;
			 else {
				ThisGSet=pfGetGSet(ThisGeode,i);
				SearchGeoSet(ThisGSet);;
				if (CloseZHeight > -200.0f && SeekZMode==ZMODE_ANY) Done=TRUE;
				i++;
			}
		}
	}
}



void RecursiveZCollisionSearch(pfNode *SearchNode)
{
	long i;
	long NumChildren;
	long NodeType;

	if (SearchNode==NULL) return;
	if (NODE_INTERSECTION_DISABLED(SearchNode)) return;
	NodeType=pfGetType(SearchNode);
	if (NodeType==PFTYPE_GEODE) {
		ZCollisionSearchGeode((pfGeode *)SearchNode);
	} else {
		if (NodeType & PFCLASS_GROUP) {
			if (NodeType & PFCLASS_SCS) {
				if (NODE_NOROTATION(SearchNode)) {
				}
				
				if (NodeType==PFTYPE_SCS) {
				} else { /* DCS */
				}
			}
			NumChildren=pfGetNumChildren(SearchNode);
			for (i=0;i<NumChildren;i++) {
				RecursiveZCollisionSearch(pfGetChild(SearchNode,i));
				if (CloseZHeight > -200.0f && SeekZMode==ZMODE_ANY) return; /* Got one, so return it */
			}
		}
	}
}


float ZCollisionSearch(pfNode *ThisNode, pfVec3 Src, short ZMode)
{
	long NumSets,i;
	pfGeoSet *ThisGSet;
	Boolean Done;
	
	CloseGSet=NULL;
	CloseZHeight=-600.0f;
	PFCOPY_VEC3(SeekPoint,Src);
	SeekType=SEARCH_Z;
	SeekFunc=PolyZSearch;
	SeekIndexedFunc=PolyIndexZSearch;
	SeekZMode=ZMode;

	if (ThisNode != NULL) RecursiveZCollisionSearch(ThisNode);
	return (CloseZHeight);
}


void TransformForSphere(long TState, pfVec3 Seek, pfVec3 Orig, pfVec3 SeekAx, pfVec3 OrigAx)
{
	pfMatrix TMat;
	pfMatrix IMat;

	if (TState & TSTATE_MATWITHTRANS) {
		IMStackGetMat(TMat);
		IDebugIncrement(IDTransformsUsed);
		DIDump0("Transform Used\n");
			
		if (TState & TSTATE_MATWITHROT) {
			pfInvertOrthoMat(IMat,TMat);
			pfXformPt3(Seek,Orig,IMat);
			if (SeekType==SEARCH_LOZENGE) {
				pfXformVec3(SeekAx,OrigAx,IMat);
				SeekLozengePosLimit=SeekLozengePosLimitOrig + PFDOT_VEC3(SeekAx,IMat[PF_T]);
				SeekLozengeNegLimit=SeekLozengeNegLimitOrig + PFDOT_VEC3(SeekAx,IMat[PF_T]);
			}
		} else {
			PFSUB_VEC3(Seek,Orig,TMat[PF_T]);
		
			if (SeekType==SEARCH_LOZENGE) {
				PFCOPY_VEC3(SeekAx,OrigAx);
				SeekLozengePosLimit=SeekLozengePosLimitOrig - PFDOT_VEC3(SeekAx,TMat[PF_T]);
				SeekLozengeNegLimit=SeekLozengeNegLimitOrig - PFDOT_VEC3(SeekAx,TMat[PF_T]);
			}
			
		}
	} else {
		PFCOPY_VEC3(Seek,Orig);
		if (SeekType==SEARCH_LOZENGE) {
			PFCOPY_VEC3(SeekAx,OrigAx);
			SeekLozengePosLimit=SeekLozengePosLimitOrig;
			SeekLozengeNegLimit=SeekLozengeNegLimitOrig;
		}
	}
}


void UntransformResults(long TState)
{
	pfMatrix TMat;
	long i;
	long ShiftTest;

	if (TState & TSTATE_MATWITHTRANS) {
		IMStackGetMat(TMat);
		
		if (TState & TSTATE_MATWITHROT) {
			if (SeekType == SEARCH_MULTISEG) {
				ShiftTest=0x01;
				for (i=0;i<SeekMultiSegNum;i++) {
					if (State_NewCloseData & ShiftTest) {
						pfXformPt3(SeekMultiResultPoints[i],SeekMultiResultPoints[i],TMat);
						pfXformVec3(SeekMultiResultNormals[i],SeekMultiResultNormals[i],TMat);
					}
					ShiftTest = ShiftTest << 1;
				}
			} else {
				pfXformPt3(ClosePoint,ClosePoint,TMat);
				if (SeekType != SEARCH_SPHERE) pfXformVec3(CloseNormal,CloseNormal,TMat);
			}
		} else {
			if (SeekType == SEARCH_MULTISEG) {
				ShiftTest=0x01;
				for (i=0;i<SeekMultiSegNum;i++) {
					if (State_NewCloseData & ShiftTest) {
						PFADD_VEC3(SeekMultiResultPoints[i],SeekMultiResultPoints[i],TMat[PF_T]);
					}
					ShiftTest = ShiftTest << 1;
				}
			} else {
				PFADD_VEC3(ClosePoint,ClosePoint,TMat[PF_T]);
			}
		}
	}
}

	 
void SphereCollisionSearchGeode(pfGeode *SearchGeode, pfVec3 OrigPoint, long TState)
{
	long NumSets, i, numHits;
	float	dist;
	pfGeoSet *ThisGSet;
	pfBox BBox;
	pfBox *BBoxPtr;
	pfVec3 SeekHigh,SeekLow;

	if (SearchGeode==NULL) return;

	if (State_NewMat) {
		/* If NewMatrix then invert and calc new seekPoint */
		State_NewMat=FALSE;

		TransformForSphere(TState,SeekPoint,OrigPoint,SeekLozengeAxis,SeekLozengeAxisOrig);
		
	}

	

	SeekHigh[PF_X]=SeekPoint[PF_X]+SeekRadius;
	SeekHigh[PF_Y]=SeekPoint[PF_Y]+SeekRadius;
	SeekHigh[PF_Z]=SeekPoint[PF_Z]+SeekRadius;
	SeekLow[PF_X]=SeekPoint[PF_X]-SeekRadius;
	SeekLow[PF_Y]=SeekPoint[PF_Y]-SeekRadius;
	SeekLow[PF_Z]=SeekPoint[PF_Z]-SeekRadius;

	BBoxPtr=&(BBox);
	
#if USE_GEODE_BBOXES 
	ISECT_FETCH_GEODE_BBOX(SearchGeode,BBoxPtr); /* in isectnotshared.h */
	if (BBoxPtr != NULL && !TestBBox2(BBoxPtr,SeekHigh,SeekLow)) {
		
		return;
	}
#endif
	
	IDebugIncrement(IDGeodesSearched);
	DIDump0("Geode Searched\n");
	
	NumSets=pfGetNumGSets(SearchGeode);
	
	
	for (i=0;i<NumSets;i++) {
		IDebugIncrement(IDGSetsEncountered);
		DIDump0("GSet Encountered\n");
		ISECT_FETCH_GSET_AND_BBOX(ThisGSet,SearchGeode,i,BBox,BBoxPtr); /* in isectnotshared.h */
	
		if (TestBBox2(BBoxPtr,SeekHigh,SeekLow)) {
/*		if (TestBBox(BBoxPtr,SeekPoint,SeekRadius)) { */
			SearchGeoSet(ThisGSet);
		}
	}

	if (State_NewCloseData) {
		/* Calc New Close Data based on matrix */
		UntransformResults(TState);
		State_NewCloseData=0x00;
	}
}


#define SPHEREDCS 1



void RecursiveSphereCollisionSearch(pfNode *SearchNode, pfVec3 OrigPoint, long TState)
{
	long i;
	long NodeType;
	pfMatrix TMat;
	long NumChildren;
	pfNode *TChild;
	
	DIDump0("NodeSearched\n");
	IDebugIncrement(IDNodesSearched);
	
	NodeType=pfGetType(SearchNode);
	if (NodeType==PFTYPE_GEODE) {
		DIDump0("Geode Encountered\n");
		IDebugIncrement(IDGeodesEncountered);
		SphereCollisionSearchGeode((pfGeode *)SearchNode,OrigPoint,TState);
	} else {
		if (NodeType & PFCLASS_GROUP) {
#if SPHEREDCS
			if (NodeType & PFCLASS_SCS) {
				if (NODE_NOROTATION(SearchNode)) {
					TState |= TSTATE_MATWITHTRANS;
				} else {
					TState |= (TSTATE_MATWITHTRANS | TSTATE_MATWITHROT);
				}
				  
				if (NodeType==PFTYPE_SCS) {
					pfGetSCSMat((pfSCS *)SearchNode,TMat);
				} else { /* DCS */
					pfGetDCSMat((pfDCS *)SearchNode,TMat);
				}
				
				/* PushMat */
	 			IMStackPushMultMat(TMat);
				State_NewMat=TRUE;
				DIDump0("Transform Pushed\n");
				IDebugIncrement(IDTransformsPushed);
			}
#endif
			DIDepthIncr;
			NumChildren=pfGetNumChildren(SearchNode);
			for (i=0;i<NumChildren;i++) {
				TChild=pfGetChild(SearchNode,i);
				if (TChild != NULL) {
					IDebugIncrement(IDNodesEncountered);
					DIDump3("NodeEncountered: (%x) [%x] [%x]\n",TChild->Type,TChild->DrawTravMask,TChild->ISectTravMask);

/*					DIDump3("  TravMasks: (%x) [%x] [%x]\n",
(pfGetNodeTravMask(Search,PFTRAV_ISECT) & ISectTravMaskAll),
ISectTravMaskAll,ISectTravMaskAny); */
						  
					if  (NODE_INTERSECTION_ENABLED(TChild)) {
						RecursiveSphereCollisionSearch(TChild,OrigPoint,TState);
					}
				}
			}
			DIDepthDecr;
#if SPHEREDCS
			if (NodeType & PFCLASS_SCS) {
				IMStackPopMat();
				State_NewMat=TRUE;
			}
#endif			
		}
	}
}

Boolean SphereCollisionSearch(pfNode *SearchNode, pfVec3 Src, float Radius, Boolean DoZ)
{
	pfVec3 TVec;
	long TState;

	DIDump0("Sphere\n");
	
	if (SearchNode==NULL) return(FALSE);

	/* IDebugReset(); */
	
	SeekRadius=Radius;
	SeekRadiusSquared=Radius*Radius;
	PFCOPY_VEC3(SeekPoint,Src);

	CloseGSet=NULL;
	CloseDistance = SeekRadiusSquared * 1.1f;	/* to catch borderline cases */
	SeekType=SEARCH_SPHERE;
	SeekFunc=PolySphereSearch;
	SeekIndexedFunc=PolyIndexSphereSearch;

	State_NewMat=FALSE;
	State_NewCloseData=0x00;
	TState=0x0000;
	IMStackInit();
	
	RecursiveSphereCollisionSearch(SearchNode,Src,TState);

	/* IDebugPrint("Sphere"); */
	

/*********************************************************************\
	Tried averaging hit points below, and Mark and I decided that it
	won't work this way. Changed back to the simpler "closest point"
	method.	- ej 6/21/95  FXW (Code removed 8Mar96)
\*********************************************************************/
	if (CloseDistance < (SeekRadiusSquared)) {
		PFSUB_VEC3(TVec,Src,ClosePoint);
		pfNormalizeVec3(TVec);

		pfScaleVec3(TVec, Radius - pfSqrt(CloseDistance), TVec);
		Src[PF_X] += TVec[PF_X];
		Src[PF_Y] += TVec[PF_Y];
		if (DoZ) Src[PF_Z] += TVec[PF_Z];
		return(TRUE);
		if (CloseGSet != NULL) CollisionResultGSet = CloseGSet;
	}

	return(FALSE);
	


}





void LozengeCollisionSearchSetup(pfVec3 Src, float Radius, pfVec3 LozengeAxis, float LozengeUp, float LozengeDown)
{
	float SrcKVal;


	/* IDebugReset(); */
	
	SeekRadius=Radius;
	SeekRadiusSquared=Radius*Radius;
	PFCOPY_VEC3(SeekPoint,Src);


	PFCOPY_VEC3(SeekLozengeAxis,LozengeAxis);
	PFCOPY_VEC3(SeekLozengeAxisOrig,LozengeAxis);
	
	SrcKVal = PFDOT_VEC3(Src,LozengeAxis);   /* The KVal for the source point. This is from the plane equation */

	SeekLozengePosLimit = SrcKVal + LozengeUp;
	SeekLozengeNegLimit = SrcKVal + LozengeDown;

	SeekLozengePosLimitOrig=SeekLozengePosLimit;
	SeekLozengeNegLimitOrig=SeekLozengeNegLimit;
	

	CloseGSet=NULL;
	CloseDistance = SeekRadiusSquared * 1.1f;	/* to catch borderline cases */
	SeekType=SEARCH_LOZENGE;
	SeekFunc=PolySphereSearch;
	SeekIndexedFunc=PolyIndexSphereSearch;

	State_NewMat=FALSE;
	State_NewCloseData=0x00;
	IMStackInit();

}




Boolean LozengeCollisionSearchCompletion(pfVec3 Src, float Radius,
							   pfVec3 LozengeAxis, float LozengeUp, float LozengeDown,
							   pfVec3 ResultNormal, pfVec3 HitPointDirection,
							   pfVec3 ResultClosePt)
{
	pfVec3 TVec;

	/* IDebugPrint("Lozenge"); */

	if (CloseDistance < (SeekRadiusSquared)) {

		PFCOPY_VEC3(ResultNormal,CloseNormal);			

		/* OK. Now we need to update the Src position and keep it away from whatever we ran into */
		

#if 0
		/* This is the old sphere method */
		
		PFSUB_VEC3(TVec,Src,ClosePoint);
		pfNormalizeVec3(TVec);
		
		pfScaleVec3(TVec, Radius - pfSqrt(CloseDistance), TVec);
		Src[PF_X] += TVec[PF_X];
		Src[PF_Y] += TVec[PF_Y];
		Src[PF_Z] += TVec[PF_Z];

#else

		
		{
			/* P is the point on the rim closest to the close point. C is Close point S is Center/Source*/
			/* Three cases to worry about:

			   		A) Closest point directly below center (HitNormal is parallel to Lozenge Axis)
					B) Closest point in Cone formed by the center and the top or bottom disc
					C) Closest point not in any of above (Normal sphere hit)

			*/

			pfVec3 P,h;
			pfVec3 q,qUp,qDown,TempCross;
			float  TLen,d;
			pfVec3 RUp,RDown;
			pfVec3 UpCross,DownCross;


			PFSUB_VEC3(TVec,Src,ClosePoint);
			pfNormalizeVec3(TVec);
			


			pfCrossVec3(TempCross,TVec,LozengeAxis);
			pfCrossVec3(q,TempCross,LozengeAxis); 			/* q is a vector perpendicular to the lozenge axis
															   	that points from Src to the closest point */

			TLen = pfLengthVec3(q);
			
			if (TLen < 0.01f) {
				/* Case A */

					/* Check if we're above or below */
					if (PFDOT_VEC3(TVec,LozengeAxis) < 0.0f) {
						pfScaleVec3(TVec, -(LozengeUp - pfSqrt(CloseDistance)), ResultNormal);
					} else {
						pfScaleVec3(TVec, -LozengeDown - pfSqrt(CloseDistance), ResultNormal);
					}
					
					Src[PF_X] += TVec[PF_X];
					Src[PF_Y] += TVec[PF_Y];
					Src[PF_Z] += TVec[PF_Z];

					PFNEGATE_VEC3(HitPointDirection,ResultNormal);
					
			} else {
				
				pfScaleVec3 (q,1.0f/TLen,q); /* Normalize q */

				/* Find qUp, qDown, RUp and RDown, which point to the disc limits in the direction of the closest point */
				pfScaleVec3(qUp,pfSqrt(SeekRadiusSquared-LozengeUp*LozengeUp),q);
				pfAddScaledVec3(RUp,qUp,LozengeUp,LozengeAxis);
				pfScaleVec3(qDown,pfSqrt(SeekRadiusSquared-LozengeDown*LozengeDown),q);
				pfAddScaledVec3(RDown,qDown,LozengeDown,LozengeAxis);

#if 0				
				pfCrossVec3(UpCross,RUp,ResultNormal);   /* Why are we using the normals here??? */
				pfCrossVec3(DownCross,RDown,ResultNormal);
#else
				pfCrossVec3(UpCross,RUp,TVec);     /* WOLFY: changed 30Jul97. I think we should be using TVec instead of the normal */
				pfCrossVec3(DownCross,RDown,TVec);

#endif				

				if (PFDOT_VEC3(UpCross,DownCross) < 0.0f) { /* If they point different directions, we're between them */
					/* Case C */
					pfScaleVec3(TVec, Radius - pfSqrt(CloseDistance), TVec);
					Src[PF_X] += TVec[PF_X];
					Src[PF_Y] += TVec[PF_Y];
					Src[PF_Z] += TVec[PF_Z];
					
					PFNEGATE_VEC3(HitPointDirection,ResultNormal);

				} else {

					if (PFDOT_VEC3(LozengeAxis,ResultNormal) > 0.0f) {  /* the wall's below us */
						PFADD_VEC3(P,Src,RDown);
						PFCOPY_VEC3(HitPointDirection,RDown);
					} else {
						PFADD_VEC3(P,Src,RUp);
						PFCOPY_VEC3(HitPointDirection,RDown);
					}

					pfNormalizeVec3(HitPointDirection);

					/* More Math:
					     E=Closest point on intersected polys plane to P
						 d = distance from P to E
						 N = poly normal

						 E = P + dN
						 (E.N) = (C.N)

						 (P+dN).N = C.N

						 P.N + d(N.N) = C.N

						 d=(C.N-P.N)   */

					d = (PFDOT_VEC3(ClosePoint,ResultNormal) - PFDOT_VEC3(P,ResultNormal));
					pfAddScaledVec3(Src,Src,d,ResultNormal);

/* printf("    C: %f [%f] =%f\n",d,ResultNormal[PF_Z],d*ResultNormal[PF_Z]); */
					
				}
				
			}
		}

#endif		

		PFCOPY_VEC3(ResultClosePt,ClosePoint);
		if (CloseGSet != NULL) CollisionResultGSet = CloseGSet;
		return(TRUE);
	}

	return(FALSE);
}


Boolean LozengeCollisionSearch(pfNode *SearchNode, pfVec3 Src, float Radius,
							   pfVec3 LozengeAxis, float LozengeUp, float LozengeDown,
							   pfVec3 ResultNormal, pfVec3 HitPointDirection,
							   pfVec3 ResultClosePt)
{
	long TState;

	DIDump0("Lozenge\n");
	
	if (SearchNode==NULL) return(FALSE);
	
	LozengeCollisionSearchSetup(Src, Radius, LozengeAxis, LozengeUp, LozengeDown);

	TState=0x00;

	RecursiveSphereCollisionSearch(SearchNode,Src,TState);

	return(LozengeCollisionSearchCompletion(Src,Radius, LozengeAxis, LozengeUp, LozengeDown,
											    ResultNormal, HitPointDirection, ResultClosePt));
}



Boolean LozengeCollisionSearchList(Boolean AreGeodes, void **DataList, short *IDexes, pfVec3 Src, float Radius,
							   pfVec3 LozengeAxis, float LozengeUp, float LozengeDown,
							   pfVec3 ResultNormal, pfVec3 HitPointDirection,
							   pfVec3 ResultClosePt)
{

	long TState;
	long i;
	pfGeode *TGeode;
	pfGeoSet *TGSet;
	pfVec3  OrigPoint;
	pfVec3 SeekHigh,SeekLow;
	

	DIDump0("Lozenge List\n");
	
	if (IDexes==NULL) return(FALSE);

	LozengeCollisionSearchSetup(Src, Radius, LozengeAxis, LozengeUp, LozengeDown);

	TState=0x00;

	SeekHigh[PF_X]=SeekPoint[PF_X]+SeekRadius;
	SeekHigh[PF_Y]=SeekPoint[PF_Y]+SeekRadius;
	SeekHigh[PF_Z]=SeekPoint[PF_Z]+SeekRadius;
	SeekLow[PF_X]=SeekPoint[PF_X]-SeekRadius;
	SeekLow[PF_Y]=SeekPoint[PF_Y]-SeekRadius;
	SeekLow[PF_Z]=SeekPoint[PF_Z]-SeekRadius;
	

	i=0;

	if (AreGeodes) {
		while (IDexes[i]!=-1) {
			TGeode=((pfGeode *)(DataList[IDexes[i]]));
			SphereCollisionSearchGeode(TGeode,OrigPoint,TState);
			i++;
		}
	} else {
		while (IDexes[i]!=-1) {
			TGSet=((pfGeoSet *)(DataList[IDexes[i]]));

			IDebugIncrement(IDGSetsEncountered);
			DIDump0("GSet Encountered\n");
			
			if (TestBBox2(&(TGSet->BBox),SeekHigh,SeekLow)) {
				SearchGeoSet(TGSet);
			}
			i++;
		}
	}

	return(LozengeCollisionSearchCompletion(Src,Radius, LozengeAxis, LozengeUp, LozengeDown,
											    ResultNormal, HitPointDirection, ResultClosePt));
}





void TransformSegs(long TState, long HowMany, pfSeg *Seek, pfSeg *Orig)
{
	pfMatrix TMat;
	pfMatrix IMat;
	long i;

	if (TState & TSTATE_MATWITHTRANS) {
		IMStackGetMat(TMat);
		DIDump0("Transform Used\n");
		IDebugIncrement(IDTransformsUsed);
			
		if (TState & TSTATE_MATWITHROT) {
			pfInvertOrthoMat(IMat,TMat);
			for (i=0;i<HowMany;i++) {
				pfXformPt3(Seek[i].pos,Orig[i].pos,IMat);
				pfXformVec3(Seek[i].dir,Orig[i].dir,IMat);
				/* Seek[i].length=Orig[i].length; */ /* length should never change */
			}
		} else {
			for (i=0;i<HowMany;i++) {
				PFSUB_VEC3(Seek[i].pos,Orig[i].pos,TMat[PF_T]);
				PFCOPY_VEC3(Seek[i].dir,Orig[i].dir);
				/* Seek[i].length=Orig[i].length; */ /* length should never change */
			}
		}
	} else {
		for (i=0;i<HowMany;i++) {
			PFCOPY_VEC3(Seek[i].pos,Orig[i].pos);
			PFCOPY_VEC3(Seek[i].dir,Orig[i].dir);
			/* Seek[i].length=Orig[i].length; */ /* length should never change */
		}
	}
}




void SegCollisionSearchGeode(pfGeode *SearchGeode, pfSeg *OrigSeg, long TState)
{
	long NumSets,i;
	pfGeoSet *ThisGSet;
	pfBox BBox;
	pfBox *BBoxPtr;
	Boolean NeedToTransformMulti;
	float	rad;
	long	CullMode;
	pfMatrix TMat;
	pfVec3   SeekSegEnd;
	
	if (SearchGeode==NULL) {
		CloseDistance=-1.0f;
	} else {
		
		NeedToTransformMulti=FALSE;
		
		if (State_NewMat) {
			/* If NewMatrix then invert and calc new seekseg */
			State_NewMat=FALSE;
			if (SeekType == SEARCH_MULTISEG) NeedToTransformMulti=TRUE;

			TransformSegs(TState,1,&SeekSeg,OrigSeg);
			
		}

		
		if (SeekType == SEARCH_MULTISEG) rad = SeekMultiBoundingCylinderRadius;
		 else rad = 0;		

		pfAddScaledVec3(SeekSegEnd,SeekSeg.pos,SeekSeg.length,SeekSeg.dir);

		if (SeekSeg.dir[PF_X] < 0) {
			SeekSegMax[PF_X]=SeekSeg.pos[PF_X]+rad;
			SeekSegMin[PF_X]= SeekSegEnd[PF_X]-rad;
		} else {
			SeekSegMin[PF_X]=SeekSeg.pos[PF_X]-rad;
			SeekSegMax[PF_X]= SeekSegEnd[PF_X]+rad;
		}

		if (SeekSeg.dir[PF_Y] < 0) {
			SeekSegMax[PF_Y]=SeekSeg.pos[PF_Y]+rad;
			SeekSegMin[PF_Y]= SeekSegEnd[PF_Y]-rad;
		} else {
			SeekSegMin[PF_Y]=SeekSeg.pos[PF_Y]-rad;
			SeekSegMax[PF_Y]= SeekSegEnd[PF_Y]+rad;
		}

		if (SeekSeg.dir[PF_Z] < 0) {
			SeekSegMax[PF_Z]=SeekSeg.pos[PF_Z]+rad;
			SeekSegMin[PF_Z]= SeekSegEnd[PF_Z]-rad;
		} else {
			SeekSegMin[PF_Z]=SeekSeg.pos[PF_Z]-rad;
			SeekSegMax[PF_Z]= SeekSegEnd[PF_Z]+rad;
		}
		
		

		BBoxPtr=&(BBox);

#if USE_GEODE_BBOXES 
		ISECT_FETCH_GEODE_BBOX(SearchGeode,BBoxPtr); /* in isectnotshared.h */

		if ((BBoxPtr != NULL) && !TestSegBBox(SeekSegMax,SeekSegMin,BBoxPtr)) {
			return;
		}
#endif		
		DIDump0("Geode Searched\n");
		IDebugIncrement(IDGeodesSearched);
		
		NumSets=pfGetNumGSets(SearchGeode);
		
		for (i=0;i<NumSets;i++) {
			DIDump0("GSet Encountered\n");
			IDebugIncrement(IDGSetsEncountered);
			ISECT_FETCH_GSET_AND_BBOX(ThisGSet,SearchGeode,i,BBox,BBoxPtr); /* in isectnotshared.h */

			if (TestSegBBox(SeekSegMax,SeekSegMin,BBoxPtr)) {

				 if (SegISectBothfacesOverride) {
					 SegISectBackfaces=TRUE;
					 SegISectFrontfaces=TRUE;
				 } else {
					 CullMode=pfGetGStateMode(pfGetGSetGState(ThisGSet),PFSTATE_CULLFACE);
					 if (CullMode==PFCF_BACK || CullMode==PFCF_BOTH) SegISectBackfaces=FALSE;
					  else SegISectBackfaces=TRUE;
					 if (CullMode==PFCF_FRONT || CullMode==PFCF_BOTH) SegISectFrontfaces=FALSE;
					  else SegISectFrontfaces=TRUE;
				 }

				 if (NeedToTransformMulti) TransformSegs(TState,SeekMultiSegNum,SeekMultiSeg,SeekMultiSegOrig);
			 
				 SearchGeoSet(ThisGSet);
			 }
		}

		if (State_NewCloseData) {
			/* Calc New Close Data based on matrix */
			UntransformResults(TState);
			State_NewCloseData=0x00;
		}
	}		
}

#define SEGDCS 1


void RecursiveSegCollisionSearch(pfNode *SearchNode, pfSeg *OrigSeg, long TState)
{
	long i;
	long NodeType;
	pfMatrix TMat;
	long NumChildren;
	pfNode *TChild;

	DIDump0("NodeSearched\n");
	IDebugIncrement(IDNodesSearched);
	
	NodeType=pfGetType(SearchNode);
	if (NodeType==PFTYPE_GEODE) {
		DIDump0("Geode Encountered\n");
		IDebugIncrement(IDGeodesEncountered);
		SegCollisionSearchGeode((pfGeode *)SearchNode,OrigSeg,TState);
	} else {
		if (NodeType & PFCLASS_GROUP) {
#if SEGDCS			
			if (NodeType & PFCLASS_SCS) {
				if (NODE_NOROTATION(SearchNode)) {
					TState |= TSTATE_MATWITHTRANS;
				} else {
					TState |= (TSTATE_MATWITHTRANS | TSTATE_MATWITHROT);
				}
				  
				if (NodeType==PFTYPE_SCS) {
					pfGetSCSMat((pfSCS *)SearchNode,TMat);
				} else { /* DCS */
					pfGetDCSMat((pfDCS *)SearchNode,TMat);
				}
				
				/* PushMat */
	 			IMStackPushMultMat(TMat);
				DIDump0("Transform Pushed\n");
				IDebugIncrement(IDTransformsPushed);
				State_NewMat=TRUE;
			}
#endif
			DIDepthIncr;
			NumChildren=pfGetNumChildren(SearchNode);
			for (i=0;i<NumChildren;i++) {
				TChild=pfGetChild(SearchNode,i);
				if (TChild != NULL) {
					IDebugIncrement(IDNodesEncountered);
					DIDump3("NodeEncountered: (%x) [%x] [%x]\n",TChild->Type,TChild->DrawTravMask,TChild->ISectTravMask);
					if  (NODE_INTERSECTION_ENABLED(TChild)) {
						RecursiveSegCollisionSearch(TChild,OrigSeg,TState);
					}
				}
			}
			DIDepthDecr;
#if SEGDCS
			if (NodeType & PFCLASS_SCS) {
				IMStackPopMat();
				State_NewMat=TRUE;
			}
#endif			
		}
	}
}
			

float SegCollisionSearch(pfNode *SearchNode, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal)
{
	long i;
	long TState;

	/* IDebugReset(); */
	DIDump0("Seg\n");
	
	if (SearchNode==NULL) CloseDistance=-1.0f;
	 else {
		CloseDistance=Src->length+200.0f;

		CloseGSet=NULL;
			
		SeekSeg.length=Src->length;
		PFCOPY_VEC3(SeekSeg.dir,Src->dir);
		PFCOPY_VEC3(SeekSeg.pos,Src->pos);
		SeekType=SEARCH_SEG;
		SeekFunc=PolySegSearch;
		SeekIndexedFunc=PolyIndexSegSearch;
		
		State_NewMat=FALSE;
		State_NewCloseData=0x00;
		TState=0x0000;
		IMStackInit();
		/* Seek Seg is copied from Src */
	
		RecursiveSegCollisionSearch(SearchNode,Src,TState);
	
		if (CloseDistance > Src->length) CloseDistance = -1.0f;
		 else {
			PFCOPY_VEC3(ResultPoint,ClosePoint);
			PFCOPY_VEC3(ResultNormal,CloseNormal);
		}

	}
	if (CloseGSet != NULL) CollisionResultGSet = CloseGSet;
	/* IDebugPrint("Seg"); */
	return(CloseDistance);	
}



pfGeoSet *FindIsectGSet(pfNode *SearchNode, pfSeg *Src)
{
	long i;
	long TState;

	if (SearchNode==NULL) return(NULL);
	 else {
		CloseDistance=Src->length+200.0f;

		CloseGSet=NULL;
			
		SeekSeg.length=Src->length;
		PFCOPY_VEC3(SeekSeg.dir,Src->dir);
		PFCOPY_VEC3(SeekSeg.pos,Src->pos);
		SeekType=SEARCH_SEG;
		SeekFunc=PolySegSearch;
		SeekIndexedFunc=PolyIndexSegSearch;
		
		State_NewMat=FALSE;
		State_NewCloseData=0x00;
		TState=0x0000;
		IMStackInit();
		/* Seek Seg is copied from Src */
	
		RecursiveSegCollisionSearch(SearchNode,Src,TState);
//printf("closedist = %f\n", CloseDistance);	
		if (CloseDistance > Src->length) return(NULL);
		 else return(CloseGSet);
	}
}


float SegCollisionGSet(pfGeoSet *SearchGSet, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal)
{
	long i;
	long TState;
	long	CullMode;
	pfVec3   SeekSegEnd;

	DIDump0("SegGSet\n");
		
	/* IDebugReset(); */
	if (SearchGSet==NULL) CloseDistance=-1.0f;
	 else {
		CloseDistance=Src->length+200.0f;
			
		SeekSeg.length=Src->length;
		PFCOPY_VEC3(SeekSeg.dir,Src->dir);
		PFCOPY_VEC3(SeekSeg.pos,Src->pos);
		SeekType=SEARCH_SEG;
		SeekFunc=PolySegSearch;
		SeekIndexedFunc=PolyIndexSegSearch;

		pfAddScaledVec3(SeekSegEnd,SeekSeg.pos,SeekSeg.length,SeekSeg.dir);

		if (SeekSeg.dir[PF_X] < 0) {
			SeekSegMax[PF_X]=SeekSeg.pos[PF_X];
			SeekSegMin[PF_X]= SeekSegEnd[PF_X];
		} else {
			SeekSegMin[PF_X]=SeekSeg.pos[PF_X];
			SeekSegMax[PF_X]= SeekSegEnd[PF_X];
		}

		if (SeekSeg.dir[PF_Y] < 0) {
			SeekSegMax[PF_Y]=SeekSeg.pos[PF_Y];
			SeekSegMin[PF_Y]= SeekSegEnd[PF_Y];
		} else {
			SeekSegMin[PF_Y]=SeekSeg.pos[PF_Y];
			SeekSegMax[PF_Y]= SeekSegEnd[PF_Y];
		}

		if (SeekSeg.dir[PF_Z] < 0) {
			SeekSegMax[PF_Z]=SeekSeg.pos[PF_Z];
			SeekSegMin[PF_Z]= SeekSegEnd[PF_Z];
		} else {
			SeekSegMin[PF_Z]=SeekSeg.pos[PF_Z];
			SeekSegMax[PF_Z]= SeekSegEnd[PF_Z];
		}
	
		
		if (SegISectBothfacesOverride) {
			SegISectBackfaces=TRUE;
			SegISectFrontfaces=TRUE;
		} else {
			CullMode=pfGetGStateMode(pfGetGSetGState(SearchGSet),PFSTATE_CULLFACE);
			if (CullMode==PFCF_BACK || CullMode==PFCF_BOTH) SegISectBackfaces=FALSE;
			 else SegISectBackfaces=TRUE;
			if (CullMode==PFCF_FRONT || CullMode==PFCF_BOTH) SegISectFrontfaces=FALSE;
			 else SegISectFrontfaces=TRUE;
		}

		SearchGeoSet(SearchGSet);
		
		if (CloseDistance > Src->length) CloseDistance = -1.0f;
		 else {
			PFCOPY_VEC3(ResultPoint,ClosePoint);
			PFCOPY_VEC3(ResultNormal,CloseNormal);
		}

	}
	if (CloseGSet != NULL) CollisionResultGSet = CloseGSet;
	/* IDebugPrint("Seg GSet"); */
	return(CloseDistance);	
}



float SegCollisionList(Boolean AreGeodes, void **DataList, short *IDexes, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal)
{
	long i;
	long TState;
	long	CullMode;
	pfVec3   SeekSegEnd;
	pfSeg	*OrigSeg;
	pfGeode *TGeode;
	pfGeoSet *TGSet;
	pfBox *BBoxPtr ;
	

	DIDump0("SegList\n");
		
	/* IDebugReset(); */
	if (IDexes==NULL) CloseDistance=-1.0f;
	 else {
		CloseDistance=Src->length+200.0f;
			
		SeekSeg.length=Src->length;
		PFCOPY_VEC3(SeekSeg.dir,Src->dir);
		PFCOPY_VEC3(SeekSeg.pos,Src->pos);
		SeekType=SEARCH_SEG;
		SeekFunc=PolySegSearch;
		SeekIndexedFunc=PolyIndexSegSearch;

		pfAddScaledVec3(SeekSegEnd,SeekSeg.pos,SeekSeg.length,SeekSeg.dir);

		if (SeekSeg.dir[PF_X] < 0) {
			SeekSegMax[PF_X]=SeekSeg.pos[PF_X];
			SeekSegMin[PF_X]= SeekSegEnd[PF_X];
		} else {
			SeekSegMin[PF_X]=SeekSeg.pos[PF_X];
			SeekSegMax[PF_X]= SeekSegEnd[PF_X];
		}

		if (SeekSeg.dir[PF_Y] < 0) {
			SeekSegMax[PF_Y]=SeekSeg.pos[PF_Y];
			SeekSegMin[PF_Y]= SeekSegEnd[PF_Y];
		} else {
			SeekSegMin[PF_Y]=SeekSeg.pos[PF_Y];
			SeekSegMax[PF_Y]= SeekSegEnd[PF_Y];
		}

		if (SeekSeg.dir[PF_Z] < 0) {
			SeekSegMax[PF_Z]=SeekSeg.pos[PF_Z];
			SeekSegMin[PF_Z]= SeekSegEnd[PF_Z];
		} else {
			SeekSegMin[PF_Z]=SeekSeg.pos[PF_Z];
			SeekSegMax[PF_Z]= SeekSegEnd[PF_Z];
		}
	
		
		if (SegISectBothfacesOverride) {
			SegISectBackfaces=TRUE;
			SegISectFrontfaces=TRUE;
		}

		TState=0x0000;
		OrigSeg=NULL;


		i=0;

		if (AreGeodes) {
			while (IDexes[i]!=-1) {
				TGeode=((pfGeode *)(DataList[IDexes[i]]));
				SegCollisionSearchGeode(TGeode,OrigSeg,TState);
				i++;
			}
		} else {
			while (IDexes[i]!=-1) {
				TGSet=((pfGeoSet *)(DataList[IDexes[i]]));

				DIDump0("GSet Encountered\n");
				IDebugIncrement(IDGSetsEncountered);


				BBoxPtr=&(TGSet->BBox);
				if (TestSegBBox(SeekSegMax,SeekSegMin,BBoxPtr)) {
				
					if (!SegISectBothfacesOverride) {
						CullMode=pfGetGStateMode(pfGetGSetGState(TGSet),PFSTATE_CULLFACE);
						if (CullMode==PFCF_BACK || CullMode==PFCF_BOTH) SegISectBackfaces=FALSE;
						 else SegISectBackfaces=TRUE;
						if (CullMode==PFCF_FRONT || CullMode==PFCF_BOTH) SegISectFrontfaces=FALSE;
						 else SegISectFrontfaces=TRUE;
					}
					SearchGeoSet(TGSet);
				}
				i++;
			}
		}

		
		if (CloseDistance > Src->length) CloseDistance = -1.0f;
		 else {
			PFCOPY_VEC3(ResultPoint,ClosePoint);
			PFCOPY_VEC3(ResultNormal,CloseNormal);
		}

	}
	if (CloseGSet != NULL) CollisionResultGSet = CloseGSet;

	return(CloseDistance);	
}










	
void MultiSegCollisionSearch(pfNode *SearchNode, pfSeg *SrcArray, long HowManySegs, pfSeg *CylinderSeg, float CylinderRadius, float *ResultDists, pfVec3 *ResultPoints, pfVec3 *ResultNormals)
{
	long TState;
	long i;

	/* IDebugReset(); */
	
	if (SearchNode==NULL) {
		for (i = 0; i < HowManySegs; i++) {
			ResultDists[i] = -1.0f;
		}
	} else {

		if (HowManySegs > MAXMULTISEGS) HowManySegs = MAXMULTISEGS;
		
		CloseGSet=NULL;
		
		SeekMultiSegOrig = SrcArray;
		
		SeekMultiSegNum = HowManySegs;
		SeekMultiBoundingCylinderSeg = CylinderSeg;
		SeekMultiBoundingCylinderRadius = CylinderRadius;
		SeekMultiResultDists = ResultDists;
		SeekMultiResultPoints = ResultPoints;
		SeekMultiResultNormals = ResultNormals;
		
		for (i = 0; i < SeekMultiSegNum; i++) {
			SeekMultiResultDists[i] = SrcArray[i].length+200.0f;
			
			PFCOPY_VEC3(SeekMultiSeg[i].dir,SrcArray[i].dir);
			PFCOPY_VEC3(SeekMultiSeg[i].pos,SrcArray[i].pos);
			SeekMultiSeg[i].length=SrcArray[i].length;
		}
		
		SeekSeg.length=CylinderSeg->length;
		PFCOPY_VEC3(SeekSeg.dir,CylinderSeg->dir);
		PFCOPY_VEC3(SeekSeg.pos,CylinderSeg->pos);
		SeekType=SEARCH_MULTISEG;
		SeekFunc=PolyMultiSegSearch;
		SeekIndexedFunc=PolyIndexMultiSegSearch;

		State_NewMat=FALSE;
		State_NewCloseData=0x00;
		TState=0x0000;
		IMStackInit();
		/* Seek Seg is copied from Src */
		
		RecursiveSegCollisionSearch(SearchNode, CylinderSeg, TState);

		for (i = 0; i < SeekMultiSegNum; i++) {
			if (SeekMultiResultDists[i] > SrcArray[i].length) SeekMultiResultDists[i] = -1.0f;
		}
		
	}
	/* IDebugPrint("MultiSeg"); */
}
	
	



/* Rotational shields used for troopers */

float SegIntersectsShield(pfSeg *TheSeg, pfVec3 shieldPos, float shieldArm, float shieldWidth, float shieldMinZ, float shieldMaxZ, pfVec3 Dst)
{
#if 0
	pfPlane	plane;
	pfVec3	pos, norm, hit;
	float	result, dist;

	/* Old (as of 22Sep96) */

#if 0	
	PFSCALE_VEC3(pos, -shieldArm, TheSeg->dir);
#else
	PFSCALE_VEC2(pos, -shieldArm, TheSeg->dir);
	pos[PF_Z]=0.0f;
#endif	
	
	PFADD_VEC3(pos, shieldPos, pos);
	PFSCALE_VEC3(norm, -1.0, TheSeg->dir);
	pfMakeNormPtPlane(&plane, norm, pos);
	
	result = SegIntersectsPlane(&plane, TheSeg, Dst);
	if (result < 0.0) return(-1.0);
	if (Dst[PF_Z] < shieldMinZ) return(-1.0);
	if (Dst[PF_Z] > shieldMaxZ) return(-1.0);
	
	dist = PFSQR_DISTANCE_PT2(pos, Dst);
	if (dist > shieldWidth * shieldWidth) return(-1.0);
	
	return(result);
#else

	
	pfPlane	plane;
	pfVec3	pos, norm, hit, OffsetDir;
	float	result, dist;

	PFCOPY_VEC3(OffsetDir,TheSeg->dir);
	OffsetDir[PF_Z]=0.0f;
	pfNormalizeVec3(OffsetDir);
	PFSCALE_VEC3(norm, -1.0, OffsetDir);

	PFSCALE_VEC3(OffsetDir, -shieldArm, OffsetDir);
	
	PFADD_VEC3(pos, shieldPos, OffsetDir);
	
	pfMakeNormPtPlane(&plane, norm, pos);
	
	result = SegIntersectsPlane(&plane, TheSeg, Dst);

	if (result > 0.0f) {
		if (Dst[PF_Z] >= shieldMinZ &&  Dst[PF_Z] <= shieldMaxZ) {
			dist = PFSQR_DISTANCE_PT2(pos, Dst);
			if (dist < shieldWidth * shieldWidth) {
				return(result);
			}
		}
	}

	if (TheSeg->dir[PF_Z] > 0.0f) {  /* It goes up */
		PFCOPY_VEC3(pos,shieldPos);
		pos[PF_Z]=shieldMinZ;
		PFSET_VEC3(norm,0.0f,0.0f,-1.0f);
		pfMakeNormPtPlane(&plane, norm, pos);
		
		result = SegIntersectsPlane(&plane, TheSeg, Dst);

		if (result > 0.0f) {
			dist = PFSQR_DISTANCE_PT2(pos, Dst);
			if (dist < shieldArm * shieldArm) {
				return(result);
			}
		}
	} else { /* it goes down */
	
		pos[PF_Z]=shieldMaxZ;
		PFSET_VEC3(norm,0.0f,0.0f,1.0f);
		pfMakeNormPtPlane(&plane, norm, pos);
	
		result = SegIntersectsPlane(&plane, TheSeg, Dst);

		if (result > 0.0f) {
			dist = PFSQR_DISTANCE_PT2(pos, Dst);
			if (dist < shieldArm * shieldArm) {
				return(result);
			}
		}
	}

	return(-1.0f);


#endif
	
}






/***************************************************************************************\

    ISect Matrix Stack Stuff

\***************************************************************************************/


static short IMStackSize = 0;
#define MAX_IMAT_STACK	32


pfMatrix IMatrixStack[MAX_IMAT_STACK+1];
long	IMStackDepth=0;

void IMStackInit(void)
{
	IMStackDepth=0;
	PFMAKE_IDENT_MAT(IMatrixStack[0]);
}


void IMStackPushMat(pfMatrix TMat)
{
	if (IMStackDepth < MAX_IMAT_STACK) {
		IMStackDepth++;

		PFCOPY_VEC4(IMatrixStack[IMStackDepth][0],TMat[0]);
		PFCOPY_VEC4(IMatrixStack[IMStackDepth][1],TMat[1]);
		PFCOPY_VEC4(IMatrixStack[IMStackDepth][2],TMat[2]);
		PFCOPY_VEC4(IMatrixStack[IMStackDepth][3],TMat[3]);
	}
}

void IMStackPushMultMat(pfMatrix TMat)
{
	pfMatrix *dst,*aa,*bb;
	
	if (IMStackDepth < MAX_IMAT_STACK) {
		IMStackDepth++;

		dst = &(IMatrixStack[IMStackDepth]);
		aa = &(IMatrixStack[IMStackDepth-1]);

		(*dst)[0][0]=(*aa)[0][0]*(TMat)[0][0]+(*aa)[1][0]*(TMat)[0][1]+(*aa)[2][0]*(TMat)[0][2]+(*aa)[3][0]*(TMat)[0][3];
		(*dst)[0][1]=(*aa)[0][1]*(TMat)[0][0]+(*aa)[1][1]*(TMat)[0][1]+(*aa)[2][1]*(TMat)[0][2]+(*aa)[3][1]*(TMat)[0][3];
		(*dst)[0][2]=(*aa)[0][2]*(TMat)[0][0]+(*aa)[1][2]*(TMat)[0][1]+(*aa)[2][2]*(TMat)[0][2]+(*aa)[3][2]*(TMat)[0][3];
		(*dst)[0][3]=(*aa)[0][3]*(TMat)[0][0]+(*aa)[1][3]*(TMat)[0][1]+(*aa)[2][3]*(TMat)[0][2]+(*aa)[3][3]*(TMat)[0][3];
		(*dst)[1][0]=(*aa)[0][0]*(TMat)[1][0]+(*aa)[1][0]*(TMat)[1][1]+(*aa)[2][0]*(TMat)[1][2]+(*aa)[3][0]*(TMat)[1][3];
		(*dst)[1][1]=(*aa)[0][1]*(TMat)[1][0]+(*aa)[1][1]*(TMat)[1][1]+(*aa)[2][1]*(TMat)[1][2]+(*aa)[3][1]*(TMat)[1][3];
		(*dst)[1][2]=(*aa)[0][2]*(TMat)[1][0]+(*aa)[1][2]*(TMat)[1][1]+(*aa)[2][2]*(TMat)[1][2]+(*aa)[3][2]*(TMat)[1][3];
		(*dst)[1][3]=(*aa)[0][3]*(TMat)[1][0]+(*aa)[1][3]*(TMat)[1][1]+(*aa)[2][3]*(TMat)[1][2]+(*aa)[3][3]*(TMat)[1][3];
		(*dst)[2][0]=(*aa)[0][0]*(TMat)[2][0]+(*aa)[1][0]*(TMat)[2][1]+(*aa)[2][0]*(TMat)[2][2]+(*aa)[3][0]*(TMat)[2][3];
		(*dst)[2][1]=(*aa)[0][1]*(TMat)[2][0]+(*aa)[1][1]*(TMat)[2][1]+(*aa)[2][1]*(TMat)[2][2]+(*aa)[3][1]*(TMat)[2][3];
		(*dst)[2][2]=(*aa)[0][2]*(TMat)[2][0]+(*aa)[1][2]*(TMat)[2][1]+(*aa)[2][2]*(TMat)[2][2]+(*aa)[3][2]*(TMat)[2][3];
		(*dst)[2][3]=(*aa)[0][3]*(TMat)[2][0]+(*aa)[1][3]*(TMat)[2][1]+(*aa)[2][3]*(TMat)[2][2]+(*aa)[3][3]*(TMat)[2][3];
		(*dst)[3][0]=(*aa)[0][0]*(TMat)[3][0]+(*aa)[1][0]*(TMat)[3][1]+(*aa)[2][0]*(TMat)[3][2]+(*aa)[3][0]*(TMat)[3][3];
		(*dst)[3][1]=(*aa)[0][1]*(TMat)[3][0]+(*aa)[1][1]*(TMat)[3][1]+(*aa)[2][1]*(TMat)[3][2]+(*aa)[3][1]*(TMat)[3][3];
		(*dst)[3][2]=(*aa)[0][2]*(TMat)[3][0]+(*aa)[1][2]*(TMat)[3][1]+(*aa)[2][2]*(TMat)[3][2]+(*aa)[3][2]*(TMat)[3][3];
		(*dst)[3][3]=(*aa)[0][3]*(TMat)[3][0]+(*aa)[1][3]*(TMat)[3][1]+(*aa)[2][3]*(TMat)[3][2]+(*aa)[3][3]*(TMat)[3][3];

	}
}

void IMStackGetMat(pfMatrix TMat)
{
	PFCOPY_VEC4(TMat[0],IMatrixStack[IMStackDepth][0]);
	PFCOPY_VEC4(TMat[1],IMatrixStack[IMStackDepth][1]);
	PFCOPY_VEC4(TMat[2],IMatrixStack[IMStackDepth][2]);
	PFCOPY_VEC4(TMat[3],IMatrixStack[IMStackDepth][3]);
}

void IMStackPopMat(void)
{
	if (IMStackDepth > 0) IMStackDepth--;
}




void ISectTravMask(long _trav, long _mask)
{
	if (_trav==WOLFPFCHANTRAV_ISECTANY) {
		ISectTravMaskAny=_mask;
	}
	if (_trav==WOLFPFCHANTRAV_ISECTALL) {
		ISectTravMaskAll=_mask;
	} 
}

long GetISectTravMask(long _trav)
{
	if (_trav==WOLFPFCHANTRAV_ISECTANY) {
		return(ISectTravMaskAny);
	} else if (_trav==WOLFPFCHANTRAV_ISECTALL) {
		return(ISectTravMaskAll);
	} else {
		return(-1);
	}
}


#endif
