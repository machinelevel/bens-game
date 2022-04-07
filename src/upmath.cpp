
/*
 */


#include "genincludes.h"




void XformVec3To4(pfVec4 _dst, const pfVec3 _v, const pfMatrix _m) /* In u64 coord space, which has the matrix sideways */
{
	_dst[0] = (_m)[0][0] * (_v)[0] + (_m)[1][0] * (_v)[1] + (_m)[2][0] * (_v)[2] + (_m)[3][0];
	_dst[1] = (_m)[0][1] * (_v)[0] + (_m)[1][1] * (_v)[1] + (_m)[2][1] * (_v)[2] + (_m)[3][1];
	_dst[2] = (_m)[0][2] * (_v)[0] + (_m)[1][2] * (_v)[1] + (_m)[2][2] * (_v)[2] + (_m)[3][2];
	_dst[3] = (_m)[0][3] * (_v)[0] + (_m)[1][3] * (_v)[1] + (_m)[2][3] * (_v)[2] + (_m)[3][3];
}

void SpecialXformVec3To4(pfVec4 _dst, const pfVec3 _v, const pfMatrix _m) /* In u64 coord space, which has the matrix sideways */
{
	_dst[0] = (_m)[0][0] * (_v)[0] + (_m)[0][1] * (_v)[1] + (_m)[0][2] * (_v)[2];
	_dst[1] = (_m)[1][0] * (_v)[0] + (_m)[1][1] * (_v)[1] + (_m)[1][2] * (_v)[2];
	_dst[2] = (_m)[2][0] * (_v)[0] + (_m)[2][1] * (_v)[1] + (_m)[2][2] * (_v)[2];
	_dst[3] = 1.0f;
}





void DecomposeMatrix(pfMatrix TransMat, pfVec3 Translation, pfMatrix Rotation, pfVec3 Scale)
{
	float TLength;

	TLength = pfLengthVec3(TransMat[0]);
	Scale[PF_X]=TLength;
	PFSCALE_VEC3(Rotation[0],1.0f/TLength,TransMat[0]);
	Rotation[0][3]=0.0f;
	
	TLength = pfLengthVec3(TransMat[1]);
	Scale[PF_Y]=TLength;
	PFSCALE_VEC3(Rotation[1],1.0f/TLength,TransMat[1]);
	Rotation[1][3]=0.0f;
	
	TLength = pfLengthVec3(TransMat[2]);
	Scale[PF_Z]=TLength;
	PFSCALE_VEC3(Rotation[2],1.0f/TLength,TransMat[2]);
	Rotation[2][3]=0.0f;

	Translation[PF_X]=TransMat[PF_T][PF_X];
	Translation[PF_Y]=TransMat[PF_T][PF_Y];
	Translation[PF_Z]=TransMat[PF_T][PF_Z];
	
	Rotation[3][0]=0.0f;
	Rotation[3][1]=0.0f;
	Rotation[3][2]=0.0f;
	Rotation[3][3]=1.0f;
}

void ComposeMatrix(pfMatrix TransMat, pfVec3 Translation, pfMatrix Rotation, pfVec3 Scale)
{

/*	pfPostTransMat(TransMat,Rotation,Translation[PF_X],Translation[PF_Y],Translation[PF_Z]); */
	pfCopyMat(TransMat, Rotation);
	pfAddVec3(TransMat[PF_T], TransMat[PF_T], Translation);
	
	pfPreScaleMat(TransMat,Scale[PF_X],Scale[PF_Y],Scale[PF_Z],TransMat); 
}









void foxClosestPtOnPlane(pfPlane *ThePlane, pfVec3 Src, pfVec3 Dst)
{
	float k,NDotS;
	
	NDotS=PFDOT_VEC3(Src,ThePlane->normal);
	k=(ThePlane->offset)-NDotS;
	
	PFSCALE_VEC3(Dst,k,ThePlane->normal);
	PFADD_VEC3(Dst,Src,Dst);	
	
}


