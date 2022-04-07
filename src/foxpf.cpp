#define INFOXPF

#include <math.h>

#include "genincludes.h"
#include "types.h"

#include "foxpf.h"



#define CHECK_FOR_UNDERFLOW		0

#define SCALEZEROTOL 0.000001f
#define NORMALZEROTOL 0.0001f

#define USE_SMALL_MATS		1
#define CHECK_DCS_BOUNDS	0	/* halt if we overflow DCS bounds */

void pfSinCos(float arg, float* s, float* c)
{
	float ang;
	
	ang= (arg * (float)PF_PI) / 180.0f;
	*c = cosf(ang);
	*s = sinf(ang);
}


float 	pfTan(float _arg)
{
	float TSin,TCos;

	pfSinCos(_arg,&TSin,&TCos);

	return(TSin/TCos);
}


float pfArcSin(float arg)
{
	float CurVal,savearg;
	float arg2,arg3,arg5,arg7,arg9,arg11;
	Boolean Mirror;


#define sin45 0.70710678f

	if (arg > 0.999999f) return(90.0f);
	 else if (arg < -0.999999f) return(-90.0f);
	if (arg < sin45 && arg > -sin45) {
		Mirror=FALSE;
	} else {
		Mirror=TRUE;
		savearg=arg;
		if (arg < 0.0f) arg= -pfSqrt(1-arg*arg);
		 else arg= pfSqrt(1-arg*arg);
	}
	
	if (arg < 0.001f && arg > -0.001f) CurVal=arg;
	 else {

#define SReal3  0.166666667f   /* (1)/(2*3) */
#define SReal5  0.075f         /* (1*3)/(2*4*5) */
#define SReal7  0.044642857f   /* (1*3*5)/(2*4*6*7) */
#define SReal9  0.030381944f   /* (1*3*5*7)/(2*4*6*8*9) */

#define SApprox3   0.212749f
#define SApprox5   0.105502f
#define SApprox7   0.066797f
#define SApprox9   0.047446f
#define SApprox11  0.036055f

	
	arg2=arg*arg;
	arg3=arg*arg2;
	arg5=arg3*arg2;
	arg7=arg5*arg2;
	arg9=arg7*arg2;
/*	arg11=arg9*arg2; */
		

#if 0		
	CurVal=arg+arg3*SApprox3; /* 0.212749  Error=.176064 */
	CurVal=arg+arg3*SReal3+arg5*SApprox5; /* 0.105502  Error=.040989*/
	CurVal=arg+arg3*SReal3+arg5*SReal5+arg7*SApprox7; /* 0.066797 Error=.011658 */
#endif
		
	CurVal=arg+arg3*SReal3+arg5*SReal5+arg7*SReal7+arg9*SApprox9; /* 0.047446  Error=.003716 */
		
#if 0
	CurVal=arg+arg3*SReal3+arg5*SReal5+arg7*SReal7+arg9*SReal9+arg11*SApprox11; /* 0.036055  Error=.001274 */
#endif		

    }


	CurVal=CurVal * 180.0f / (float)PF_PI;


	
	if (Mirror) {
		if (savearg < 0.0f) CurVal = -90.0f - CurVal;
		 else CurVal=90.0f-CurVal;
	}

	return(CurVal);
}



float pfArcCos(float arg)
{
	return (90.0f-pfArcSin(arg));
}

float pfArcTan2(float y, float x)
{

	float CurVal,UseX,UseY;
	float arg,arg2,arg3,arg5,arg7,arg9,arg11,arg13;
	Boolean Mirror;

#define TanZeroTol 0.0001f	

	if (x<TanZeroTol && x >=-TanZeroTol) CurVal=90.0f;
	 else if (y<TanZeroTol && y >=-TanZeroTol) CurVal=0.0f;
	 else {

		UseY=PF_ABS(y);
		UseX=PF_ABS(x);

		if (UseY > UseX) {
			Mirror=TRUE;
			arg=UseX/UseY;
		} else {
			Mirror=FALSE;
			arg=UseY/UseX;
		}

		if (arg<TanZeroTol && arg >=-TanZeroTol) CurVal=0.0f;
		else {

#define TReal3  0.333333333333f  /* 1/3 */		
#define TReal5  0.2f 		     /* 1/5 */		
#define TReal7  0.142857142857f  /* 1/7 */		
#define TReal9  0.111111111111f  /* 1/9 */		
#define TReal11 0.090909090909f  /* 1/11 */

#define TApprox3   0.226290f
#define TApprox5   0.123660f
#define TApprox7   0.083920f
#define TApprox9   0.063235f
#define TApprox11  0.050640f
#define TApprox13  0.042193f
	
		
			arg2=arg*arg;
			arg3=arg*arg2;
			arg5=arg3*arg2;
			arg7=arg5*arg2;
			arg9=arg7*arg2;
	/*		arg11=arg9*arg2;
			arg13=arg11*arg2; */
			
	
	#if 0		
			CurVal=arg-arg3*TApprox3; /* 0.226290 Error=.669273*/
			CurVal=arg-arg3*TReal3+arg5*TApprox5; /* 0.123660 Error=.281597 */
			CurVal=arg-arg3*TReal3+arg5*TReal5-arg7*TApprox7; /* 0.083920 Error=.152428 */
	#endif
			
			CurVal=arg-arg3*TReal3+arg5*TReal5-arg7*TReal7+arg9*TApprox9; /* 0.063235 Error=.094246 */
			
	#if 0
			CurVal=arg-arg3*TReal3+arg5*TReal5-arg7*TReal7+arg9*TReal9-arg11*TApprox11; /* 0.050640  Error=.063969 */
			CurVal=arg-arg3*TReal3+arg5*TReal5-arg7*TReal7+arg9*TReal9-arg11*TReal11+arg13*TApprox13; /* 0.042193  Error=.046181*/
	#endif		
			
	
			CurVal=CurVal * 180.0f / (float)PF_PI;
	
		
		}
		if (Mirror) CurVal=90.0f-CurVal;  /* Flip around 45 degrees */
	}  

	if (x < -TanZeroTol) CurVal = (180.0f-CurVal);
	if (y < -TanZeroTol) CurVal = -CurVal;
	
	return(CurVal);
}






void pfAddVec2(pfVec2 _dst, const pfVec2 _v1, const pfVec2 _v2)
{
	_dst[0]=_v1[0]+_v2[0];
	_dst[1]=_v1[1]+_v2[1];
}

void pfScaleVec2(pfVec2 _dst, float _s, pfVec2 _v)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	
	if (_s < SCALEZEROTOL && _s > -SCALEZEROTOL) {
		_dst[PF_X] = 0.0f;
		_dst[PF_Y] = 0.0f;
	} else {
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
	}
#else
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
#endif
}

void pfAddScaledVec2(pfVec2 _dst, const pfVec2 _v1, float _s, pfVec2 _v2)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v2[PF_X],NORMALZEROTOL)) _v2[PF_X]=0.0f;
	if (PF_ABSLT(_v2[PF_Y],NORMALZEROTOL)) _v2[PF_Y]=0.0f;
	
	if (_s < SCALEZEROTOL && _s > -SCALEZEROTOL) {
		_dst[PF_X] = _v1[PF_X];
		_dst[PF_Y] = _v1[PF_Y];
	} else {
		_dst[PF_X] = _v1[PF_X] + _v2[PF_X] * _s;
		_dst[PF_Y] = _v1[PF_Y] + _v2[PF_Y] * _s;
	}
#else
	_dst[PF_X] = _v1[PF_X] + _v2[PF_X] * _s;
	_dst[PF_Y] = _v1[PF_Y] + _v2[PF_Y] * _s;
#endif
}


float pfLengthVec2(pfVec2 _v)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	if (PF_ABSLT(_v[PF_Y],NORMALZEROTOL)) _v[PF_Y]=0.0f;
#endif
	return(pfSqrt(_v[PF_X]*_v[PF_X] + _v[PF_Y] * _v[PF_Y]));
}

float pfNormalizeVec2(pfVec2 _v)
{
	float Len;
	
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	if (PF_ABSLT(_v[PF_Y],NORMALZEROTOL)) _v[PF_Y]=0.0f;
#endif
	Len=PFLENGTH_VEC2(_v);
	if (Len>=NORMALZEROTOL) {
		_v[PF_X]=_v[PF_X]/Len;
		_v[PF_Y]=_v[PF_Y]/Len;
	}
	return(Len);
}

float pfSqrDistancePt2(pfVec2 _v1, pfVec2 _v2)
{
	float dv1,dv2;

	dv1=_v1[0]-_v2[0];
	dv2=_v1[1]-_v2[1];
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(dv1,NORMALZEROTOL)) dv1=0.0f;
	if (PF_ABSLT(dv2,NORMALZEROTOL)) dv2=0.0f;
#endif
	return(dv1*dv1 + dv2*dv2);
}




