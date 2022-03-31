#if 0
/*
 *	macsound.cpp
 *
 *  Copyright © 2004 Make-a-Wish Foundation.  All Rights Reserved.
 *
 */

#ifdef __MACH__
	#include <Carbon/Carbon.h>
	#include <CarbonSound/Sound.h>
	#include <CarbonCore/FixMath.h>
	#include <Quicktime/Quicktime.h>
#else
	#include <FixMath.h>
	#include <ConditionalMacros.h>
	#include <Movies.h>
	#include <QuickTimeComponents.h>
	#include <Sound.h>
	#include <Folders.h>
	#include <ToolUtils.h>
	#include <Gestalt.h>
	#include <Navigation.h>
#endif

#include "types.h"
#include "random.h"
#include "sound.h"
#include "tsufile.h"
#include "umath.h"
#include "foxpf.h"
#include "slides.h"

#include "macutils.h"

SOUNDTYPE *loadSound(char *name)
{
	int32			result = 0;
	SOUNDTYPE		*snd = NULL;
	char			*tsuptr;
	uint32			tsusize;
	char			*pdata;
	uint32			size;
	WAVEFORMATEX 	*format;

	if ((name == NULL) || (name[0] == 0)) return(NULL);

	tsuMarkFile(name);
	tsuptr = (char*)tsuGetMemFile(name, &tsusize);
	if (tsuptr) 
	{
		snd = new (SOUNDTYPE);
		if (snd != NULL)
		{
			snd->sndMoviePtr = NULL;
			snd->sndMovieDataPtr = NULL;			
			snd->dataSize = tsusize;
			snd->tsuData = tsuptr;
			snd->format = (WAVEFORMATEX *)(tsuptr+20);
			snd->pData = (char *)(tsuptr+40);
			snd->pDataSize = tsusize-40;
/*
			// Now twist the data
			TWIST(snd->format->wFormatTag);
			TWIST(snd->format->nChannels); 
			TWIST(snd->format->nSamplesPerSec); 
			TWIST(snd->format->nAvgBytesPerSec); 
			TWIST(snd->format->nBlockAlign); 
			TWIST(snd->format->wBitsPerSample); 
			TWIST(snd->format->cbSize); 
*/
		}
	}
	
	return(snd);
}

void mac_sound_free()
{
	int index;
	
	for (index = 1; index < gSizeOfSoundSampleList; index++)
	{
		if (gSoundSampleList[index].csnd != NULL)
		{
			if (gSoundSampleList[index].csnd->sndMoviePtr != NULL)
			{
				DisposeHandle( (Handle) gSoundSampleList[index].csnd->sndMovieDataPtr);
				DisposeMovie((Movie) gSoundSampleList[index].csnd->sndMoviePtr);
				gSoundSampleList[index].csnd->sndMovieDataPtr = NULL;
				gSoundSampleList[index].csnd->sndMoviePtr = NULL;
			}
		}
	}
}


void mac_sound_play(int32 id, int32 volume, Fixed pitch, int32 pan)
{
    Handle                  myHandle, dataRef = nil;
    Movie                   movie;
    MovieImportComponent    miComponent;
    Track                   targetTrack = nil;
    TimeValue               addedDuration = 0;
    long                    outFlags = 0;
    OSErr                   err;
    ComponentResult         result;
	Ptr 					waveDataPtr;
	long					waveDataSize;
	Fixed					curRate;
	
	if (gSoundSampleList[id].csnd->sndMoviePtr == NULL)
	{
		waveDataSize = gSoundSampleList[id].csnd->dataSize;
		waveDataPtr = (char *) gSoundSampleList[id].csnd->tsuData;	// pData;

	    myHandle = NewHandleClear((Size)waveDataSize);
	    BlockMove(waveDataPtr,
	                *myHandle,
	                waveDataSize);

	    err = PtrToHand(&myHandle,
	                    &dataRef,
	                    sizeof(Handle));

	    miComponent = OpenDefaultComponent(MovieImportType,
	                                        kQTFileTypeWave);
	    movie = NewMovie(0);

	    result = MovieImportDataRef(miComponent,
	                                dataRef,
	                                HandleDataHandlerSubType,
	                                movie,
	                                nil,
	                                &targetTrack,
	                                nil,
	                                &addedDuration,
	                                movieImportCreateTrack,
	                                &outFlags);
		
		gSoundSampleList[id].csnd->sndMoviePtr = (void *) movie;
		gSoundSampleList[id].csnd->sndMovieDataPtr = (void *) myHandle;
	}
	else
	{
		movie = (Movie) gSoundSampleList[id].csnd->sndMoviePtr;
		result = noErr;
	}
	
	
    if (result == noErr)
    {
	    GoToBeginningOfMovie(movie);
	    SetMovieVolume(movie, kFullVolume);
	    StartMovie(movie);
		SetMovieRate(movie, pitch);

/*	    while (!IsMovieDone(movie))
	    {
	        MoviesTask(movie, 0);
	        err = GetMoviesError();
	    }
*/
	}
}


void playSound2D(int32 id, float volume, float pitch, float pan)
{
	int32	ipitch;
	int32	ivolume;
	int32	sampleID;

	if (id < 0 || id >= SOUND_HOWMANY) 
	{
		printf("sound id is out of range\n");
		return;
	}

	if (gSoundLinks[id].numSamples == 0) 
	{
		return;
	}

	sampleID = gSoundLinks[id].sampleList[(RANDOM_INT(0, 65535)) % gSoundLinks[id].numSamples];

	if (sampleID < 0 || sampleID >= gSizeOfSoundSampleList)
	{
		return;
	}

	if (gSoundSampleList[sampleID].csnd == NULL) 
	{
		printf("sound is NULL\n");
		return;
	}

//	printf("playing sound\n");

	if (gSoundSampleList[sampleID].flags & SOUND_FLAG_VOICE) 
	{
		if (!gOptionVoicesOn) return;
		pitch *= gChipmunkFactor;
	} else {
		if (!gOptionSoundEffectsOn) {
			return;
		}
	}

	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
//	ivolume = (int)logf(volume);
//	if (ivolume < -10000) ivolume = -10000;
//	if (ivolume > 0) ivolume = 0;

	ipitch = (int32)(pitch * 22050);
	ivolume = (volume * 10000.0f) - 10000;
//printf("snd vol:%f ivol:%d pitch:%f\n", volume, ivolume, pitch);
//	DSUtil_PlaySound(snd);
//	snd->Play(0xFFFFFFFF, 0, DSBVOLUME_MAX, DSBFREQUENCY_ORIGINAL, DSBPAN_CENTER);//, volume, pitch, pan);
//	gSoundList[id].csnd->Play(0, 0);
//	gSoundSampleList[sampleID].csnd->Play(0, 0, ivolume, ipitch, 0);

	mac_sound_play(sampleID, ivolume, (Fixed) (pitch * fixed1), pan);
	return;
}

#endif
