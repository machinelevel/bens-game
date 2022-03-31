
/************************************************************\
	slides.cpp
	Files for controlling the slides in Ben's project,
	This is the "hub" of the game.
\************************************************************/

/***************************************************\


\***************************************************/

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
#include "umath.h"
#include "upmath.h"
#include "camera.h"
#include "controls.h"
#include "timer.h"
#include "random.h"
#include "CellGrid.h"
#include "Slides.h"
#include "glider.h"
#include "texture.h"
#include "hud.h"
#include "boss.h"
#include "spline.h"
#include "cellport.h"
#include "sound.h"
#include "font.h"
#include "translation.h"

#if MAC_BUILD
#include "macutils.h"
#endif

extern SDL_Window* main_sdl_window;

extern bool doneFlag;

Slides		*gSlides = NULL;

int32	gAlertButton = 0;
float	gAlertFader = 0.0f;
int		gNumPlayers = 1;
int		gDifficultySetting = DIFFICULTY_EASY;
pfVec2	gHintPos = {320,320};
float	gHintFader = 0.0f;
char	*gHintText = NULL;

#define MAX_PLAYER_IMAGES	1000
PlayerImage *gPlayerImages[MAX_PLAYER_IMAGES];
int			gNumPlayerImages = 0;
PlayerImage *gBoardImages[MAX_PLAYER_IMAGES];
int			gNumBoardImages = 0;


int		benCurrentQuote = -1;
int		gKeyNumber = 0;
char	*benQuotes[] = {
	TRANSLATE(TXT_Great_job__keep_it_up),
	TRANSLATE(TXT_Dont_be_afraid),
	TRANSLATE(TXT_Take_it_one_day_at_a_time),
	TRANSLATE(TXT_Dont_give_up__be_strong),
	TRANSLATE(TXT_Every_day_is_a_great_day),
	TRANSLATE(TXT_Dont_forget_to_take_your_pills),
	TRANSLATE(TXT_A_nap_a_day_keeps_the_blahs_away),
	TRANSLATE(TXT_Never_think_bad_thoughts),
	TRANSLATE(TXT_If_you_have_a_tummyache_make_yourself_a_milkshake),
	TRANSLATE(TXT_Dont_stop_now_youre_doing_great),
	TRANSLATE(TXT_Keep_going__zoom_to_the_finish),
	TRANSLATE(TXT_So_youre_bald__big_deal),
	TRANSLATE(TXT_noosllewteg),
	TRANSLATE(TXT_Youre_a_star),
	NULL
};

#define BOX_TITLE_Y	(70+50)
#define BOX_TEXT_Y	(190+50)
#define BOX_CONTROLS_X	(360)
#define BOX_CONTROLS_Y	(275)

Box	helpBoxes1[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y-50,640,50, 0, 1.0f, TRANSLATE(TXT_Welcome_to_Bens_Game)},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y-50,280,200, 0, 0.7f, TRANSLATE(TXT_This_is_a_game_about_fighting_cancer_In_Bens_Game_you_play_a_high_speed_hero_with_good_friends_and_all_the_right_tools)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,640,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_IMAGE,                BOX_CONTROLS_X+2,BOX_CONTROLS_Y,200,200, TEXID_CONTROLS},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X+64,BOX_CONTROLS_Y,640,200, 0, 0.75f, TRANSLATE(TXT_Player_1)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X-64,BOX_CONTROLS_Y,640,200, 0, 0.75f, TRANSLATE(TXT_Player_2)},

	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X,BOX_CONTROLS_Y+30,640,200, 0, 0.5f, TRANSLATE(TXT_Move)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X,BOX_CONTROLS_Y+60,640,200, 0, 0.5f, TRANSLATE(TXT_Stop)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X,BOX_CONTROLS_Y+85,640,200, 0, 0.5f, TRANSLATE(TXT_Shoot)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X,BOX_CONTROLS_Y+127,640,200, 0, 0.5f, TRANSLATE(TXT_Switch_Weapon)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, BOX_CONTROLS_X,BOX_CONTROLS_Y+169,640,200, 0, 0.5f, TRANSLATE(TXT_Adjust_Camera)},
	{BOX_FLAG_END}
};

Box	helpBoxes2[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,640,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y,320,50, 0, 1.0f, TRANSLATE(TXT_Find_Seven_Shields)},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y,280,200, 0, 0.7f, TRANSLATE(TXT_There_are_SEVEN_SHIELDS_to_collect_These_are_shields_for_protection_against_some_yucky_side_effects)},
	{BOX_FLAG_IMAGE,                140,170,60,60, TEXID_SHIELD_POX},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 140,170+60,500,60, 0, 0.5f, TRANSLATE(TXT_Chicken_Pox)},
	{BOX_FLAG_IMAGE,                200,240,60,60, TEXID_SHIELD_FEVER},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 200,240+60,500,60, 0, 0.5f, TRANSLATE(TXT_Fever)},
	{BOX_FLAG_IMAGE,                140,310,60,60, TEXID_SHIELD_FOOT},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 140,310+60,500,60, 0, 0.5f, TRANSLATE(TXT_Bleeding)},
	{BOX_FLAG_IMAGE,                280,310,60,60, TEXID_SHIELD_HAIR},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 280,310+60,500,60, 0, 0.5f, TRANSLATE(TXT_Hair_Loss)},
	{BOX_FLAG_IMAGE,                360,270,60,60, TEXID_SHIELD_BARF},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 360,270+60,500,60, 0, 0.5f, TRANSLATE(TXT_Barf)},
	{BOX_FLAG_IMAGE,                440,240,60,60, TEXID_SHIELD_RASH},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 440,240+60,500,60, 0, 0.5f, TRANSLATE(TXT_Rash)},
	{BOX_FLAG_IMAGE,                520,270,60,60, TEXID_SHIELD_COLDS},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 520,270+60,500,60, 0, 0.5f, TRANSLATE(TXT_Colds)},
	{BOX_FLAG_END}
};
Box	helpBoxes4[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,1000,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y,320,50, 0, 1.0f, TRANSLATE(TXT_Defeat_Monsters)},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y,280,200, 0, 0.7f, TRANSLATE(TXT_Each_shield_is_guarded_by_a_MONSTER_who_generates_MUTATED_CELLS)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_IMAGE,                430,240,200,100, TEXID_HELP_MONSTERS},
	{BOX_FLAG_END}
};
Box	helpBoxes3[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,640,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y,320,50, 0, 1.0f, TRANSLATE(TXT_Staying_Healthy)},
	{BOX_FLAG_IMAGE,                280,200,100,100, TEXID_HELP_HEALTH},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y,280,200, 0, 0.6f, TRANSLATE(TXT_Your_hero_has_HEALTH_from_the_hospital_AMMO_from_the_pharmacy_ATTITUDE_from_home_When_you_bump_a_MUTATED_CELL_you_lose_HEALTH_To_get_it_back_fly_to_the_HEALTH_BUBBLE_in_the_corner_of_the_board)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_END}
};
Box	helpBoxes5[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,640,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y-20,320,50, 0, 1.0f, TRANSLATE(TXT_Setbacks)},
	{BOX_FLAG_IMAGE,                440,240,200,100, TEXID_HELP_SETBACK},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y-20,280,200, 0, 0.7f, TRANSLATE(TXT_Electrical_barriers_in_the_game_are_called_SETBACKS_When_you_hit_them_you_lose_ATTITUDE)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_END}
};
Box	helpBoxes6[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,480,640,200, 0, 0.5f, TRANSLATE(TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben)},
	{BOX_FLAG_IMAGE,                400,256,100,100, TEXID_HELP_WEAPON},
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y,320,50, 0, 1.0f, TRANSLATE(TXT_Weapons_and_Ammo)},
	{BOX_FLAG_TEXT,                 440,BOX_TEXT_Y,280,200, 0, 0.7f, TRANSLATE(TXT_There_are_WEAPONS_floating_in_bubbles_in_the_game_To_get_a_weapon_just_run_into_it)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_END}
};
Box	helpBoxes7[] = {
	{BOX_FLAG_TEXT|BOX_FLAG_CENTER, 320,BOX_TITLE_Y-50,640,50, 0, 1.0f, TRANSLATE(TXT_Customize_Bens_Game)},
	{BOX_FLAG_IMAGE,                320,320,100,50, TEXID_HELP_CUSTOM},
	{BOX_FLAG_TEXT,                 440-20,BOX_TEXT_Y-50,280+50,200, 0, 0.7f, TRANSLATE(TXT_The_character_can_look_like_anything_you_want_CONT)},
	{BOX_FLAG_IMAGE,                160,240,200,200, TEXID_BEN},
	{BOX_FLAG_END}
};

int	gHelpScreen = 0;
Box	*gHelpBoxList[] = {
	helpBoxes1,
	helpBoxes2,
	helpBoxes3,
	helpBoxes4,
	helpBoxes5,
	helpBoxes6,
	helpBoxes7,
	NULL
};

pfVec4	BoxFontColor[2] = {{1,1,1,1},{0.5f,1,1,1}};

enum {
	THANKS_SCREEN_THANKS,
	THANKS_SCREEN_CREDITS,
	THANKS_SCREEN_BETA_1,
	THANKS_SCREEN_BETA_2,

	THANKS_SCREEN_HOWMANY
};

int	gThanksScreen = 0;