void pfSetVec2(pfVec2 v, float xx, float yy)
{
	v[0]=xx;
	v[1]=yy;
}

void pfSetVec3(pfVec3 v, float xx, float yy, float zz)
{
	v[0]=xx;
	v[1]=yy;
	v[2]=zz;
}

void pfCopyVec3(pfVec3 dst, pfVec3 src)
{
	dst[0]=src[0];
	dst[1]=src[1];
	dst[2]=src[2];
}

void pfCopyVec4(pfVec4 dst, pfVec4 src)
{
	dst[0]=src[0];
	dst[1]=src[1];
	dst[2]=src[2];
	dst[3]=src[3];
}


void pfNegateVec3(pfVec3 dst, pfVec3 src)
{
	dst[0]=-src[0];
	dst[1]=-src[1];
	dst[2]=-src[2];
}

Boolean pfEqualVec3(pfVec3 v1, pfVec3 v2)
{
	if (v1[PF_X]==v2[PF_X] && v1[PF_Y]==v2[PF_Y] && v1[PF_Z]==v2[PF_Z]) return(TRUE);
	return(FALSE);
}


void pfAddVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2)
{
	_dst[0]=_v1[0]+_v2[0];
	_dst[1]=_v1[1]+_v2[1];
	_dst[2]=_v1[2]+_v2[2];
}

void pfSubVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2)
{
	_dst[0]=_v1[0]-_v2[0];
	_dst[1]=_v1[1]-_v2[1];
	_dst[2]=_v1[2]-_v2[2];
}

float pfDotVec3(pfVec3 _v1, pfVec3 _v2)
{
	return(_v1[0]*_v2[0] + _v1[1]*_v2[1] + _v1[2]*_v2[2]);
}



float pfLengthVec3(pfVec3 _v)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	if (PF_ABSLT(_v[PF_Y],NORMALZEROTOL)) _v[PF_Y]=0.0f;
	if (PF_ABSLT(_v[PF_Z],NORMALZEROTOL)) _v[PF_Z]=0.0f;
#endif
	return(pfSqrt(PFDOT_VEC3(_v, _v)));
}


float pfSqrDistancePt3(pfVec3 _v1, pfVec3 _v2)
{
	float dv1,dv2,dv3;

	dv1=_v1[0]-_v2[0];
	dv2=_v1[1]-_v2[1];
	dv3=_v1[2]-_v2[2];
	return(dv1*dv1 + dv2*dv2 + dv3*dv3);
}

float pfDistancePt2(pfVec2 _v1, pfVec2 _v2)
{
	pfVec2 TVec;

	PFSUB_VEC2(TVec,_v2,_v1);
	return(pfSqrt(PFDOT_VEC2(TVec,TVec)));
}

float pfDistancePt3(pfVec3 _v1, pfVec3 _v2)
{
	pfVec3 TVec;

	PFSUB_VEC3(TVec,_v2,_v1);
	return(pfSqrt(PFDOT_VEC3(TVec,TVec)));
}


float pfNormalizeVec3(pfVec3 _v)
{
	float Len;

	Len=PFLENGTH_VEC3(_v);
	if (Len>=NORMALZEROTOL) {
		_v[PF_X]=_v[PF_X]/Len;
		_v[PF_Y]=_v[PF_Y]/Len;
		_v[PF_Z]=_v[PF_Z]/Len;
	}
	return(Len);
}

void pfCrossVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2)
{
	pfVec3 Temp;
	
	Temp[PF_X]= _v1[PF_Y]*_v2[PF_Z] - _v1[PF_Z]*_v2[PF_Y];
	Temp[PF_Y]= _v1[PF_Z]*_v2[PF_X] - _v1[PF_X]*_v2[PF_Z];
	Temp[PF_Z]= _v1[PF_X]*_v2[PF_Y] - _v1[PF_Y]*_v2[PF_X];
	_dst[PF_X]=Temp[PF_X];
	_dst[PF_Y]=Temp[PF_Y];
	_dst[PF_Z]=Temp[PF_Z];
}

void pfScaleVec3(pfVec3 _dst, float _s, pfVec3 _v)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	if (PF_ABSLT(_v[PF_Y],NORMALZEROTOL)) _v[PF_Y]=0.0f;
	if (PF_ABSLT(_v[PF_Z],NORMALZEROTOL)) _v[PF_Z]=0.0f;
	
	if (_s < SCALEZEROTOL && _s > -SCALEZEROTOL) {
		_dst[PF_X] = 0.0f;
		_dst[PF_Y] = 0.0f;
		_dst[PF_Z] = 0.0f;
	} else {
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
		_dst[PF_Z] = _v[PF_Z] * _s;
	}
#else
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
		_dst[PF_Z] = _v[PF_Z] * _s;
#endif
}

void pfAddScaledVec3(pfVec3 _dst, const pfVec3 _v1, float _s, pfVec3 _v2)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v2[PF_X],NORMALZEROTOL)) _v2[PF_X]=0.0f;
	if (PF_ABSLT(_v2[PF_Y],NORMALZEROTOL)) _v2[PF_Y]=0.0f;
	if (PF_ABSLT(_v2[PF_Z],NORMALZEROTOL)) _v2[PF_Z]=0.0f;
	
	if (_s < SCALEZEROTOL && _s > -SCALEZEROTOL) {
		_dst[PF_X] = _v1[PF_X];
		_dst[PF_Y] = _v1[PF_Y];
		_dst[PF_Z] = _v1[PF_Z];
	} else {
		_dst[PF_X] = _v1[PF_X] + _v2[PF_X] * _s;
		_dst[PF_Y] = _v1[PF_Y] + _v2[PF_Y] * _s;
		_dst[PF_Z] = _v1[PF_Z] + _v2[PF_Z] * _s;
	}
#else
	_dst[PF_X] = _v1[PF_X] + _v2[PF_X] * _s;
	_dst[PF_Y] = _v1[PF_Y] + _v2[PF_Y] * _s;
	_dst[PF_Z] = _v1[PF_Z] + _v2[PF_Z] * _s;
#endif
}

void	pfCombineVec3(pfVec3 _dst, float _s1, const pfVec3 _v1, float _s2, const pfVec3 _v2)
{
	_dst[PF_X] = _v1[PF_X] * _s1 + _v2[PF_X] * _s2;
	_dst[PF_Y] = _v1[PF_Y] * _s1 + _v2[PF_Y] * _s2;
	_dst[PF_Z] = _v1[PF_Z] * _s1 + _v2[PF_Z] * _s2;
}

void pfSetMatRowVec3(pfMatrix _dst, long _r, const pfVec3 _v)
{
	_dst[_r][PF_X]=_v[PF_X];
	_dst[_r][PF_Y]=_v[PF_Y];
	_dst[_r][PF_Z]=_v[PF_Z];
}

void pfGetMatRowVec3(pfMatrix _m, long _r, pfVec3 _dst)
{
	_dst[PF_X] = _m[_r][PF_X];
	_dst[PF_Y] = _m[_r][PF_Y];
	_dst[PF_Z] = _m[_r][PF_Z];
}



void pfCopyMat(pfMatrix _dst, const pfMatrix a)
{
	long i,j;

	for(i=0;i<4;i++) {
		for(j=0;j<4;j++) {
			_dst[i][j]=a[i][j];
		}
	}
}


void pfMultMat(pfMatrix _dst, const pfMatrix a, const pfMatrix b)
{
	pfMatrix aa, bb;

	PFCOPY_MAT(aa,a);
	PFCOPY_MAT(bb,b);

	_dst[0][0]=bb[0][0]*aa[0][0]+bb[1][0]*aa[0][1]+bb[2][0]*aa[0][2]+bb[3][0]*aa[0][3];
	_dst[0][1]=bb[0][1]*aa[0][0]+bb[1][1]*aa[0][1]+bb[2][1]*aa[0][2]+bb[3][1]*aa[0][3];
	_dst[0][2]=bb[0][2]*aa[0][0]+bb[1][2]*aa[0][1]+bb[2][2]*aa[0][2]+bb[3][2]*aa[0][3];
	_dst[0][3]=bb[0][3]*aa[0][0]+bb[1][3]*aa[0][1]+bb[2][3]*aa[0][2]+bb[3][3]*aa[0][3];
	_dst[1][0]=bb[0][0]*aa[1][0]+bb[1][0]*aa[1][1]+bb[2][0]*aa[1][2]+bb[3][0]*aa[1][3];
	_dst[1][1]=bb[0][1]*aa[1][0]+bb[1][1]*aa[1][1]+bb[2][1]*aa[1][2]+bb[3][1]*aa[1][3];
	_dst[1][2]=bb[0][2]*aa[1][0]+bb[1][2]*aa[1][1]+bb[2][2]*aa[1][2]+bb[3][2]*aa[1][3];
	_dst[1][3]=bb[0][3]*aa[1][0]+bb[1][3]*aa[1][1]+bb[2][3]*aa[1][2]+bb[3][3]*aa[1][3];
	_dst[2][0]=bb[0][0]*aa[2][0]+bb[1][0]*aa[2][1]+bb[2][0]*aa[2][2]+bb[3][0]*aa[2][3];
	_dst[2][1]=bb[0][1]*aa[2][0]+bb[1][1]*aa[2][1]+bb[2][1]*aa[2][2]+bb[3][1]*aa[2][3];
	_dst[2][2]=bb[0][2]*aa[2][0]+bb[1][2]*aa[2][1]+bb[2][2]*aa[2][2]+bb[3][2]*aa[2][3];
	_dst[2][3]=bb[0][3]*aa[2][0]+bb[1][3]*aa[2][1]+bb[2][3]*aa[2][2]+bb[3][3]*aa[2][3];
	_dst[3][0]=bb[0][0]*aa[3][0]+bb[1][0]*aa[3][1]+bb[2][0]*aa[3][2]+bb[3][0]*aa[3][3];
	_dst[3][1]=bb[0][1]*aa[3][0]+bb[1][1]*aa[3][1]+bb[2][1]*aa[3][2]+bb[3][1]*aa[3][3];
	_dst[3][2]=bb[0][2]*aa[3][0]+bb[1][2]*aa[3][1]+bb[2][2]*aa[3][2]+bb[3][2]*aa[3][3];
	_dst[3][3]=bb[0][3]*aa[3][0]+bb[1][3]*aa[3][1]+bb[2][3]*aa[3][2]+bb[3][3]*aa[3][3];
}


