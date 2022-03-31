
/************************************************************\
	translation.cpp
	Files for international translation in Ben's project
\************************************************************/
#ifdef WIN32
 #define _WIN32_WINNT	0x0501 // needed for directory ops
 #include <windows.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "genincludes.h"
#include "texture.h"
#include "draw.h"
#include "slides.h"
#include "sound.h"
#include "font.h"
#include "translation.h"
#include "tsufile.h"

#if MAC_BUILD
 #include "umath.h"
 #include "macutils.h"
#else
 #define TWIST(_x) (_x)
#endif

char	gLanguageList[MAX_LANGUAGES][256];
char	*gLanguageData[MAX_LANGUAGES];
int		gNumLanguages = 0;
bool	gLanguageChanged = false;
bool	gUnicode = false;
int		gCurrentLanguage = 0;

char	gTranslationTable[MAX_NUM_TRANSLATION_LINES][MAX_TRANSLATION_LINE_LENGTH] = {
	"Ben's Game",
	"Great job - keep it up!",
	"Don't be afraid.",
	"Take it one day at a time.",
	"Don't give up - be strong.",
	"Every day is a great day.",
	"Don't forget to take your pills!",
	"A nap a day keeps the blahs away.",
	"Never think bad thoughts.",
	"If you have a tummyache, make yourself a milkshake.",
	"Don't stop now, you're doing great.",
	"Keep going - zoom to the finish!",
	"So you're bald - big deal!",
	"noosllewteg!",
	"You're a star.",

	"Welcome to Ben's Game!",
	"This is a game about fighting cancer. In Ben's Game, you play a high-speed hero with good friends and all the right tools.",
	"For more detailed instructions, visit www.makewish.org/ben.",
	"Find Seven Shields",
	"There are SEVEN SHIELDS to collect. These are shields for protection against some yucky side-effects.",
	"Chicken Pox",
	"Fever",
	"Bleeding",
	"Hair Loss",
	"Barf",
	"Rash",
	"Colds",
	"Defeat Monsters",
	"Each shield is guarded by a MONSTER who generates MUTATED CELLS.",
	"Staying Healthy",
	"Your hero has:\n\n       HEALTH from the hospital\n\n       AMMO from the pharmacy\n\n       ATTITUDE from home.\n\nWhen you bump a MUTATED CELL, you lose HEALTH. To get it back, fly to the HEALTH BUBBLE in the corner of the board.",
	"Setbacks",
	"Electrical barriers in the game are called SETBACKS. When you hit them, you lose ATTITUDE.",
	"Weapons and Ammo",
	"There are WEAPONS floating in bubbles in the game. To get a weapon, just run into it.",
	"Customize Ben's Game!",
	"The character can look like anything you want! It can look like you, your pet, a spaceship... anything! To create a custom character (this might require a grown-up's help), look in the 'textures' folder, and use the existing pictures as templates. For details on how to customize the character, visit www.makewish.org/ben",
	"Make-A-Wish",	//BUTTON_ID_QUIT,
	"How to Play",	//BUTTON_ID_THANKS,
	"Thanks!",	//BUTTON_ID_MAW,
	"Options",	//BUTTON_ID_OPTIONS,
	"Message from Ben",	//BUTTON_ID_HELP,
	"Quit",	//BUTTON_ID_SCORES,
	"One Player",	//BUTTON_ID_1PLAYER,
	"Two Players",	//BUTTON_ID_2PLAYER,
	"Go Back",	//,BUTTON_ID_BACK
	"Play",	//,BUTTON_ID_PLAY
	"Start a brand new game",	//,BUTTON_ID_SAVE1
	"Continue where you left off",	//,BUTTON_ID_SAVE2
	"Yes, bye bye.",	//BUTTON_ID_ALERT_QUIT,
	"No, keep playing!",	//BUTTON_ID_ALERT_NOQUIT,
	"Next",	//BUTTON_ID_ALERT_BACK,

	"The Greater Bay Area",
	"Make-A-Wish Foundation",
	"This game was made possible when Ben and Eric were introduced to one another through the Greater Bay Area Make-A-Wish Foundation.\n\nMake-a-Wish to grants the wishes of children with life-threatening medical conditions to enrich the human experience with hope, strength, and joy.\n\nIf you enjoy Ben's Game, please consider using the links below to make a donation to Make-A-Wish. Your contribution will help to make sure they have the resources to keep granting wishes for kids like Ben.",
	"These buttons will take you to the Make-A-Wish web pages.",
	"Make a Donation",
	"Visit Make-A-Wish",
	"Ben's Game Page",
	"A Message From Ben!",
	"Game Options",
	"Start up Ben's Game in a window next time",
	"Use higher quality graphics (may be slower)",
	"Enable sound effects",
	"Enable character voices",
	"Wavy motion for cells",
	"Auto-save my progress",
	"Ben and Eric wish to say\nTHANKS\nto the following people",
	"Are you sure you\nwant to quit?",
	"Version: Release 1 (6/29/04)",
	"Welcome to Ben's Game!\nTo get an updated version, or to make comments and report bugs, please visit www.makewish.org/ben",
	"New Game",
	"Continue Game",

	"font2Ar24",
	"font2Ar24000",
	"----Checkpoint1----",

	"Thank you!",
	"COMPLETE",
	"Play!",
	"PLAY\nEASY",
	"PLAY\nMEDIUM",
	"PLAY\nHARD",
	"You Win!",
	"Player 1",
	"Player 2",
	"Move",
	"Stop",
	"Fire",
	"Switch Weapon",
	"Adjust Camera",
};

