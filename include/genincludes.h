
/************************************************************\
	genincludes.h
\************************************************************/

#ifndef GENINCLUDES_H
#define GENINCLUDES_H

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#define MY_SGN(x)	(((x) < 0) ? -1 : 1)
#define SETFLAG(val,flag) (val |= (flag))
#define CLRFLAG(val,flag) (val &= ~(flag))

#include "types.h"
#include "foxpf.h"

#endif /* GENINCLUDES_H */