void pfPreMultMat(pfMatrix _dst, const pfMatrix b)
{
	pfMatrix a;
	
	PFCOPY_MAT(a,_dst);
/*	GfxMatrix4Multiply((Matrix4 *)_temp,(Matrix4 *)_m,(Matrix4 *)_dst); */
	_dst[0][0]=a[0][0]*b[0][0]+a[1][0]*b[0][1]+a[2][0]*b[0][2]+a[3][0]*b[0][3];
	_dst[0][1]=a[0][1]*b[0][0]+a[1][1]*b[0][1]+a[2][1]*b[0][2]+a[3][1]*b[0][3];
	_dst[0][2]=a[0][2]*b[0][0]+a[1][2]*b[0][1]+a[2][2]*b[0][2]+a[3][2]*b[0][3];
	_dst[0][3]=a[0][3]*b[0][0]+a[1][3]*b[0][1]+a[2][3]*b[0][2]+a[3][3]*b[0][3];
	_dst[1][0]=a[0][0]*b[1][0]+a[1][0]*b[1][1]+a[2][0]*b[1][2]+a[3][0]*b[1][3];
	_dst[1][1]=a[0][1]*b[1][0]+a[1][1]*b[1][1]+a[2][1]*b[1][2]+a[3][1]*b[1][3];
	_dst[1][2]=a[0][2]*b[1][0]+a[1][2]*b[1][1]+a[2][2]*b[1][2]+a[3][2]*b[1][3];
	_dst[1][3]=a[0][3]*b[1][0]+a[1][3]*b[1][1]+a[2][3]*b[1][2]+a[3][3]*b[1][3];
	_dst[2][0]=a[0][0]*b[2][0]+a[1][0]*b[2][1]+a[2][0]*b[2][2]+a[3][0]*b[2][3];
	_dst[2][1]=a[0][1]*b[2][0]+a[1][1]*b[2][1]+a[2][1]*b[2][2]+a[3][1]*b[2][3];
	_dst[2][2]=a[0][2]*b[2][0]+a[1][2]*b[2][1]+a[2][2]*b[2][2]+a[3][2]*b[2][3];
	_dst[2][3]=a[0][3]*b[2][0]+a[1][3]*b[2][1]+a[2][3]*b[2][2]+a[3][3]*b[2][3];
	_dst[3][0]=a[0][0]*b[3][0]+a[1][0]*b[3][1]+a[2][0]*b[3][2]+a[3][0]*b[3][3];
	_dst[3][1]=a[0][1]*b[3][0]+a[1][1]*b[3][1]+a[2][1]*b[3][2]+a[3][1]*b[3][3];
	_dst[3][2]=a[0][2]*b[3][0]+a[1][2]*b[3][1]+a[2][2]*b[3][2]+a[3][2]*b[3][3];
	_dst[3][3]=a[0][3]*b[3][0]+a[1][3]*b[3][1]+a[2][3]*b[3][2]+a[3][3]*b[3][3];
}
void pfPostMultMat(pfMatrix _dst, const pfMatrix a)
{
	pfMatrix b;
	
	PFCOPY_MAT(b,_dst);
/*	GfxMatrix4Multiply((Matrix4 *)_m,(Matrix4 *)_temp,(Matrix4 *)_dst); */
	_dst[0][0]=a[0][0]*b[0][0]+a[1][0]*b[0][1]+a[2][0]*b[0][2]+a[3][0]*b[0][3];
	_dst[0][1]=a[0][1]*b[0][0]+a[1][1]*b[0][1]+a[2][1]*b[0][2]+a[3][1]*b[0][3];
	_dst[0][2]=a[0][2]*b[0][0]+a[1][2]*b[0][1]+a[2][2]*b[0][2]+a[3][2]*b[0][3];
	_dst[0][3]=a[0][3]*b[0][0]+a[1][3]*b[0][1]+a[2][3]*b[0][2]+a[3][3]*b[0][3];
	_dst[1][0]=a[0][0]*b[1][0]+a[1][0]*b[1][1]+a[2][0]*b[1][2]+a[3][0]*b[1][3];
	_dst[1][1]=a[0][1]*b[1][0]+a[1][1]*b[1][1]+a[2][1]*b[1][2]+a[3][1]*b[1][3];
	_dst[1][2]=a[0][2]*b[1][0]+a[1][2]*b[1][1]+a[2][2]*b[1][2]+a[3][2]*b[1][3];
	_dst[1][3]=a[0][3]*b[1][0]+a[1][3]*b[1][1]+a[2][3]*b[1][2]+a[3][3]*b[1][3];
	_dst[2][0]=a[0][0]*b[2][0]+a[1][0]*b[2][1]+a[2][0]*b[2][2]+a[3][0]*b[2][3];
	_dst[2][1]=a[0][1]*b[2][0]+a[1][1]*b[2][1]+a[2][1]*b[2][2]+a[3][1]*b[2][3];
	_dst[2][2]=a[0][2]*b[2][0]+a[1][2]*b[2][1]+a[2][2]*b[2][2]+a[3][2]*b[2][3];
	_dst[2][3]=a[0][3]*b[2][0]+a[1][3]*b[2][1]+a[2][3]*b[2][2]+a[3][3]*b[2][3];
	_dst[3][0]=a[0][0]*b[3][0]+a[1][0]*b[3][1]+a[2][0]*b[3][2]+a[3][0]*b[3][3];
	_dst[3][1]=a[0][1]*b[3][0]+a[1][1]*b[3][1]+a[2][1]*b[3][2]+a[3][1]*b[3][3];
	_dst[3][2]=a[0][2]*b[3][0]+a[1][2]*b[3][1]+a[2][2]*b[3][2]+a[3][2]*b[3][3];
	_dst[3][3]=a[0][3]*b[3][0]+a[1][3]*b[3][1]+a[2][3]*b[3][2]+a[3][3]*b[3][3];
	
	
}



void pfInvertOrthoMat(pfMatrix _dst, const pfMatrix _m)
{
	float HoldX,HoldY,HoldZ;

	HoldX=PFDOT_VEC3(_m[0],_m[0]);  /* X scale squared */
	HoldY=PFDOT_VEC3(_m[1],_m[1]);  /* etc. */
	HoldZ=PFDOT_VEC3(_m[2],_m[2]);
	  
	_dst[0][1]=_m[1][0]/HoldY;
	_dst[0][2]=_m[2][0]/HoldZ;
	_dst[1][2]=_m[2][1]/HoldZ;

	_dst[1][0]=_m[0][1]/HoldX;
	_dst[2][0]=_m[0][2]/HoldX;
	_dst[2][1]=_m[1][2]/HoldY;

	_dst[0][0]=_m[0][0]/HoldX;
	_dst[1][1]=_m[1][1]/HoldY;
	_dst[2][2]=_m[2][2]/HoldZ;

	_dst[0][3]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][3]=0.0f;

	_dst[3][3]=1.0f;
	
	HoldX=_m[3][0];
	HoldY=_m[3][1];
	HoldZ=_m[3][2];

	_dst[3][0]=-(HoldX * _dst[0][0] + HoldY * _dst[1][0] + HoldZ * _dst[2][0]);
	_dst[3][1]=-(HoldX * _dst[0][1] + HoldY * _dst[1][1] + HoldZ * _dst[2][1]);
	_dst[3][2]=-(HoldX * _dst[0][2] + HoldY * _dst[1][2] + HoldZ * _dst[2][2]);
}