static SlideButton	buttonRects[] = {
	{0,0,0,0,0,0},	//BUTTON_ID_NONE,
	{320, 240, 280, 140, TEXID_BUTTON_BIG_PLAY, NULL,320,320, 1},	//BUTTON_ID_BIGPLAY,

//	{320,240,0,0, TEXID_BUTTON_BLANK, 1},	//BUTTON_ID_QUIT,
	{104+50,140,128,128, TEXID_BUTTON_MAW, TRANSLATE(TXT_Make_A_Wish),320,320, 1},	//BUTTON_ID_QUIT,
	{536-50,140,128,128, TEXID_BUTTON_HELP, TRANSLATE(TXT_How_to_Play),320,320, 1},	//BUTTON_ID_THANKS,
	{104,240,128,128, TEXID_BUTTON_THANKS, TRANSLATE(TXT_Thanks),320,320, 1},	//BUTTON_ID_MAW,
	{536,240,128,128, TEXID_BUTTON_OPTIONS, TRANSLATE(TXT_Options),320,320, 1},	//BUTTON_ID_OPTIONS,
	{104+50,340,128,128, TEXID_BUTTON_FORTUNE, TRANSLATE(TXT_Message_from_Ben),320,320, 1},	//BUTTON_ID_HELP,
	{536-50,340,128,128, TEXID_BUTTON_QUIT, TRANSLATE(TXT_Quit),320,320, 1},	//BUTTON_ID_SCORES,

	{0,0,128,128, TEXID_BUTTON_SCORES, NULL,320,320, 1},	//BUTTON_ID_FORTUNE,

	{320-75,190-0,128,128, TEXID_BUTTON_1P, TRANSLATE(TXT_One_Player),320,320, 1},	//BUTTON_ID_1PLAYER,
	{320+75,190+0,128,128, TEXID_BUTTON_2P, TRANSLATE(TXT_Two_Players),320,320, 1},	//BUTTON_ID_2PLAYER,
	{320,350,64,64, TEXID_BUTTON_BACK, TRANSLATE(TXT_Go_Back),320,320, 1},	//,BUTTON_ID_BACK
	{320,350,64,64, TEXID_BUTTON_PLAY, TRANSLATE(TXT_Play),320,320, 1},	//,BUTTON_ID_PLAY

	{320,240,64,64, TEXID_BUTTON_BLANK, TRANSLATE(TXT_Start_a_brand_new_game),320,320, 1},	//,BUTTON_ID_SAVE1
	{320,240,64,64, TEXID_BUTTON_BLANK, TRANSLATE(TXT_Continue_where_you_left_off),320,320, 1},	//,BUTTON_ID_SAVE2
	{320,240,64,64, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//,BUTTON_ID_SAVE3
	{320,240,64,64, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//,BUTTON_ID_SAVE4

	{320,240+120,150,150, TEXID_BUTTON_EASY, NULL,320,320, 1},	//BUTTON_ID_EASY,
	{320,240,   150,150, TEXID_BUTTON_MEDIUM, NULL,320,320, 1},	//BUTTON_ID_MEDIUM,
	{320,240-120,150,150, TEXID_BUTTON_HARD, NULL,320,320, 1},	//BUTTON_ID_HARD,

	{(320-80),140,48,48, TEXID_BUTTON_BODY, NULL,320,320, 1},	//BUTTON_ID_BODY_1A,
	{(320+80),140,48,48, TEXID_BUTTON_BODY, NULL,320,320, 1},	//BUTTON_ID_BODY_1B,
	{(320-80),140,48,48, TEXID_BUTTON_BODY, NULL,320,320, 1},	//BUTTON_ID_BODY_2A,
	{(320+80),140,48,48, TEXID_BUTTON_BODY, NULL,320,320, 1},	//BUTTON_ID_BODY_2B,
	{(320-80),220,48,48, TEXID_BUTTON_BOARD, NULL,320,320, 1},	//BUTTON_ID_BOARD_1A,
	{(320+80),220,48,48, TEXID_BUTTON_BOARD, NULL,320,320, 1},	//BUTTON_ID_BOARD_1B,
	{(320-80),220,48,48, TEXID_BUTTON_BOARD, NULL,320,320, 1},	//BUTTON_ID_BOARD_2A,
	{(320+80),220,48,48, TEXID_BUTTON_BOARD, NULL,320,320, 1},	//BUTTON_ID_BOARD_2B,
	{(320+50),250,32,32, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_NAME_1,
	{(320-50),250,32,32, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_NAME_2,

	{320+32,350,64,64, TEXID_BUTTON_PLAY, TRANSLATE(TXT_Play),320,320, 1},	//BUTTON_ID_PROCEED1,

	{320,700,0,0, TEXID_BUTTON_FLAG_00, gLanguageList[0],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_01, gLanguageList[1],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_02, gLanguageList[2],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_03, gLanguageList[3],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_04, gLanguageList[4],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_05, gLanguageList[5],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_06, gLanguageList[6],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_07, gLanguageList[7],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_08, gLanguageList[8],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_09, gLanguageList[9],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_10, gLanguageList[10],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_11, gLanguageList[11],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_12, gLanguageList[12],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_13, gLanguageList[13],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_14, gLanguageList[14],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_15, gLanguageList[15],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_16, gLanguageList[16],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_17, gLanguageList[17],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_18, gLanguageList[18],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_19, gLanguageList[19],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_20, gLanguageList[20],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_21, gLanguageList[21],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_22, gLanguageList[22],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_23, gLanguageList[23],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_24, gLanguageList[24],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_25, gLanguageList[25],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_26, gLanguageList[26],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_27, gLanguageList[27],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_28, gLanguageList[28],320,449, 1},	//Flags!,
	{320,700,0,0, TEXID_BUTTON_FLAG_29, gLanguageList[29],320,449, 1},	//Flags!,

	/**** alert buttons start here ****/
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_OK,
	{320,240,0,0, TEXID_BUTTON_QUIT, TRANSLATE(TXT_Yes_bye_bye),320,350, 1},	//BUTTON_ID_ALERT_QUIT,
	{320,240,0,0, TEXID_BUTTON_BACK, TRANSLATE(TXT_No_keep_playing),320,350, 1},	//BUTTON_ID_ALERT_NOQUIT,
	{320,240,0,0, TEXID_BUTTON_BACK, TRANSLATE(TXT_Go_Back),320,320, 1},	//BUTTON_ID_ALERT_BACK,
	{320,240,0,0, TEXID_BUTTON_PLAY, TRANSLATE(TXT_Next),320,320, 1},	//BUTTON_ID_ALERT_NEXT,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_1,

	{320,240,0,0, TEXID_BUTTON_DONATION, NULL,320,320, 1},	//BUTTON_ID_ALERT_WEB_DONATION_SITE,
	{320,240,0,0, TEXID_BUTTON_BODY, NULL,320,320, 1},	//BUTTON_ID_ALERT_WEB_BENSGAME_SITE,
	{320,240,0,0, TEXID_BUTTON_MAW, NULL,320,320, 1},	//BUTTON_ID_ALERT_WEB_MAKEWISH_SITE,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_WEB_INSTRUCTIONS_SITE,

	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_WINDOW,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_MIPMAP,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_SOUND,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_VOICE,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_WAVES,
	{320,240,0,0, TEXID_BUTTON_BLANK, NULL,320,320, 1},	//BUTTON_ID_ALERT_CHECKBOX_SAVE,

};

static int	buttonListAlertMaW[] = { BUTTON_ID_ALERT_BACK,
									BUTTON_ID_ALERT_WEB_DONATION_SITE,
									BUTTON_ID_ALERT_WEB_MAKEWISH_SITE,
									BUTTON_ID_ALERT_WEB_BENSGAME_SITE, 0 };

static int	buttonListAlertThanks[] = { BUTTON_ID_ALERT_NEXT, BUTTON_ID_ALERT_BACK, 0 };

static int	buttonListAlertFortune[] = { BUTTON_ID_ALERT_BACK, 0 };

static int	buttonListAlertHelp[] = { BUTTON_ID_ALERT_NEXT,
								BUTTON_ID_ALERT_BACK, 0 };

static int	buttonListAlertOptions[] = { BUTTON_ID_ALERT_BACK,
										BUTTON_ID_ALERT_CHECKBOX_WINDOW,
										BUTTON_ID_ALERT_CHECKBOX_MIPMAP,
										BUTTON_ID_ALERT_CHECKBOX_SOUND,
										BUTTON_ID_ALERT_CHECKBOX_VOICE,
										BUTTON_ID_ALERT_CHECKBOX_WAVES,
										BUTTON_ID_ALERT_CHECKBOX_SAVE,
										0 };

static int	buttonListAlertQuit[] = { BUTTON_ID_ALERT_QUIT,
								BUTTON_ID_ALERT_NOQUIT, 0 };

static int	original_buttonListPlay[] = { BUTTON_ID_BIGPLAY,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT, 0 };

static int	buttonListPlay[256] = { BUTTON_ID_BIGPLAY,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT, 0 };

static int	buttonListYouWin[] = { BUTTON_ID_PLAY };

static int	buttonListSave[] = { BUTTON_ID_SAVE1,
								BUTTON_ID_SAVE2,
								BUTTON_ID_BACK, 
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT,0 };

static int	buttonListNumPlayers[] = { BUTTON_ID_1PLAYER,
								BUTTON_ID_2PLAYER,
								BUTTON_ID_BACK,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT, 0 };

static int	buttonListChoose1Player[] = { 
								BUTTON_ID_PROCEED1,
								BUTTON_ID_BODY_1A,
								BUTTON_ID_BODY_1B,
								BUTTON_ID_BOARD_1A,
								BUTTON_ID_BOARD_1B,
								BUTTON_ID_NAME_1,
								BUTTON_ID_BACK,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT, 0 };

static int	buttonListChoose2Player[] = { 
								BUTTON_ID_PROCEED1,
								BUTTON_ID_BODY_1A,
								BUTTON_ID_BODY_1B,
								BUTTON_ID_BOARD_1A,
								BUTTON_ID_BOARD_1B,
								BUTTON_ID_NAME_1,
								BUTTON_ID_BODY_2A,
								BUTTON_ID_BODY_2B,
								BUTTON_ID_BOARD_2A,
								BUTTON_ID_BOARD_2B,
								BUTTON_ID_NAME_2,
								BUTTON_ID_BACK,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT, 0 };

static int	buttonListDifficulty[] = { BUTTON_ID_EASY,
								BUTTON_ID_MEDIUM,
								BUTTON_ID_HARD,
								BUTTON_ID_BACK,
								0 };

static int	buttonListPause[] = {BUTTON_ID_PLAY,
								BUTTON_ID_MAW,
								BUTTON_ID_HELP,
								BUTTON_ID_THANKS,
								BUTTON_ID_OPTIONS,
								BUTTON_ID_FORTUNE,
								BUTTON_ID_QUIT,
								BUTTON_ID_1PLAYER,
								BUTTON_ID_2PLAYER, 0 };

#define SPIN_MAX	900.0f
#define SPIN_MIN	90.0f
#define SPIN_REDUCE_RATE	1000.0f
float	gSpinSpeed[2] = { SPIN_MAX, SPIN_MAX };

char *gThanksNames[2] = {
	"Patricia Wilson\n"
	"Chris Miles\n"
	"Ellen Meijers\n"
	"Everyone at Greater Bay Area Make-A-Wish\n"
	"Dr. Seymour Zoger and the staff at UCSF\n"
	"Anne & Brian Duskin\n"
	"Sue Johnston\n"
	"Alissa Reiter\n"
	"Barbara and Bill Cancilla\n"
	"Pat and Dave Duskin\n"
	"Lynne Whittaker\n"
	"David Perry\n"
	"Josh Unger\n"
	"Elizabeth Powers\n"
	"Rich and Wendie Johnston\n"
	"Todd and Fiona Walter\n"
	"Brad Post\n"
	"Mark Blattel\n"
	"Katherine Wallen\n"
	"Linden Siahann\n"
	"Christina Frasco\n"
	"Grant Ross\n"
	"The Zeum Staff\n"
	"\n"
	,
	"Mark J. Kilgard\n"
	"Nate Robins\n"
	"John Conway\n"
	"\n"
	"Nick Porcino\n"
	"Tom and Bo Harper and Kaeli\n"
	"Mark Barbolak\n"
	"Anne Marie Stein\n"
	"Victoria van Ysseldyk\n"
	"Dinah Jaye Houghtaling\n"
	"Jennifer McKenna\n"
	"Lynn Huffaker\n"
	"Jo Donaldson\n"
	"Mike Nelson\n"
	"Catherine Durand\n"
	"Simon Jeffery\n"
	"George Lucas\n"
	"\n"
	"Alpha Testers:\n"
	"Dallas and Bryce\n"
	"Reese and Kiana\n"
	"Everyone at Sue's 2003 Cookie Party\n"
	"The LucasArts staff\n"
};

char	*gCreditTitles[] = {
	"Character Art",
	"",
	"Sound Effects",
	"",
	"Macintosh Programming",
	"",
	"Voice",
	"",
	"Translations",
	NULL
};

char	*gCreditNames[] = {
	"Chris Miles",
	"",
	"Ellen Meijers",
	"",
	"Brad Post",
	"",
	"Christina Frasco",
	"",
	"Delia Tasso",
	"Paola Manca",
	"Hermelinda De La Torre",
	"Ellen Meijers",
	"Lynne Whittaker",
	"Sue Johnston",
	"Chelsea Wilson",
	"Andrea Guido",
	"Tatiana Popov",
	"Yuri Popov",
	"Fabian Schmitz",
	"Henry Matzerath",
	"Jochen Rother",
	"Chris Gripeos",
	"Mary Tsalkithou",
	"Komei Harada",
	"Nick Porcino",
	NULL
};


char *gBetaTesterNames[] = {
	"Abilio Cabrera",
	"Ace Mystical",
	"Adam Bauer",
	"Agathe Curie",
	"Alan Blevins",
	"Alba Lopez Romero",
	"Alex Glanville",
	"Alex Khavich",
	"Alex Solano",
	"Alexandra Garcia",
	"Amy Bull",
	"Andrew Kang",
	"Andrew O'Dell",
	"Andy Sereno",
	"Angela Uceta Lago",
	"Ann Wise",
	"Anna Iuli",
	"Anthony Egwu",
	"Barbara Masters",
	"Ben Arfmann",
	"Ben Tolson",
	"Benjamin Klein",
	"Benjamin Waite",
	"Benjamin Barrett",
	"Bich Vu",
	"Blake Schreurs",
	"Bob Mather",
	"Bob Rankl",
 	"Brad Willman",
	"Bradley Bremen",
	"Brian Murphy",
	"Brick Kane",
	"Bruce Bell-Myers",
	"Bryon O",
	"Buimanh Khoa",
	"C.J. Elliott",
	"Caitlin Cloud",
	"Calvin Cronan",
	"Cameron Alexander",
	"Carl Etheridge",
	"Carles Muniesa",
	"Chad Trexler",
	"Charles Tomalin",
	"Chase Wilson",
	"Chris Blore",
	"Chris Bond",
	"Chris Copeland",
	"Christopher Corbett",
	"Clark Sorensen",
	"Cornelia Schrefl",
	"Cyndy Bunte",
	"Dan Block",
	"Dang Son",
	"Daniel Kaufman",
	"David Bower",
	"David Bry",
	"David De Torres",
	"David Hibbard",
	"David Levy",
	"David Sheff",
	"David Canning",
	"David Simpkin",
	"Decil Gab",
	"Delfosse Arthur",
	"Denes House",
	"Denis Welsch",
	"Dennis Nguyen",
	"Dimovich Von Lichtenshtein",
	"Donald Pierce",
	"Doug Mason",
	"Doug Cox",
	"Duo Maxwell",
	"Duong Dung",
	"Dylan Yudaken",
	"Ed Morrell",
	"Eric Bass",
	"Eric Cantsay",
	"Eric Hokenson",
	"Eric Saidel",
	"Essam Tariq",
	"Ezra Ekman",
	"Fabian Schmitz",
	"Faith Lee",
	"Farhan Ahmed",
	"Fgyt Hdsfhdfs",
	"Forrest Livengood",
	"Gagarini Espino",
	"Gerrit Huybreghts",
	"Gerry Kirk",
	"Gibb Jarutirasarn",
	"Glenn Purkis",
	"Gloria Burd",
	"Gregory Richter",
	"Harold Paiva",
	"Hal Barwood",
	"Heather Armstrong",
	"Heather Teets",
	"Heidi Chun",
	"Hien Quang",
	"Hong Dung Nguyen",
	"Hung Nguyen Manh",
	"Huong Vu",
	"Huu Linh Nguyen Pham",
	"Idyioio Dhi",
	"Igntius Boyone",
	"J. Daryl Kern",
	"Jack Stewart",
	"Jane Fitz, Md",
	"Jason Artis",
	"Jason Burke",
	"Jason Shealy",
	"Jeffrey Nagata",
	"Jeffrey Vigil",
	"Jennifer Petree",
	"Jill Cote",
	"Jo Ervin",
	"Joel Batmale",
	"Joey Coogan",
	"Jon Thompson",
	"Jonathan Van Tuijl",
	"Jonh Smith",
	"Jordan Carder",
	"Jordan Traylor",
	"Joseph A. Nagy, Jr.",
	"Joshua Newhoff",
	"Judith Lucero",
	"Jun Kim",
	"Karon Wetzler",
	"Kelly Smith",
	"Kelly Stewart",
	"Ken Langley",
	"Ken Hu Hu",
	"Kevin Smith",
	"Khalid Rehman",
	"Khanh Nguyen",
	"Kimhue Nguyen",
	"Kris Keochinda",
	"Kris O'Kelly",
	"Kristian Danielsen",
	"Larry McDonald",
	"Laura Sexton",
	"Laura Kerbyson",
	"Laurie Davis-Ybarra",
	"Le Hoang",
	"Le Lele",
	"Le Thuc",
	"Le Khanh An",
	"Leonardo Valvassori",
	"Lester Vecsey",
	"Linda Khaw",
	"Loan Nguyen",
	"Luc René De Cotret",
	"Luis Pandolfo",
	"Luke Rademacher",
	"Lynne Whittaker",
	"Maarten Vandendungen",
	"Marc Potvin",
	"Mark Roden",
	"Mary Waller",
	"Mary Kay McAvoy",
	"Mata Dragoumanou",
	"Matthew Hatton",
	"Matthew Parsons",
	"Maurice Theriot",
	"Maurice Dufresne",
	"Michael Cox",
	"Michael Pledl",
	"Michael Rich",
	"Michael & Ryan Bartnett",
	"Michel Binkhorst",
	"Minh Thao Nguyen",
	"Monteix Aurélie",
	"Muhammad Javaid",
	"Narges Grammi",
	"Narice May",
	"Neil Sutter",
	"Nhung Nguyen",
	"Nicholas Quinlan",
	"Nick O'Keefe",
	"Nicko Van Someren",
	"Noah Jefferson",
	"Nopdanai Chaiyo Nuy",
	"Odin Liam Wright",
	"Ong Liwen",
	"Osborne Jacob",
	"Otto Glatt",
	"Pat Jenkinson",
	"Patricia Laposte",
	"Patricia Wilson",
	"Patrick Karjala",
	"Patrick Murphy",
	"Patrick O'Neal",
	"Paula Richard",
	"Pauline King",
	"Philip Peterson",
	"Raf Batista",
	"Rémy Lé",
	"Riccardo Gorone",
	"Richard McSorley",
	"Richard Silverman",
	"Rick Davidson",
	"Rick Wood",
	"Robert Velasquez",
	"Robert Cox",
	"Robert L. Vandegrift",
	"Robin Arnold",
	"Rodolphe Millard",
	"Ron Hipschman",
	"Rory Fuller",
	"Rosemarie Hitchens",
	"Russell Myers",
	"Ryan Campbell",
	"Ryan Seper",
	"Sajjad Kausar",
	"Sam Garito",
	"Sam Heathfield",
	"Sandeep Singh",
	"Sao Dao",
	"Sarah Nicolle",
	"Scott Barrows",
	"Scott Cunningham",
	"Scott Markwell",
	"Scott Wheelock",
	"Sean Brock",
	"Sean Cribbs",
	"Serban Mihai",
	"Shawn McCool",
	"Shelley Leibovitz",
	"Sinu Colonna",
	"Sonelle Braid",
	"Stacie Celender",
	"Stacy Taylor",
	"Stephen Cousins",
	"Steve Bucci",
	"Steve Corn",
	"Steven Bracht",
	"Steven Cooley",
	"Steven Campbell",
	"T Runyan",
	"Ted Tong",
	"Ted Wilcox",
	"Terry Orzechowski",
	"Tham Tu",
	"Thanh Dat",
	"Thanh Nguyen",
	"Thel Fergison",
	"Thelonious Williams",
	"Theo Louis",
	"Thery Louis",
	"Thu Huong",
	"Thuan Nong",
	"Thuy Yen",
	"Tobias Scheller",
	"Todd Gallina",
	"Trang Nguyen",
	"Truong Dangcuong Cuong",
	"Tuananh Nguyen",
	"Tucker Monticelli",
	"Tyler Frenzel",
	"Victor Ramirez",
	"Victoria Garcia Perea",
	"Waqas Weki",
	"Wasan Grichan",
	"Wei Ming Pan",
	"Winston Walker",
	"Xandi Konigstorfer",
	"Xavier See",
	"Yotam Dvir",
};

int *Slides::GetAlertButtonList(void)
{
	switch (gAlertButton) {
		case BUTTON_ID_MAW:		return(buttonListAlertMaW); break;
		case BUTTON_ID_THANKS:	return(buttonListAlertThanks); break;
		case BUTTON_ID_FORTUNE:	return(buttonListAlertFortune); break;
		case BUTTON_ID_HELP:	return(buttonListAlertHelp); break;
		case BUTTON_ID_OPTIONS:	return(buttonListAlertOptions); break;
		case BUTTON_ID_QUIT:	return(buttonListAlertQuit); break;
		default: return(NULL);
	}
}

void addPlayerImage(char *fileName)
{
	char	*cp, name[256];
	int		i, k;
	bool	ok, repeat;

	if (!strcmp("_mask.jpg", fileName + strlen(fileName) - 9)) {
	} else {
		strcpy(name, fileName);
		cp = strrchr(name, '.');
		if (cp) *cp = 0;

		repeat = false;
		for (k = 0; k < gNumPlayerImages && !repeat; k++) {
			if (!strcmp(name, gPlayerImages[k]->mFileName)) {
				repeat = true;
			}
		}
		if (!repeat) {
			PlayerImage	*im = new PlayerImage;
			printf("found player image %s\n", fileName);
			gPlayerImages[gNumPlayerImages++] = im;

			strcpy(im->mFileName, name);

			im->mPlayerName[0] = 0;
			cp = strchr(im->mFileName, '_');
			if (cp) {
				strcpy(im->mPlayerName, cp+1);
				for (cp = im->mPlayerName; *cp; cp++) {
					if (*cp == '_') *cp = ' ';
				}
			}
		}
	}
}

void addBoardImage(char *fileName)
{
	char	*cp, name[256];
	int		i, k;
	bool	ok, repeat;

	if (!strcmp("_mask.jpg", fileName + strlen(fileName) - 9)) {
	} else {
		strcpy(name, fileName);
		cp = strrchr(name, '.');
		if (cp) *cp = 0;

		repeat = false;
		for (k = 0; k < gNumBoardImages && !repeat; k++) {
			if (!strcmp(name, gBoardImages[k]->mFileName)) {
				repeat = true;
			}
		}
		if (!repeat) {
			PlayerImage	*im = new PlayerImage;
			printf("found player image %s\n", fileName);
			gBoardImages[gNumBoardImages++] = im;

			strcpy(im->mFileName, name);

			im->mPlayerName[0] = 0;
			cp = strchr(im->mFileName, '_');
			if (cp) {
				strcpy(im->mPlayerName, cp+1);
				for (cp = im->mPlayerName; *cp; cp++) {
					if (*cp == '_') *cp = ' ';
				}
			}
		}
	}
}

void Slides::ScanPlayerImages(void)
{
	int		i;
	bool	ok;

	for (i = 0; i < gNumPlayerImages; i++) {
		if (gPlayerImages[i]) delete gPlayerImages[i];
	}
	gNumPlayerImages = 0;
	for (i = 0; i < gNumBoardImages; i++) {
		if (gBoardImages[i]) delete gBoardImages[i];
	}
	gNumBoardImages = 0;

	addPlayerImage("player_Ben.jpg");
	addPlayerImage("player_Christina.jpg");
	addBoardImage("board_Jet_1.jpg");
	addBoardImage("board_Jet_2.jpg");

#ifdef WIN32
	WIN32_FIND_DATA FindFileData;
	HANDLE			hFind;
	Glider	*g;


	/**** Get player image list ****/
	hFind = FindFirstFileEx("textures\\player_*.jpg", FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0 );
	if (hFind != INVALID_HANDLE_VALUE) {
		ok = true;
		while (ok) {
			addPlayerImage(FindFileData.cFileName);
			ok = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
	/**** Get board image list ****/
	hFind = FindFirstFileEx("textures\\board_*.jpg", FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0 );
	if (hFind != INVALID_HANDLE_VALUE) {
		ok = true;
		while (ok) {
			addBoardImage(FindFileData.cFileName);
			ok = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
//	if (gNumPlayerImages == 0) return;
//	if (gNumBoardImages == 0) return;
	for (i = 0; i < MAX_NUM_GLIDERS; i++) {
		g = gGliders[i];
		if (gNumPlayerImages > 0) {
			if (g->mPlayerImageNum >= gNumPlayerImages) {
				g->mPlayerImageNum = g->mPlayerNum % gNumPlayerImages;
			}
			switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
			strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
		}
		if (gNumBoardImages > 0) {
			if (g->mBoardImageNum >= gNumBoardImages) {
				g->mBoardImageNum = g->mPlayerNum % gNumBoardImages;
				switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
			}
		}
	}
#endif

#if MAC_BUILD
	Glider	*g;
	uint16	itemCount = 0;
	char	fileName[256];
	char 	*fileNamePtr;
	Boolean	foundFile = false;
		

	/**** Get player image list ****/
	itemCount = mac_count_folder_contents("textures");
	
	for (i = 0; i < itemCount; i++)
	{
		foundFile = mac_get_filename_by_index("textures", fileName, i);
		
		if (foundFile)
		{
			if (!strncmp(fileName, "player_", strlen("player_")))
			{
				addPlayerImage(fileName);
			}
			else if (!strncmp(fileName, "board_", strlen("board_")))
			{
				addBoardImage(FindFileData.cFileName);
			}
		}
	}
	
//	if (gNumPlayerImages == 0) return;
//	if (gNumBoardImages == 0) return;
	for (i = 0; i < MAX_NUM_GLIDERS; i++) {
		g = gGliders[i];
		if (gNumPlayerImages > 0) {
			if (g->mPlayerImageNum >= gNumPlayerImages) {
				g->mPlayerImageNum = g->mPlayerNum % gNumPlayerImages;
			}
			switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
			strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
		}
		if (gNumBoardImages > 0) {
			if (g->mBoardImageNum >= gNumBoardImages) {
				g->mBoardImageNum = g->mPlayerNum % gNumBoardImages;
				switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
			}
		}
	}
#endif
}

int Slides::GetRectButton(int *list, int x, int y, int *secondaryList)
{
	int win_width, win_height;
	SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
	float	fx = x / (win_width / 640.0f);
	float	fy = y / (win_height / 480.0f);
	int		pos = 0;
	
	if (gAlertButton) {
		//return(0);
		list = GetAlertButtonList();
	}

	while (list && list[pos]) {
		SlideButton	*sb = &(buttonRects[list[pos]]);
		float	w = sb->w * sb->selectScale;
		float	h = sb->h * sb->selectScale;
		if (fx >= (sb->cx-(w/2)) && fx <= (sb->cx+(w/2))) {
			if (fy >= (sb->cy-(h/2)) && fy <= (sb->cy+(h/2))) {
				return(list[pos]);
			}
		}
		pos++;
	}
	return(0);
}

Slides::Slides()
{
	int	i;
	mCurrentSlide = SLIDE_PRE_TITLE;
	mPlayStage = 1;
	mCurrentButton = 0;
	mWobbleX = 0.0f;
	mWobbleY = 0.0f;
	mWobbleScale = 0.0f;
	mSplatCount = 0;

	for (i = 0; i < SLIDE_HOWMANY; i++) {
		mSlideList[i].mTexName = "screen_makewish";
		mSlideList[i].mTexID = TEXID_SLIDE1;
		mSlideList[i].mSize[0] = mSlideList[i].mSize[1] = 0;
		mSlideList[i].mButtonList = NULL;
	}
	mSlideList[SLIDE_PRE_TITLE].mTexName = "screen_makewish";
	mSlideList[SLIDE_TITLE].mTexName = "screen_title3";
//	mSlideList[SLIDE_WISH].mTexName = "screen_makewish";
//	mSlideList[SLIDE_START].mTexName = "screen_start";

	mSlideList[SLIDE_BUTTON_PLAY].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_PLAY].mButtonList = buttonListPlay;
	mSlideList[SLIDE_YOU_WIN].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_YOU_WIN].mButtonList = buttonListYouWin;
	mSlideList[SLIDE_BUTTON_SAVE].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_SAVE].mButtonList = buttonListSave;
	mSlideList[SLIDE_BUTTON_NUMPLAYERS].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_NUMPLAYERS].mButtonList = buttonListNumPlayers;
	mSlideList[SLIDE_BUTTON_CHOOSE1PLAYER].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_CHOOSE1PLAYER].mButtonList = buttonListChoose1Player;
	mSlideList[SLIDE_BUTTON_CHOOSE2PLAYER].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_CHOOSE2PLAYER].mButtonList = buttonListChoose2Player;
	mSlideList[SLIDE_BUTTON_DIFFICULTY].mTexName = NULL;//"screen_bigbuttons";
	mSlideList[SLIDE_BUTTON_DIFFICULTY].mButtonList = buttonListDifficulty;
	mSlideList[SLIDE_PAUSE].mTexName = NULL;
	mSlideList[SLIDE_PAUSE].mButtonList = buttonListPause;

	mWobbleSpeed[0] = 1.0f;
	mWobbleSpeed[1] = 1.2f;
	mWobbleSpeed[2] = 1.4f;
	mWobbleSpeed[3] = 1.6f;
	for (i = 0; i < 4; i++) {
		mWobbleTimer[i] = 0.0;
	}
	ScanPlayerImages();
}

Slides::~Slides()
{
}

float	gIntroTimer = 0.0f;
int		gIntroStage = 0;
float	gIntroWipeTimer = 0.0f;
float	gIntroFader = 0.0f;

enum {
	INTRO_SPACE,
	INTRO_BLACK,
	INTRO_WIPE_LOGO_BEGIN,
	INTRO_WIPE_LOGO,
	INTRO_WIPE_LOGO_END,
	INTRO_GOTO_A,
	INTRO_GOTO_STAR,
	INTRO_AFTER_STAR,
	INTRO_FADE_DOWN,
	INTRO_SLIDE_BEN,
	INTRO_SLIDE_ALL_DONE,
};

void Slides::Think(void)
{
	float	x, y;

	UpdateAlertSlide();

	gIntroTimer += UnscaledDeltaTime;
	switch (gIntroStage) {
		case INTRO_SPACE:
			gFireFlies->mLifeTimer = 1.0f;	//keep them alive
			gFireFlies->mAcc = 0.0f;
			gFireFlies->mBuddyAcc = 0.0f;
			pfSetVec3(gFireFlies->mCenter, 320, 240, 0);
			gFireFlies->mSpread = 30.0f;
			gFireFlies->mBurst = false;
			if (gIntroTimer > 0.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_BLACK:
			gFireFlies->mLifeTimer = 1.0f;	//keep them alive
			pfSetVec3(gFireFlies->mCenter, 320, 240, 0);
			gFireFlies->mSpread = 30.0f;
			gFireFlies->mBurst = false;
			gFireFlies->mAcc = 1000.0f;
			gFireFlies->mBuddyAcc = 100.0f;
			if (gIntroTimer > 5.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_WIPE_LOGO_BEGIN:
			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			x = 320 - (120/2.0f)*gIntroTimer;
			x = 180;
			pfSetVec3(gFireFlies->mCenter, x, 240, 0);
			gFireFlies->mSpread = 30.0f;
			if (gIntroTimer > 1.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_WIPE_LOGO:
			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			x = 200 + (240/4.0f)*gIntroTimer;
			x = 640-180;
			gIntroWipeTimer = gIntroTimer / 4.0f;
			pfSetVec3(gFireFlies->mCenter, x, 240, 0);
			gFireFlies->mSpread = 30.0f;
			if (gIntroTimer > 2.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_GOTO_STAR:
			gIntroWipeTimer = 1.0f;
			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			x = 436;//410;
			y = 166;//180;
			pfSetVec3(gFireFlies->mCenter, x, y, 0);
			gFireFlies->mSpread = 5.0f;
			if (gIntroTimer > 3.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_AFTER_STAR:
			gIntroWipeTimer = 1.0f;
			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			gFireFlies->mBurst = true;
			if (gIntroTimer > 4.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_FADE_DOWN:
			gIntroWipeTimer = 1.0f;
			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			gFireFlies->mBurst = true;
			gIntroFader = gIntroTimer / 1.0f;
			if (gIntroFader > 1.0f) gIntroFader = 1.0f;
			if (gIntroFader < 0.0f) gIntroFader = 0.0f;
			if (gIntroTimer > 1.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
				NextSlide();
			}
			break;
		case INTRO_SLIDE_BEN:
			gIntroWipeTimer = 1.0f;
			gIntroFader = 1.0f-(gIntroTimer / 1.0f);
			if (gIntroFader > 1.0f) gIntroFader = 1.0f;
			if (gIntroFader < 0.0f) gIntroFader = 0.0f;
			gFireFlies->mBurst = true;
			if (gIntroTimer > 1.0f) {
				gIntroStage++;
				gIntroTimer = 0.0f;
			}
			break;
		case INTRO_SLIDE_ALL_DONE:
//			gFireFlies->mBurst = false;
//			gFireFlies->mSpread = 10000000.0f;
//			gFireFlies->mLifeTimer = 5.0f;	//keep them alive
			break;
		default:
			gIntroStage++;
			gIntroTimer = 0.0f;
			break;
	}
}

void Slides::NextSlide(void)
{
	if (mCurrentSlide == SLIDE_PRE_TITLE) {
		mCurrentSlide = SLIDE_TITLE;
	} else if (mCurrentSlide == SLIDE_TITLE) {
		mCurrentSlide = SLIDE_BUTTON_PLAY;
//mCurrentSlide = SLIDE_YOU_WIN;
	}
	switchLoadedTexture(TEXID_SLIDE1, mSlideList[mCurrentSlide].mTexName);
}

void Slides::NewGame(int numPlayers) {
	int i;
	Glider *g;

	if (gGameMode == GAME_MODE_SLIDES) {
//		while (gActiveGliderList) delete gActiveGliderList;
//		for (i = 0; i < numPlayers; i++) {
//			new Glider;
//		}
		gCells->Clear();

		gNextLevelNumber = 0;
		for (i = 0; i < SHIELD_HOWMANY; i++) {
			gLevels[i].mShieldTexID = TEXID_SHIELD_POX + i;
			gLevels[i].mBossID = BOSS_TYPE_EVILCHICKEN + i;
			gLevels[i].mDifficulty = gDifficultySetting;
			gLevels[i].mLevelNumber = -1;
			gLevels[i].mComplete = false;
		}

		/**** Fix up the list to avoid re-writing more code. ****/
		gActiveGliderList = gGliders[0];
		gGliders[1]->mNext = NULL;
		if (gNumPlayers == 2) {
			gGliders[0]->mNext = gGliders[1];
		} else {
			gGliders[0]->mNext = NULL;
		}

		for (i = 0; i < 7; i++) {
			gShields[i].have = false;
			gShields[i].icon = TEXID_SHIELD_BLANK;
		}

		for (g = gActiveGliderList; g; g = g->mNext) {
			if (g->mPlayerNum < gNumPlayers) SETFLAG(g->mFlags, GLIDER_FLAG_ACTIVE);
			else CLRFLAG(g->mFlags, GLIDER_FLAG_ACTIVE);
			pfSetVec3(g->mMatrix[PF_T], 0, 0, 0);
			pfSetVec3(g->mVelocity, 0, 0, 0);
			pfCopyMat(g->mLastMatrix, g->mMatrix);
			g->mTrailNext = -1;
		}
		gGameMode = GAME_MODE_INTRO;
	}
}


void Slides::UpdateAlertSlide(void)
{
	if (gAlertButton) {
		gAlertFader += 5.0f * UnscaledDeltaTime;
		if (gAlertFader > 1.0f) gAlertFader = 1.0f;
//		if (gAlertFader == 1.0f && gAlertButton == BUTTON_ID_QUIT) {
			//extern bool	doneFlag;
			//doneFlag = true;
//		}
	} else {
		gAlertFader -= 5.0f * UnscaledDeltaTime;
		if (gAlertFader < 0.0f) gAlertFader = 0.0f;
	}
}

void Slides::StartAlertSlide(int whichSlide)
{
	gAlertButton = whichSlide;
	gHintFader = 0.0f;
	if (whichSlide == BUTTON_ID_FORTUNE) {
		if (benCurrentQuote < 0) {
			benCurrentQuote = (int)(RANDOM_IN_RANGE(0, 10));
		}
		gKeyNumber = -3;
		benCurrentQuote++;
		if (benQuotes[benCurrentQuote] == NULL) {
			benCurrentQuote = 0;
		}
	}
}

char	*donationURL = "https://secure2.wish.org/site/SPageServer?pagename=donate_today&chid=025-000";
char	*wishURL = "https://wish.org/greaterbay";
char	*benURL = "https://en.wikipedia.org/wiki/Ben's_Game";

#ifdef WIN32
void mac_LaunchURL(char *url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
#endif


void Slides::Click(int x, int y)
{
	int			i, j;
	Glider		*g;
	SaveSlot	*slot;

	mCurrentButton = GetRectButton(mSlideList[mCurrentSlide].mButtonList, x, y, NULL);	

	if (mCurrentButton) {
		playSound2D(SOUND_BUTTON_CLICK, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
	}

	if (mCurrentButton >= BUTTON_ID_FLAG_00 && mCurrentButton <= BUTTON_ID_FLAG_29) {
		FindLanguages();
		TextSetLanguage(mCurrentButton - BUTTON_ID_FLAG_00);
		FullSetLanguage();
		if (GetLanguageName()) {
			strcpy(gOptionLanguage, GetLanguageName());
		}
		SaveOptions();
	} else if (gAlertButton) {
		switch (gAlertButton) {
			case BUTTON_ID_MAW:
				switch (mCurrentButton) {
					case BUTTON_ID_ALERT_BACK:
						gAlertButton = 0;
						break;
					case BUTTON_ID_ALERT_WEB_DONATION_SITE:
						//system(donationURL);
						//mac_LaunchURL(donationURL);
						break;
					case BUTTON_ID_ALERT_WEB_MAKEWISH_SITE:
						//system(wishURL);
						//mac_LaunchURL(wishURL);
						break;
					case BUTTON_ID_ALERT_WEB_BENSGAME_SITE:
						//system(benURL);
						//mac_LaunchURL(benURL);
						break;
					default:
						break;
				}
				break;
			case BUTTON_ID_THANKS:
				switch (mCurrentButton) {
				case BUTTON_ID_ALERT_BACK:
					gAlertButton = 0;
					break;
				case BUTTON_ID_ALERT_NEXT:
					gThanksScreen++;
					if (gThanksScreen >= THANKS_SCREEN_HOWMANY) {
						gThanksScreen = 0;
					}
					break;
				default: break;
				}
				break;
			case BUTTON_ID_FORTUNE:
				switch (mCurrentButton) {
				case BUTTON_ID_ALERT_BACK: gAlertButton = 0; break;
				default: break;
				}
				break;
			case BUTTON_ID_HELP:
				switch (mCurrentButton) {
				case BUTTON_ID_ALERT_BACK:
					gAlertButton = 0;
					gHelpScreen = 0;
					break;
				case BUTTON_ID_ALERT_NEXT:
					gHelpScreen++;
					if (gHelpBoxList[gHelpScreen] == NULL) {
						gAlertButton = 0;
						gHelpScreen = 0;
					}
					break;
				default: break;
				}
				break;
			case BUTTON_ID_OPTIONS:
				switch (mCurrentButton) {
				case BUTTON_ID_ALERT_BACK:
					gAlertButton = 0;
					if (1) {
						bool needToSaveOptions = false;
						bool needToRefreshGraphics = false;
						bool choice;
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_WINDOW].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionLaunchInWindow != choice) {
							gOptionLaunchInWindow = choice;
							needToSaveOptions = true;
						}
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_MIPMAP].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionMipMap != choice) {
							gOptionMipMap = choice;
							needToSaveOptions = true;
							needToRefreshGraphics = true;
						}
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_SOUND].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionSoundEffectsOn != choice) {
							gOptionSoundEffectsOn = choice;
							needToSaveOptions = true;
						}
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_VOICE].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionVoicesOn != choice) {
							gOptionVoicesOn = choice;
							needToSaveOptions = true;
						}
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_WAVES].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionWaves != choice) {
							gOptionWaves = choice;
							needToSaveOptions = true;
						}
						choice = buttonRects[BUTTON_ID_ALERT_CHECKBOX_SAVE].tex == TEXID_BUTTON_CHECKBOX;
						if (gOptionAutoSave != choice) {
							gOptionAutoSave = choice;
							needToSaveOptions = true;
						}
						if (needToSaveOptions) {
							SaveOptions();
						}
						if (needToRefreshGraphics) {
							ReloadAllTextures();
							gFontSys->LoadAll();
						}
					}
					break;
				case BUTTON_ID_ALERT_CHECKBOX_WINDOW:
				case BUTTON_ID_ALERT_CHECKBOX_MIPMAP:
				case BUTTON_ID_ALERT_CHECKBOX_SOUND:
				case BUTTON_ID_ALERT_CHECKBOX_VOICE:
				case BUTTON_ID_ALERT_CHECKBOX_WAVES:
				case BUTTON_ID_ALERT_CHECKBOX_SAVE:
					if (buttonRects[mCurrentButton].tex == TEXID_BUTTON_BLANK) {
						buttonRects[mCurrentButton].tex = TEXID_BUTTON_CHECKBOX;
					} else {
						buttonRects[mCurrentButton].tex = TEXID_BUTTON_BLANK;
					}
					break;
				default: break;
				}
				break;
			case BUTTON_ID_QUIT:
				switch (mCurrentButton) {
					case BUTTON_ID_ALERT_QUIT:
						doneFlag = true;
						break;
					case BUTTON_ID_ALERT_NOQUIT:
						gAlertButton = 0;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	} else {
		switch (mCurrentSlide) {
			case SLIDE_YOU_WIN:
				switch (mCurrentButton) {
				case BUTTON_ID_PLAY:
//					gDifficultySetting++;	/**** On to the next difficulty setting! ****/
//					if (gDifficultySetting > DIFFICULTY_HARD) {
//						gDifficultySetting = DIFFICULTY_HARD;
//					}
//					NewGame(gNumPlayers);
					mCurrentSlide = SLIDE_BUTTON_PLAY;
					break;
				default:
					NewSplat(x, y, RANDOM_IN_RANGE(50, 100));
					break;
				}
				break;
			case SLIDE_BUTTON_PLAY:
			case SLIDE_BUTTON_SAVE:
			case SLIDE_BUTTON_NUMPLAYERS:
			case SLIDE_BUTTON_CHOOSE1PLAYER:
			case SLIDE_BUTTON_CHOOSE2PLAYER:
			case SLIDE_BUTTON_DIFFICULTY:
				switch (mCurrentButton) {
			case BUTTON_ID_BIGPLAY:
				if (gSave.slots[0].numPlayers > 0 && gOptionAutoSave) mCurrentSlide = SLIDE_BUTTON_SAVE;
				else mCurrentSlide = SLIDE_BUTTON_NUMPLAYERS;
				playSound2D(VO_BEN_BENSGAME, 0.8f, 1.0f);
				playSound2D(VO_SET_BENSGAME, 0.8f, 1.0f);
				break;
			case BUTTON_ID_SAVE1:	/**** new game ****/
				mCurrentSlide = SLIDE_BUTTON_NUMPLAYERS;
				break;
			case BUTTON_ID_SAVE2:	/**** continue saved game ****/
				SaveSlotToGame();
				break;
			case BUTTON_ID_MAW:
			case BUTTON_ID_HELP:
			case BUTTON_ID_THANKS:
			case BUTTON_ID_FORTUNE:
			case BUTTON_ID_QUIT:
				StartAlertSlide(mCurrentButton);
				break;
			case BUTTON_ID_OPTIONS:
				StartAlertSlide(mCurrentButton);
				if (gOptionLaunchInWindow) buttonRects[BUTTON_ID_ALERT_CHECKBOX_WINDOW].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_WINDOW].tex = TEXID_BUTTON_BLANK;
				if (gOptionMipMap) buttonRects[BUTTON_ID_ALERT_CHECKBOX_MIPMAP].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_MIPMAP].tex = TEXID_BUTTON_BLANK;
				if (gOptionSoundEffectsOn) buttonRects[BUTTON_ID_ALERT_CHECKBOX_SOUND].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_SOUND].tex = TEXID_BUTTON_BLANK;
				if (gOptionVoicesOn) buttonRects[BUTTON_ID_ALERT_CHECKBOX_VOICE].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_VOICE].tex = TEXID_BUTTON_BLANK;
				if (gOptionWaves) buttonRects[BUTTON_ID_ALERT_CHECKBOX_WAVES].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_WAVES].tex = TEXID_BUTTON_BLANK;
				if (gOptionAutoSave) buttonRects[BUTTON_ID_ALERT_CHECKBOX_SAVE].tex = TEXID_BUTTON_CHECKBOX;
				else buttonRects[BUTTON_ID_ALERT_CHECKBOX_SAVE].tex = TEXID_BUTTON_BLANK;
				break;
			case BUTTON_ID_BODY_1A:
				gSpinSpeed[0] = -SPIN_MAX;
				g = gGliders[0];
				if (g && gNumPlayerImages > 0) {
					g->mPlayerImageNum--;
					if (g->mPlayerImageNum < 0) g->mPlayerImageNum = gNumPlayerImages-1;
					if (g->mPlayerImageNum >= gNumPlayerImages) g->mPlayerImageNum = 0;
					switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
					strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
				}
				break;
			case BUTTON_ID_BODY_1B:
				gSpinSpeed[0] = SPIN_MAX;
				g = gGliders[0];
				if (g && gNumPlayerImages > 0) {
					g->mPlayerImageNum++;
					if (g->mPlayerImageNum < 0) g->mPlayerImageNum = gNumPlayerImages-1;
					if (g->mPlayerImageNum >= gNumPlayerImages) g->mPlayerImageNum = 0;
					switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
					strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
				}
				break;
			case BUTTON_ID_BODY_2A:
				gSpinSpeed[1] = -SPIN_MAX;
				g = gGliders[1];
				if (g && gNumPlayerImages > 0) {
					g->mPlayerImageNum--;
					if (g->mPlayerImageNum < 0) g->mPlayerImageNum = gNumPlayerImages-1;
					if (g->mPlayerImageNum >= gNumPlayerImages) g->mPlayerImageNum = 0;
					switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
					strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
				}
				break;
			case BUTTON_ID_BODY_2B:
				gSpinSpeed[1] = SPIN_MAX;
				g = gGliders[1];
				if (g && gNumPlayerImages > 0) {
					g->mPlayerImageNum++;
					if (g->mPlayerImageNum < 0) g->mPlayerImageNum = gNumPlayerImages-1;
					if (g->mPlayerImageNum >= gNumPlayerImages) g->mPlayerImageNum = 0;
					switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
					strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
				}
				break;
			case BUTTON_ID_BOARD_1A:
				gSpinSpeed[0] = -SPIN_MAX;
				g = gGliders[0];
				if (g && gNumBoardImages > 0) {
					g->mBoardImageNum--;
					if (g->mBoardImageNum < 0) g->mBoardImageNum = gNumBoardImages-1;
					if (g->mBoardImageNum >= gNumBoardImages) g->mBoardImageNum = 0;
					switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
				}
				break;
			case BUTTON_ID_BOARD_1B:
				gSpinSpeed[0] = SPIN_MAX;
				g = gGliders[0];
				if (g && gNumBoardImages > 0) {
					g->mBoardImageNum++;
					if (g->mBoardImageNum < 0) g->mBoardImageNum = gNumBoardImages-1;
					if (g->mBoardImageNum >= gNumBoardImages) g->mBoardImageNum = 0;
					switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
				}
				break;
			case BUTTON_ID_BOARD_2A:
				gSpinSpeed[1] = -SPIN_MAX;
				g = gGliders[1];
				if (g && gNumBoardImages > 0) {
					g->mBoardImageNum--;
					if (g->mBoardImageNum < 0) g->mBoardImageNum = gNumBoardImages-1;
					if (g->mBoardImageNum >= gNumBoardImages) g->mBoardImageNum = 0;
					switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
				}
				break;
			case BUTTON_ID_BOARD_2B:
				gSpinSpeed[1] = SPIN_MAX;
				g = gGliders[1];
				if (g && gNumBoardImages > 0) {
					g->mBoardImageNum++;
					if (g->mBoardImageNum < 0) g->mBoardImageNum = gNumBoardImages-1;
					if (g->mBoardImageNum >= gNumBoardImages) g->mBoardImageNum = 0;
					switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
				}
				break;
			case BUTTON_ID_PROCEED1:
				mCurrentSlide = SLIDE_BUTTON_DIFFICULTY;
				break;
			case BUTTON_ID_1PLAYER:
				gNumPlayers = 1;
				ScanPlayerImages();
				mCurrentSlide = SLIDE_BUTTON_CHOOSE1PLAYER;
				break;
			case BUTTON_ID_2PLAYER:
				gNumPlayers = 2;
				ScanPlayerImages();
				mCurrentSlide = SLIDE_BUTTON_CHOOSE2PLAYER;
				break;
			case BUTTON_ID_EASY:
				gDifficultySetting = DIFFICULTY_EASY;
				NewGame(gNumPlayers);
				GameToSaveSlot();
				WriteSaves();
				break;
			case BUTTON_ID_MEDIUM:
				gDifficultySetting = DIFFICULTY_MEDIUM;
				NewGame(gNumPlayers);
				GameToSaveSlot();
				WriteSaves();
				break;
			case BUTTON_ID_HARD:
				gDifficultySetting = DIFFICULTY_HARD;
				NewGame(gNumPlayers);
				GameToSaveSlot();
				WriteSaves();
				break;
			case BUTTON_ID_BACK:
				mCurrentSlide = SLIDE_BUTTON_PLAY;
				break;
			default:
				break;
				}
				break;
			case SLIDE_PAUSE:
				switch (mCurrentButton) {
			case BUTTON_ID_MAW:
			case BUTTON_ID_HELP:
			case BUTTON_ID_THANKS:
			case BUTTON_ID_OPTIONS:
			case BUTTON_ID_FORTUNE:
				StartAlertSlide(mCurrentButton);
				break;
			case BUTTON_ID_QUIT:
				gGameMode = GAME_MODE_SLIDES;
				mCurrentSlide = SLIDE_BUTTON_PLAY;
				break;
			case BUTTON_ID_PLAY:
				gPaused = false;
				break;
			case BUTTON_ID_1PLAYER:
				if (gNumPlayers == 2) {
					gNumPlayers = 1;
					gActiveGliderList = gGliders[0];
					gGliders[0]->mNext = NULL;
					gGliders[1]->mNext = NULL;
				}
				break;
			case BUTTON_ID_2PLAYER:
				if (gNumPlayers == 1) {
					gNumPlayers = 2;
					gActiveGliderList = gGliders[0];
					gGliders[0]->mNext = gGliders[1];
					gGliders[1]->mNext = NULL;
				}
				g = gGliders[1];
				SETFLAG(g->mFlags, GLIDER_FLAG_ACTIVE);
				pfSetVec3(g->mMatrix[PF_T], 0, 0, 0);
				pfSetVec3(g->mVelocity, 0, 0, 0);
				pfCopyMat(g->mLastMatrix, g->mMatrix);
				g->mTrailNext = -1;
				break;
			default:
				if (gAlertButton) gAlertButton = 0;
				break;
				}
				break;
			case SLIDE_PRE_TITLE:
				if (gIntroStage < INTRO_FADE_DOWN) {
					gIntroTimer = 0.0f;
					gIntroStage = INTRO_FADE_DOWN;
				}
				break;
			case SLIDE_TITLE:
				NextSlide();
				if (mCurrentSlide == SLIDE_START) {
					playSound2D(VO_BEN_PRESSSTART);
					playSound2D(VO_SET_PRESSSTART);
				} else if (mCurrentSlide == SLIDE_TITLE) {
					playSound2D(VO_BEN_BENSGAME);
					playSound2D(VO_SET_BENSGAME);
				}
				break;
				//	case SLIDE_START:
				//		NewGame(1);
				//
				//		gCells->Clear();
				//		for (g = gActiveGliderList; g; g = g->mNext) {
				//			pfSetVec3(g->mMatrix[PF_T], 0, 0, 0);
				//			pfSetVec3(g->mVelocity, 0, 0, 0);
				//			pfCopyMat(g->mLastMatrix, g->mMatrix);
				//		}
				//		gGameMode = GAME_MODE_PORT;
				//		break;
			default:
				break;
		}
	}
}


