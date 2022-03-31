/*
 *	macsound.cpp
 */

#include "macsound.h"
#include "tsufile.h"

#if __MACH__
	#include <Quicktime/Quicktime.h>
#else
	#include <Quicktime.h>
#endif

SOUNDTYPE *loadSound(char *name)
{
	int32	result = 0;
	SOUNDTYPE	*snd = NULL;
//	HRESULT hr;
	char	*tsuptr;
	uint32	tsusize;
	byte	*pdata;
	uint32	size;
//	LPWAVEFORMATEX format;

#if 0
	if (!gSoundMgr) {
		HWND	hw = NULL;
		hw = GetForegroundWindow();
		gSoundMgr = new CSoundManager;
		hr = gSoundMgr->Initialize(hw, DSSCL_PRIORITY);
		hr = gSoundMgr->SetPrimaryBufferFormat( 2, 22050, 16 );
	}
	if (!gSoundMgr) return(NULL);
#endif

	if ((name == NULL) || (name[0] == 0)) return(NULL);

	tsuMarkFile(name);
	tsuptr = (char*)tsuGetMemFile(name, &tsusize);
	if (tsuptr) 
	{
		snd = new SOUNDTYPE;
		if (snd != NULL)
		{
			snd->bufferSize = tsusize;
			snd->buffer = tsuptr;
		}
	}

	return(snd);
}


void mac_sound_play(int32 id, float volume, float pitch, float pan)
{
    Handle                  myHandle, dataRef = nil;
    Movie                   movie;
    MovieImportComponent    miComponent;
    Track                   targetTrack = nil;
    TimeValue               addedDuration = 0;
    long                    outFlags = 0;
    OSErr                   err;
    ComponentResult         result;
	Ptr waveDataPtr;
	long waveDataSize;
	
	waveDataSize = gSoundSampleList[id].csnd->bufferSize - 40;
	waveDataPtr = gSoundSampleList[id].csnd->buffer + 40;

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

    SetMovieVolume(movie, kFullVolume);
    GoToBeginningOfMovie(movie);
    StartMovie(movie);
    while (!IsMovieDone(movie))
    {
        MoviesTask(movie, 0);
        err = GetMoviesError();
    }
}

/*
OSErr PlayWAVfile ()
{
    SFTypeList        myTypeList;
    StandardFileReply myReply;
    OSErr             err = noErr;
    short             movieRefNum, resID = 0;
    Movie             movie;

        StandardGetFilePreview(NULL, -1, myTypeList, &myReply);
        if (!myReply.sfGood)
        {
            err = userCanceledErr;
            return err;
        }
        else
        {
            err = OpenMovieFile(&myReply.sfFile, &movieRefNum, fsRdPerm);
            if (!err)
            {
                err = NewMovieFromFile(&movie,
                                       movieRefNum,
                                       &resID,
                                       NULL,
                                       newMovieActive,
                                       NULL);
            }

            if (err)
            {
                if (movie)
                {
                    DisposeMovie(movie);
                }
            }
            else
            {
                SetMovieVolume(movie, kFullVolume);
                GoToBeginningOfMovie(movie);
                StartMovie(movie);
                while (!IsMovieDone(movie))
                {
                    MoviesTask(movie, 0);
                    err = GetMoviesError();
                }
            }
        }

        return err;
} 


 void ImportWAVDataFromMemory(Ptr waveDataPtr, long waveDataSize)
{
    Handle                  myHandle, dataRef = nil;
    Movie                   movie;
    MovieImportComponent    miComponent;
    Track                   targetTrack = nil;
    TimeValue               addedDuration = 0;
    long                    outFlags = 0;
    OSErr                   err;
    ComponentResult         result;

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

        SetMovieVolume(movie, kFullVolume);
        GoToBeginningOfMovie(movie);
        StartMovie(movie);
        while (!IsMovieDone(movie))
        {
            MoviesTask(movie, 0);
            err = GetMoviesError();
        }
} 

*/