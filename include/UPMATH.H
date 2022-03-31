
void XformVec3To4(pfVec4 _dst, const pfVec3 _v, const pfMatrix _m); /* In u64 coord space, which has the matrix sideways */
void SpecialXformVec3To4(pfVec4 _dst, const pfVec3 _v, const pfMatrix _m); /* In u64 coord space, which has the matrix sideways */

void DecomposeMatrix(pfMatrix TransMat, pfVec3 Translation, pfMatrix Rotation, pfVec3 Scale);
void ComposeMatrix(pfMatrix TransMat, pfVec3 Translation, pfMatrix Rotation, pfVec3 Scale);





void  foxClosestPtOnPlane(pfPlane *ThePlane, pfVec3 Src, pfVec3 Dst);
void  foxClosestPtOnSeg(pfVec3 Src, pfVec3 P1, pfVec3 P2, pfVec3 Result);
float foxClosestPtInTri(pfVec3 Src, pfVec3 C1, pfVec3 C2, pfVec3 C3, float DistSqMax, pfVec3 Result, pfVec3 ResultNormal);
float DistanceToPlane(pfPlane *ThePlane, pfVec3 Src);
float ZPtOnPlane(pfPlane *ThePlane, pfVec3 Src);
Boolean ZPointInQuad(pfVec3 Pt, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 C4);
Boolean ZPointInTriangle(pfVec3 Pt, pfVec3 C1, pfVec3 C2, pfVec3 C3);

void uMakePointNormalMat(pfMatrix Dst, pfVec3 P1, pfVec3 N1, float Offset);
void uMakePointNormalMatWithXVector(pfMatrix Dst, pfVec3 P1, pfVec3 N1, pfVec3 X1, float Offset);
void DoLookAt(pfVec3 Eye, pfVec3 Targ, pfMatrix Result, pfCoord *TempCoord, float Roll);