void WriteTranslationFile(void)
{
	char	*fname = "textures\\language_English.txt";
	FILE	*fp;
	int		i;

	fp = fopen(fname, "w");
	if (fp) {
		fprintf(fp, "Translation file for Ben's Game\n\n");

		for (i = 0; i < MAX_NUM_TRANSLATION_LINES; i++) {
			fprintf(fp, "line %d\n[%s]\n\n", i, gTranslationTable[i]);
		}
		fclose(fp);
	}

	fp = fopen("ascii_europe.txt", "w");
	if (fp) {
		fprintf(fp, "Text table for European languages\n\n");

		for (i = 32; i < 255; i++) {
			fprintf(fp, "ascii %d = '%c'\n", i, (char)i);
		}
		fclose(fp);
	}
}

void ReadTranslationFile(void)
{
}

void LoadLanguageButton(int buttonID, char *baseName)
{
	char	name[1024], fullPath[1024];
	FILE	*fp = NULL;
	void	*tsuptr;
	uint32	tsusize;

	sprintf(fullPath, "textures/%s_%s.jpg", gLanguageList[gCurrentLanguage], baseName);

	tsuMarkFile(fullPath);
	tsuptr = tsuGetMemFile(fullPath, &tsusize);
	if (!tsuptr) {
		fp = fopen(fullPath, "r");
	}
	if (fp || tsuptr) {
		if (fp) fclose(fp);
		sprintf(name, "%s_%s", gLanguageList[gCurrentLanguage], baseName);
	} else {
		sprintf(name, baseName);
	}
	switchLoadedTexture(buttonID, name);
}

bool ImportLanguageText(void)
{
	char	*src, *dst;
	uint16	*uniSrc;
	int		i, count, code, lookup;

	src = gLanguageData[gCurrentLanguage];
	uniSrc = (uint16*)src;
	if (TWIST(uniSrc[0]) == 0xfeff) {
		gUnicode = true;
		uniSrc++;
	} else {
		gUnicode = false;
	}
	if (gUnicode) {
		for (i = 0; i < MAX_NUM_TRANSLATION_LINES; i++) {
			count = 0;
			dst = gTranslationTable[i];
			while (*uniSrc && (*uniSrc != '[')) uniSrc++;
			if (*uniSrc == 0) {
				printf("Language error: early null\n");
				return(false);
			}
			uniSrc++;
			while (*uniSrc && (*uniSrc != ']') && (count < MAX_TRANSLATION_LINE_LENGTH)) {
				code = TWIST(uniSrc[0]);
				uniSrc++;
				if (code == '[') {
					printf(" language error: missing ']' near line %d\n", i);
				}
				if (code < 256) {
					*dst++ = code;
				} else {
					lookup = gFontSys->GetSlotFromUnicode(code);
					*dst++ = '^';
					*dst++ = ((lookup / 100) % 10) + '0';
					*dst++ = ((lookup / 10) % 10) + '0';
					*dst++ = (lookup % 10) + '0';
				}
				count++;
			}
			*dst = 0;
		}
	} else {
		for (i = 0; i < MAX_NUM_TRANSLATION_LINES; i++) {
			count = 0;
			dst = gTranslationTable[i];
			while (*src && (*src != '[')) src++;
			if (*src == 0) {
				printf("Language error: early null\n");
				return(false);
			}
			src++;
			if (0 == strncmp(dst, "----Checkpoint", strlen("----Checkpoint"))) {
				if (0 != strncmp(src, "----Checkpoint", strlen("----Checkpoint"))) {
					printf(" language checkpoint line %d failed.\n", i);
					return(false);
				} else {
	//				printf(" language checkpoint line %d ok...\n", i);
				}
			}
			while (*src && (*src != ']') && (count < MAX_TRANSLATION_LINE_LENGTH)) {
				if (*src == '[') {
					printf(" language error: missing ']' near line %d\n", i);
				}
				*dst++ = *src++;
				count++;
			}
			*dst = 0;
		}
	}
	return(true);
}

