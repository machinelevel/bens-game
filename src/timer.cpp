/********************************************************************************\
	timer.c

	Some simple code to keep the time in seconds. Many many
	other functions will use DeltaTime.
\********************************************************************************/

#include "genincludes.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include <stdio.h>
#include "timer.h"

#if MAC_BUILD
#include "macutils.h"
#endif

float	NowTime = 0.0f;
float	LastTime = 0.0f;
float	DeltaTime = 1.0f;
float	UnscaledDeltaTime = 1.0f;
float	LastFrameRate = 0.0f;
float	FrameRateCounterTotal = 0.0f;
int		FrameRateCounter = 0;
float	DeltaTimeCap = 1.0f / 15.0f;
float	gTimeScale = 1.0f;

#ifdef __cplusplus
//extern "C" {
#endif

uint32 GetMillisecondTimer(void)
{
#ifdef WIN32
	static uint32 startTimeMS = 0;

	if (1) {
		LARGE_INTEGER	count, freq;
		uint32			ms;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&count);
		ms = (uint32)((count.QuadPart * 1000) / freq.QuadPart);
		if (startTimeMS == 0) {
			startTimeMS = ms;
		}
		ms -= startTimeMS;
//		printf("ms = %u, freq = %u\n", ms, ((uint32)freq.QuadPart));
		return(ms);
	}
#endif

#if MAC_BUILD
	return (mac_get_time());
#endif

}

void UpdateDeltaTime(void)
{
	LastTime = NowTime;

	NowTime = (float)(((double)GetMillisecondTimer()) * ((double)0.001f));
	DeltaTime = (NowTime - LastTime);
	UnscaledDeltaTime = DeltaTime;		/// for debugging and HUD stuff
	DeltaTime *= gTimeScale;
	if (DeltaTime > DeltaTimeCap) DeltaTime = DeltaTimeCap;
	if (DeltaTime < 0.0f) DeltaTime = 0.00001f;

	if (DeltaTime != 0.0f) {
		FrameRateCounterTotal += 1.0f / DeltaTime;
		FrameRateCounter++;
		if (FrameRateCounter > 60) {
			LastFrameRate = FrameRateCounterTotal / FrameRateCounter;
			FrameRateCounter = 0;
			FrameRateCounterTotal = 0.0f;
		}
	}
}

#ifdef __cplusplus
//}
#endif
