
#define ZMODE_ANY 0
#define ZMODE_UP 1
#define ZMODE_DOWN 2
#define ZMODE_CLOSEST 3

extern Boolean SphereISectBackfaces;


void ClearCollisionResultGSet(void);
pfGeoSet *GetCollisionResultGSet(void);

void SetSegISectBothFaceOverride(Boolean Val);

float SegIntersectsShield(pfSeg *TheSeg, pfVec3 shieldPos, float shieldArm, float shieldWidth, float shieldMinZ, float shieldMaxZ, pfVec3 Dst);
short TwoSphereCollisionSearch(pfNode *SearchNode, pfVec3 Src, float Radius2, float Radius1, Boolean DoZ);
float SegIntersectsPlane(pfPlane *ThePlane, pfSeg *TheSeg, pfVec3 Dst);

Boolean SimpleSegHitTri(pfPlane *ThePlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfSeg *ThisSeg);



float   ZCollisionSearch(pfNode *ThisNode, pfVec3 Src, short ZMode);
Boolean SphereCollisionSearch(pfNode *SearchNode, pfVec3 Src, float Radius, Boolean DoZ);
float   SegCollisionSearch(pfNode *SearchNode, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal);
void    MultiSegCollisionSearch(pfNode *SearchNode, pfSeg *SrcArray, long HowManySegs, pfSeg *CylinderSeg,
							 float CylinderRadius, float *ResultDists, pfVec3 *ResultPoints, pfVec3 *ResultNormals);
Boolean LozengeCollisionSearch(pfNode *SearchNode, pfVec3 Src, float Radius, pfVec3 LozengeAxis, float LozengeUp,
							   float LozengeDown, pfVec3 ResultNormal, pfVec3 HitPointDirection, pfVec3 ResultClosePt);

pfGeoSet *FindIsectGSet(pfNode *SearchNode, pfSeg *Src);
float SegCollisionGSet(pfGeoSet *SearchGSet, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal);

Boolean LozengeCollisionSearchList(Boolean AreGeodes, void **DataList, short *IDexes, pfVec3 Src, float Radius,
							   pfVec3 LozengeAxis, float LozengeUp, float LozengeDown,
							   pfVec3 ResultNormal, pfVec3 HitPointDirection,
							   pfVec3 ResultClosePt);
float SegCollisionList(Boolean AreGeodes, void **DataList, short *IDexes, pfSeg *Src, pfVec3 ResultPoint, pfVec3 ResultNormal);



void ISectTravMask(long _trav, long _mask);
long GetISectTravMask(long _trav);