void foxClosestPtOnSeg(pfVec3 Src, pfVec3 P1, pfVec3 P2, pfVec3 Result)
{
	float u;
	pfVec3 Diff;
	float DotSD,DotPD,DotDD;
	
	PFSUB_VEC3(Diff,P2,P1);
	DotSD=PFDOT_VEC3(Src,Diff);
	DotPD=PFDOT_VEC3(P1,Diff);
	DotDD=PFDOT_VEC3(Diff,Diff);
	if (DotDD<=0.001) {
		PFCOPY_VEC3(Result,P1);
		return;
	}
	u=(DotSD-DotPD)/DotDD;
	if (u<=0.0f) {
		PFCOPY_VEC3(Result,P1);
		return;
	}
	if (u>=1.0f) {
		PFCOPY_VEC3(Result,P2);
		return;
	}
	
	PFSCALE_VEC3(Result,u,Diff);
	PFADD_VEC3(Result,Result,P1);
}


Boolean foxPlanePointInTriangle(pfVec3 PtInPlane, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 C12, pfVec3 C23, pfVec3 C31)
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


/* Return distance squared, limit searches to those within DistSqMax (use -1 to check always) */
float foxClosestPtInTri(pfVec3 Src, pfVec3 C1, pfVec3 C2, pfVec3 C3, float DistSqMax, pfVec3 Result, pfVec3 ResultNormal)
{
	pfVec3 C12,C23,C31,Diff;
	pfVec3 PtInPlane;
	pfPlane ThePlane;

	float	TempDistance;
	float Distance;
	Boolean ReverseNormal;

	pfMakePtsPlane(&ThePlane,C1,C2,C3);

	PFSUB_VEC3(Diff,Src,C1);
	if (PFDOT_VEC3(Diff,ThePlane.normal)<0) {		  /* Plane is facing away from us */
		ReverseNormal=TRUE;
	} else {
		ReverseNormal=FALSE;
	}
	
	foxClosestPtOnPlane(&ThePlane,Src,Result); 
	Distance=(PFSQR_DISTANCE_PT3(Src,Result));

	if (DistSqMax > 0.0f && Distance > DistSqMax) {
		/* It can't be closer so don't do any more checking */
		return(-1.0f);
	}

	PFSUB_VEC3(C12,C2,C1);
	PFSUB_VEC3(C23,C3,C2);
	PFSUB_VEC3(C31,C1,C3); 
		
	if (!foxPlanePointInTriangle(Result,C1,C2,C3,C12,C23,C31)) {
		foxClosestPtOnSeg(Result,C1,C2,C12);
		foxClosestPtOnSeg(Result,C2,C3,C23);
		foxClosestPtOnSeg(Result,C3,C1,C31);
			
		Distance=(PFSQR_DISTANCE_PT3(Src,C12));

		PFCOPY_VEC3(Result,C12);
		
		TempDistance=(PFSQR_DISTANCE_PT3(Src,C23));
		if (TempDistance < Distance) {
			PFCOPY_VEC3(Result,C23);
			Distance=TempDistance;
		}
		TempDistance=(PFSQR_DISTANCE_PT3(Src,C31));
		if (TempDistance < Distance) {
			PFCOPY_VEC3(Result,C31);
			Distance=TempDistance;
		}
	}
	if (ReverseNormal) {
		PFNEGATE_VEC3(ResultNormal,ThePlane.normal);
	} else {
		PFCOPY_VEC3(ResultNormal,ThePlane.normal);
	}
	return(Distance);
}






float DistanceToPlane(pfPlane *ThePlane, pfVec3 Src)
{
	float NDotS;
	
	NDotS=PFDOT_VEC3(Src,ThePlane->normal);

	return((ThePlane->offset)-NDotS);
}



float ZPtOnPlane(pfPlane *ThePlane, pfVec3 Src)
{
	float k,NDotS;
	
	NDotS=PFDOT_VEC3(Src,ThePlane->normal);
	k=((ThePlane->offset)-NDotS)/ThePlane->normal[PF_Z];

	return(Src[PF_Z]+k);
}

float CrossNoZ(pfVec3 V1, pfVec3 V2)
{
	return (V1[PF_X]*V2[PF_Y] - V1[PF_Y] * V2[PF_X]);
}