void Slides::MousePos(int x, int y)
{
	int	oldButton = mCurrentButton;

	mCurrentButton = GetRectButton(mSlideList[mCurrentSlide].mButtonList, x, y, NULL);
	if (mCurrentButton && (mCurrentButton != oldButton)) {
		playSound2D(SOUND_BUTTON_GROW, 0.9f, RANDOM_IN_RANGE(0.7f, 1.2f));
	}

	if (gIntroStage == INTRO_SLIDE_ALL_DONE) {
		int win_width, win_height;
		SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
		float	fx = x / (win_width / 640.0f);
		float	fy = y / (win_height / 480.0f);
		pfSetVec3(gFireFlies->mCenter, fx, fy, 0);
	}

	switch (mCurrentSlide) {
	case SLIDE_BUTTON_PLAY:
		if (1) {
			/**** Add the flags to the button list! ****/
			int	i, slot = 0;
			for (i = 0; original_buttonListPlay[i]; i++) {
				buttonListPlay[slot++] = original_buttonListPlay[i];
			}
			for (i = 0; i < gNumLanguages; i++) {
				buttonListPlay[slot++] = BUTTON_ID_FLAG_00+i;
			}
			buttonListPlay[slot] = 0;
		}
		break;
	case SLIDE_YOU_WIN:
	case SLIDE_BUTTON_SAVE:
	case SLIDE_BUTTON_NUMPLAYERS:
	case SLIDE_BUTTON_CHOOSE1PLAYER:
	case SLIDE_BUTTON_CHOOSE2PLAYER:
	case SLIDE_BUTTON_DIFFICULTY:
		break;
	default:
		break;
	}
}

