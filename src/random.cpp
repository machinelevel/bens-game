/********************************************************************************\
	random.c
	
	Just a simple and non-excellent random number generator.
\********************************************************************************/

#include "random.h"

#ifdef __cplusplus
extern "C" {
#endif

static int	randomSeed = 0x2750250;

void SetRandomSeed(int seed)
{
    randomSeed = seed;
}

int GetRandomInt(void)
{
    randomSeed = randomSeed * 1103515245 + 12345;

	if (randomSeed == 0x80000000) return(0);
	if (randomSeed < 0) {
		if (randomSeed <  0x80000041) return(0x7fffffbf);
		return(-randomSeed);
	} else {
		if (randomSeed >  0x7fffffbf) return(0x7fffffbf);
		return(randomSeed);
	}
}

#ifdef __cplusplus
}
#endif

