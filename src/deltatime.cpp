/************************************************************\
	deltatime.c
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "genincludes.h"
#include "umath.h"
#include "upmath.h"
#include "deltatime.h"

double	NowTime = 0.0f;
double	LastTime = 0.0f;
double	DeltaTime = 1.0f;
double	LastFrameRate = 0.0f;
double	FrameRateCounterTotal = 0.0f;
long	FrameRateCounter = 0;

void UpdateDeltaTime(void)
{
	LastTime = NowTime;

	NowTime = ((double)SDL_GetTicks()) * 0.001f;

	DeltaTime = NowTime - LastTime;
	if (DeltaTime != 0.0f) {
		FrameRateCounterTotal += 1.0f / DeltaTime;
		FrameRateCounter++;
		if (FrameRateCounter > 20) {
			LastFrameRate = FrameRateCounterTotal / FrameRateCounter;
			FrameRateCounter = 0;
			FrameRateCounterTotal = 0.0f;
		}
	}
//	printf("Now = %f, delta = %f, fr = %f\n", (float)NowTime, (float)DeltaTime, (float)LastFrameRate);
}