void Slides::DrawAlertBubble(void)
{
	float angle, radius, sval , cval;
	int		i;
	float	alpha = gAlertFader*gAlertFader;
	switch (gAlertButton) {
		case BUTTON_ID_MAW:
			if (1) {
				pfVec4	fc1 = {1.0f,1.0f,1,alpha};
				pfVec4	fc2 = {0.5f,0.5f,1,alpha};
				pfVec4	fc3 = {1.0f,1.0f,0.5f,alpha};
				pfVec4	fc4 = {0.7f,0.7f,0.7f,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetBox(100, 100, 640, 480);
				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);

				gFontSys->SetScale(1, 1);
				gFontSys->SetPos(320, 80-2*30);
				gFontSys->DrawString(TRANSLATE(TXT_The_Greater_Bay_Area));
				gFontSys->SetScale(1.25, 1.5);
				gFontSys->SetPos(320, 100-2*30);
				gFontSys->DrawString(TRANSLATE(TXT_Make_A_Wish_Foundation));
				gFontSys->SetScale(0.6, 0.6);
				gFontSys->SetPos(320, 110-1*30);
				gFontSys->SetColor(fc1, fc4);
				gFontSys->DrawString("www.makewish.org");
				gFontSys->SetColor(fc1, fc2);
				
				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(0.6, 0.6);
				gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);
				gFontSys->SetPos(100, 100);
				gFontSys->SetBox(100, 100, 640-100, 480-100);
				gFontSys->DrawString(TRANSLATE(TXT_This_game_was_made_possible_when_Ben_and_Eric_were_introduced_CONT));
				gFontSys->SetBox(100, 100, 640, 480);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetScale(0.5f, 0.5f);
				gFontSys->SetColor(fc1, fc3);
				gFontSys->SetPos(320, 245);
				gFontSys->DrawString(TRANSLATE(TXT_These_buttons_will_take_you_to_the_Make_A_Wish_web_pages));
				gFontSys->SetPos(320, 365);
				gFontSys->DrawString(TRANSLATE(TXT_Make_a_Donation));
				gFontSys->SetPos(200, 355);
				gFontSys->DrawString(TRANSLATE(TXT_Visit_Make_A_Wish));
				gFontSys->SetPos(440, 355);
				gFontSys->DrawString(TRANSLATE(TXT_Bens_Game_Page));
				gFontSys->EndDraw();

				UseLibTexture(TEXID_CIRCLE_R);
				glColor4f(1,1,1,1);
				hudQuickRect(470,40,16,16);
			}
			if (1) {
				TendButtonPos(BUTTON_ID_ALERT_WEB_DONATION_SITE, 320,320, 500);
				TendButtonSize(BUTTON_ID_ALERT_WEB_DONATION_SITE,		96, 96, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_WEB_DONATION_SITE);

				TendButtonPos(BUTTON_ID_ALERT_WEB_MAKEWISH_SITE, 200,320, 500);
				TendButtonSize(BUTTON_ID_ALERT_WEB_MAKEWISH_SITE,		64, 64, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_WEB_MAKEWISH_SITE);

				TendButtonPos(BUTTON_ID_ALERT_WEB_BENSGAME_SITE, 440,320, 500);
				TendButtonSize(BUTTON_ID_ALERT_WEB_BENSGAME_SITE,		64, 64, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_WEB_BENSGAME_SITE);

				TendButtonPos(BUTTON_ID_ALERT_BACK, 320,440, 500);
				TendButtonSize(BUTTON_ID_ALERT_BACK,		64, 64, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_BACK);
			}
			break;
		case BUTTON_ID_FORTUNE:
			if (1) {
				pfVec4	fc1 = {0.5f,0.5f,1,alpha};
				pfVec4	fc2 = {0,0,1,alpha};
				pfVec4	fc3 = {1.0f,1.0,1,alpha};
				pfVec4	fc4 = {1,1,0,alpha};
				char	str[2048];
				int		len;
				static float	blinkTimer = 0.0f;
				static float	keyTimer = 0.0f;
				static bool		bar = false;

				strcpy(str, benQuotes[benCurrentQuote]);
				len = strlen(str);
				blinkTimer -= 8.0f * DeltaTime;
				if (blinkTimer < 0.0f) {
					bar = !bar;
					blinkTimer += 1.0f;
				}
				keyTimer -= DeltaTime;
				if (keyTimer < 0.0f) {
					gKeyNumber++;
					keyTimer = RANDOM_IN_RANGE(0.1f, 0.5f);
				}
				
				if (gKeyNumber < 0) {
					str[0] = 0;
				} else {
					if (gKeyNumber < len) {
						if (1 || bar) {
							char	*cp;
							int		count;

							cp = str;
							for (count = 0; *cp && count < gKeyNumber; count++) {
								if (*cp == '^') cp += 4;
								else cp++;
							}
							cp[0] = '_';
							cp[1] = 0;
						} else {
							str[gKeyNumber] = 0;
						}
					}
				}


				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1.25, 1.5);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 150-1*30);
				gFontSys->DrawString(TRANSLATE(TXT_A_Message_From_Ben));

				gFontSys->SetColor(fc3, fc4);
				gFontSys->SetScale(1.0f, 1.0f);
				gFontSys->SetPos(320, 240);
				gFontSys->DrawString(str);
				gFontSys->EndDraw();
			}
			if (1) {				
				TendButtonPos(BUTTON_ID_ALERT_BACK,		320, 440, 500);
				TendButtonSize(BUTTON_ID_ALERT_BACK,		96, 96, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_BACK);
			}
			break;
		case BUTTON_ID_OPTIONS:
			if (1) {
				pfVec4	fc1 = {0.5f,0.5f,1,alpha};
				pfVec4	fc2 = {0,0,1,alpha};
				pfVec4	fc3 = {1,1,1,alpha};
				pfVec4	fc4 = {0.5f,1,1,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1.25, 1.5);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 100-1*30);
				gFontSys->DrawString(TRANSLATE(TXT_Game_Options));

				gFontSys->EndDraw();
			}
			if (1) {
				float delta = 20;
				angle = -90+(-delta*2);
				radius = 150;
				pfSinCos(angle, &sval, &cval);

				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0,1,0,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);
				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(0.75f, 0.75f);
				gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);

				gFontSys->SetBox(100, 100, 640-100, 480-100);

				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_WINDOW, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_WINDOW,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Start_up_Bens_Game_in_a_window_next_time));

				angle += delta;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_MIPMAP, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_MIPMAP,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Use_higher_quality_graphics__may_be_slower_));

				angle += delta;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_SOUND, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_SOUND,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Enable_sound_effects));

				angle += delta;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_VOICE, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_VOICE,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Enable_character_voices));

				angle += delta;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_WAVES, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_WAVES,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Wavy_motion_for_cells));

				angle += delta;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_CHECKBOX_SAVE, 320+radius*sval,240+radius*cval, 500);
				TendButtonSize(BUTTON_ID_ALERT_CHECKBOX_SAVE,		48, 48, 200);
				gFontSys->SetPos((28)+320+radius*sval,(-10)+240+radius*cval);
				gFontSys->DrawString(TRANSLATE(TXT_Auto_save_my_progress));

				gFontSys->EndDraw();

				TendButtonPos(BUTTON_ID_ALERT_BACK,		320, 440, 500);
				TendButtonSize(BUTTON_ID_ALERT_BACK,		96, 96, 200);

				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_WINDOW);
				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_MIPMAP);
				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_SOUND);
				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_VOICE);
				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_WAVES);
				DrawStandardUIButton(BUTTON_ID_ALERT_CHECKBOX_SAVE);
				DrawStandardUIButton(BUTTON_ID_ALERT_BACK);

				gFontSys->SetBox(100, 100, 640, 480);
			}
			break;
		case BUTTON_ID_HELP:
			if (0) {
				pfVec4	fc1 = {0.5f,0.5f,1,alpha};
				pfVec4	fc2 = {0,0,1,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1.25, 1.5);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 150-1*30);
				gFontSys->DrawString(TRANSLATE(TXT_How_to_Play));

				gFontSys->EndDraw();
			}
			DrawBoxes(gHelpBoxList[gHelpScreen]);
			if (1) {				
				TendButtonPos(BUTTON_ID_ALERT_NEXT,		440, 340, 500);
				TendButtonSize(BUTTON_ID_ALERT_NEXT,		64, 64, 500);
				DrawStandardUIButton(BUTTON_ID_ALERT_NEXT);
				TendButtonPos(BUTTON_ID_ALERT_BACK,		320, 440, 500);
				TendButtonSize(BUTTON_ID_ALERT_BACK,		96, 96, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_BACK);
			}
			break;
		case BUTTON_ID_THANKS:
			if (1) {
				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0,0,1,alpha};
				pfVec4	fc3 = {1,1,1,alpha};
				pfVec4	fc4 = {1,1,0,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1, 1);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 40);
				gFontSys->DrawString(TRANSLATE(TXT_Ben_and_Eric_wish_to_say_THANKS_to_the_following_people));
			}

			switch (gThanksScreen) {
			case THANKS_SCREEN_THANKS:
				{
					pfVec4	fc3 = {1,1,1,alpha};
					pfVec4	fc4 = {1,1,0,alpha};
					gFontSys->BeginDraw(FONT_ID_MAIN);
					gFontSys->SetColor(fc3, fc4);
					gFontSys->SetScale(0.5f, 0.5f, true);
					gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
					gFontSys->SetPos(320-100, 120);
					gFontSys->DrawString(gThanksNames[0]);
					gFontSys->SetPos(320+100, 120);
					gFontSys->DrawString(gThanksNames[1]);
					gFontSys->EndDraw();
					if (1) {
						/**** LEC logo ****/
						float	size = 48;
						float	x = 320-size/2, y = 285-size/2;
						UseLibTexture(TEXID_LEC_LOGO);
						glColor4f(1,1,1,1);
						glBegin(GL_QUADS);
						glTexCoord2f(0,1);
						glVertex3f(x, y,0);
						glTexCoord2f(1,1);
						glVertex3f(x+size, y,0);
						glTexCoord2f(1,0);
						glVertex3f(x+size, y+size,0);
						glTexCoord2f(0,0);
						glVertex3f(x, y+size,0);
						glEnd();
					}
				}
				break;
			case THANKS_SCREEN_CREDITS:
				{
					pfVec4	fc3 = {1,1,1,alpha};
					pfVec4	fc4 = {0,1,0,alpha};
					pfVec4	fc5 = {1,1,0,alpha};
					gFontSys->BeginDraw(FONT_ID_MAIN);
					gFontSys->SetColor(fc3, fc4);
					gFontSys->SetScale(0.5f, 0.5f, true);
					gFontSys->SetAlign(FONT_ALIGNMENT_RIGHT);
					for (i = 0; gCreditTitles[i]; i++) {
						gFontSys->SetPos(320-8, 120 + 11*i);
						gFontSys->DrawString(gCreditTitles[i]);
					}
					gFontSys->SetColor(fc3, fc5);
					gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);
					for (i = 0; gCreditNames[i]; i++) {
						gFontSys->SetPos(320+8, 120 + 11*i);
						gFontSys->DrawString(gCreditNames[i]);
					}
					gFontSys->EndDraw();
				}
				break;
			case THANKS_SCREEN_BETA_1:
			case THANKS_SCREEN_BETA_2:
				{
					pfVec4	fc3 = {1,1,1,alpha};
					pfVec4	fc4 = {0,1,0,alpha};
					pfVec4	fc5 = {1,1,0,alpha};
					int	numNames = sizeof(gBetaTesterNames)/sizeof(gBetaTesterNames[0]);
					int screen = gThanksScreen - THANKS_SCREEN_BETA_1;
					int	rowCount = 27;
					int	colCount = 5;
					int	screenNameCount = rowCount * colCount;
					int	startName = screen * screenNameCount;
					int	endName = startName + screenNameCount;

					if (startName > numNames) startName = numNames;
					if (endName > numNames) endName = numNames;

					gFontSys->BeginDraw(FONT_ID_MAIN);

					gFontSys->SetColor(fc3, fc4);
					gFontSys->SetScale(0.6f, 0.6f, true);
					gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
					gFontSys->SetPos(320, 120);
					gFontSys->DrawString("Beta Testing");

					gFontSys->SetColor(fc3, fc5);
					gFontSys->SetScale(0.4f, 0.4f, true);
					gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
					for (i = startName; i < endName; i++) {
						int	index = (i - startName);
						int row = index % rowCount;
						int	col = index / rowCount;
						int x = 120 + 100 * col;
						int y = 140 + 9*row;
						gFontSys->SetPos(x, y);
						gFontSys->DrawString(gBetaTesterNames[i]);
					}
					gFontSys->EndDraw();
				}
				break;
			}
			if (1) {				
				angle = 55;
				radius = 250;
				pfSinCos(angle, &sval, &cval);
				TendButtonPos(BUTTON_ID_ALERT_BACK,		320, 440, 500);
				TendButtonSize(BUTTON_ID_ALERT_BACK,		96, 96, 200);
				DrawStandardUIButton(BUTTON_ID_ALERT_BACK);
			}
			if (1) {				
				TendButtonPos(BUTTON_ID_ALERT_NEXT,		440, 440, 500);
				TendButtonSize(BUTTON_ID_ALERT_NEXT,		64, 64, 500);
				DrawStandardUIButton(BUTTON_ID_ALERT_NEXT);
			}
			break;
		case BUTTON_ID_QUIT:
			if (1) {
				pfVec4	fc1 = {1.0f,1.0f,1.0f,alpha};
				pfVec4	fc2 = {1.0f,1.0f,0.5f,alpha};
//				pfVec4	fc1 = {RANDOM0TO1,RANDOM0TO1,RANDOM0TO1,alpha};
//				pfVec4	fc2 = {RANDOM0TO1,RANDOM0TO1,RANDOM0TO1,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 150-0*30);
				gFontSys->SetScale(1.0f, 1.0f);
				gFontSys->DrawString(TRANSLATE(TXT_Are_you_sure_you_want_to_quit));
				
				gFontSys->EndDraw();
			}
			if (1) {				
				TendButtonPos(BUTTON_ID_ALERT_QUIT, 320, 240+30, 500);
				TendButtonPos(BUTTON_ID_ALERT_NOQUIT, 320-100, 240+30, 500);
				
				TendButtonSize(BUTTON_ID_ALERT_QUIT,		128, 128, 500);
				TendButtonSize(BUTTON_ID_ALERT_NOQUIT,		64, 64, 500);
				DrawStandardUIButton(BUTTON_ID_ALERT_QUIT);
				DrawStandardUIButton(BUTTON_ID_ALERT_NOQUIT);
			}

			break;
		default: break;
	}
}

