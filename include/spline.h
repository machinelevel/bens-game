/*
 *  Spline objects
 */


#ifndef SPLINE_H
#define SPLINE_H




#define MAXSPLINES 		512
#define MAXSPLBRANCHES	2
#define MAXSPLBACKBRANCHES	4
#define MAXSPLIDLIST	8  /* should be MAXSPLBRANCHES ^ 4 */


#define SPLGETPOINT 0x01
#define SPLGETTAN 0x02
#define SPLGETSLOPE 0x04
#define SPLGETNORMAL 0x08
#define SPLGETBASIC (SPLGETPOINT | SPLGETTAN | SPLGETNORMAL)

#define SPLFORWARD 1
#define SPLBACKWARD 2

#define SPL_SPLINE 0
#define SPL_BEZIER 1


typedef struct {
	pfVec3 RetPoint;
	pfVec3 RetTan;
	pfVec3 RetSlope;
	pfVec3 RetNormal;
} SplineReturn;



typedef struct {
	short	NumNexts;
	short	NumPrevs;
	short	NextSpline[MAXSPLBRANCHES];
	short	PrevSpline[MAXSPLBACKBRANCHES];
	pfVec3	Loc;
	pfVec3	Norm;
	pfVec3  InVec;
	pfVec3  OutVec;
	short	Order;
	short	OtherIDs[MAXSPLIDLIST];
} Spline;
typedef Spline* SplinePtr;

typedef struct {
	short   Type; /* SPLINE or BEZIER */
	long    NumSplines;       /* The number of real knots (the size of the array) */
	long    NumVirtualKnots;  /* The number of knots including the branch ones which are merely alias numbers */
	Spline  *SplineList;
} SplineSet;


void GetSplineInfo(SplineSet *SData, long ReturnFlags, float UVal, long SI[4], SplineReturn *Results);










typedef struct {
	SplineSet *SData;
	float	SplineFactor;
	float	SplineTime;
	float	Speed;
	long	SplineIndex[4];
	Boolean AtSplineEnd,AtSplineStart;
	long	DesiredSwitch;
	long    CurrentIDSetting;
} SplinedObj;
typedef SplinedObj* SplinedObjPtr;



void FetchSplinePathForward(SplinedObjPtr ThisSO, pfMatrix TransMat, float Offset);
void FetchSplinePath(SplinedObjPtr ThisSO, pfMatrix TransMat);
void GetKnotLoc(SplineSet *SData, long i, pfVec3 Loc);
long GetKnotOrder(SplineSet *SData, long i);
long GetNthKnot(SplineSet *SData, long i, long StartingAt);
long GetSplineObjKnotID(SplinedObj *ThisSpline, long WhichIndex);
void GetSplineMat(SplineSet *SData, pfMatrix TMat, long Knot0, long Knot1, long Knot2, long Knot3, float Time);
void GetSplineMatOneKnot(SplineSet *SData, pfMatrix TMat, long Knot0, float Time);
void SetStartingKnots(SplinedObjPtr ThisSO, long StartKnot);


SplinedObj *BuildBasicSplinePath(pfVec3 *knotList, uint32 numKnots, uint32 flags);
void SmoothBasicSplinePath(SplinedObj *sobj);
void DrawBasicSplinePath(SplinedObj *sobj);
void DrawSplineTube(SplinedObj *sobj, float radius, float samplesPerKnot, int32 tubeSides, float uOffset, float vOffset, float firstKnot, float numKnots, uint32 flags);


#endif
