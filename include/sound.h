
/************************************************************\
	sound.h
	Some general sound code
\************************************************************/

#ifndef _BENSGAME_SOUND_H_
#define _BENSGAME_SOUND_H_

#define SOUND_FLAG_VOICE		0x00000001
#define SOUND_FLAG_RAPID_FIRE	0x00000002

enum {
	SOUND_NULL,
	SOUND_TEST1,
	SOUND_TEST2,

	SOUND_BUBBLE_POP,
	SOUND_ELECTRICAL_BOING,
	SOUND_HEALTH_FILL,
	SOUND_BUTTON_GROW,
	SOUND_BUTTON_CLICK,
	SOUND_BARF_LAUNCH,
	SOUND_BARF_SPLAT,	
	SOUND_JET_1,	
	SOUND_JET_2,
	SOUND_BEEP_1,
	SOUND_FIRE_1,

	SOUND_MORTAR_1,		
	SOUND_BLASTER_1,		
	SOUND_SWORD_1,		
	SOUND_SIZZLE_1,		
	SOUND_SWISH_1,		

	SOUND_BOSS_VOICE,
	SOUND_CHICKEN_VOICE,

	VO_BEN_BENSGAME,	
	VO_BEN_GOTIT,		
	VO_BEN_IWIN,		
	VO_BEN_OW,		
	VO_BEN_PRESSSTART,
	VO_BEN_WHOOPEE,	
	VO_BEN_YEAH,		

	VO_BOTH_COMPLETE,	
	VO_BOTH_LEVEL1,	
	VO_BOTH_LEVEL2,	
	VO_BOTH_LEVEL3,	
	VO_BOTH_LEVEL4,	
	VO_BOTH_LEVEL5,	
	VO_BOTH_LEVEL6,	
	VO_BOTH_LEVEL7,	
	VO_BOTH_YEAH,		

	VO_SET_OW,		
	VO_SET_BENSGAME,	
	VO_SET_GOTIT,		
	VO_SET_IWIN,		
	VO_SET_PRESSSTART,
	VO_SET_WHOOPEE,	
	VO_SET_YEAH,		

	SOUND_HOWMANY
};

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

struct SdlSound
{
	Mix_Chunk* chunk;
};
#define SOUNDTYPE SdlSound

#if MAC_BUILD

typedef struct { 
  int16  wFormatTag; 
  int16  nChannels; 
  int32 nSamplesPerSec; 
  int32 nAvgBytesPerSec; 
  int16  nBlockAlign; 
  int16  wBitsPerSample; 
  int16  cbSize; 
} WAVEFORMATEX; 

typedef struct macsound_type
{
	void			*sndMoviePtr;
	void			*sndMovieDataPtr;
	int32			dataSize;
	char			*tsuData;
	WAVEFORMATEX	*format;
	char 			*pData;
	int32			pDataSize;		
} macsound_type;

#define SOUNDTYPE	macsound_type

// periodic
void SndSnip_CheckBuffers (void);

#endif

typedef struct SoundSampleStruct {
	int32	soundID;
	char	*fileName;	/**** the file name ****/
	int32	flags;
	SOUNDTYPE	*csnd;		/**** the OS sound pointer ****/
} SoundSampleStruct;

typedef struct SoundLinkStruct {
	int32	numSamples;
	int32	*sampleList;
} SoundLinkStruct;

// Globals
extern float				gChipmunkFactor;
extern SoundLinkStruct		gSoundLinks[SOUND_HOWMANY];
extern SoundSampleStruct	gSoundSampleList[];
extern int32				gSizeOfSoundSampleList;

void initAllSounds(void);
void playSound2D(int32 id, float volume=1.0f, float pitch=1.0f, float pan=0.0f);
SOUNDTYPE *loadSound(char *name);

#endif	// _BENSGAME_SOUND_H_