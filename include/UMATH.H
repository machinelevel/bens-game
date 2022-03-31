#ifndef UMATH_H
#define UMATH_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif



#define COS_00 1.0f
#define COS_30 0.866f
#define COS_45 0.7071f
#define COS_60 0.5f
#define COS_90 0.0f

#define FLOORF(x) ( ((x)<0) ? ((long)(x-0.999999f)) : ((long)(x))  )
#define MY_SGN(x)	(((x) < 0) ? -1 : 1)

			/* Return TRUE if value is within range */
#define INRANGE(val,lo,hi) (((val)>=(lo))?(((val)<=(hi))?TRUE:FALSE):FALSE)

			/* Caps a value to the given range */
#define RANGELIMIT(val,lo,hi) (((val)>=(lo))?(((val)<=(hi))?val:hi):lo)

/**** interps simply as T goes from zero to one. ****/
#define SIMPLE_INTERP(_t, _lo, _hi) ((((_hi)-(_lo))*(_t))+(_lo))

#define SETFLAG(val,flag) (val |= (flag))
#define CLRFLAG(val,flag) (val &= ~(flag))
#define XORFLAG(val,flag) (val ^= flag)
#define SETMASKFLAG(val,mask,flag) (val =  ((val) & ~(mask)) | (flag))
#define GETMASKFLAG(val,mask) (val & (mask))


/* the following macros operate on the nth bit of memory following any pointer */

#define	SET_PTR_BIT(_bit, _ptr) ((long *)(_ptr))[(_bit) >> 5] |= (1 << (31 - ((_bit) & 31)))
#define	CLEAR_PTR_BIT(_bit, _ptr) ((long *)(_ptr))[(_bit) >> 5] &= (~(1 << (31 - ((_bit) & 31))))
#define	GET_PTR_BIT(_bit, _ptr) ((((long *)(_ptr))[(_bit) >> 5] & (1 << (31 - ((_bit) & 31)))) != 0)

// the following macros operate on the nth bit of a long

#define	GET_LONG_BIT(_val, _num) ((_val & (1 << _num)) != 0)
#define	CLR_LONG_BIT(_val, _num) (_val &= (~(1 << _num)))
#define	SET_LONG_BIT(_val, _num) (_val |= (1 << _num))

#define ARRAY_COUNT(_a)	(sizeof(_a) / sizeof(_a[0]))



float umGeneralCurveClamp(float inval, long power, float scale, float clamp);
float umRampDownCalc(float RampParam, float DeltaTime);

#if (COMPILE_TYPE==CT_PC)

// #ifndef __MWERKS__
//remedial math
double sin(double x);
double cos(double x);
double sqrt(double x);

#ifndef sqrtf
#define sqrtf(_x)	((float)sqrt((double)(_x)))
#endif
#ifndef sinf
#define sinf(_x)	((float)sin((double)(_x)))
#endif
#ifndef cosf
#define cosf(_x)	((float)cos((double)(_x)))
#endif
// #endif __MWERKS__

#define TWIST_LONG(_x)	((((*(unsigned long *)&(_x)) << 24) & 0xff000000) | (((*(unsigned long *)&(_x)) <<  8) & 0x00ff0000) | (((*(unsigned long *)&(_x)) >>  8) & 0x0000ff00) | (((*(unsigned long *)&(_x)) >> 24) & 0x000000ff))
//#define TWIST_LONG(_x)	(((((unsigned long)(_x)) << 24) & 0xff000000) | ((((unsigned long)(_x)) <<  8) & 0x00ff0000) | ((((unsigned long)(_x)) >>  8) & 0x0000ff00) | ((((unsigned long)(_x)) >> 24) & 0x000000ff))
#define TWIST_SHORT(_x)	((((*(unsigned short *)&(_x)) <<  8) & 0x0000ff00) | (((*(unsigned short *)&(_x)) >>  8) & 0x000000ff))
//#define TWISTED(_x)	((sizeof(_x) == 4) ? TWIST_LONG(_x) : ((sizeof(_x) == 2) ? TWIST_SHORT(_x) : (_x)))
//#define TWIST(_x)	{_x = TWISTED(_x);}
#define TWIST(_x)	{ if (sizeof(_x) == 4) *(unsigned long *)&(_x) = TWIST_LONG(_x); else if (sizeof(_x) == 2) *(unsigned short *)&(_x) = TWIST_SHORT(_x);}
#define TWIST_LONG_ARRAY(_x) { long _i; for (_i = 0; _i < sizeof(_x) / sizeof(long); _i++) TWIST(((long *)&(_x))[_i]); }
#define TWIST_SHORT_ARRAY(_x) { long _i; for (_i = 0; _i < sizeof(_x) / sizeof(short); _i++) TWIST(((short *)&(_x))[_i]); }
#define TWIST_N_LONGS(_x, _n) { long _i; for (_i = 0; _i < _n; _i++) TWIST(((long *)_x)[_i]); }
#define TWIST_N_SHORTS(_x, _n) { long _i; for (_i = 0; _i < _n; _i++) TWIST(((short *)_x)[_i]); }

#else

#define TWIST_LONG(_x)	((((*(unsigned long *)&(_x)) << 24) & 0xff000000) | (((*(unsigned long *)&(_x)) <<  8) & 0x00ff0000) | (((*(unsigned long *)&(_x)) >>  8) & 0x0000ff00) | (((*(unsigned long *)&(_x)) >> 24) & 0x000000ff))
#define TWIST_SHORT(_x)	((((*(unsigned short *)&(_x)) <<  8) & 0x0000ff00) | (((*(unsigned short *)&(_x)) >>  8) & 0x000000ff))
#define TWIST(_x)	{ if (sizeof(_x) == 4) *(unsigned long *)&(_x) = TWIST_LONG(_x); else if (sizeof(_x) == 2) *(unsigned short *)&(_x) = TWIST_SHORT(_x);}
#define TWIST_LONG_ARRAY(_x)
#define TWIST_SHORT_ARRAY(_x)
#define TWIST_N_LONGS(_x, _n)
#define TWIST_N_SHORTS(_x, _n)
#endif

#endif

// U_MATH_H