void pfInvertOrthoNMat(pfMatrix _dst, const pfMatrix _m)
{
	float HoldX,HoldY,HoldZ;
	
	_dst[0][1]=_m[1][0];
	_dst[0][2]=_m[2][0];
	_dst[1][2]=_m[2][1];

	_dst[1][0]=_m[0][1];
	_dst[2][0]=_m[0][2];
	_dst[2][1]=_m[1][2];

	_dst[0][0]=_m[0][0];
	_dst[1][1]=_m[1][1];
	_dst[2][2]=_m[2][2];

	_dst[0][3]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][3]=0.0f;

	_dst[3][3]=1.0f;
	
	HoldX=_m[3][0];
	HoldY=_m[3][1];
	HoldZ=_m[3][2];

	_dst[3][0]=-(HoldX * _dst[0][0] + HoldY * _dst[1][0] + HoldZ * _dst[2][0]);
	_dst[3][1]=-(HoldX * _dst[0][1] + HoldY * _dst[1][1] + HoldZ * _dst[2][1]);
	_dst[3][2]=-(HoldX * _dst[0][2] + HoldY * _dst[1][2] + HoldZ * _dst[2][2]);
}






long  pfinvDecompose3(pfMatrix _dst, pfMatrix _src, long index[3])
{
    long    i, j, k;		/* Loop Indices */
    long    imax;		/* Maximum i Index */
    float big;		/* Non-Zero Largest Element */
    float sum;		/* Accumulator */
    float temp;		/* Scratch Variable */
    float vv[3];		/* Implicit Scaling of Each Row */

    for (i=0;  i < 3;  i++) {
		big = 0.0f;
		for (j=0;  j < 3;  j++) {
			_dst[i][j]=_src[i][j];
			temp=PF_ABS(_src[i][j]);
			if (temp > big) {
				big = temp;
				if (big == 0.0f) return(0);
				vv[i] = 1.0f / big;
			}
		}
	}

    for (j=0;  j < 3;  ++j) {
	    for (i=0;  i < j;  i++) {
			sum = _dst[i][j];
		    for (k=0;  k < i;  k++)
			sum -= _dst[i][k] * _dst[k][j];
		    _dst[i][j] = sum;
		}

		big = 0;
		for (i=j;  i < 3;  i++) {
			sum = _dst[i][j];
		    for (k=0;  k < j;  k++)
			sum -= _dst[i][k] * _dst[k][j];
		    _dst[i][j] = sum;
		    temp = vv[i] * PF_ABS(sum);
		    if (temp >= big) {
				big = temp;
				imax = i;
		    }
		}

		if (j != imax)	{
			for (k=0;  k < 3;  k++) {
				temp = _dst[imax][k];
				_dst[imax][k] = _dst[j][k];
				_dst[j][k] = temp;
		    }
		    vv[imax] = vv[j];
		}

		index[j] = imax;
		if (_dst[j][j] == 0.0f) return (0);
		
		if (j != (3-1)) {
			temp = 1.0f / _dst[j][j];
		    for (i=j+1;  i < 3;  i++) _dst[i][j] *= temp;
		}
    }
    return (1);
}

void  pfinvBacksub3(pfMatrix matrix, long index[3], pfVec3 B)
{
    long    i,j;
    long    ii;
    long    ip;
    float   sum;

    ii = -1;

    for (i=0;  i < 3;  i++) {
    	ip = index[i];
		sum = B[ip];
		B[ip] = B[i];
		if (ii >= 0)  {
		    for (j=ii;  j <= i-1;  j++) sum -= matrix[i][j] * B[j];
		} else {
			if (sum) ii = i;
		}
		B[i] = sum;
    }

    for (i=(3-1);  i >= 0;  i--) {
    	sum = B[i];
		for (j=i+1;  j < 3;  j++) {
		    sum -= matrix[i][j] * B[j];
		}
		B[i] = sum / matrix[i][i];
    }
}

#if 0
void pfInvertAffineMat(pfMatrix _dst, const pfMatrix _m)
{
	long i,j;
	long index[3];
	pfVec3 column;
	pfMatrix inverse;
	float HoldX,HoldY,HoldZ;

    if (!pfinvDecompose3(_dst, _m, index)) {
		/* Singular, undefined */
    } else {

	    for (j=0;  j < 3;  j++) {
	    	for (i=0;  i < 3;  i++) column[i] = 0.0f;
			column[j] = 1.0f;
			pfinvBacksub3(_dst, index, column);
			for (i=0;  i < 3;  i++) {
			    inverse[i][j] = column[i];
			}
		}

		PFCOPY_VEC3(_dst[0],inverse[0]);
		PFCOPY_VEC3(_dst[1],inverse[1]);
		PFCOPY_VEC3(_dst[2],inverse[2]);
	
		_dst[0][3]=0.0f;
		_dst[1][3]=0.0f;
		_dst[2][3]=0.0f;
	
		_dst[3][3]=1.0f;
	
		HoldX=_m[3][0];
		HoldY=_m[3][1];
		HoldZ=_m[3][2];

		_dst[3][0]=-(HoldX * _dst[0][0] + HoldY * _dst[1][0] + HoldZ * _dst[2][0]);
		_dst[3][1]=-(HoldX * _dst[0][1] + HoldY * _dst[1][1] + HoldZ * _dst[2][1]);
		_dst[3][2]=-(HoldX * _dst[0][2] + HoldY * _dst[1][2] + HoldZ * _dst[2][2]);
		
	}
}
#endif

long  pfinvDecompose4(pfMatrix _dst, pfMatrix _src, long index[4])
{
    long    i, j, k;		/* Loop Indices */
    long    imax;		/* Maximum i Index */
    float big;		/* Non-Zero Largest Element */
    float sum;		/* Accumulator */
    float temp;		/* Scratch Variable */
    float vv[4];		/* Implicit Scaling of Each Row */

    for (i=0;  i < 4;  i++) {
		big = 0.0f;
		for (j=0;  j < 4;  j++) {
			_dst[i][j]=_src[i][j];
			temp=PF_ABS(_src[i][j]);
			if (temp > big) {
				big = temp;
				if (big == 0.0f) return(0);
				vv[i] = 1.0f / big;
			}
		}
	}

    for (j=0;  j < 4;  ++j) {
	    for (i=0;  i < j;  i++) {
			sum = _dst[i][j];
		    for (k=0;  k < i;  k++)
			sum -= _dst[i][k] * _dst[k][j];
		    _dst[i][j] = sum;
		}

		big = 0;
		for (i=j;  i < 4;  i++) {
			sum = _dst[i][j];
		    for (k=0;  k < j;  k++)
			sum -= _dst[i][k] * _dst[k][j];
		    _dst[i][j] = sum;
		    temp = vv[i] * PF_ABS(sum);
		    if (temp >= big) {
				big = temp;
				imax = i;
		    }
		}

		if (j != imax)	{
			for (k=0;  k < 4;  k++) {
				temp = _dst[imax][k];
				_dst[imax][k] = _dst[j][k];
				_dst[j][k] = temp;
		    }
		    vv[imax] = vv[j];
		}

		index[j] = imax;
		if (_dst[j][j] == 0.0f) return (0);
		
		if (j != (4-1)) {
			temp = 1.0f / _dst[j][j];
		    for (i=j+1;  i < 4;  i++) _dst[i][j] *= temp;
		}
    }
    return (1);
}

void  pfinvBacksub4(pfMatrix matrix, long index[4], pfVec4 B)
{
    long    i,j;
    long    ii;
    long    ip;
    float   sum;

    ii = -1;

    for (i=0;  i < 4;  i++) {
    	ip = index[i];
		sum = B[ip];
		B[ip] = B[i];
		if (ii >= 0)  {
		    for (j=ii;  j <= i-1;  j++) sum -= matrix[i][j] * B[j];
		} else {
			if (sum) ii = i;
		}
		B[i] = sum;
    }

    for (i=(4-1);  i >= 0;  i--) {
    	sum = B[i];
		for (j=i+1;  j < 4;  j++) {
		    sum -= matrix[i][j] * B[j];
		}
		B[i] = sum / matrix[i][i];
    }
}

#if 0
long pfInvertMat(pfMatrix _dst, const pfMatrix _m)
{
	long i,j;
	long index[4];
	pfVec4 column;
	pfMatrix inverse;


    if (!pfinvDecompose4(_dst,_m, index)) {
		/* Singular */
		return(0);
    }

    for (j=0;  j < 4;  j++) {
    	for (i=0;  i < 4;  i++) column[i] = 0.0f;
		column[j] = 1.0f;
		pfinvBacksub4(_dst, index, column);
		for (i=0;  i < 4;  i++) {
		    inverse[i][j] = column[i];
		}
	}

	PFCOPY_MAT(_dst,inverse);
	return(1);

}

#endif









