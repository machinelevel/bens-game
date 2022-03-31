
/************************************************************\
	sound.cpp
	Some general sound code
\************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <math.h>
#include "genincludes.h"
#include "umath.h"
#include "upmath.h"
#include "camera.h"
#include "controls.h"
#include "timer.h"
#include "slides.h"
#ifdef WIN32
#include "dsutil.h"
#include "dxutil.h"
#endif
#include "sound.h"
#include "random.h"
#include "tsufile.h"

float	gChipmunkFactor = 1.5f*0.85F;
bool	gRapidFire = false;

SoundLinkStruct			gSoundLinks[SOUND_HOWMANY];
SoundSampleStruct		gSoundSampleList[] = {
	{ NULL,																		0, NULL },
	{ SOUND_TEST1,				"sounds/test.wav",								0, NULL },
	{ SOUND_TEST2,				"sounds/test.wav",								0, NULL },
	{ SOUND_BUBBLE_POP,			"sounds/Lo_Res/Bubble_Pop.wav",					0, NULL },
	{ SOUND_ELECTRICAL_BOING,	"sounds/Lo_Res/Electrical_Boing.wav",			0, NULL },
	{ SOUND_HEALTH_FILL,		"sounds/Lo_Res/Health_Fill.wav",				0, NULL },
	{ SOUND_BUTTON_GROW,		"sounds/Lo_Res/Bubbles_27_12.wav",				0, NULL },
	{ SOUND_BUTTON_CLICK,		"sounds/Lo_Res/bubblesMud_01.wav",				0, NULL },
	{ SOUND_BARF_LAUNCH,		"sounds/Lo_Res/wetJuicyImpact_01.wav",			0, NULL },
	{ SOUND_BARF_SPLAT,			"sounds/Lo_Res/wetJuicyImpact_02.wav",			0, NULL },
	{ SOUND_JET_1,				"sounds/Lo_Res/Gum_0533_blow_bubble.wav",			0, NULL },
	{ SOUND_JET_2,				"sounds/Lo_Res/Gum_0533_blow_bubble.wav",			0, NULL },
	{ SOUND_BEEP_1,				"sounds/Lo_Res/BEEPFreq_01.wav",			0, NULL },
	{ SOUND_FIRE_1,				"sounds/Lo_Res/wetJuicyImpact_04.wav",			SOUND_FLAG_RAPID_FIRE, NULL },

	{ SOUND_MORTAR_1,				"sounds/Lo_Res/shot_ben_mortar_02.wav",			0, NULL },
	{ SOUND_MORTAR_1,				"sounds/Lo_Res/shot_ben_mortar_01.wav",			0, NULL },
	{ SOUND_BLASTER_1,				"sounds/Lo_Res/shot_ben_blaster.wav",			SOUND_FLAG_RAPID_FIRE, NULL },
	{ SOUND_SWORD_1,				"sounds/Lo_Res/impact_ben_sword_02.wav",			0, NULL },
	{ SOUND_SWORD_1,				"sounds/Lo_Res/impact_ben_sword_01.wav",			0, NULL },
	{ SOUND_SIZZLE_1,				"sounds/Lo_Res/impact_ben_sizzle_01.wav",			0, NULL },
	{ SOUND_SIZZLE_1,				"sounds/Lo_Res/impact_ben_sizzle_02.wav",			0, NULL },
	{ SOUND_SIZZLE_1,				"sounds/Lo_Res/impact_ben_sizzle_03.wav",			0, NULL },
//	{ SOUND_SIZZLE_1,				"sounds/Lo_Res/impact_ben_sizzle_04.wav",			0, NULL },
	{ SOUND_SWISH_1,				"sounds/Lo_Res/ben_swish_01.wav",			SOUND_FLAG_RAPID_FIRE, NULL },

	{ SOUND_BOSS_VOICE,			"sounds/voice/Best/vo_ben_ow_05.wav",	0, NULL },
	{ SOUND_BOSS_VOICE,			"sounds/voice/Best/vo_ben_yeah_04.wav",	0, NULL },
	{ SOUND_BOSS_VOICE,			"sounds/voice/Best/vo_set_ay_01.wav",	0, NULL },
	{ SOUND_CHICKEN_VOICE,		"sounds/Lo_Res/SilkyChicken_01.wav",	0, NULL },
	{ SOUND_CHICKEN_VOICE,		"sounds/Lo_Res/SilkyChicken_x_03.wav",	0, NULL },
	{ SOUND_CHICKEN_VOICE,		"sounds/Lo_Res/SilkyChicken_x_04.wav",	0, NULL },

	/**** best voices ****/
	{ VO_BEN_BENSGAME,		"sounds/voice/Best/vo_ben_bensgame_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_GOTIT,		"sounds/voice/Best/vo_ben_gotit_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_IWIN,		"sounds/voice/Best/vo_ben_iwin_05.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vo_ben_ow_05.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_PRESSSTART,		"sounds/voice/Best/vo_ben_pressstart_05.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_WHOOPEE,		"sounds/voice/Best/vo_ben_whoopy_02.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vo_ben_yah_06.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vo_ben_yeah_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vo_ben_yes_03.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vo_set_ay_01.wav",	SOUND_FLAG_VOICE, NULL },

	{ VO_BOTH_COMPLETE,		"sounds/voice/Best/vox_both_complete_02.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL1,		"sounds/voice/Best/vox_both_levelone.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL2,		"sounds/voice/Best/vox_both_leveltwo.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL3,		"sounds/voice/Best/vox_both_levelthree.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL4,		"sounds/voice/Best/vox_both_levelfour.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL5,		"sounds/voice/Best/vox_both_levelfive.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL6,		"sounds/voice/Best/vox_both_levelsix.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_LEVEL7,		"sounds/voice/Best/vox_both_levelseven.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BOTH_YEAH,		"sounds/voice/Best/vo_ben_yeah_04.wav",	SOUND_FLAG_VOICE, NULL },

	{ VO_SET_OW,		"sounds/voice/Best/vox_set_ay_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_BENSGAME,		"sounds/voice/Best/vox_set_bensgame_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_GOTIT,		"sounds/voice/Best/vox_set_gotit_05.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_IWIN,		"sounds/voice/Best/vox_set_iwin_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_OW,		"sounds/voice/Best/vox_set_ow_11.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_PRESSSTART,		"sounds/voice/Best/vox_set_pressstart_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_WHOOPEE,		"sounds/voice/Best/vox_set_whoopy_03.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_YEAH,		"sounds/voice/Best/vox_set_yah_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_YEAH,		"sounds/voice/Best/vox_set_yeah_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_SET_YEAH,		"sounds/voice/Best/vox_set_yes_11.wav",	SOUND_FLAG_VOICE, NULL },

	/**** other voices ****/
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ay_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ay_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ay_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ay_hi_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_02.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_03.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_OW,		"sounds/voice/Best/vox_ben_ow_11.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yah_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yah_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yeah_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yeah_02.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yeah_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yes_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yes_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_YEAH,		"sounds/voice/Best/vox_ben_yes_11.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_GOTIT,		"sounds/voice/Best/vox_ben_gotit_01.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_GOTIT,		"sounds/voice/Best/vox_ben_gotit_04.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_GOTIT,		"sounds/voice/Best/vox_ben_gotit_10.wav",	SOUND_FLAG_VOICE, NULL },
	{ VO_BEN_GOTIT,		"sounds/voice/Best/vox_ben_gotit_11.wav",	SOUND_FLAG_VOICE, NULL },
};