Boolean ZPointInTriangle(pfVec3 Pt, pfVec3 C1, pfVec3 C2, pfVec3 C3)
{

	pfVec3 C12,C23,C31;
	pfVec3 Temp1,Temp2,Temp3;
	float X1,X2,X3;

	PFSUB_VEC3(C12,C2,C1);
	PFSUB_VEC3(C23,C3,C2);
	PFSUB_VEC3(C31,C1,C3);

	PFSUB_VEC3(Temp1,C1,Pt);
	PFSUB_VEC3(Temp2,C2,Pt);
	PFSUB_VEC3(Temp3,C3,Pt);

	X1=CrossNoZ(C12,Temp1);	
	X2=CrossNoZ(C23,Temp2);	
	X3=CrossNoZ(C31,Temp3);

	if (X1 > 0.0f) {
		if (X2 >= 0.0f && X3 >= 0.0f) return (TRUE);
		 else return (FALSE);
	} else if (X1 < 0.0f) {
		if (X2 <= 0.0f && X3 <= 0.0f) return (TRUE);
		 else return (FALSE);
	} else {	
		if (X2 > 0.0f) {
			if (X3 >= 0.0f) return (TRUE);
			 else return (FALSE);
		} else if (X2 < 0.0f) {
			if (X3 <= 0.0f) return (TRUE);
			 else return (FALSE);
		} else {
			return (TRUE);
		}
	}
}


Boolean ZPointInQuad(pfVec3 Pt, pfVec3 C1, pfVec3 C2, pfVec3 C3, pfVec3 C4)
{

#if 1  /* if we have to deal with concave polys */


	return (ZPointInTriangle(Pt, C1, C2, C4) || ZPointInTriangle(Pt, C2, C3, C4));

	
	
#else  /* if we can guarantee convex polys */
	

	pfVec3 C12,C23,C34,C41;
	pfVec3 Temp1,Temp2,Temp3,Temp4;
	float X1,X2,X3,X4;

	PFSUB_VEC3(C12,C2,C1);
	PFSUB_VEC3(C23,C3,C2);
	PFSUB_VEC3(C34,C4,C3);
	PFSUB_VEC3(C41,C1,C4);

	PFSUB_VEC3(Temp1,C1,Pt);
	PFSUB_VEC3(Temp2,C2,Pt);
	PFSUB_VEC3(Temp3,C3,Pt);
	PFSUB_VEC3(Temp4,C4,Pt);

	X1=CrossNoZ(C12,Temp1);	
	X2=CrossNoZ(C23,Temp2);	
	X3=CrossNoZ(C34,Temp3);
	X4=CrossNoZ(C41,Temp4);

	if (X1 > 0.0f) {
		if (X2 >= 0.0f && X3 >= 0.0f && X4 >= 0.0f) return (TRUE);
		 else return (FALSE);
	} else if (X1 < 0.0f) {
		if (X2 <= 0.0f && X3 <= 0.0f && X4 <= 0.0f) return (TRUE);
		 else return (FALSE);
	} else {	
		if (X2 > 0.0f) {
			if (X3 >= 0.0f && X4 >= 0.0f) return (TRUE);
			 else return (FALSE);
		} else if (X2 < 0.0f) {
			if (X3 <= 0.0f && X4 <= 0.0f) return (TRUE);
			 else return (FALSE);
		} else {
			return (TRUE);
		}
	}
#endif
}






void uMakePointNormalMat(pfMatrix Dst, pfVec3 P1, pfVec3 N1, float Offset)
{
	Dst[3][0]=0.0f;
	Dst[3][1]=0.0f;
	Dst[3][2]=0.0f;
	Dst[3][3]=1.0f;
	PFADD_SCALED_VEC3(Dst[PF_T],P1,Offset,N1);
	if (N1[PF_Z] >= 0.9f || N1[PF_Z] <= -0.9f) {
		PFCOPY_VEC3(Dst[PF_Y],N1);
		PFSET_VEC3(Dst[PF_X],0.0f,-N1[PF_Z],N1[PF_Y]);
		pfNormalizeVec3(Dst[PF_X]);
		pfCrossVec3(Dst[PF_Z],Dst[PF_X],Dst[PF_Y]);
	} else {
		PFCOPY_VEC3(Dst[PF_Y],N1);
		PFSET_VEC3(Dst[PF_X],-N1[PF_Y],N1[PF_X],0.0f);
		pfNormalizeVec3(Dst[PF_X]);
		pfCrossVec3(Dst[PF_Z],Dst[PF_X],Dst[PF_Y]);
	}
}