void pfXformVec3(pfVec3 _dst, const pfVec3 _v, const pfMatrix _m)
{
	pfVec3 Temp;
	
	Temp[0] = (_m)[0][0] * (_v)[0] + (_m)[1][0] * (_v)[1] + (_m)[2][0] * (_v)[2];
	Temp[1] = (_m)[0][1] * (_v)[0] + (_m)[1][1] * (_v)[1] + (_m)[2][1] * (_v)[2];
	Temp[2] = (_m)[0][2] * (_v)[0] + (_m)[1][2] * (_v)[1] + (_m)[2][2] * (_v)[2];
	PFCOPY_VEC3(_dst,Temp);
}

void pfXformPt3(pfVec3 _dst, const pfVec3 _v, const pfMatrix _m)
{
	pfVec3 Temp;
	
	Temp[0] = (_m)[0][0] * (_v)[0] + (_m)[1][0] * (_v)[1] + (_m)[2][0] * (_v)[2] + (_m)[3][0];
	Temp[1] = (_m)[0][1] * (_v)[0] + (_m)[1][1] * (_v)[1] + (_m)[2][1] * (_v)[2] + (_m)[3][1];
	Temp[2] = (_m)[0][2] * (_v)[0] + (_m)[1][2] * (_v)[1] + (_m)[2][2] * (_v)[2] + (_m)[3][2];
	PFCOPY_VEC3(_dst,Temp);
}



void pfSetVec4(pfVec4 v, float xx, float yy, float zz, float ww)
{
	v[0]=xx;
	v[1]=yy;
	v[2]=zz;
	v[3]=ww;
}

void pfScaleVec4(pfVec4 _dst, float _s, pfVec4 _v)
{
#if CHECK_FOR_UNDERFLOW
	if (PF_ABSLT(_v[PF_X],NORMALZEROTOL)) _v[PF_X]=0.0f;
	if (PF_ABSLT(_v[PF_Y],NORMALZEROTOL)) _v[PF_Y]=0.0f;
	if (PF_ABSLT(_v[PF_Z],NORMALZEROTOL)) _v[PF_Z]=0.0f;
	if (PF_ABSLT(_v[3],NORMALZEROTOL)) _v[3]=0.0f;
	
	if (_s < SCALEZEROTOL && _s > -SCALEZEROTOL) {
		_dst[PF_X] = 0.0f;
		_dst[PF_Y] = 0.0f;
		_dst[PF_Z] = 0.0f;
		_dst[3] = 0.0f;
	} else {
	
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
		_dst[PF_Z] = _v[PF_Z] * _s;
		_dst[3]    = _v[3] * _s;
	}
#else
		_dst[PF_X] = _v[PF_X] * _s;
		_dst[PF_Y] = _v[PF_Y] * _s;
		_dst[PF_Z] = _v[PF_Z] * _s;
		_dst[3]    = _v[3] * _s;
#endif
}

void pfXformVec4(pfVec4 _dst, const pfVec4 _v, const pfMatrix _m)
{
	pfVec4 Temp;
	
	Temp[0] = (_m)[0][0] * (_v)[0] + (_m)[1][0] * (_v)[1] + (_m)[2][0] * (_v)[2] + (_m)[3][0] * (_v)[3];
	Temp[1] = (_m)[0][1] * (_v)[0] + (_m)[1][1] * (_v)[1] + (_m)[2][1] * (_v)[2] + (_m)[3][1] * (_v)[3];
	Temp[2] = (_m)[0][2] * (_v)[0] + (_m)[1][2] * (_v)[1] + (_m)[2][2] * (_v)[2] + (_m)[3][2] * (_v)[3];
	Temp[3] = (_m)[0][3] * (_v)[0] + (_m)[1][3] * (_v)[1] + (_m)[2][3] * (_v)[2] + (_m)[3][3] * (_v)[3];
	PFCOPY_VEC4(_dst,Temp);
}



void pfGetOrthoMatCoord(pfMatrix _src, pfCoord* _dst)
{
	pfVec3 TempHeading,TempUpVec,TempLeftVec,Projection,ProjectLeft;
	float ProjectionLength,TempLength;
	float Theta;
	float Division;

	PFSET_VEC3(_dst->xyz,_src[3][0],_src[3][1],_src[3][2]);
			
	PFSET_VEC3(TempHeading,_src[1][0],_src[1][1],_src[1][2]);
	PFSET_VEC3(TempUpVec,_src[2][0],_src[2][1],_src[2][2]);
	PFSET_VEC3(TempLeftVec,-_src[0][0],-_src[0][1],-_src[0][2]);
	
	Projection[PF_X]=TempHeading[PF_X];
	Projection[PF_Y]=TempHeading[PF_Y];
	Projection[PF_Z]=0.0f;
	ProjectionLength=PFLENGTH_VEC3(Projection);
		
	if (ProjectionLength<0.001) {
		Theta=pfArcCos(-TempLeftVec[PF_X]);
		if ((TempLeftVec[PF_Y] > 0.0f) == (TempHeading[PF_Z] > 0.0f)) { 
			Theta=-Theta;                     /* There was a bug here for shadows. The > was reversed, */
		}									  /*  and there was no check for the heading direction FXW (8Apr97) */
		_dst->hpr[PF_R]=Theta;
		_dst->hpr[PF_H]=0.0f;
	} else {
		Division=(Projection[PF_Y]/ProjectionLength);
		if (Division > 1.0f) {
			Theta=0.0f;
		} else {
			Theta=pfArcCos(Projection[PF_Y]/ProjectionLength);
			if (TempHeading[PF_X] > 0.0f) Theta=-Theta;
		}
		_dst->hpr[PF_H]=Theta;
	}
	
	if (ProjectionLength < 0.001) _dst->hpr[PF_P]=90.0f;
	 else {
		Theta=(PFDOT_VEC3(Projection,TempHeading)/ProjectionLength);
		if (Theta >= 1.0f) _dst->hpr[PF_P]=0.0f;
		 else {
			 Theta=pfArcCos(Theta);
			 _dst->hpr[PF_P]=Theta;
		 }
	}
	if (TempHeading[PF_Z] < 0.0f) _dst->hpr[PF_P] = - _dst->hpr[PF_P];
	
	ProjectLeft[PF_X]= -Projection[PF_Y];
	ProjectLeft[PF_Y]= Projection[PF_X];
	ProjectLeft[PF_Z]=0.0f;
	
	TempLength=PFLENGTH_VEC3(ProjectLeft);
	
	if (ProjectionLength >= 0.001) { /* Otherwise the roll has already been calculated */
		Theta= (PFDOT_VEC3(ProjectLeft,TempLeftVec)/TempLength);
		if (Theta>=1.0f) _dst->hpr[PF_R]=0.0f;
		 else if (Theta <= -1.0f) _dst->hpr[PF_R]=180.0f;
		 else {
			 Theta=pfArcCos(Theta);
			 _dst->hpr[PF_R]=Theta;
		}
		if (TempLeftVec[PF_Z] < 0.0f) _dst->hpr[PF_R] = - _dst->hpr[PF_R];
	}
}


void SetEulerPortionMat(pfMatrix _dst, float h, float p, float r)
{
	float sh,ch,sp,cp,sr,cr,hrCC,hrSS,hrCS,hrSC;
	
	pfSinCos(h,&sh,&ch);
	pfSinCos(p,&sp,&cp);
	pfSinCos(r,&sr,&cr);
	hrCC=ch*cr;
	hrCS=ch*sr;
	hrSC=sh*cr;
	hrSS=sh*sr;
	_dst[0][0]=hrCC-hrSS*sp;
	_dst[0][1]=hrSC+hrCS*sp;
	_dst[0][2]= -sr*cp;
	_dst[1][0]= -sh*cp;
	_dst[1][1]=ch*cp;
	_dst[1][2]=sp;
	_dst[2][0]=hrCS+hrSC*sp;
	_dst[2][1]=hrSS-hrCC*sp;
	_dst[2][2]=cr*cp;
		
}

void SetEulerPortionSmallMat(pfSmallMatrix _dst, float h, float p, float r)
{
	float sh,ch,sp,cp,sr,cr,hrCC,hrSS,hrCS,hrSC;
	
	pfSinCos(h,&sh,&ch);
	pfSinCos(p,&sp,&cp);
	pfSinCos(r,&sr,&cr);
	hrCC=ch*cr;
	hrCS=ch*sr;
	hrSC=sh*cr;
	hrSS=sh*sr;
	_dst[0][0]=hrCC-hrSS*sp;
	_dst[0][1]=hrSC+hrCS*sp;
	_dst[0][2]= -sr*cp;
	_dst[1][0]= -sh*cp;
	_dst[1][1]=ch*cp;
	_dst[1][2]=sp;
	_dst[2][0]=hrCS+hrSC*sp;
	_dst[2][1]=hrSS-hrCC*sp;
	_dst[2][2]=cr*cp;
		
}