void Slides::DrawStandardUIButton(int buttonID, bool mirror)
{
	float	t, alertx = 320, alerty = 240, alertw = 800, alerth = 800;
	float	x, y, w, h, alpha;

	if (1) {
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);

		alpha = 1.0f - (0.95f * gAlertFader);
		if (buttonID >= BUTTON_ID_ALERT_OK) {
			alpha = 1.0f;
		}
//		glColor4f(alpha,alpha,alpha,1);
		glColor4f(1,1,1,alpha);
		w = buttonRects[buttonID].w * buttonRects[buttonID].selectScale;
		h = buttonRects[buttonID].h * buttonRects[buttonID].selectScale;
		x = buttonRects[buttonID].cx - w/2;
		y = buttonRects[buttonID].cy - h/2;

		if (buttonID == gAlertButton) {
			t = gAlertFader;
			glColor4f(0.5f,0.5f,0.5f,1);
			UseLibTexture(TEXID_BUTTON_SKINNY);
			w = (alertw * t) + (w * (1-t));
			h = (alerth * t) + (h * (1-t));
			y = ((alerty-alerth/2) * t) + (y * (1-t));
			x = ((alertx-alertw/2) * t) + (x * (1-t));
			hudQuickRect(x, y, w, h);
		} else {
			UseLibTexture(buttonRects[buttonID].tex);
			if (mirror) hudQuickRect(x+w, y, -w, h);
			else hudQuickRect(x, y, w, h);
			if (mCurrentButton == buttonID) {
				if (1) {
					gHintText = buttonRects[buttonID].hint;
					gHintPos[PF_X] = buttonRects[buttonID].hintX;
					gHintPos[PF_Y] = buttonRects[buttonID].hintY;
					gHintFader = 1.0f;
					if (gAlertButton == BUTTON_ID_QUIT) {
					} else if (gAlertButton == BUTTON_ID_MAW) {
						gHintText = NULL;
					} else if (gAlertButton == BUTTON_ID_THANKS) {
						gHintText = NULL;
					} else if (gAlertButton == BUTTON_ID_FORTUNE) {
						gHintText = NULL;
					} else if (gAlertButton == BUTTON_ID_HELP) {
						gHintText = NULL;
					} else if (gAlertButton == BUTTON_ID_OPTIONS) {
						gHintText = NULL;
					} else if (mCurrentSlide == SLIDE_BUTTON_PLAY) {
					} else if (mCurrentSlide == SLIDE_PAUSE) {
						gHintPos[PF_X] = 320;
						gHintPos[PF_Y] = 245;
					} else if (mCurrentSlide == SLIDE_BUTTON_SAVE) {
						gHintPos[PF_X] = 320;
						gHintPos[PF_Y] = 395;
					} else {
						gHintPos[PF_X] = buttonRects[buttonID].cx;
						gHintPos[PF_Y] = buttonRects[buttonID].cy + (20.0f + 0.5f * buttonRects[buttonID].h);
						if (gHintPos[PF_X] < 70) {
							gHintPos[PF_X] = 70;
							gHintPos[PF_Y] = buttonRects[buttonID].cy - (30.0f + 0.5f * buttonRects[buttonID].h);
						}
						if (gHintPos[PF_X] > 640-75) {
							gHintPos[PF_X] = 640-75;
							gHintPos[PF_Y] = buttonRects[buttonID].cy - (30.0f + 0.5f * buttonRects[buttonID].h);
						}
					}
				}

				//if (gAlertFader == 0.0f) {
					float bright = RANDOM_IN_RANGE(0.25f, 1.0f);
					if (buttonID == BUTTON_ID_EASY) {
						hudQuickRect(x, y, w, h);
						glColor4f(0,bright,0,1);
					} else if (buttonID == BUTTON_ID_MEDIUM) {
						hudQuickRect(x, y, w, h);
						glColor4f(bright, bright,0,1);
					} else if (buttonID == BUTTON_ID_HARD) {
						hudQuickRect(x, y, w, h);
						glColor4f(bright,0,0,1);
					} else {
						glColor4f(RANDOM_IN_RANGE(0.75f, 1.0f),RANDOM_IN_RANGE(0.75f, 1.0f),RANDOM_IN_RANGE(0.75f, 1.0f),1);
					}
					UseLibTexture(TEXID_BUTTON_BLANK);
					hudQuickRect(x, y, w, h);
				//}
				glColor4f(1,1,1,1);
				buttonRects[buttonID].selectScale += 2.0f * UnscaledDeltaTime;
				if (buttonRects[buttonID].selectScale > 1.25f) buttonRects[buttonID].selectScale = 1.25f;
//printf("scale %f\n", buttonRects[buttonID].selectScale);
			} else {
				buttonRects[buttonID].selectScale -= 2.0f * UnscaledDeltaTime;
				if (buttonRects[buttonID].selectScale < 1.0f) buttonRects[buttonID].selectScale = 1.0f;
			}
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void Slides::TendButtonSize(int buttonID, float w, float h, float speed)
{
//	speed = 1.0f;
//	buttonRects[buttonID].w += UnscaledDeltaTime * speed * (w - buttonRects[buttonID].w);
//	buttonRects[buttonID].h += UnscaledDeltaTime * speed * (h - buttonRects[buttonID].h);

	if (buttonRects[buttonID].w < w) {
		buttonRects[buttonID].w += UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].w > w) buttonRects[buttonID].w = w;
	} else if (buttonRects[buttonID].w > w) {
		buttonRects[buttonID].w -= UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].w < w) buttonRects[buttonID].w = w;
	}
	if (buttonRects[buttonID].h < h) {
		buttonRects[buttonID].h += UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].h > h) buttonRects[buttonID].h = h;
	} else if (buttonRects[buttonID].h > h) {
		buttonRects[buttonID].h -= UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].h < h) buttonRects[buttonID].h = h;
	}

}