void uMakePointNormalMatWithXVector(pfMatrix Dst, pfVec3 P1, pfVec3 N1, pfVec3 X1, float Offset)
{
	float	dot;
	
	Dst[3][0]=0.0f;
	Dst[3][1]=0.0f;
	Dst[3][2]=0.0f;
	Dst[3][3]=1.0f;
	PFADD_SCALED_VEC3(Dst[PF_T],P1,Offset,N1);
	
	dot = PFDOT_VEC3(N1, X1);
	
	if (dot >= 0.95f || dot <= -0.95f) {
		/**** This is the exception case, and probably needs to be fixed. ****/
		PFSCALE_VEC3(Dst[PF_Y],-1.0f,N1); /**** need to turn it around ****/
		PFSET_VEC3(Dst[PF_X],0.0f,0.0f,1.0f);
		pfCrossVec3(Dst[PF_Z],Dst[PF_X],Dst[PF_Y]);
		pfCrossVec3(Dst[PF_X],Dst[PF_Y],Dst[PF_Z]);
		pfNormalizeVec3(Dst[PF_X]);
		pfNormalizeVec3(Dst[PF_Z]);
	} else {
		PFSCALE_VEC3(Dst[PF_Y],-1.0f,N1); /**** need to turn it around ****/
		PFCOPY_VEC3(Dst[PF_X], X1);
		pfCrossVec3(Dst[PF_Z],Dst[PF_X],Dst[PF_Y]);
		pfCrossVec3(Dst[PF_X],Dst[PF_Y],Dst[PF_Z]);
		pfNormalizeVec3(Dst[PF_X]);
		pfNormalizeVec3(Dst[PF_Z]);
	}
}

void DoLookAt(pfVec3 Eye, pfVec3 Targ, pfMatrix Result, pfCoord *TempCoord, float Roll)
{
	float Length;
	pfVec3 Diff,Right,Up;
	
#if 1
	PFCOPY_VEC3(TempCoord->xyz,Eye);
	PFSUB_VEC3(Diff,Targ,Eye);
				
	Length=PFLENGTH_VEC3(Diff);
	if (Length > 0.0001f) {
		pfScaleVec3(Diff,1.0f/Length,Diff);
				
		TempCoord->hpr[PF_P]= pfArcSin(Diff[PF_Z]);
		TempCoord->hpr[PF_R]=Roll;

		if (Diff[PF_Y] < 0.0001 && Diff[PF_Y]> -0.0001) {
			if (Diff[PF_X]<-0.0001) TempCoord->hpr[PF_H]=90.0f;
			 else if (Diff[PF_X]>0.0001) TempCoord->hpr[PF_H]=-90.0f;
			 else TempCoord->hpr[PF_H]=0.0f;  /* Pointed Up */				
		} else TempCoord->hpr[PF_H]=pfArcTan2(-Diff[PF_X],Diff[PF_Y]);
		pfMakeCoordMat(Result,TempCoord);
	} else {
		pfMakeTransMat(Result,Eye[PF_X],Eye[PF_Y],Eye[PF_Z]);
	}
#else
	PFSUB_VEC3(Diff,Targ,Eye);  /* Our Forward Vector */
	PFSET_VEC3(Right,Diff[PF_Y],-Diff[PF_X],0.0f);  /* Right=Forward X (0,0,1) */
	pfCrossVec3(Up,Right,Diff);  /* Up=Right X Forward */
	pfNormalizeVec3(Diff);
	pfNormalizeVec3(Right);
	pfNormalizeVec3(Up);
	pfSetMatRowVec3(Result,PF_X,Right);
	pfSetMatRowVec3(Result,PF_Y,Diff);
	pfSetMatRowVec3(Result,PF_Z,Up);
	pfSetMatRowVec3(Result,PF_T,Eye);
	Result[0][3]=0.0f;
	Result[1][3]=0.0f;
	Result[2][3]=0.0f;
	Result[3][3]=1.0f;
	pfGetOrthoMatCoord(Result,TempCoord);
#endif
		
}

