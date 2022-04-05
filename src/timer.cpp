/********************************************************************************\
	timer.c

	Some simple code to keep the time in seconds. Many many
	other functions will use DeltaTime.
\********************************************************************************/

#include "genincludes.h"

#include <stdio.h>
#include "timer.h"
#include <SDL2/SDL.h>

float	NowTime = 0.0f;
float	LastTime = 0.0f;
float	DeltaTime = 1.0f;
float	UnscaledDeltaTime = 1.0f;
float	LastFrameRate = 0.0f;
float	FrameRateCounterTotal = 0.0f;
int		FrameRateCounter = 0;
float	DeltaTimeCap = 1.0f / 15.0f;
float	gTimeScale = 1.0f;


uint32 GetMillisecondTimer(void)
{
	return (double)SDL_GetTicks();
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

