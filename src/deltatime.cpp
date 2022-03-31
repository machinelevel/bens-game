/************************************************************\
	deltatime.c
\************************************************************/

#include <stdio.h>
#include <glut.h>
#include "genincludes.h"
#include "umath.h"
#include "upmath.h"
#include "deltatime.h"

#ifdef _WIN32
#define GLUT_TIMER_BROKEN	1
#else
#define GLUT_TIMER_BROKEN	0
#endif

#if GLUT_TIMER_BROKEN
#include <windows.h>
#include <mmsystem.h>
#endif

double	NowTime = 0.0f;
double	LastTime = 0.0f;
double	DeltaTime = 1.0f;
double	LastFrameRate = 0.0f;
double	FrameRateCounterTotal = 0.0f;
long	FrameRateCounter = 0;

void UpdateDeltaTime(void)
{
	LastTime = NowTime;

#if GLUT_TIMER_BROKEN
	NowTime = ((double)timeGetTime()) * 0.001f;
#else
	NowTime = ((double)glutGet(GLUT_ELAPSED_TIME)) / (double)1000;
#endif

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