void pfMakeEulerMat(pfMatrix _dst, float h, float p, float r)
{
	_dst[3][0]=0.0f;
	_dst[3][1]=0.0f;
	_dst[3][2]=0.0f;
	_dst[0][3]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][3]=0.0f;
	_dst[3][3]=1.0f;
	SetEulerPortionMat(_dst,h,p,r);
		
}

void pfMakeCoordMat(pfMatrix _dst, const pfCoord* _c)
{
	
	_dst[3][0]=_c->xyz[PF_X];
	_dst[3][1]=_c->xyz[PF_Y];
	_dst[3][2]=_c->xyz[PF_Z];
	_dst[0][3]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][3]=0.0f;
	_dst[3][3]=1.0f;
	SetEulerPortionMat(_dst,_c->hpr[PF_H],_c->hpr[PF_P],_c->hpr[PF_R]);
		
}

void pfMakeScaleMat(pfMatrix _dst, float xx, float yy, float zz)
{
	_dst[0][0]=xx;
	_dst[0][1]=0.0f;
	_dst[0][2]=0.0f;
	_dst[0][3]=0.0f;
	_dst[1][0]=0.0f;
	_dst[1][1]=yy;
	_dst[1][2]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][0]=0.0f;
	_dst[2][1]=0.0f;
	_dst[2][2]=zz;
	_dst[2][3]=0.0f;
	_dst[3][0]=0.0f;
	_dst[3][1]=0.0f;
	_dst[3][2]=0.0f;
	_dst[3][3]=1.0f;
	
}

void pfMakeTransMat(pfMatrix _dst, float xx, float yy, float zz)
{
	_dst[0][0]=1.0f;
	_dst[0][1]=0.0f;
	_dst[0][2]=0.0f;
	_dst[0][3]=0.0f;
	_dst[1][0]=0.0f;
	_dst[1][1]=1.0f;
	_dst[1][2]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][0]=0.0f;
	_dst[2][1]=0.0f;
	_dst[2][2]=1.0f;
	_dst[2][3]=0.0f;
	_dst[3][0]=xx;
	_dst[3][1]=yy;
	_dst[3][2]=zz;
	_dst[3][3]=1.0f;
	
}

void pfMakeRotMat(pfMatrix _dst, float degrees, float x, float y, float z)
{

/*

   Take p the length of the projection of [x,y,z] into the x,y plane. Then p^2 = x^2+y^2 or 1-z^2

   The required matrix is then  R1 x R2 x Ra x R2' x R1'

      |  y/p  x/p   0 |   
R1=   | -x/p  y/p   0 |   since cos a = y/p and sin a = x/p  (To rotate the vector to the y-z plane)
      |   0    0    1 |

      |  y/p -x/p   0 |   
R1'=  |  x/p  y/p   0 |   to rotate oppositely from R1
      |   0    0    1 |
	  

      |   0    0    0 |   
R2=   |   0    p   -z |   since cos b = p and sin b = z (To rotate the vector negatively down to the y axis)
      |   0    z    p |

      |   0    0    0 |   
R2'=  |   0    p    z |   to rotate oppositely from R2
      |   0   -z    p |
	  

      | cos d  0  -sin d |   
Ra=   |   0    1     0   |    where d is the rotation angle specified by degrees
      | sin d  0   cos d |


              
			 |  y/p    x    -zx/p  |
R1  x  R2 =  | -x/p    y    -zy/p  |
	  		 |   0     z      p    |

			 |  y/p   -x/p     0   |
R2' x  R1' = |   x      y      z   |
	  		 | -xz/p  -zy/p    p   |

			 
			 
			 

After multiplying [(R1 X R2) x Ra x (R2' x R1')] and simplifying we get the result of the code below where

c=cos(d)   s=sin(d)   c1= 1-c
xy = x * y   zy = z * y   zx = z * x
sx = x sin(d)   sy = y sin(d)   sz = z sin(d)
x2 = x^2   y2 = y^2   z2 = z^2    p2 = p^2
cx2 = (x^2)cos(d)    cy2 = (y^2)cos(d)


*/




	
	float c,s,c1,xy,zy,zx,sx,sy,sz,x2,y2,z2,p2,cy2,cx2;

	pfSinCos(degrees,&s,&c);
	
	if (z >= 0.999f) {
		_dst[0][0]=c;
		_dst[1][1]=c;
		
		_dst[0][1]=s;
		_dst[1][0]=-s;

		_dst[0][2]=0.0f;
		_dst[1][2]=0.0f;
		_dst[2][0]=0.0f;
		_dst[2][1]=0.0f;
		_dst[2][2]=1.0f;
		
	} else if (z <= -0.999f) {
		_dst[0][0]=c;
		_dst[1][1]=c;
		
		_dst[0][1]=-s;
		_dst[1][0]=s;

		_dst[0][2]=0.0f;
		_dst[1][2]=0.0f;
		_dst[2][0]=0.0f;
		_dst[2][1]=0.0f;
		_dst[2][2]=1.0f;

	} else {
		c1=1-c;
		xy=x*y;
		zy=z*y;
		zx=z*x;
		sx=s*x;
		sy=s*y;
		sz=s*z;
		x2=x*x;
		y2=y*y;
		z2=1-x2-y2;
		p2=1-z2;
		cy2=c*y2;
		cx2=c*x2;
	
		_dst[0][0]= x2 + (cy2+cx2*z2)/p2;
		_dst[1][1]= y2 + (cx2+cy2*z2)/p2;
	
		_dst[2][2]= z2+cx2+cy2;
	
		_dst[0][1]= c1*xy + sz;
		_dst[1][0]= c1*xy - sz;
	
		_dst[0][2]= c1*zx - sy;
		_dst[1][2]= c1*zy + sx;
		_dst[2][0]= c1*zx + sy;
		_dst[2][1]= c1*zy - sx;

	}
	_dst[3][0]=0.0f;
	_dst[3][1]=0.0f;
	_dst[3][2]=0.0f;
	_dst[0][3]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][3]=0.0f;
	_dst[3][3]=1.0f;



}



void pfPreRotMat(pfMatrix _dst, float degrees, float xx, float yy, float zz, pfMatrix _m)
{
	pfMatrix TMat;

	pfMakeRotMat(TMat,degrees,xx,yy,zz);
	pfMultMat(_dst,TMat,_m);
}


void pfMakeIdentMat(pfMatrix _dst)
{
	_dst[0][0]=1.0f;
	_dst[0][1]=0.0f;
	_dst[0][2]=0.0f;
	_dst[0][3]=0.0f;
	_dst[1][0]=0.0f;
	_dst[1][1]=1.0f;
	_dst[1][2]=0.0f;
	_dst[1][3]=0.0f;
	_dst[2][0]=0.0f;
	_dst[2][1]=0.0f;
	_dst[2][2]=1.0f;
	_dst[2][3]=0.0f;
	_dst[3][0]=0.0f;
	_dst[3][1]=0.0f;
	_dst[3][2]=0.0f;
	_dst[3][3]=1.0f;
}



void pfPostRotMat(pfMatrix _dst, pfMatrix _m, float degrees, float x, float y, float z)
{
	pfMatrix R;

	pfMakeRotMat(R,degrees,x,y,z);
	pfMultMat(_dst,_m,R);
}


void pfPreScaleMat(pfMatrix _dst, float xx, float yy, float zz, pfMatrix _m)
{
	_dst[0][0]=_m[0][0] * xx;
	_dst[0][1]=_m[0][1] * xx;
	_dst[0][2]=_m[0][2] * xx;
	_dst[0][3]=_m[0][3] * xx;
	_dst[1][0]=_m[1][0] * yy;
	_dst[1][1]=_m[1][1] * yy;
	_dst[1][2]=_m[1][2] * yy;
	_dst[1][3]=_m[1][3] * yy;
	_dst[2][0]=_m[2][0] * zz;
	_dst[2][1]=_m[2][1] * zz;
	_dst[2][2]=_m[2][2] * zz;
	_dst[2][3]=_m[2][3] * zz;

	_dst[3][0]=_m[3][0];
	_dst[3][1]=_m[3][1];
	_dst[3][2]=_m[3][2];
	_dst[3][3]=_m[3][3];
}
	 
void pfPostScaleMat(pfMatrix _dst, pfMatrix _m, float xx, float yy, float zz)
{
	_dst[0][0]=_m[0][0] * xx;
	_dst[0][1]=_m[0][1] * yy;
	_dst[0][2]=_m[0][2] * zz;
	_dst[0][3]=_m[0][3];
	_dst[1][0]=_m[1][0] * xx;
	_dst[1][1]=_m[1][1] * yy;
	_dst[1][2]=_m[1][2] * zz;
	_dst[1][3]=_m[1][3];
	_dst[2][0]=_m[2][0] * xx;
	_dst[2][1]=_m[2][1] * yy;
	_dst[2][2]=_m[2][2] * zz;
	_dst[2][3]=_m[2][3];

	_dst[3][0]=_m[3][0] * xx;
	_dst[3][1]=_m[3][1] * yy;
	_dst[3][2]=_m[3][2] * zz;
	_dst[3][3]=_m[3][3];
}
	 
