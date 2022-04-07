/******************************************************************************\

	types.h - standard universal typedefs
   
	NOT ENGINE SPECIFIC
	NOT GAME SPECIFIC
	NOT PLATFORM SPECIFIC
	
	LucasArts Proprietary and Confidential.
	If found, drop in any mailbox.  Postage is pre-paid.
	Do not submerge or take internally.
	No user-serviceable parts inside.
	Any similarity to actual persons, living or dead, is entirely coincidental.
	This end up.
	Do not stack more than four high.
	Dry clean only.
	One size fits all.
	
\******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char	byte;
typedef unsigned char	uchar;

#ifndef _SYS_BSD_TYPES_H
typedef unsigned short	ushort;
typedef unsigned long	ulong;
#endif

typedef unsigned char	uint8;
typedef signed char		int8;
typedef unsigned short	uint16;
typedef signed short	int16;
typedef unsigned int	uint32;
typedef signed int		int32;

#define Boolean long

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif	/* TYPES_H */