int32	gSizeOfSoundSampleList = sizeof(gSoundSampleList)/sizeof(gSoundSampleList[0]);

void initAllSounds(void)
{
	int					i, j, slot;
	SoundSampleStruct	*sp;

	for (i = 0; i < sizeof(gSoundSampleList)/sizeof(gSoundSampleList[0]); i++) {
		sp = &(gSoundSampleList[i]);
		if (sp->flags & SOUND_FLAG_RAPID_FIRE) gRapidFire = true;
		else gRapidFire = false;
		sp->csnd = loadSound(sp->fileName);
	}

	for (i = 0; i < SOUND_HOWMANY; i++) {
		gSoundLinks[i].numSamples = 0;
		for (j = 0; j < sizeof(gSoundSampleList)/sizeof(gSoundSampleList[0]); j++) {
			if (gSoundSampleList[j].soundID == i && gSoundSampleList[j].csnd != NULL) {
				gSoundLinks[i].numSamples++;
			}
		}
		if (gSoundLinks[i].numSamples > 0) {
			gSoundLinks[i].sampleList = new int32[gSoundLinks[i].numSamples];
			slot = 0;
			for (j = 0; j < sizeof(gSoundSampleList)/sizeof(gSoundSampleList[0]); j++) {
				if (gSoundSampleList[j].soundID == i && gSoundSampleList[j].csnd != NULL) {
					gSoundLinks[i].sampleList[slot++] = j;
				}
			}
		}
	}
}