void pfPreTransMat(pfMatrix _dst, float xx, float yy, float zz, pfMatrix _m)
{
	_dst[0][0]=_m[0][0];
	_dst[0][1]=_m[0][1];
	_dst[0][2]=_m[0][2];
	_dst[0][3]=_m[0][3];
	_dst[1][0]=_m[1][0];
	_dst[1][1]=_m[1][1];
	_dst[1][2]=_m[1][2];
	_dst[1][3]=_m[1][3];
	_dst[2][0]=_m[2][0];
	_dst[2][1]=_m[2][1];
	_dst[2][2]=_m[2][2];
	_dst[2][3]=_m[2][3];

	_dst[3][0]= xx * _m[0][0] + yy * _m[1][0] + zz * _m[2][0] + _m[3][0];
	_dst[3][1]= xx * _m[0][1] + yy * _m[1][1] + zz * _m[2][1] + _m[3][1];
	_dst[3][2]= xx * _m[0][2] + yy * _m[1][2] + zz * _m[2][2] + _m[3][2];
	_dst[3][3]= xx * _m[0][3] + yy * _m[1][3] + zz * _m[2][3] + _m[3][3];
	  
}








void pfMakePtsPlane(pfPlane *ThePlane, pfVec3 p1, pfVec3 p2, pfVec3 p3)
{
	pfVec3 D1,D2,XVec;
	
	PFSUB_VEC3(D1,p2,p1);
	PFSUB_VEC3(D2,p3,p2);
	pfCrossVec3(ThePlane->normal,D1,D2);
	pfNormalizeVec3(ThePlane->normal);
	ThePlane->offset=PFDOT_VEC3(ThePlane->normal,p1);
}


void pfMakeNormPtPlane(pfPlane *ThePlane, pfVec3 norm, pfVec3 pos)
{
	PFCOPY_VEC3(ThePlane->normal,norm);
	pfNormalizeVec3(ThePlane->normal);
	ThePlane->offset=PFDOT_VEC3(ThePlane->normal,pos);
}





#define DCSDIRTY 0x03


void pfDCSRot(pfDCS *ThisDCS, float hh, float pp, float rr)
{
	SetEulerPortionSmallMat(ThisDCS->Matrix,hh,pp,rr);
	ThisDCS->NodeInfo.BitFields |= DCSDIRTY;
}

void pfDCSTrans(pfDCS *ThisDCS, float xx, float yy, float zz)
{
	ThisDCS->Matrix[PF_T][PF_X]=xx;
	ThisDCS->Matrix[PF_T][PF_Y]=yy;
	ThisDCS->Matrix[PF_T][PF_Z]=zz;

	ThisDCS->NodeInfo.BitFields |= DCSDIRTY;
}


void pfDCSMat(pfDCS *ThisDCS, pfMatrix ThisMatrix)
{
#if CHECK_DCS_BOUNDS
	{
		long	i, j;
		
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (PF_ABSGT(ThisMatrix[i][j], 32000.0)) {
					CauseSystemCrash();
				}
			}
		}
	}
#endif
	PFCOPY_TO_SMALL_MAT(ThisDCS->Matrix,ThisMatrix);
	
	ThisDCS->NodeInfo.BitFields |= DCSDIRTY;
}

void pfGetDCSMat(pfDCS *ThisDCS, pfMatrix ThisMatrix)
{
	PFCOPY_FROM_SMALL_MAT(ThisMatrix,ThisDCS->Matrix);
}

void pfGetSCSMat(pfSCS *ThisSCS, pfMatrix ThisMatrix)
{
	PFCOPY_FROM_SMALL_MAT(ThisMatrix,ThisSCS->Matrix);
}


void pfDCSCoord(pfDCS *ThisDCS, pfCoord *c)
{
	pfMatrix ThisMat;
	
	pfMakeCoordMat(ThisMat,c);

	pfDCSMat(ThisDCS,ThisMat);
}





void pfSwitchVal(pfSwitch *ThisSwitch, long Val)
{
	ThisSwitch->SwitchVal=Val;

	/*  Change ultra 64 data structure to do the right thing */
}

long pfGetSwitchVal(pfSwitch *ThisSwitch)
{
	return(ThisSwitch->SwitchVal);
}





void pfLODRange(pfLOD* _lod, long _index, float _range)
{
	if (_index < MAX_LOD_RANGES && _index >= 0) {
		_lod->LODRanges[_index]=_range;
	}
}
	 
float pfGetLODRange(pfLOD* _lod, long _index)
{

	if (_index < MAX_LOD_RANGES && _index >= 0) {
		return(_lod->LODRanges[_index]);
	}
	return(-1);
}







long pfGetType(pfNode* _node)
{
	return(_node->Type);
}

long pfGetNumChildren(pfGroup* _node)
{
	return(_node->GroupInfo.NumChildren);
}

#if 0   /* This is troublesome on the ultra64 and really shouldn't be used */
		/* The code is left in in case it ever needs to be reworked in */

void pfAddChild(pfGroup* _group, pfNode* _child) 
{
	short i,j;
	
	if (_group != NULL && _child != NULL) {
		if (_group->NodeInfo.NumChildren < NUMCHILDREN && _child->NumParents < NUMPARENTS) {
			_group->NodeInfo.Children[_group->NodeInfo.NumChildren]=_child;
			_group->NodeInfo.NumChildren++;
			_child->Parents[_child->NumParents]=(pfNode *)_group;
			_child->NumParents++;
		}
	}
}

long pfRemoveChild(pfGroup* _group, pfNode* _child) 
{
	short i,j;
	pfNode *ThisChild;

	if (_group != NULL && _child != NULL) {
		i=0;
		while (i<_group->NodeInfo.NumChildren && _group->NodeInfo.Children[i]!=_child) i++;
		if (i>=_group->NodeInfo.NumChildren) return(0);
		 else {
			ThisChild=_group->NodeInfo.Children[i];
			_group->NodeInfo.NumChildren--;
			for (j=i;j < _group->NodeInfo.NumChildren; j++) {
				_group->NodeInfo.Children[j]=_group->NodeInfo.Children[j+1];
			}
			_group->NodeInfo.Children[_group->NodeInfo.NumChildren]=NULL;

			i=0;
			while (i<ThisChild->NumParents && ThisChild->Parents[i] != (pfNode *)(_group)) i++;
			if (i<ThisChild->NumParents) {
				 ThisChild->NumParents--;
				 for (j=i;j < ThisChild->NumParents; j++) {
					 ThisChild->Parents[j]=ThisChild->Parents[j+1];
				 }
			 }

			ThisChild->Parents[ThisChild->NumParents]=NULL;
			return(1);
		}
	} else return(0);
}
#endif



pfNode* pfGetChild(pfGroup* _group, long _childnum)
{
	if (_group == NULL) return(NULL);
	 else return((pfNode *)(_group->GroupInfo.Children[_childnum]));
}

/*
long pfGetNumParents(pfNode* _node)
{
	return(_node->NumParents);
}

pfNode* pfGetParent(pfNode* _node, long _parentnum)
{
	if (_node == NULL) return(NULL);
	 else return((pfNode *)(_node->Parents[_parentnum]));
}


*/


/*******************************************************************\

  			Geode and GeoSet routines 

\*******************************************************************/

long pfGetGSetNumPrims(pfGeoSet* _gset)
{
	return(_gset->NumPrims);
}

long pfGetGSetPrimType(pfGeoSet* _gset)
{
	return(_gset->PrimType);
}

long* pfGetGSetPrimLengths(pfGeoSet* _gset)
{
	return(_gset->PrimLengths);
}

void pfGetGSetAttrLists(pfGeoSet* _gset, long _attr, void** _alist, ushort** _ilist)
{
	if (_attr==PFGS_COORD3) {
		*_alist=_gset->VData; /*****  were Using one big MasterVertexList now */
/*		*_alist=MasterVertexList; */
		*_ilist=(ushort *)_gset->IData;
	} else {
		/* isect.c needs a vert list, and tmInf.c needs texture coordinates (to scroll) */
		*_alist=NULL;
		*_ilist=NULL;
	}
}

void pfGetGSetBBox(pfGeoSet* _gset, pfBox* _box)
{
	PFCOPY_VEC3(_box->min,_gset->BBox.min);
	PFCOPY_VEC3(_box->max,_gset->BBox.max);
}


long pfGetNumGSets(pfGeode* _geode)
{
	return(_geode->NumGSets);
}

pfGeoSet* pfGetGSet(pfGeode* _geode, long _index)
{

	return (_geode->GeoSets[_index]);
}