void TextSetLanguage(int whichLanguage)
{
	if (whichLanguage < 0) return;
	if (whichLanguage >= gNumLanguages) return;
	if (whichLanguage == gCurrentLanguage) return;
	printf("-> %s\n", gLanguageList[whichLanguage]);
	gLanguageChanged = true;
	gCurrentLanguage = whichLanguage;

	LoadLanguageButton(TEXID_BUTTON_BIG_PLAY, "button_big_play");
	LoadLanguageButton(TEXID_BUTTON_BIG_WIN, "button_you_win");
	LoadLanguageButton(TEXID_BUTTON_DONATION, "button_donation");

	ImportLanguageText();
	switchLoadedTexture(TEXID_FONT_MAIN, gTranslationTable[TXT_fontAr16000]);
	gFontSys->LoadAll();
	ImportLanguageText();

	if (!strcmp("Japanese", gLanguageList[gCurrentLanguage])) {
		gFontSys->mMinimumSize = 0.75f;
		gFontSys->mAlwaysSquare = true;
	} else if (!strcmp("Chinese", gLanguageList[gCurrentLanguage])) {
		gFontSys->mMinimumSize = 0.75f;
		gFontSys->mAlwaysSquare = true;
	} else if (!strcmp("Korean", gLanguageList[gCurrentLanguage])) {
		gFontSys->mMinimumSize = 0.75f;
		gFontSys->mAlwaysSquare = true;
	} else {
		gFontSys->mMinimumSize = 0.0f;
		gFontSys->mAlwaysSquare = false;
	}
}

void FullSetLanguage(void)
{
	if (!gLanguageChanged) return;
	gLanguageChanged = false;
}

void AddLanguage(char *languageFile, bool isTsuFile)
{
	char	*cp, str[256];
	int		i;

	cp = strrchr(languageFile, '_');
	if (!cp) return;
	cp++;
	strcpy(gLanguageList[gNumLanguages], cp);
	cp = strrchr(gLanguageList[gNumLanguages], '.');
	if (!cp) return;
	*cp = 0;

	for (i = 0; i < gNumLanguages; i++) {
		if (!strcmp(gLanguageList[gNumLanguages], gLanguageList[i])) {
			return;	/**** it's a dupe ****/
		}
	}


	if (isTsuFile) {
		char	loadName[256];
		void	*tsuptr;
		uint32	tsusize;

		sprintf(loadName, "textures/%s", languageFile);
		tsuMarkFile(loadName);
		tsuptr = (char*)tsuGetMemFile(loadName, &tsusize);
		if (tsuptr) {
			gLanguageData[gNumLanguages] = (char*)tsuptr;
			gLanguageData[gNumLanguages][tsusize-1] = 0;
		}
	} else {
		FILE	*fp;
		char	loadName[256];
		int		size;

		sprintf(loadName, "textures/%s", languageFile);
		tsuMarkFile(loadName);
		fp = fopen(loadName, "rb");
		if (!fp) return;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (!size) return;
		fseek(fp, 0, SEEK_SET);
		gLanguageData[gNumLanguages] = (char*)malloc(size + 1);
		if (!gLanguageData[gNumLanguages]) {
			fclose(fp);
			return;
		}
		fread(gLanguageData[gNumLanguages], 1, size, fp);
		gLanguageData[gNumLanguages][size-1] = 0;
		fclose(fp);
	}

	sprintf(str, "%s_flag", gLanguageList[gNumLanguages]);
	LoadLanguageButton(TEXID_BUTTON_FLAG_00 + gNumLanguages, str);

//	printf("Found language data: %s\n", gLanguageList[gNumLanguages]);
	gNumLanguages++;
}

bool SetLanguageByName(char *name)
{
	int	i;
	for (i = 0; i < gNumLanguages; i++) {
		if (!strcmp(name, gLanguageList[i])) {
			TextSetLanguage(i);
			FullSetLanguage();
			return(true);
		}
	}
	return(false);
}

char *GetLanguageName(void)
{
	if (gNumLanguages == 0) return("English");
	else return(gLanguageList[gCurrentLanguage]);
}

void FindLanguages(void)
{
	int		i;
	bool	ok;

	gNumLanguages = 0;

	AddLanguage("language_English.txt", false);
	AddLanguage("language_English.txt", true);

#ifdef WIN32
	WIN32_FIND_DATA FindFileData;
	HANDLE			hFind;

	hFind = FindFirstFileEx("textures\\language_*.txt", FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0 );
	if (hFind != INVALID_HANDLE_VALUE) {
		ok = true;
		while (ok) {
			AddLanguage(FindFileData.cFileName, false);
			ok = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
#endif

#if MAC_BUILD
	uint16	itemCount = 0;
	char	fileName[256];
	char 	*fileNamePtr;
	Boolean	foundFile = false;

	itemCount = mac_count_folder_contents("textures");
	for (i = 0; i < itemCount; i++)
	{
		foundFile = mac_get_filename_by_index("textures", fileName, i);
		if (foundFile) {
			if (!strncmp(fileName, "language_", strlen("language_"))) {
				AddLanguage(fileName, false);
			}
		}
	}
#endif

#if 1
	/**** if these are in the TSU file, add them ****/
	AddLanguage("language_Dutch.txt", true);
	AddLanguage("language_French.txt", true);
	AddLanguage("language_German.txt", true);
	AddLanguage("language_Greek.txt", true);
	AddLanguage("language_Italian.txt", true);
	AddLanguage("language_Japanese.txt", true);
	AddLanguage("language_Russian.txt", true);
	AddLanguage("language_Spanish.txt", true);
#endif

}

void InitTranslation(void)
{
	if (0) WriteTranslationFile();
	FindLanguages();
}