void Slides::TendButtonPos(int buttonID, float cx, float cy, float speed)
{
	if (buttonRects[buttonID].cx < cx) {
		buttonRects[buttonID].cx += UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].cx > cx) buttonRects[buttonID].cx = cx;
	} else if (buttonRects[buttonID].cx > cx) {
		buttonRects[buttonID].cx -= UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].cx < cx) buttonRects[buttonID].cx = cx;
	}
	if (buttonRects[buttonID].cy < cy) {
		buttonRects[buttonID].cy += UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].cy > cy) buttonRects[buttonID].cy = cy;
	} else if (buttonRects[buttonID].cy > cy) {
		buttonRects[buttonID].cy -= UnscaledDeltaTime * speed;
		if (buttonRects[buttonID].cy < cy) buttonRects[buttonID].cy = cy;
	}
}

void Slides::DrawSelectionModel(int player, float cx, float cy, float scale)
{
	Glider	*g;
	static float	spin[2] = {0,90};

	g = gGliders[player];
	if (g == NULL) return;

	if (gSpinSpeed[player] > 0.0f) {
		gSpinSpeed[player] -= SPIN_REDUCE_RATE * UnscaledDeltaTime;
		if (gSpinSpeed[player] < SPIN_MIN) gSpinSpeed[player] = SPIN_MIN;
	} else {
		gSpinSpeed[player] += SPIN_REDUCE_RATE * UnscaledDeltaTime;
		if (gSpinSpeed[player] > -SPIN_MIN) gSpinSpeed[player] = -SPIN_MIN;
	}

//vvvvv	spin[player] += 0.2f * gSpinSpeed[player] * UnscaledDeltaTime;
	spin[player] += gSpinSpeed[player] * UnscaledDeltaTime;
	if (spin[player] > 360.0f) {
		spin[player] -= 360.0f;
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(cx, cy, 0);
	glScalef(scale, scale, scale);
	glRotatef(70, 1, 0, 0);
	glRotatef(spin[player], 0, 0, 1);
//	glRotatef(spin[player], 0, 1, 0);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

glDisable(GL_DEPTH_TEST);

	g->DrawDude();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Slides::DrawWobbleButton(float x, float y, float w, float h)
{
	float	alpha = 1.0f - gAlertFader;
	float	sval, cval;

	glColor4f(1,1,1,alpha);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(x, y, 0);
	glTexCoord2f(0,0);
	pfSinCos(mWobbleTimer[0] * 360.0f, &sval, &cval);
	glVertex3f((x-w*mWobbleScale)+sval*mWobbleX, (y+h*mWobbleScale)+cval*mWobbleY, 0);
	glTexCoord2f(1,0);
	pfSinCos(mWobbleTimer[1] * 360.0f, &sval, &cval);
	glVertex3f((x+w*mWobbleScale)+sval*mWobbleX, (y+h*mWobbleScale)+cval*mWobbleY, 0);
	glTexCoord2f(1,1);
	pfSinCos(mWobbleTimer[2] * 360.0f, &sval, &cval);
	glVertex3f((x+w*mWobbleScale)+sval*mWobbleX, (y-h*mWobbleScale)+cval*mWobbleY, 0);
	glTexCoord2f(0,1);
	pfSinCos(mWobbleTimer[3] * 360.0f, &sval, &cval);
	glVertex3f((x-w*mWobbleScale)+sval*mWobbleX, (y-h*mWobbleScale)+cval*mWobbleY, 0);
	glTexCoord2f(0,0);
	pfSinCos(mWobbleTimer[0] * 360.0f, &sval, &cval);
	glVertex3f((x-w*mWobbleScale)+sval*mWobbleX, (y+h*mWobbleScale)+cval*mWobbleY, 0);
	glEnd();
}

#define BACKFAN_COUNT	36
void Slides::DrawBackFan(void)
{
	static float	svals[BACKFAN_COUNT];
	static float	cvals[BACKFAN_COUNT];
	static bool		init = false;
	int				i;
	float			radius = 401.0f;

	if (!init) {
		for (i = 0; i < BACKFAN_COUNT; i++) {
			pfSinCos((float)i*360.0f/(float)BACKFAN_COUNT, &(svals[i]), &(cvals[i]));
		}
		init = true;
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0,0,0,1);
	glVertex3f(320, 240, 0);
	glColor4f(93.0f/255.0f,0,202.0f/255.0f,1);
	for (i = 0; i <= BACKFAN_COUNT; i++) {
		float x = 320+radius*svals[i%BACKFAN_COUNT];
		float y = 240+radius*cvals[i%BACKFAN_COUNT];
//		if (y < 240-160) y = 240-160;
//		if (y > 240+160) y = 240+160;
		glVertex3f(x, y, 0);
	}
	glEnd();
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	glColor4f(0,0,0,1); glVertex3f(0, 0, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 0, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 40, 0);
	glColor4f(0,0,0,1); glVertex3f(0, 40, 0);

	glColor4f(0,0,0,1); glVertex3f(0, 40, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 40, 0);
	glColor4f(0,0,0,0); glVertex3f(640, 80, 0);
	glColor4f(0,0,0,0); glVertex3f(0, 80, 0);

	glColor4f(0,0,0,1); glVertex3f(0, 480, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 480, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 440, 0);
	glColor4f(0,0,0,1); glVertex3f(0, 440, 0);

	glColor4f(0,0,0,1); glVertex3f(0, 440, 0);
	glColor4f(0,0,0,1); glVertex3f(640, 440, 0);
	glColor4f(0,0,0,0); glVertex3f(640, 400, 0);
	glColor4f(0,0,0,0); glVertex3f(0, 400, 0);
	glEnd();
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void Slides::Draw(void)
{
	int		win_width, win_height, i;
	pfVec4	hue = {1,1,1,1};
	float	x = 0, y = 0, w = 100, h = 100, alpha;
	int		texWidth, texHeight;
	float	win_aspect, aspect640x480, sval, cval;
	float	baseScale = 640.0f / 1024.0f;
//	static float	bx = 0.0f, by = 0.0f, scale = 0.0f;
	Glider	*g;

	float	px1, px2;

	SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);

	win_aspect = (float)win_width/(float)win_height;
	aspect640x480 = 640.0f/480.0f;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, -1000, 1000);
//	glOrtho(0, 640, 480*aspect640x480/win_aspect, 0, -1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glDepthMask(false);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	if (mSlideList[mCurrentSlide].mTexName) {
		if (getTextureInfo(mSlideList[mCurrentSlide].mTexID, NULL, &texWidth, &texHeight)) {
			w = texWidth * baseScale;
			h = texHeight * baseScale * (win_aspect/aspect640x480);
			x = 320.0f - (w * 0.5f);
			y = 240.0f - (h * 0.5f);
			UseLibTexture(mSlideList[mCurrentSlide].mTexID);
//glDisable(GL_TEXTURE_2D);//vvvvv
			glColor4fv(hue);
			hudQuickRect(x, y, w, h);
		}
	} else if (mCurrentSlide != SLIDE_PAUSE) {
		DrawBackFan();
	}

	/**** now some special drawing? ****/
	switch (mCurrentSlide) {
		case SLIDE_PRE_TITLE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_TEXTURE_2D);
			x = gIntroWipeTimer * 2*640.0f;
			glBegin(GL_QUADS);
			glColor4f(0,0,0,1); glVertex3f(x+100, 100, 0);
			glColor4f(0,0,0,1); glVertex3f(x+640-100, 100, 0);
			glColor4f(0,0,0,1); glVertex3f(x+640-100, 480-100, 0);
			glColor4f(0,0,0,1); glVertex3f(x+100, 480-100, 0);
			glColor4f(0,0,0,0); glVertex3f(x-100, 100, 0);
			glColor4f(0,0,0,1); glVertex3f(x+100, 100, 0);
			glColor4f(0,0,0,1); glVertex3f(x+100, 480-100, 0);
			glColor4f(0,0,0,0); glVertex3f(x-100, 480-100, 0);
			glEnd();

			if (gIntroFader > 0.0f) {
				glColor4f(0,0,0,gIntroFader);
				hudQuickRect(0, 0, 640, 480);
			}

			glEnable(GL_TEXTURE_2D);
			break;

		case SLIDE_TITLE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_TEXTURE_2D);
			if (gIntroFader > 0.0f) {
				glColor4f(0,0,0,gIntroFader);
				hudQuickRect(0, 0, 640, 480);
			}
			glColor4f(1,1,1,1);
			glEnable(GL_TEXTURE_2D);
			if (1) {
				alpha = 1.0f;
				pfVec4	fc1 = {1,1,0,alpha};
				pfVec4	fc2 = {0.5f,0.5f,0,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(0.7f, 0.7f);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(320, 370);
				gFontSys->DrawString("Version: Release 1 (August 10, 2004)");
				gFontSys->SetPos(320, 420);
				gFontSys->SetScale(0.5f, 0.5f);
				gFontSys->DrawString(TRANSLATE(TXT_Welcome_Bens_Game_To_get_an_updated_version_or_to_make_comments_and_report_bugs_please_visit_wwwmakewishorg_ben));

				gFontSys->EndDraw();
			}
			break;

		case SLIDE_PAUSE:
			glEnable(GL_BLEND);
			if (1) {
				/**** darken the area behind ****/
				float	alpha = 0.75f * gPauseFader;
				int		sides = 20;
				float	angle, sval, cval, xradius = 400, yradius = 320;
				glDisable(GL_TEXTURE_2D);
				glColor4f(0,0,0,alpha);
				glBegin(GL_TRIANGLE_FAN);
				glVertex3f(320, 240, 0);
				glColor4f(0,0,0,0);
				for (i = 0; i <= sides; i++) {
					angle = 360.0f * (float)i / (float)sides;
					pfSinCos(angle, &sval, &cval);
					glVertex3f(320+xradius*sval, 240+yradius*cval, 0);
				}
				glEnd();
				glEnable(GL_TEXTURE_2D);
			}
			TendButtonSize(BUTTON_ID_MAW,		64, 64, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_QUIT,		64, 64, 200);
			TendButtonSize(BUTTON_ID_THANKS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_HELP,		64, 64, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	64, 64, 200);
			TendButtonSize(BUTTON_ID_PLAY,		96, 96, 200);
			
			TendButtonPos(BUTTON_ID_MAW,		320-1.5*60, 240-50, 500);
			TendButtonPos(BUTTON_ID_THANKS,		320-2*60, 240, 500);
			TendButtonPos(BUTTON_ID_FORTUNE,	320-1.5*60, 240+50, 500);
			TendButtonPos(BUTTON_ID_HELP,		320+1.5*60, 240-50, 500);
			TendButtonPos(BUTTON_ID_OPTIONS,	320+2*60, 240, 500);
			TendButtonPos(BUTTON_ID_QUIT,		320+1.5*60, 240+50, 500);
			TendButtonPos(BUTTON_ID_PLAY,		320, 240-50, 500);

			DrawStandardUIButton(BUTTON_ID_MAW);
			DrawStandardUIButton(BUTTON_ID_OPTIONS);
			DrawStandardUIButton(BUTTON_ID_QUIT);
			DrawStandardUIButton(BUTTON_ID_THANKS);
			DrawStandardUIButton(BUTTON_ID_HELP);
			DrawStandardUIButton(BUTTON_ID_FORTUNE);
			DrawStandardUIButton(BUTTON_ID_PLAY);
			if (gNumPlayers == 1) {
				TendButtonSize(BUTTON_ID_1PLAYER,		0, 0, 200);
				TendButtonSize(BUTTON_ID_2PLAYER,		64, 64, 200);
				TendButtonPos(BUTTON_ID_1PLAYER,	320+0*60, 240+50+20, 500);
				TendButtonPos(BUTTON_ID_2PLAYER,	320+0*60, 240+50+20, 500);
				DrawStandardUIButton(BUTTON_ID_1PLAYER);
				DrawStandardUIButton(BUTTON_ID_2PLAYER);
			} else {
				TendButtonSize(BUTTON_ID_1PLAYER,		64, 64, 200);
				TendButtonSize(BUTTON_ID_2PLAYER,		0, 0, 200);
				TendButtonPos(BUTTON_ID_1PLAYER,	320+0*60, 240+50+20, 500);
				TendButtonPos(BUTTON_ID_2PLAYER,	320+0*60, 240+50+20, 500);
				DrawStandardUIButton(BUTTON_ID_1PLAYER);
				DrawStandardUIButton(BUTTON_ID_2PLAYER);
			}
			DrawAlertBubble();
			break;
		case SLIDE_BUTTON_SAVE:
			glEnable(GL_BLEND);
			TendButtonSize(BUTTON_ID_SAVE1,	48, 48, 500);
			TendButtonSize(BUTTON_ID_SAVE2,	48, 48, 500);
			TendButtonSize(BUTTON_ID_BACK,	64, 64, 200);
			
			TendButtonPos(BUTTON_ID_SAVE1,	320-80, 240-30, 500);
			TendButtonPos(BUTTON_ID_SAVE2,	320-80, 240+30, 500);
			TendButtonPos(BUTTON_ID_BACK,		320, 350, 500);
			
			DrawStandardUIButton(BUTTON_ID_SAVE1);
			DrawStandardUIButton(BUTTON_ID_SAVE2);
			DrawStandardUIButton(BUTTON_ID_BACK);

			if (1) {
				alpha = 1.0f - (0.95f * gAlertFader);
				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0.5f,1,0.5f,alpha};
				gFontSys->BeginDraw(FONT_ID_MAIN);

				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1.25f, 1.25f);
				gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);
				gFontSys->SetPos((320-80)+30, (240-30)-15);
				gFontSys->DrawString(TRANSLATE(TXT_New_Game));
				gFontSys->SetPos((320-80)+30, (240+30)-15);
				gFontSys->SetScale(1.25f, 1.25f);
				gFontSys->DrawString(TRANSLATE(TXT_Continue_Game));

				gFontSys->EndDraw();
			}


			TendButtonSize(BUTTON_ID_MAW,		128, 128, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	128, 128, 200);
			TendButtonSize(BUTTON_ID_QUIT,		128, 128, 200);
			TendButtonSize(BUTTON_ID_THANKS,	128, 128, 200);
			TendButtonSize(BUTTON_ID_HELP,		128, 128, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	128, 128, 200);

			TendButtonPos(BUTTON_ID_MAW,		104+50,140, 200);
			TendButtonPos(BUTTON_ID_OPTIONS,	536,240, 200);
			TendButtonPos(BUTTON_ID_QUIT,		536-50,340, 200);
			TendButtonPos(BUTTON_ID_THANKS,		104,240, 200);
			TendButtonPos(BUTTON_ID_HELP,		536-50,140, 200);
			TendButtonPos(BUTTON_ID_FORTUNE,	104+50,340, 200);

			DrawStandardUIButton(BUTTON_ID_MAW);
			DrawStandardUIButton(BUTTON_ID_OPTIONS);
			DrawStandardUIButton(BUTTON_ID_QUIT);
			DrawStandardUIButton(BUTTON_ID_THANKS);
			DrawStandardUIButton(BUTTON_ID_HELP);
			DrawStandardUIButton(BUTTON_ID_FORTUNE);

			DrawAlertBubble();
			break;
		case SLIDE_BUTTON_NUMPLAYERS:
			glEnable(GL_BLEND);
			TendButtonSize(BUTTON_ID_MAW,		64, 64, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_QUIT,		64, 64, 200);
			TendButtonSize(BUTTON_ID_THANKS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_HELP,		64, 64, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	64, 64, 200);
			TendButtonSize(BUTTON_ID_BACK,	64, 64, 200);
			TendButtonSize(BUTTON_ID_1PLAYER,		128, 128, 200);
			TendButtonSize(BUTTON_ID_2PLAYER,		128, 128, 200);
			
			TendButtonPos(BUTTON_ID_MAW,		320-3*60, 350-30, 500);
			TendButtonPos(BUTTON_ID_THANKS,		320-2*60, 350-10, 500);
			TendButtonPos(BUTTON_ID_FORTUNE,	320-1*60, 350, 500);
			TendButtonPos(BUTTON_ID_HELP,		320+3*60, 350-30, 500);
			TendButtonPos(BUTTON_ID_OPTIONS,	320+2*60, 350-10, 500);
			TendButtonPos(BUTTON_ID_QUIT,		320+1*60, 350, 500);
			TendButtonPos(BUTTON_ID_BACK,		320, 350, 500);
			TendButtonPos(BUTTON_ID_1PLAYER,	320-75,190-0, 500);
			TendButtonPos(BUTTON_ID_2PLAYER,	320+75,190-0, 500);

			DrawStandardUIButton(BUTTON_ID_MAW);
			DrawStandardUIButton(BUTTON_ID_OPTIONS);
			DrawStandardUIButton(BUTTON_ID_QUIT);
			DrawStandardUIButton(BUTTON_ID_THANKS);
			DrawStandardUIButton(BUTTON_ID_HELP);
			DrawStandardUIButton(BUTTON_ID_FORTUNE);
			DrawStandardUIButton(BUTTON_ID_1PLAYER);
			DrawStandardUIButton(BUTTON_ID_2PLAYER);
			DrawStandardUIButton(BUTTON_ID_BACK);
			DrawAlertBubble();
			break;
		case SLIDE_BUTTON_CHOOSE1PLAYER:
		case SLIDE_BUTTON_CHOOSE2PLAYER:
			if (gNumPlayers == 1) {
				px1 = px2 = 320.0f;
			} else {
				px1 = 180.0f;
				px2 = 460.0f;
			}

			glEnable(GL_BLEND);

			if (gAlertFader <= 0.0f) {
				alpha = 1.0f;
				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0,1,0.5f,alpha};

				DrawSelectionModel(0, px1, 260, 70.0f);
				if (gNumPlayers == 2) {
					DrawSelectionModel(1, px2, 260, 70.0f);
				}

				gFontSys->BeginDraw(FONT_ID_MAIN);
				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1, 1);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(px1, 270);
				gFontSys->DrawString(gGliders[0]->mName);
				if (gNumPlayers == 2) {
					gFontSys->SetPos(px2, 270);
					gFontSys->DrawString(gGliders[1]->mName);
				}
				gFontSys->EndDraw();
			}

			TendButtonSize(BUTTON_ID_MAW,		64, 64, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_QUIT,		64, 64, 200);
			TendButtonSize(BUTTON_ID_THANKS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_HELP,		64, 64, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	64, 64, 200);
			TendButtonSize(BUTTON_ID_BACK,	64, 64, 200);

			TendButtonPos(BUTTON_ID_BODY_1A,	px1-80, 140, 500);
			TendButtonPos(BUTTON_ID_BODY_1B,	px1+80, 140, 500);
			TendButtonPos(BUTTON_ID_BODY_2A,	px2-80, 140, 500);
			TendButtonPos(BUTTON_ID_BODY_2B,	px2+80, 140, 500);
			TendButtonPos(BUTTON_ID_BOARD_1A,	px1-80, 220, 500);
			TendButtonPos(BUTTON_ID_BOARD_1B,	px1+80, 220, 500);
			TendButtonPos(BUTTON_ID_BOARD_2A,	px2-80, 220, 500);
			TendButtonPos(BUTTON_ID_BOARD_2B,	px2+80, 220, 500);

			TendButtonPos(BUTTON_ID_NAME_1,		px1+(16+0.5f*gFontSys->GetStringWidth(gGliders[0]->mName)), 280, 200);
			if (gNumPlayers == 2) {
				TendButtonPos(BUTTON_ID_NAME_2,		px2-(16+0.5f*gFontSys->GetStringWidth(gGliders[1]->mName)), 280, 200);
			}

			TendButtonPos(BUTTON_ID_MAW,		320-285, 350-30, 500);
			TendButtonPos(BUTTON_ID_THANKS,		320-235, 350-0, 500);
			TendButtonPos(BUTTON_ID_FORTUNE,	320-175, 350, 500);
			TendButtonPos(BUTTON_ID_HELP,		320+285, 350-30, 500);
			TendButtonPos(BUTTON_ID_OPTIONS,	320+235, 350-0, 500);
			TendButtonPos(BUTTON_ID_QUIT,		320+175, 350, 500);
			TendButtonPos(BUTTON_ID_BACK,		320-32, 350, 500);

			DrawStandardUIButton(BUTTON_ID_MAW);
			DrawStandardUIButton(BUTTON_ID_OPTIONS);
			DrawStandardUIButton(BUTTON_ID_QUIT);
			DrawStandardUIButton(BUTTON_ID_THANKS);
			DrawStandardUIButton(BUTTON_ID_HELP);
			DrawStandardUIButton(BUTTON_ID_FORTUNE);
			DrawStandardUIButton(BUTTON_ID_BACK);

			DrawStandardUIButton(BUTTON_ID_BODY_1A, true);
			DrawStandardUIButton(BUTTON_ID_BODY_1B);
			DrawStandardUIButton(BUTTON_ID_BOARD_1A, true);
			DrawStandardUIButton(BUTTON_ID_BOARD_1B);
