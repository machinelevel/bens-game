/**** random.h ****/

#ifndef _RANDOM_H_
#define _RANDOM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Float randoms do not return the equivalent of 1.0f */

#define		RANDOM0TO1	((GetRandomInt()/2147483648.0f))
#define		RANDOMPLUSMINUS	((GetRandomInt()/1073741824.0f)-1.0f)
#define		RANDOM_IN_RANGE(_min,_max)	((_min)+(((_max)-(_min))*RANDOM0TO1))
#define		RANDOM() ((int)(GetRandomInt()))
#define		RANDOM_INT(_min,_max)	((_min)+((int)(((_max)-(_min)+1)*RANDOM0TO1)))

void SetRandomSeed(int seed);
int GetRandomInt(void);


#ifdef __cplusplus
}
#endif
#endif /**** _RANDOM_H_ ****/

