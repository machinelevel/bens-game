

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "genincludes.h"
#include "texture.h"
#include "tsufile.h"
#include "slides.h"
#include "translation.h"

#define TLF_GRAY_TO_ALPHA		0x00000001
#define TLF_WHITEOUT			0x00000002
#define TLF_MAKESPHERE			0x00000004
#define TLF_MIP					0x00000008
#define TLF_USE_MASK_IMAGE		0x00000010
#define TLF_CLAMP_S				0x00000020
#define TLF_CLAMP_T				0x00000040
#define TLF_NO_BLUR				0x00000080
#define TLF_MIP_LITE			0x00000100	/**** don't mip too much! ****/
#define TLF_ALPHA4				0x00000200
#define TLF_ALPHA8				0x00000400
#define TLF_RGB8				0x00000800
#define TLF_CLAMP_ST			(TLF_CLAMP_S | TLF_CLAMP_T)

long	TextureLoadFlags = 0;

char	*TexturePath = "textures/";
char	*TextureFileExtension = ".tga";
long	HighestTextureID = 0;

libTexture MasterTextureLib[] = {
	{ TEXID_NONE, "null",			NULL },	/**** no rexture ****/
//	{ TEXID_NONE, "white_grid",		NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT },
//	{ TEXID_NONE, "grid",			NULL, TLF_GRAY_TO_ALPHA },
//	{ TEXID_NONE, "sgrid2",			NULL, TLF_GRAY_TO_ALPHA },
//	{ TEXID_NONE, "white_noise2",	NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT },
//	{ TEXID_NONE, "noise2",			NULL, TLF_GRAY_TO_ALPHA },
//	{ TEXID_NONE, "lawn",			NULL, 0 },
//	{ TEXID_NONE, "lego2",			NULL, 0 },
//	{ TEXID_NONE, "lego4",			NULL, 0 },
//	{ TEXID_NONE, "cloud",			NULL, 0 },
	{ TEXID_CELL, "cell1",			NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP },
	{ TEXID_BUBBLE, "halfbubble1",			NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP },
//	{ TEXID_CHARACTER, "Character2_front",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BEN, "player_Ben",			NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_CHARACTER1, "player_Ben",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP_LITE | TLF_CLAMP_ST},
	{ TEXID_CHARACTER2, "player_Christina",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP_LITE | TLF_CLAMP_ST},
	{ TEXID_BOARD1, "board_Jet_1",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOARD2, "board_Jet_2",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_GRID, "grid1",			NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP},
	{ TEXID_FULLBUBBLE, "bubble2",			NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP},

//	{ TEXID_PATIENT, "body_port",			NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_SHIELD_BLANK, "shield_blank",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_POX, "shield_pox",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_COLDS, "shield_colds",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_RASH, "shield_rash",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_BARF, "shield_barf",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_FEVER, "shield_fever",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_HAIR, "shield_hair",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_SHIELD_FOOT, "shield_bleeding",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_THERMOMETER, "thermometer",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST | TLF_NO_BLUR},
	{ TEXID_COMPASS, "compass1",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},

	{ TEXID_ICON_HEALTH, "icon_health",		NULL, TLF_MIP | TLF_CLAMP_ST | TLF_NO_BLUR},
	{ TEXID_ICON_AMMO,   "icon_pharmacy",		NULL, TLF_MIP | TLF_CLAMP_ST | TLF_NO_BLUR},
	{ TEXID_ICON_ATTITUDE, "icon_house",		NULL, TLF_MIP | TLF_CLAMP_ST | TLF_NO_BLUR},

	{ TEXID_ZAP, "zap1",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_GLOWSPARK, "glowspark1",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_GLOWSWEEP, "glowsweep1",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_WEAPON_BLASTER, "weapon_crossbow",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_SLINGSHOT, "weapon_wristrocket",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_SWORD, "weapon_sword",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_MISSILE, "weapon_missile",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_BALLISTIC, "weapon_crossbow",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_CROSSBOW, "weapon_crossbow",						NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_WEAPON_PARTICLE_ACCELERATOR, "weapon_crossbow",			NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_SLIDE1, "screen_makewish",		NULL, TLF_RGB8},
	{ TEXID_CONTROLS, "key_controls",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_HELP_MONSTERS, "help_monsters",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_HELP_SETBACK, "help_setback",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_HELP_WEAPON, "help_weapon",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_HELP_HEALTH, "help_health",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_HELP_CUSTOM, "help_custom",		NULL, TLF_CLAMP_ST},
	{ TEXID_CIRCLE_R, "circle_r",		NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4},

	{ TEXID_HUD_GRAPH, "hud_graph",		NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA8},
	{ TEXID_TEXT_COMPLETE, "text_complete",		NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA4 | TLF_CLAMP_ST},

	{ TEXID_BOSS1A, "boss1a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS1F, "boss1f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS2A, "boss2a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS2F, "boss2f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS3A, "boss3a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS3F, "boss3f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS4A, "boss4a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS4F, "boss4f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS5A, "boss5a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS5F, "boss5f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS6A, "boss6a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS6F, "boss6f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS7A, "boss7a",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BOSS7F, "boss7f",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_LAUNCH_BARF, "launch_barf",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_SNOW, "launch_snow",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_FIRE, "launch_fire",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_BALL, "launch_ball",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_BATS, "launch_bat",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_DUST, "launch_dust",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_POX, "launch_pox",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_LAUNCH_RUBBER_CHICKEN, "launch_barf",		NULL, TLF_USE_MASK_IMAGE | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_BUTTON_BIG_PLAY, "button_big_play",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_BUTTON_BIG_WIN, "button_you_win",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_BUTTON_BLANK, "button_blank",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_SKINNY, "button_skinny",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_QUIT, "button_quit",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_THANKS, "button_thanks",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_MAW, "button_maw",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_OPTIONS, "button_options",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_HELP, "button_help",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_FORTUNE, "button_fortune",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_SCORES, "button_scores",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_1P, "button_1p",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_2P, "button_2p",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_BACK, "button_back",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_PLAY, "button_play",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_EASY, "button_easy",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_MEDIUM, "button_medium",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_HARD, "button_hard",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_BODY, "button_body",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BUTTON_BOARD, "button_board",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_MIP | TLF_CLAMP_ST},
	{ TEXID_BUTTON_DONATION, "button_donation",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},
	{ TEXID_BUTTON_CHECKBOX, "button_checkbox",		NULL, TLF_GRAY_TO_ALPHA | TLF_ALPHA8 | TLF_CLAMP_ST},

	{ TEXID_LEC_LOGO, "lec_logo",		NULL, TLF_USE_MASK_IMAGE | TLF_CLAMP_ST},
	{ TEXID_FONT_MAIN, TRANSLATE(TXT_fontAr16000),		NULL, TLF_GRAY_TO_ALPHA | TLF_WHITEOUT | TLF_ALPHA8 | TLF_MIP | TLF_CLAMP_ST},

	{ TEXID_BUTTON_FLAG_00, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_01, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_02, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_03, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_04, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_05, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_06, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_07, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_08, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_09, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_10, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_11, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_12, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_13, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_14, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_15, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_16, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_17, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_18, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_19, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_20, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_21, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_22, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_23, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_24, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_25, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_26, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_27, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_28, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	{ TEXID_BUTTON_FLAG_29, "blsnk_flag",		NULL, TLF_CLAMP_ST | TLF_RGB8},
	

	{ 0, NULL, NULL }	/**** just to terminate ****/
};

void makeTexturePathName(char *shortName, char *pathNameOut, int32 mipLevel)
{
	if (mipLevel) {
		sprintf(pathNameOut, "%s%s_mip%d%s", TexturePath, shortName, mipLevel, TextureFileExtension);
	} else {
		sprintf(pathNameOut, "%s%s%s", TexturePath, shortName, TextureFileExtension);
	}
}


void switchLoadedTexture(int32 id, char *name)
{
	int	i;
	libTexture	*tp;
	char		fullPathName[256];

	if (!name) return;
	for (i = 0; MasterTextureLib[i].fileName != NULL; i++) {
		tp = &(MasterTextureLib[i]);
		if (tp->globalID == id) {
			tp->fileName = name;
			if (tp->image) {
				free(tp->image);
				tp->image = NULL;
			}
			TextureLoadFlags = tp->loadFlags;
			makeTexturePathName(tp->fileName, fullPathName, 0);
			tp->image = read_texture(fullPathName, &(tp->width), &(tp->height), &(tp->components));
			tp->mips = NULL;
			if (tp->loadFlags & (TLF_GRAY_TO_ALPHA | TLF_MAKESPHERE)) tp->components = 4;
			if (i > HighestTextureID) HighestTextureID = i;
			UseLibTexture(i, true);
		}
	}
}

bool getTextureInfo(int id, char **pname, int *pw, int *ph)
{
	int	i;
	for (i = 0; MasterTextureLib[i].fileName != NULL; i++) {
		if (MasterTextureLib[i].globalID == id) {
			if (pname) *pname = MasterTextureLib[i].fileName;
			if (pw) *pw = MasterTextureLib[i].width;
			if (ph) *ph = MasterTextureLib[i].height;
			return(true);
		}
	}
	if (pname) *pname = NULL;
	if (pw) *pw = 0;
	if (ph) *ph = 0;
	return(false);
}

long GetTextureID(char *name)
{
	long	i;
	
	for (i = 0; MasterTextureLib[i].fileName != NULL; i++) {
		if (!strcmp(name, MasterTextureLib[i].fileName)) {
			return(i);
		}
	}
	return(0);
}

long NextTextureID(long id, long jump)
{
	long	count;
	
	count = HighestTextureID + 1;
	id += jump;
	while (id < 0) id += count;
	while (id >= count) id -= count;
	return(id);
}

void InitAllTextures(void)
{
	long		i;
	char		fullPathName[256], fullMipName[256];
	libTexture	*tp;

//return;
	HighestTextureID = 0;
	for (i = 0; MasterTextureLib[i].fileName != NULL; i++) {
		if (i != 0) {
			tp = &(MasterTextureLib[i]);
			if (tp->image == NULL) {
				TextureLoadFlags = tp->loadFlags;
				makeTexturePathName(tp->fileName, fullPathName, 0);
				tp->image = read_texture(fullPathName, &(tp->width), &(tp->height), &(tp->components));

				tp->mips = NULL;
				
				if (tp->loadFlags & (TLF_GRAY_TO_ALPHA | TLF_MAKESPHERE)) tp->components = 4;
				if (i > HighestTextureID) HighestTextureID = i;
			}
		}
	}
}

void ReloadAllTextures(void)
{
	long		i;
	libTexture	*tp;

	for (i = 0; MasterTextureLib[i].fileName != NULL; i++) {
		tp = &(MasterTextureLib[i]);
		if (tp->image) {
			free(tp->image);
			tp->image = NULL;
		}
	}
	InitAllTextures();
	for (i = 0; i < TEXID_HOWMANY; i++) {
		UseLibTexture(i, true);
	}
}

void UseLibTexture(long id, bool refresh)
{
	libTexture	*tp;
	int32		mipLevel;
	int32		index;

	if (id <= TEXID_NONE || id >= TEXID_HOWMANY) return;

	tp = NULL;
	for (index = 0; (!tp) && MasterTextureLib[index].fileName != NULL; index++) {
		if (MasterTextureLib[index].globalID == id) {
			tp = &(MasterTextureLib[id]);
		}
	}
	if (!tp) return;

	if (tp->binding == 0) {
		glGenTextures((long) 1, (GLuint *)&tp->binding);
		refresh = true;
	}

	if (! refresh) {
		glBindTexture(GL_TEXTURE_2D, tp->binding);
	} else {
		glBindTexture(GL_TEXTURE_2D, tp->binding);

		if (id == 0 || tp->image == NULL) {
			glDisable(GL_TEXTURE_2D);
		} else {
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			if (tp->loadFlags & TLF_CLAMP_S) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			}
			if (tp->loadFlags & TLF_CLAMP_T) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			if (tp->loadFlags & TLF_NO_BLUR) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
//			glTexImage2D(GL_TEXTURE_2D, 0, tp->components, tp->width, tp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tp->image);
			glEnable(GL_TEXTURE_2D);

			bool mipOK = gOptionMipMap;
			int internalFormat = GL_RGBA4;
			if (tp->loadFlags & TLF_ALPHA4) internalFormat = GL_ALPHA4;
			if (tp->loadFlags & TLF_ALPHA8) internalFormat = GL_ALPHA8;
			if (tp->loadFlags & TLF_RGB8) internalFormat = GL_RGB8;
			
			if (mipOK && (tp->loadFlags & TLF_MIP)) {
				gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, tp->width, tp->height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)(tp->image));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			} else if (mipOK && (tp->loadFlags & TLF_MIP_LITE)) {
				gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, tp->width, tp->height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)(tp->image));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			} else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tp->width, tp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tp->image);
			}
		}
	}
}