//			DrawStandardUIButton(BUTTON_ID_NAME_1);
			DrawStandardUIButton(BUTTON_ID_PROCEED1);
			if (gNumPlayers == 2) {
				DrawStandardUIButton(BUTTON_ID_BODY_2A, true);
				DrawStandardUIButton(BUTTON_ID_BODY_2B);
				DrawStandardUIButton(BUTTON_ID_BOARD_2A, true);
				DrawStandardUIButton(BUTTON_ID_BOARD_2B);
//				DrawStandardUIButton(BUTTON_ID_NAME_2);
			}


			DrawAlertBubble();
			break;
		case SLIDE_BUTTON_DIFFICULTY:
			glEnable(GL_BLEND);

			if (gAlertFader <= 0.0f) {
				alpha = 1.0f;
				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0,1,0.5f,alpha};
				px1 = 100;
				px2 = 640-100;

//vvvvv				DrawSelectionModel(0, px1, 300, 70.0f);
				DrawSelectionModel(0, px1, 400, 70.0f);
				if (gNumPlayers == 2) {
					DrawSelectionModel(1, px2, 400, 70.0f);
				}

				gFontSys->BeginDraw(FONT_ID_MAIN);
				gFontSys->SetColor(fc1, fc2);
				gFontSys->SetScale(1, 1);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(px1, 410);
				gFontSys->DrawString(gGliders[0]->mName);
				if (gNumPlayers == 2) {
					gFontSys->SetPos(px2, 410);
					gFontSys->DrawString(gGliders[1]->mName);
				}
				gFontSys->EndDraw();
			}


			TendButtonSize(BUTTON_ID_MAW,		64, 64, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_QUIT,		64, 64, 200);
			TendButtonSize(BUTTON_ID_THANKS,	64, 64, 200);
			TendButtonSize(BUTTON_ID_HELP,		64, 64, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	64, 64, 200);
			TendButtonSize(BUTTON_ID_BACK,		64, 64, 200);
			
			TendButtonPos(BUTTON_ID_MAW,		320-100,240-50, 500);
			TendButtonPos(BUTTON_ID_OPTIONS,	320+125,240, 500);
			TendButtonPos(BUTTON_ID_QUIT,		320+100,240+50, 500);
			TendButtonPos(BUTTON_ID_THANKS,		320-125,240, 500);
			TendButtonPos(BUTTON_ID_HELP,		320+100,240-50, 500);
			TendButtonPos(BUTTON_ID_FORTUNE,	320-100,240+50, 500);

			//			TendButtonPos(BUTTON_ID_MAW,		320-3*30, 235+1*40, 200);
//			TendButtonPos(BUTTON_ID_THANKS,		320-3*30, 240+2*40, 200);
//			TendButtonPos(BUTTON_ID_FORTUNE,	320-4*30, 240+3*40, 200);
//			TendButtonPos(BUTTON_ID_HELP,		320+3*30, 235+1*40, 200);
//			TendButtonPos(BUTTON_ID_OPTIONS,	320+3*30, 240+2*40, 200);
//			TendButtonPos(BUTTON_ID_QUIT,		320+4*30, 240+3*40, 200);

			TendButtonPos(BUTTON_ID_BACK,		320-120, 240, 200);

			DrawStandardUIButton(BUTTON_ID_EASY);
			DrawStandardUIButton(BUTTON_ID_MEDIUM);
			DrawStandardUIButton(BUTTON_ID_HARD);
			DrawStandardUIButton(BUTTON_ID_BACK);

			if (gAlertFader <= 0.0f) {
				alpha = 1.0f;
				pfVec4	fc1 = {1,1,1,alpha};
				pfVec4	fc2 = {0.5f,0.5f,0.5f,alpha};
				pfVec4	fcR = {0.5f,0.0f,0.0f,alpha};
				pfVec4	fcY = {0.5f,0.5f,0.0f,alpha};
				pfVec4	fcG = {0.0f,0.5f,0.0f,alpha};
				pfVec4	fcBlack = {0,0,0,alpha};
				char	*str, *cp;

				gFontSys->BeginDraw(FONT_ID_MAIN);
				gFontSys->SetScale(1.0f, 1.0f);
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				
				x = 320;
				y = (240-120)-12;
				str = TRANSLATE(TXT_PLAY_HARD);
				for (cp = str; *cp; cp++) {
					if (*cp == '\n') y -= 12;
				}
				gFontSys->SetColor(fcBlack, fcBlack);
				gFontSys->SetPos(x+1, y+1);
				gFontSys->DrawString(str);
				gFontSys->SetColor(fc1, fcR);
				gFontSys->SetPos(x, y);
				gFontSys->DrawString(str);

				y = (240)-12;
				str = TRANSLATE(TXT_PLAY_MEDIUM);
				for (cp = str; *cp; cp++) {
					if (*cp == '\n') y -= 12;
				}
				gFontSys->SetColor(fcBlack, fcBlack);
				gFontSys->SetPos(x+1, y+1);
				gFontSys->DrawString(str);
				gFontSys->SetColor(fc1, fcY);
				gFontSys->SetPos(x, y);
				gFontSys->DrawString(str);

				y = (240+120)-12;
				str = TRANSLATE(TXT_PLAY_EASY);
				for (cp = str; *cp; cp++) {
					if (*cp == '\n') y -= 12;
				}
				gFontSys->SetColor(fcBlack, fcBlack);
				gFontSys->SetPos(x+1, y+1);
				gFontSys->DrawString(str);
				gFontSys->SetColor(fc1, fcG);
				gFontSys->SetPos(x, y);
				gFontSys->DrawString(str);

				gFontSys->EndDraw();
			}

			DrawAlertBubble();
			break;
		case SLIDE_BUTTON_PLAY:
			/**** Draw the little buttons ****/
			glEnable(GL_BLEND);
			TendButtonSize(BUTTON_ID_MAW,		128, 128, 200);
			TendButtonSize(BUTTON_ID_OPTIONS,	128, 128, 200);
			TendButtonSize(BUTTON_ID_QUIT,		128, 128, 200);
			TendButtonSize(BUTTON_ID_THANKS,	128, 128, 200);
			TendButtonSize(BUTTON_ID_HELP,		128, 128, 200);
			TendButtonSize(BUTTON_ID_FORTUNE,	128, 128, 200);

			TendButtonPos(BUTTON_ID_MAW,		104+50,140, 200);
			TendButtonPos(BUTTON_ID_OPTIONS,	536,240, 200);
			TendButtonPos(BUTTON_ID_QUIT,		536-50,340, 200);
			TendButtonPos(BUTTON_ID_THANKS,		104,240, 200);
			TendButtonPos(BUTTON_ID_HELP,		536-50,140, 200);
			TendButtonPos(BUTTON_ID_FORTUNE,	104+50,340, 200);

			DrawStandardUIButton(BUTTON_ID_MAW);
			DrawStandardUIButton(BUTTON_ID_OPTIONS);
			DrawStandardUIButton(BUTTON_ID_QUIT);
			DrawStandardUIButton(BUTTON_ID_THANKS);
			DrawStandardUIButton(BUTTON_ID_HELP);
			DrawStandardUIButton(BUTTON_ID_FORTUNE);

			for (i = 0; i < gNumLanguages; i++) {
				float left = 64;
				float width = 640-2*left;
				float gap = width / (gNumLanguages-1);
				TendButtonSize(BUTTON_ID_FLAG_00+i, 32, 32, 200);
				TendButtonPos(BUTTON_ID_FLAG_00+i, left + gap * i, 440, 200);
				DrawStandardUIButton(BUTTON_ID_FLAG_00+i);
			}

			/**** Draw the big play button ****/
			for (i = 0; i < 4; i++) {
				mWobbleTimer[i] += UnscaledDeltaTime * mWobbleSpeed[i];
			}
			UseLibTexture(TEXID_BUTTON_BIG_PLAY);
			x = 320;
			y = 240;
			w = 200;
			h = 100;
			if (mCurrentButton == BUTTON_ID_BIGPLAY) {
				mWobbleScale += 4.0f * UnscaledDeltaTime;
				mWobbleX += UnscaledDeltaTime * 100.0f;
				mWobbleY += UnscaledDeltaTime * 100.0f;
				if (mWobbleScale > 1.0f) mWobbleScale = 1.0f;
				if (mWobbleX > 20.0f) mWobbleX = 20.0f;
				if (mWobbleY > 20.0f) mWobbleY = 20.0f;
			} else {
				mWobbleScale -= 4.0f * UnscaledDeltaTime;
				mWobbleX -= UnscaledDeltaTime * 20.0f;
				mWobbleY -= UnscaledDeltaTime * 20.0f;
				if (mWobbleScale < 0.7f) mWobbleScale = 0.7f;
				if (mWobbleX < 1.0f) mWobbleX = 1.0f;
				if (mWobbleY < 1.0f) mWobbleY = 1.0f;
			}
			DrawWobbleButton(x, y, w, h);
			DrawAlertBubble();
			break;
		case SLIDE_YOU_WIN:
			/**** Draw the little buttons ****/
			glEnable(GL_BLEND);

			if (1) {
				static bool	init = false;
				static int		tex[4];
				static pfVec3	pos[4];
				static pfVec3	vel[4];
				static float	size[4];
				static float	angle[4];
				static float	tumble[4];
				static float	speed = 100.0f;
				int				bounce = 0;
				if (!init) {
					init = true;
					tex[0] = TEXID_CHARACTER1;
					tex[1] = TEXID_BOARD1;
					tex[2] = TEXID_CHARACTER2;
					tex[3] = TEXID_BOARD2;
					for (i = 0; i < 4; i++) {
						pfSetVec3(pos[i], RANDOM_IN_RANGE(0, 640), RANDOM_IN_RANGE(0, 480), 0);
						pfSetVec3(vel[i], RANDOM_IN_RANGE(-1, 1), RANDOM_IN_RANGE(-1, 1), 0);
						if (vel[i][PF_X]*vel[i][PF_Y] == 0.0f) {
							vel[i][PF_X] = vel[i][PF_Y] = 1.0f;
						}
						pfNormalizeVec3(vel[i]);
						pfScaleVec3(vel[i], RANDOM_IN_RANGE(speed, 2*speed), vel[i]);
						size[i] = 30;
						angle[i] = RANDOM_IN_RANGE(0, 360);
						tumble[i] = RANDOM_IN_RANGE(-360, 360);
					}
				}
				for (i = 0; i < 4; i++) {
					pfAddScaledVec3(pos[i], pos[i], UnscaledDeltaTime, vel[i]);
					angle[i] += UnscaledDeltaTime * tumble[i];
					bounce = 0;
					if (pos[i][PF_X] + size[i] > 640 && vel[i][PF_X] > 0) {
						vel[i][PF_X] = -vel[i][PF_X];
						pos[i][PF_X] = 640 - size[i];
						bounce = -1;
					}
					if (pos[i][PF_X] - size[i] < 0 && vel[i][PF_X] < 0) {
						vel[i][PF_X] = -vel[i][PF_X];
						pos[i][PF_X] = 0 + size[i];
						bounce = 1;
					}
					if (pos[i][PF_Y] + size[i] > 480 && vel[i][PF_Y] > 0) {
						vel[i][PF_Y] = -vel[i][PF_Y];
						pos[i][PF_Y] = 480 - size[i];
						bounce = -1;
					}
					if (pos[i][PF_Y] - size[i] < 0 && vel[i][PF_Y] < 0) {
						vel[i][PF_Y] = -vel[i][PF_Y];
						pos[i][PF_Y] = 0 + size[i];
						bounce = 1;
					}
					if (bounce) {
						tumble[i] = bounce * RANDOM_IN_RANGE(0, 360);
						if (RANDOM0TO1 < 0.05f) {
							if (RANDOM0TO1 < 0.5f) {
								playSound2D(VO_BEN_IWIN, 0.8f, 1.0f);
							} else {
								playSound2D(VO_SET_IWIN, 0.8f, 1.0f);
							}
						} else {
							playSound2D(SOUND_ELECTRICAL_BOING, 0.8f, 0.8f);
						}
					}
					/**** draw them! ****/
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();

					UseLibTexture(tex[i]);
					glColor4f(1,1,1,1);
					glTranslatef(pos[i][PF_X], pos[i][PF_Y], pos[i][PF_Z]);
					glRotatef(angle[i], 0, 0, 1);
					
					glBegin(GL_QUADS);
					glTexCoord2f(0,0);  glVertex3f(-size[i], -size[i], 0);
					glTexCoord2f(1,0);  glVertex3f( size[i], -size[i], 0);
					glTexCoord2f(1,1);  glVertex3f( size[i],  size[i], 0);
					glTexCoord2f(0,1);  glVertex3f(-size[i],  size[i], 0);
					glEnd();

					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
				}
			}
			ManageSplats();
			DrawSplats();

			TendButtonSize(BUTTON_ID_PLAY,		64, 64, 200);
			TendButtonPos(BUTTON_ID_PLAY,		320, 350, 500);
			DrawStandardUIButton(BUTTON_ID_PLAY);

			MiniSparks::UpdateFireworks();

			/**** Draw the big play button ****/
			for (i = 0; i < 4; i++) {
				mWobbleTimer[i] += UnscaledDeltaTime * mWobbleSpeed[i];
			}
			UseLibTexture(TEXID_BUTTON_BIG_WIN);
			x = 320;
			y = 240;
			w = 200;
			h = 100;
			if (1) {
				mWobbleScale += 4.0f * UnscaledDeltaTime;
				mWobbleX += UnscaledDeltaTime * 100.0f;
				mWobbleY += UnscaledDeltaTime * 100.0f;
				if (mWobbleScale > 1.0f) mWobbleScale = 1.0f;
				if (mWobbleX > 5.0f) mWobbleX = 5.0f;
				if (mWobbleY > 5.0f) mWobbleY = 5.0f;
			} else {
				mWobbleScale -= 4.0f * UnscaledDeltaTime;
				mWobbleX -= UnscaledDeltaTime * 20.0f;
				mWobbleY -= UnscaledDeltaTime * 20.0f;
				if (mWobbleScale < 0.7f) mWobbleScale = 0.7f;
				if (mWobbleX < 1.0f) mWobbleX = 1.0f;
				if (mWobbleY < 1.0f) mWobbleY = 1.0f;
			}
			DrawWobbleButton(x, y, w, h);
			DrawAlertBubble();
			break;
		default: break;
	}

	if (gHintText && gHintFader > 0.0f) {
		alpha = gHintFader;
		pfVec4	fc1 = {1,1,0,alpha};
		pfVec4	fc2 = {0.5f,0.5f,0,alpha};

		gHintFader -= 1.0f * UnscaledDeltaTime;
		gFontSys->BeginDraw(FONT_ID_MAIN);

		gFontSys->SetColor(fc1, fc2);
		gFontSys->SetScale(1.0f, 1.0f);
		gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
		gFontSys->SetPos(gHintPos[PF_X], gHintPos[PF_Y]);
		gFontSys->DrawString(gHintText);

		gFontSys->EndDraw();
	}

