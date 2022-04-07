/**** timer.h ****/

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
//extern "C" {
#endif

extern float	NowTime;
extern float	LastTime;
extern float	DeltaTime;
extern float	UnscaledDeltaTime;
extern float	LastFrameRate;
extern float	gTimeScale;
extern float	FrameRateCounterTotal;
extern int		FrameRateCounter;

void UpdateDeltaTime(void);
uint32 GetMillisecondTimer(void);


#ifdef __cplusplus
//}
#endif
#endif /**** _TIMER_H_ ****/