void
bwtorgba(unsigned char *b,unsigned char *l,int n) {
    while(n--) {
        l[0] = *b;
        l[1] = *b;
        l[2] = *b;
        l[3] = 0xff;
        l += 4; b++;
    }
}

void
rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n) {
	while(n--) {
		l[0] = r[0];
		l[1] = g[0];
		l[2] = b[0];
		l[3] = 0xff;
		l += 4; r++; g++; b++;
	}
}

void
rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *a,unsigned char *l,int n) {
	while(n--) {
		l[0] = r[0];
		l[1] = g[0];
		l[2] = b[0];
		l[3] = a[0];
		l += 4; r++; g++; b++; a++;
	}
}


uint32 *convert24to32(uint32 *base, int32 width, int32 height)
{
	uint8	*dst, *src;
	uint32	i, count = width * height;

	base = (uint32*)realloc(base, count * 4);
	dst = src = (uint8*)base;
	dst += count * 4;
	src += count * 3;
	for (i = 0; i < count; i++) {
		dst -= 4;
		src -= 3;
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = 0xff;
	}
	return(base);
}

void grayToAlpha(uint32 *base, int32 width, int32 height)
{
	uint8	*dst = (uint8*)base;
	uint32	i, count = width * height;

	for (i = 0; i < count; i++) {
		dst[3] = dst[0];
		dst += 4;
	}
}