//	if (gGameMode == GAME_MODE_SLIDES) {
//		if (gAlertFader > 0.75f && gAlertButton == BUTTON_ID_QUIT) {
//			glDisable(GL_BLEND);
//			glColor4f(0,0,0,1);
//			hudQuickRect(0,0,640,480);
//		}
//	}

	gFireFlies->Draw();	/**** draw the fireflies! ****/
	for (i = 0; i < NUM_FIREWORKS; i++) {
		MiniSparks	*f = gFireworks[i];
		f->Think();
		f->Move();
		f->Draw();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
}

void Slides::NewSplat(float x, float y, float size)
{
	int		i, id = -1;
	Splat	*sp;
	int win_width, win_height;
	SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
	float	fx = x / (win_width / 640.0f);
	float	fy = y / (win_height / 480.0f);

	for (i = 0; i < mSplatCount && id < 0; i++) {
		if (mSplats[i].timer <= 0.0f) id = i;
	}
	if (id < 0) {
		if (mSplatCount < SPLAT_NUM) id = mSplatCount++;
		else return;
	}
	sp = mSplats + id;
	sp->timer = 2.0f;
	pfMakeEulerMat(sp->mat, RANDOM_IN_RANGE(0,360), 0, 0);
	pfPreScaleMat(sp->mat, size, size, size, sp->mat);
	pfSetVec3(sp->mat[PF_T], fx, fy, 0);
	playSound2D(SOUND_BARF_SPLAT, 1.0f, RANDOM_IN_RANGE(0.7f, 1.2f));
}

void Slides::ManageSplats(void)
{
	int		i, max = 0;
	Splat	*sp;

	for (i = 0; i < mSplatCount; i++) {
		sp = mSplats + i;
		if (sp->timer > 0.0f) {
			sp->timer -= 1.0f * UnscaledDeltaTime;
			if (sp->timer > 0.0f) {
				max = i;
			}
		}
	}
	mSplatCount = max+1;
}

void Slides::DrawSplats(void)
{
	int		i, max = 0;
	float	alpha;
	Splat	*sp;

	UseLibTexture(TEXID_LAUNCH_BARF);
	for (i = 0; i < mSplatCount; i++) {
		sp = mSplats + i;
		if (sp->timer > 0.0f) {
			alpha = sp->timer;
			if (alpha > 1.0f) alpha = 1.0f;
			glColor4f(0,1,0,alpha);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf((float*)sp->mat);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,0);
			glTexCoord2f(1,0);
			glVertex3f(1,-1,0);
			glTexCoord2f(1,1);
			glVertex3f(1,1,0);
			glTexCoord2f(0,1);
			glVertex3f(-1,1,0);
			glEnd();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}
}


void Slides::DrawBoxes(Box *list)
{
	while (list && ((list->flags & BOX_FLAG_END) == 0)) {
		if (list->flags & BOX_FLAG_TEXT) {
			gFontSys->BeginDraw(FONT_ID_MAIN);
			gFontSys->SetColor(BoxFontColor[0], BoxFontColor[1]);
			gFontSys->SetScale(list->scale, list->scale);
			gFontSys->SetBox(list->cx-list->w/2, list->cy-list->h/2, list->cx+list->w/2, list->cy+list->h/2);

			if (list->flags & BOX_FLAG_CENTER) {
				gFontSys->SetAlign(FONT_ALIGNMENT_CENTER);
				gFontSys->SetPos(list->cx, list->cy-list->h/2);
			} else {
				gFontSys->SetAlign(FONT_ALIGNMENT_LEFT);
				gFontSys->SetPos(list->cx-list->w/2, list->cy-list->h/2);
			}
			gFontSys->DrawString(list->text);
			gFontSys->EndDraw();
		} else if (list->flags & BOX_FLAG_IMAGE) {
			UseLibTexture(list->texID);
			glColor4f(1,1,1,1);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(list->cx-list->w/2, list->cy+list->h/2,0);
			glTexCoord2f(1,0);
			glVertex3f(list->cx+list->w/2, list->cy+list->h/2,0);
			glTexCoord2f(1,1);
			glVertex3f(list->cx+list->w/2, list->cy-list->h/2,0);
			glTexCoord2f(0,1);
			glVertex3f(list->cx-list->w/2, list->cy-list->h/2,0);
			glEnd();
		}
		list++;
	}
	gFontSys->SetBox(100, 100, 640, 480);
}

bool gOptionLaunchInWindow = false;
bool gOptionMipMap = false;
bool gOptionSoundEffectsOn = true;
bool gOptionVoicesOn = true;
bool gOptionWaves = true;
bool gOptionAutoSave = true;
char gOptionLanguage[256] = "English";

char	*gOptionsFileName = "options.txt";
char	*gSaveFileName = "savegames.txt";

SaveFile	gSave;
int			gSaveSlot = 0;

void LoadOptions(void)
{
	FILE	*fp;
	char	line[2048], str[1024];
	int		num;

	fp = fopen(gOptionsFileName, "r");
	if (fp) {
		while (!feof(fp)) {
			fgets(line, sizeof(line), fp);
			if (sscanf(line, " gOptionLaunchInWindow = %d ", &num)) gOptionLaunchInWindow = num;
			else if (sscanf(line, " gOptionMipMap = %d ", &num)) gOptionMipMap = num;
			else if (sscanf(line, " gOptionSoundEffectsOn = %d ", &num)) {
				gOptionSoundEffectsOn = num;
			} else if (sscanf(line, " gOptionVoicesOn = %d ", &num)) gOptionVoicesOn = num;
			else if (sscanf(line, " gOptionWaves = %d ", &num)) gOptionWaves = num;
			else if (sscanf(line, " gOptionAutoSave = %d ", &num)) gOptionAutoSave = num;
			else if (sscanf(line, " gOptionLanguage = %s ", str)) strcpy(gOptionLanguage, str);
		}
		fclose(fp);
	}
}

void SaveOptions(void)
{
	FILE	*fp;
	char	line[2048];
	int		num;

	fp = fopen(gOptionsFileName, "w");
	if (fp) {
		fprintf(fp, "This is the options file for Ben's game!\nTo change these values, use the checkbubbles in the game.\n\n");
		fprintf(fp, "gOptionLaunchInWindow = %d\n", gOptionLaunchInWindow);
		fprintf(fp, "gOptionMipMap = %d\n", gOptionMipMap);
		fprintf(fp, "gOptionSoundEffectsOn = %d\n", gOptionSoundEffectsOn);
		fprintf(fp, "gOptionVoicesOn = %d\n", gOptionVoicesOn);
		fprintf(fp, "gOptionWaves = %d\n", gOptionWaves);
		fprintf(fp, "gOptionAutoSave = %d\n", gOptionAutoSave);
		fprintf(fp, "gOptionLanguage = %s\n", gOptionLanguage);
		fclose(fp);
	}
}


int32 GetSaveCheckSum(SaveSlot *s)
{
	int32		checkSum = 0;

	checkSum += s->numPlayers;
	checkSum += s->difficulty;
	checkSum += s->shields[0];
	checkSum += s->shields[1]*2;
	checkSum += s->shields[2]*3;
	checkSum += s->shields[3]*4;
	checkSum += s->shields[4]*5;
	checkSum += s->shields[5]*6;
	checkSum += s->shields[6]*7;
	checkSum += s->totalCellsDestroyed;
	checkSum += s->gameTime;
	return(checkSum);
}

void WriteSaves()
{
	FILE	*fp;
	char	line[2048];
	int		i, num;

	for (i = 0; i < NUM_SAVES; i++) {
		SaveSlot	*s = &(gSave.slots[i]);
		s->checkSum = GetSaveCheckSum(s);
	}
	fp = fopen(gSaveFileName, "w");
	if (fp) {
		fprintf(fp, "This is the save file for Ben's game!\n\n\n");
		fprintf(fp, "version = %d\n", SAVE_VERSION);
		fprintf(fp, "numSaves = %d\n", NUM_SAVES);
		fprintf(fp, "sPlayers = %d %d %d %d\n", WRITE_SAVE_SLOTS(numPlayers));
		fprintf(fp, "sDiff = %d %d %d %d\n", WRITE_SAVE_SLOTS(difficulty));
		fprintf(fp, "sPName1 = %s %s %s %s\n",   WRITE_SAVE_SLOTS(playerName[0]));
		fprintf(fp, "sPName2 = %s %s %s %s\n",   WRITE_SAVE_SLOTS(playerName[1]));
		fprintf(fp, "sBName1 = %s %s %s %s\n",   WRITE_SAVE_SLOTS(boardName[0]));
		fprintf(fp, "sBName2 = %s %s %s %s\n",   WRITE_SAVE_SLOTS(boardName[1]));
		fprintf(fp, "sShields = %s %s %s %s\n", WRITE_SAVE_SLOTS(shields));
		fprintf(fp, "sCount = %d %d %d %d\n",   WRITE_SAVE_SLOTS(totalCellsDestroyed));
		fprintf(fp, "sPlayTime = %d %d %d %d\n", WRITE_SAVE_SLOTS(gameTime));
		fprintf(fp, "sCheck = %d %d %d %d\n",   WRITE_SAVE_SLOTS(checkSum));
		fclose(fp);
	}
}

void ReadSaves()
{
	FILE		*fp;
	char		line[2048];
	int			i, num;
	SaveFile	oldSave = gSave;

	fp = fopen(gSaveFileName, "r");
	if (fp) {
		while (!feof(fp)) {
			fgets(line, sizeof(line), fp);
			sscanf(line, " version = %d ", &gSave.version);
			sscanf(line, " numSaves = %d ", &gSave.numSaves);
			sscanf(line, " sPlayers = %d %d %d %d ", READ_SAVE_SLOTS(numPlayers));
			sscanf(line, " sDiff = %d %d %d %d ", READ_SAVE_SLOTS(difficulty));
			sscanf(line, " sPName1 = %s %s %s %s ",   READ_SAVE_SLOTS(playerName[0]));
			sscanf(line, " sPName2 = %s %s %s %s ",   READ_SAVE_SLOTS(playerName[1]));
			sscanf(line, " sBName1 = %s %s %s %s ",   READ_SAVE_SLOTS(boardName[0]));
			sscanf(line, " sBName2 = %s %s %s %s ",   READ_SAVE_SLOTS(boardName[1]));
			sscanf(line, " sShields = %s %s %s %s ", READ_SAVE_SLOTS(shields));
			sscanf(line, " sCount = %d %d %d %d ",   READ_SAVE_SLOTS(totalCellsDestroyed));
			sscanf(line, " sPlayTime = %d %d %d %d ", READ_SAVE_SLOTS(gameTime));
			sscanf(line, " sCheck = %d %d %d %d ",   READ_SAVE_SLOTS(checkSum));
		}
		fclose(fp);
		/**** integrity check and recover ****/
		if (gSave.version != SAVE_VERSION) {
			gSave = oldSave;
			return;
		}
		for (i = 0; i < NUM_SAVES; i++) {
			SaveSlot	*s = &(gSave.slots[i]);
			if (s->checkSum != GetSaveCheckSum(s)) {
				*s = oldSave.slots[i];
			}
		}
	} else {
		gSave.numSaves = NUM_SAVES;
		gSave.version = SAVE_VERSION;
		for (i = 0; i < NUM_SAVES; i++) {
			gSave.slots[i].numPlayers = 0;
			gSave.slots[i].checkSum = 0;
			gSave.slots[i].gameTime = 0.0f;
			gSave.slots[i].totalCellsDestroyed = 0;
			strcpy(gSave.slots[i].shields, "0000000");
			strcpy(gSave.slots[i].boardName[0], "none");
			strcpy(gSave.slots[i].boardName[1], "none");
			strcpy(gSave.slots[i].playerName[0], "none");
			strcpy(gSave.slots[i].playerName[1], "none");
		}
	}
}

void GameToSaveSlot()
{
	int			i, j;
	SaveSlot	*slot;
	Glider		*g;

	slot = &(gSave.slots[0]);
	/**** Put the saved info ****/
	slot->numPlayers = gNumPlayers;
	slot->difficulty = gDifficultySetting;
	for (j = 0; j < 2; j++) {
		g = gGliders[j];
		strcpy(slot->playerName[j], gPlayerImages[g->mPlayerImageNum]->mFileName);
		strcpy(slot->boardName[j], gBoardImages[g->mBoardImageNum]->mFileName);
	}
	for (i = 0; i < 7; i++) {
        slot->shields[i] = gShields[i].icon + '0' - TEXID_SHIELD_BLANK;
	}
	slot->shields[7] = 0;
}

void SaveSlotToGame()
{
	SaveSlot	*slot;
	int			i, j;
	Glider		*g;

	slot = &(gSave.slots[0]);
	/**** Get the saved info and start a game ****/
	gNumPlayers = slot->numPlayers;
	gDifficultySetting = slot->difficulty;
	for (j = 0; j < 2; j++) {
		g = gGliders[j];
		if (g && gNumPlayerImages > 0) {
			for (i = 0; i < gNumPlayerImages; i++) {
				if (!strcmp(slot->playerName[j], gPlayerImages[i]->mFileName)) {
					g->mPlayerImageNum = i;
				}
			}
			if (g->mPlayerImageNum < 0) g->mPlayerImageNum = gNumPlayerImages-1;
			if (g->mPlayerImageNum >= gNumPlayerImages) g->mPlayerImageNum = 0;
			switchLoadedTexture(TEXID_CHARACTER1+g->mPlayerNum, gPlayerImages[g->mPlayerImageNum]->mFileName);
			strcpy(g->mName, gPlayerImages[g->mPlayerImageNum]->mPlayerName);
		}
		if (g && gNumBoardImages > 0) {
			for (i = 0; i < gNumBoardImages; i++) {
				if (!strcmp(slot->boardName[j], gBoardImages[i]->mFileName)) {
					g->mBoardImageNum = i;
				}
			}
			if (g->mBoardImageNum < 0) g->mBoardImageNum = gNumBoardImages-1;
			if (g->mBoardImageNum >= gNumBoardImages) g->mBoardImageNum = 0;
			switchLoadedTexture(TEXID_BOARD1+g->mPlayerNum, gBoardImages[g->mBoardImageNum]->mFileName);
			strcpy(g->mName, gBoardImages[g->mBoardImageNum]->mPlayerName);
		}
	}
	gSlides->NewGame(gNumPlayers);
	for (i = 0; i < 7; i++) {
		if (slot->shields[i] < '1' || slot->shields[i] > '7') {
			gShields[i].have = false;
			gShields[i].icon = TEXID_SHIELD_BLANK;
		} else {
			gShields[i].have = true;
			gShields[i].icon = (slot->shields[i] - '0') + TEXID_SHIELD_BLANK;
			gLevels[(slot->shields[i] - '1')].mComplete = true;
		}
	}
}