#ifdef WIN32
static CSoundManager			*gSoundMgr = NULL;

SOUNDTYPE *loadSound(char *name)
{
	int32	result = 0;
	CSound	*snd = NULL;
	HRESULT hr;
	char	*tsuptr;
	uint32	tsusize;
	BYTE	*pdata;
	ULONG	size;
	int		repeat;
	LPWAVEFORMATEX format;

	if (!gSoundMgr) {
		HWND	hw = NULL;
		hw = GetForegroundWindow();
		gSoundMgr = new CSoundManager;
		hr = gSoundMgr->Initialize(hw, DSSCL_PRIORITY);
		hr = gSoundMgr->SetPrimaryBufferFormat( 2, 22050, 16 );
	}
	if (!gSoundMgr) return(NULL);

	if ((name == NULL) || (name[0] == 0)) return(NULL);

	if (gRapidFire) repeat = 10;
	else repeat = 2;

	tsuMarkFile(name);
	tsuptr = (char*)tsuGetMemFile(name, &tsusize);
	if (tsuptr) {
		format = (LPWAVEFORMATEX)(tsuptr+20);
		pdata = (BYTE*)(tsuptr+40);
		int	slop = 40;	//// try to reduce the click
		size = tsusize-(40+slop);
		hr = gSoundMgr->CreateFromMemory(&snd, pdata, size, format, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, GUID_NULL, repeat);//, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN);
	} else {
		hr = gSoundMgr->Create(&snd, name, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, GUID_NULL, repeat);//, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN);
	}
	return(snd);
}

void playSound2D(int32 id, float volume, float pitch, float pan)
{
	int32	ipitch;
	int32	ivolume;
	int32	sampleID;

	if (!gSoundMgr) {
		printf("gSoundMgr is NULL\n");
		return;
	}
	if (id < 0 || id >= SOUND_HOWMANY) {
		printf("sound id is out of range\n");
		return;
	}

	if (gSoundLinks[id].numSamples == 0) {
		return;
	}

	sampleID = gSoundLinks[id].sampleList[(RANDOM_INT(0, 65535)) % gSoundLinks[id].numSamples];

	if (sampleID < 0 || sampleID >= (sizeof(gSoundSampleList)/sizeof(gSoundSampleList[0]))) {
		return;
	}

	if (gSoundSampleList[sampleID].csnd == NULL) {
		printf("sound is NULL\n");
		return;
	}

//	printf("playing sound\n");

	if (gSoundSampleList[sampleID].flags & SOUND_FLAG_VOICE) {
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
	gSoundSampleList[sampleID].csnd->Play(0, 0, ivolume, ipitch, 0);
}
#endif 