void whiteRGB32(uint32 *base, int32 width, int32 height)
{
	uint32	i, count = width * height;

	for (i = 0; i < count; i++) {
		// endian-agnostic indexing.
		((uint8_t*)base)[0] = 255;
		((uint8_t*)base)[1] = 255;
		((uint8_t*)base)[2] = 255;
		base++;
	}
}

void getMaskImage(char *name, uint32 *base, int32 width, int32 height)
{
	char	maskName[1024], *cp;
	uint32	i, count = width * height;
	FILE	*fp = NULL;
	bool	ok;
	uint32	h, w, bitDepth, *base2 = NULL;
	uint32	*dst, *src;
	void	*tsuptr;
	uint32	tsusize;

	strcpy(maskName, name);
	cp = strrchr(maskName, '.');
	if (!cp) return;

	ok = false;
	strcpy(cp, "_mask.jpg");
	tsuMarkFile(maskName);
	tsuptr = tsuGetMemFile(maskName, &tsusize);
	if (tsuptr) fp = NULL;
	else fp = fopen(maskName, "rb");
	if (fp || tsuptr) {
		ok = ejSimpleJPEGRead(fp, tsuptr, tsusize, (void **)&base2, &w, &h, &bitDepth);
	}

	if (fp) fclose(fp);

	if (ok) {
		if (bitDepth == 24) {
			base2 = convert24to32(base2, width, height);
		}

		dst = base;
		src = base2;
		/**** Now copy the alpha over ****/
		for (i = 0; i < count; i++) {
			((uint8_t*)dst)[3] = ((uint8_t*)src)[0];
			dst++;
			src++;
		}
		/**** delete the mask image here ****/
		free(base2);
	}
}