void pfGStateAttr(pfGeoState* _gs, long _attr, void* _a)
{
	if (_attr==PFSTATE_TEXTURE) {
		_gs->TheTexture=(pfTexture *)_a;
	}
}

long pfGetGStateMode(pfGeoState* _gs, long _mode)
{
	long RetVal;

	RetVal=-1;
	if (_gs!=NULL) {
		if (_mode==PFSTATE_CULLFACE) {
			if (_gs->Flags & PFGSTATE_BACKFACE_ELIM) {
				RetVal=PFCF_BACK;
			} else {
				RetVal=PFCF_OFF;
			}
			if (_gs->Flags & PFGSTATE_FRONTFACE_ELIM) {
				if (RetVal==PFCF_OFF) {
					RetVal=PFCF_FRONT;
				} else {
					RetVal=PFCF_BOTH;
				}
			}
		}
	}
	return(RetVal);
}

pfGeoState *pfGetGSetGState(pfGeoSet* _gs)
{
	return(_gs->TheGState);
}

/*******************************************************************\

  			channel routines 

\*******************************************************************/

long GetChannelCameraNum(pfChannel *chan)
{ return(chan->CurCameraNum); }

void SetChannelCameraNum(pfChannel *chan, long cam)
{ chan->CurCameraNum = cam; }

long GetChannelFlags(pfChannel *chan)
{ return(chan->Flags); }

void SetChannelFlags(pfChannel *chan, long flags)
{ SETFLAG(chan->Flags, flags); }

void ClearChannelFlags(pfChannel *chan, long flags)
{ CLRFLAG(chan->Flags, flags); }

long GetNumChannels(void)
{
	return(MAX_NUM_CHANNELS);
}

#if 0
pfChannel *GetChannel(long id)
{
	if (id < 0 || id >= MAX_NUM_CHANNELS) return(NULL);
	return(&(ChannelArray[id]));
}
#endif



/* Do matrix conversion to coordinate system of the target platform */





void pfChanViewOffsets(pfChannel *chan, pfVec3 xyz, pfVec3 hpr)
{
	pfMakeEulerMat(chan->ViewOffset,hpr[PF_H],hpr[PF_P],hpr[PF_R]);
	pfSetMatRowVec3(chan->ViewOffset,3,xyz);
	pfMultMat(chan->CamMatrix,chan->ViewOffset,chan->CamMatrixNotOffset);
/*	SetEye(chan->CamMatrix);  */
}


void pfChanView(pfChannel *chan, pfVec3 xyz, pfVec3 hpr)
{

	pfMakeEulerMat(chan->CamMatrixNotOffset,hpr[PF_H],hpr[PF_P],hpr[PF_R]);
	pfSetMatRowVec3(chan->CamMatrixNotOffset,3,xyz);
	pfMultMat(chan->CamMatrix,chan->ViewOffset,chan->CamMatrixNotOffset);
/*	SetEye(chan->CamMatrix);  */
}

void pfGetChanView(pfChannel *chan, pfVec3 xyz, pfVec3 hpr)
{
	pfCoord TCoord;
	
	pfGetOrthoMatCoord(chan->CamMatrixNotOffset,&TCoord);
	PFCOPY_VEC3(xyz,TCoord.xyz);
	PFCOPY_VEC3(hpr,TCoord.hpr);
}



void pfChanViewMat(pfChannel *chan, pfMatrix ChanMat)
{
	pfMatrix TMat;
	pfVec3 Hold1,Hold2;
	float Hold;
	
	PFCOPY_MAT(chan->CamMatrixNotOffset,ChanMat);
	pfMultMat(chan->CamMatrix,chan->ViewOffset,chan->CamMatrixNotOffset);
/*	SetEye(chan->CamMatrix);  */
}


void pfChanLODAttr(pfChannel* _chan, long _attr, float _val)
{
	switch (_attr) {
		case PFLOD_SCALE:
			_chan->LODScale=_val;
			break;
		case PFLOD_FADE:
			_chan->LODFade=_val;
			break;
		case PFLOD_FRUST_SCALE:
			_chan->LODFrustScale=_val;
			break;
		default: ;
	}
}

float pfGetChanLODAttr(pfChannel* _chan, long _attr)
{
	switch (_attr) {
		case PFLOD_SCALE:
			return(_chan->LODScale);
			break;
		case PFLOD_FADE:
			return(_chan->LODFade);
			break;
		case PFLOD_FRUST_SCALE:
			return(_chan->LODFrustScale);
			break;
		default: return(-1.0f);
	}
}


void pfChanScene(pfChannel* _chan, pfGroup *SceneRoot)
{
	_chan->SceneRoot=SceneRoot;
}

void pfChanTravMask(pfChannel* _chan, long _trav, long _mask)
{
	if (_trav==WOLFPFCHANTRAV_DRAWANY) {
		_chan->DrawTravMaskAny=_mask;
	} 
	if (_trav==WOLFPFCHANTRAV_ISECTANY) {
		_chan->ISectTravMaskAny=_mask;
	}
	if (_trav==WOLFPFCHANTRAV_DRAWALL) {
		_chan->DrawTravMaskAll=_mask;
	} 
	if (_trav==WOLFPFCHANTRAV_ISECTALL) {
		_chan->ISectTravMaskAll=_mask;
	} 
}

	
long pfGetChanTravMask(pfChannel* _chan, long _trav)
{

	if (_trav==WOLFPFCHANTRAV_DRAWANY) {
		return(_chan->DrawTravMaskAny);
	} else if (_trav==WOLFPFCHANTRAV_ISECTANY) {
		return(_chan->ISectTravMaskAny);
	} else if (_trav==WOLFPFCHANTRAV_DRAWALL) {
		return(_chan->DrawTravMaskAll);
	} else if (_trav==WOLFPFCHANTRAV_ISECTALL) {
		return(_chan->ISectTravMaskAll);
	} else {
		return(-1);
	}
}




long pfNodeTravMask(pfNode* _node, long _which, ulong _mask, long _setMode, long _bitOp)
{
	long *ValPtr;
	long i;
	long NodeType;

	if (_node==NULL) return(0);

	if (_which==PFTRAV_ISECT) {
		ValPtr=&(_node->ISectTravMask);
	}  else if (_which==PFTRAV_DRAW) {
		ValPtr=&(_node->DrawTravMask);
	} else {
		return(0);
	}
	if (_setMode & PFTRAV_SELF) {
		if (_bitOp==PF_OR) {
			*ValPtr |= _mask;
		} else if (_bitOp==PF_AND) {
			*ValPtr &= _mask;
		} else if (_bitOp==PF_SET) {
			*ValPtr=_mask;
		}
	}

	if (_setMode & PFTRAV_DESCEND) {
	
		NodeType=pfGetType(_node);

		if (NodeType & PFCLASS_GROUP) {
			for (i=0;i<pfGetNumChildren((pfGroup *)_node);i++) {
				pfNodeTravMask(pfGetChild((pfGroup *)_node,i),_which,_mask,(_setMode & PFTRAV_SELF),_bitOp);
			}
		}
	}
	return(0);
}

	

ulong pfGetNodeTravMask(pfNode* _node, long _which)
{
	long RetVal;
	short Res;

	if (_which==PFTRAV_ISECT) {
		return(_node->ISectTravMask);
	}  else if (_which==PFTRAV_DRAW) {
		return(_node->DrawTravMask);
	} else {
		return(0);
	}
}







/***********************************************\
   Fox pf Extensions
\***********************************************/



void foxpfInitNode(pfNode *_node, long Type)
{
	_node->Type=Type;
	_node->DrawTravMask=0xffffffff;
	_node->ISectTravMask=0xffffffff;
	_node->BitFields=0;
#if 0	
	_node->CleanFrameNum=-1;
	_node->CleanDisplayList=0;
#else	
	_node->SpecialFunction1=0;
	_node->SpecialFunction2=0;
#endif

	if (Type & PFCLASS_GROUP) {
		((pfGroup *)(_node))->GroupInfo.NumChildren=0;
		((pfGroup *)(_node))->GroupInfo.Children=NULL;

		if (Type==PFTYPE_DCS) {
			PFSET_VEC3(((pfDCS *)(_node))->Pivot,0.0f,0.0f,0.0f);
			PFSET_VEC3(((pfDCS *)(_node))->Matrix[0],1.0f,0.0f,0.0f);
			PFSET_VEC3(((pfDCS *)(_node))->Matrix[1],0.0f,1.0f,0.0f);
			PFSET_VEC3(((pfDCS *)(_node))->Matrix[2],0.0f,0.0f,1.0f);
			PFSET_VEC3(((pfDCS *)(_node))->Matrix[3],0.0f,0.0f,0.0f);
		}
	}
}

void *foxpfGetGSetAttrs(pfGeoSet *_gset)
{
	return(_gset->Attributes);
}


void foxpfGSetAttrs(pfGeoSet *_gset, void *_data)
{
	_gset->Attributes=_data;
}




#undef INFOXPF