unsigned *
read_texture(char *name, int *width, int *height, int *components) {
    unsigned int *base = NULL, *lptr;
    unsigned char *rbuf, *gbuf, *bbuf, *abuf;
    int y;

	if (1) {
		bool	ok;
		char	*dot;
		FILE	*fp = NULL;
		uint32	bitDepth;
		void	*tsuptr;
		uint32	tsusize;

		ok = false;
		dot = strrchr(name, '.');
		if (dot) strcpy(dot, ".jpg");
		tsuMarkFile(name);
		tsuptr = tsuGetMemFile(name, &tsusize);
		if (tsuptr) fp = NULL;
		else fp = fopen(name, "rb");
		if (fp || tsuptr) {
			ok = ejSimpleJPEGRead(fp, tsuptr, tsusize, (void **)&base, (uint32*)width, (uint32*)height, &bitDepth);
		}

		if (fp) fclose(fp);
		if (ok) {
//printf("%s: %d x %d %d-bit\n", name, *width, *height, bitDepth);
			if (bitDepth == 24) {
				base = convert24to32(base, *width, *height);
			}
			*components = 4;
			if (TextureLoadFlags & TLF_GRAY_TO_ALPHA) {
				grayToAlpha(base, *width, *height);
			}
			if (TextureLoadFlags & TLF_WHITEOUT) {
				whiteRGB32(base, *width, *height);
			}
			if (TextureLoadFlags & TLF_USE_MASK_IMAGE) {
				getMaskImage(name, base, *width, *height);
			}
			return(base);
		}
	}
	return(NULL);
}
