/********************************************************************************\
	tsufile.cpp

	Some simple code to bundle files together.
\********************************************************************************/

#include "genincludes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsufile.h"

// External functions not in a header
extern void ShutdownApplication(int exitError);

#if MAC_BUILD
// we include this for the TWIST macro
#include "umath.h"
#include "macutils.h"
#endif

FILE	*gTsuFile = NULL;
unsigned char *gTsuData = NULL;

char 	*gTsuFileName = "data.tsu";
char 	*gTsuFileBeginStr = "Ben's Game TSU file v1.0";
char 	*gTsuFileEncryptStr = "cure for cancer";

#define TSU_MAKE 	0
#define TSU_USE		1

#if TSU_MAKE
char	*gTsuFileList[] = {
		/**** special additions ****/
        "textures/player_Grant.jpg",
        "textures/player_Grant_mask.jpg",
        "textures/player_Karen.jpg",
        "textures/player_Karen_mask.jpg",
	

		/**** normal additions ****/


        "textures/language_English.txt",
        "textures/English_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/English_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/English_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/English_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/English_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/English_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/English_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/English_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/English_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/cell1.jpg",
        "textures/halfbubble1.jpg",
        "textures/player_Ben.jpg",
        "textures/player_Ben_mask.jpg",
        "textures/player_Ben.jpg",
        "textures/player_Ben_mask.jpg",
        "textures/player_Christina.jpg",
        "textures/player_Christina_mask.jpg",
        "textures/board_Jet_1.jpg",
        "textures/board_Jet_1_mask.jpg",
        "textures/board_Jet_2.jpg",
        "textures/board_Jet_2_mask.jpg",
        "textures/grid1.jpg",
        "textures/bubble2.jpg",
        "textures/shield_blank.jpg",
        "textures/shield_blank_mask.jpg",
        "textures/shield_pox.jpg",
        "textures/shield_pox_mask.jpg",
        "textures/shield_colds.jpg",
        "textures/shield_colds_mask.jpg",
        "textures/shield_rash.jpg",
        "textures/shield_rash_mask.jpg",
        "textures/shield_barf.jpg",
        "textures/shield_barf_mask.jpg",
        "textures/shield_fever.jpg",
        "textures/shield_fever_mask.jpg",
        "textures/shield_hair.jpg",
        "textures/shield_hair_mask.jpg",
        "textures/shield_bleeding.jpg",
        "textures/shield_bleeding_mask.jpg",
        "textures/thermometer.jpg",
        "textures/thermometer_mask.jpg",
        "textures/compass1.jpg",
        "textures/compass1_mask.jpg",
        "textures/icon_health.jpg",
        "textures/icon_pharmacy.jpg",
        "textures/icon_house.jpg",
        "textures/zap1.jpg",
        "textures/glowspark1.jpg",
        "textures/glowsweep1.jpg",
        "textures/weapon_crossbow.jpg",
        "textures/weapon_wristrocket.jpg",
        "textures/weapon_sword.jpg",
        "textures/weapon_missile.jpg",
        "textures/weapon_crossbow.jpg",
        "textures/weapon_crossbow.jpg",
        "textures/weapon_crossbow.jpg",
        "textures/screen_makewish.jpg",
        "textures/key_controls.jpg",
        "textures/key_controls_mask.jpg",
        "textures/help_monsters.jpg",
        "textures/help_monsters_mask.jpg",
        "textures/help_setback.jpg",
        "textures/help_setback_mask.jpg",
        "textures/help_weapon.jpg",
        "textures/help_weapon_mask.jpg",
        "textures/help_health.jpg",
        "textures/help_health_mask.jpg",
        "textures/help_custom.jpg",
        "textures/circle_r.jpg",
        "textures/hud_graph.jpg",
        "textures/text_complete.jpg",
        "textures/boss1a.jpg",
        "textures/boss1a_mask.jpg",
        "textures/boss1f.jpg",
        "textures/boss1f_mask.jpg",
        "textures/boss2a.jpg",
        "textures/boss2a_mask.jpg",
        "textures/boss2f.jpg",
        "textures/boss2f_mask.jpg",
        "textures/boss3a.jpg",
        "textures/boss3a_mask.jpg",
        "textures/boss3f.jpg",
        "textures/boss3f_mask.jpg",
        "textures/boss4a.jpg",
        "textures/boss4a_mask.jpg",
        "textures/boss4f.jpg",
        "textures/boss4f_mask.jpg",
        "textures/boss5a.jpg",
        "textures/boss5a_mask.jpg",
        "textures/boss5f.jpg",
        "textures/boss5f_mask.jpg",
        "textures/boss6a.jpg",
        "textures/boss6a_mask.jpg",
        "textures/boss6f.jpg",
        "textures/boss6f_mask.jpg",
        "textures/boss7a.jpg",
        "textures/boss7a_mask.jpg",
        "textures/boss7f.jpg",
        "textures/boss7f_mask.jpg",
        "textures/launch_barf.jpg",
        "textures/launch_barf_mask.jpg",
        "textures/launch_snow.jpg",
        "textures/launch_snow_mask.jpg",
        "textures/launch_fire.jpg",
        "textures/launch_fire_mask.jpg",
        "textures/launch_ball.jpg",
        "textures/launch_ball_mask.jpg",
        "textures/launch_bat.jpg",
        "textures/launch_bat_mask.jpg",
        "textures/launch_dust.jpg",
        "textures/launch_dust_mask.jpg",
        "textures/launch_pox.jpg",
        "textures/launch_pox_mask.jpg",
        "textures/launch_barf.jpg",
        "textures/launch_barf_mask.jpg",
        "textures/button_big_play.jpg",
        "textures/button_big_play_mask.jpg",
        "textures/button_you_win.jpg",
        "textures/button_you_win_mask.jpg",
        "textures/button_blank.jpg",
        "textures/button_skinny.jpg",
        "textures/button_quit.jpg",
        "textures/button_thanks.jpg",
        "textures/button_maw.jpg",
        "textures/button_options.jpg",
        "textures/button_help.jpg",
        "textures/button_fortune.jpg",
        "textures/button_scores.jpg",
        "textures/button_1p.jpg",
        "textures/button_2p.jpg",
        "textures/button_back.jpg",
        "textures/button_play.jpg",
        "textures/button_easy.jpg",
        "textures/button_easy_mask.jpg",
        "textures/button_medium.jpg",
        "textures/button_medium_mask.jpg",
        "textures/button_hard.jpg",
        "textures/button_hard_mask.jpg",
        "textures/button_body.jpg",
        "textures/button_board.jpg",
        "textures/button_donation.jpg",
        "textures/button_checkbox.jpg",
        "textures/lec_logo.jpg",
        "textures/lec_logo_mask.jpg",
        "textures/font2Ar24000.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "textures/blsnk_flag.jpg",
        "sounds/test.wav",
        "sounds/test.wav",
        "sounds/Lo_Res/Bubble_Pop.wav",
        "sounds/Lo_Res/Electrical_Boing.wav",
        "sounds/Lo_Res/Health_Fill.wav",
        "sounds/Lo_Res/Bubbles_27_12.wav",
        "sounds/Lo_Res/bubblesMud_01.wav",
        "sounds/Lo_Res/wetJuicyImpact_01.wav",
        "sounds/Lo_Res/wetJuicyImpact_02.wav",
        "sounds/Lo_Res/Gum_0533_blow_bubble.wav",
        "sounds/Lo_Res/Gum_0533_blow_bubble.wav",
        "sounds/Lo_Res/BEEPFreq_01.wav",
        "sounds/Lo_Res/wetJuicyImpact_04.wav",
        "sounds/Lo_Res/shot_ben_mortar_02.wav",
        "sounds/Lo_Res/shot_ben_mortar_01.wav",
        "sounds/Lo_Res/shot_ben_blaster.wav",
        "sounds/Lo_Res/impact_ben_sword_02.wav",
        "sounds/Lo_Res/impact_ben_sword_01.wav",
        "sounds/Lo_Res/impact_ben_sizzle_01.wav",
        "sounds/Lo_Res/impact_ben_sizzle_02.wav",
        "sounds/Lo_Res/impact_ben_sizzle_03.wav",
        "sounds/Lo_Res/ben_swish_01.wav",
        "sounds/voice/Best/vo_ben_ow_05.wav",
        "sounds/voice/Best/vo_ben_yeah_04.wav",
        "sounds/voice/Best/vo_set_ay_01.wav",
        "sounds/Lo_Res/SilkyChicken_01.wav",
        "sounds/Lo_Res/SilkyChicken_x_03.wav",
        "sounds/Lo_Res/SilkyChicken_x_04.wav",
        "sounds/voice/Best/vo_ben_bensgame_01.wav",
        "sounds/voice/Best/vo_ben_gotit_04.wav",
        "sounds/voice/Best/vo_ben_iwin_05.wav",
        "sounds/voice/Best/vo_ben_ow_05.wav",
        "sounds/voice/Best/vo_ben_pressstart_05.wav",
        "sounds/voice/Best/vo_ben_whoopy_02.wav",
        "sounds/voice/Best/vo_ben_yah_06.wav",
        "sounds/voice/Best/vo_ben_yeah_04.wav",
        "sounds/voice/Best/vo_ben_yes_03.wav",
        "sounds/voice/Best/vo_set_ay_01.wav",
        "sounds/voice/Best/vox_both_complete_02.wav",
        "sounds/voice/Best/vox_both_levelone.wav",
        "sounds/voice/Best/vox_both_leveltwo.wav",
        "sounds/voice/Best/vox_both_levelthree.wav",
        "sounds/voice/Best/vox_both_levelfour.wav",
        "sounds/voice/Best/vox_both_levelfive.wav",
        "sounds/voice/Best/vox_both_levelsix.wav",
        "sounds/voice/Best/vox_both_levelseven.wav",
//        "sounds/voice/Best/vox_both_yeaaah.wav",
        "sounds/voice/Best/vox_set_ay_04.wav",
        "sounds/voice/Best/vox_set_bensgame_01.wav",
        "sounds/voice/Best/vox_set_gotit_05.wav",
        "sounds/voice/Best/vox_set_iwin_10.wav",
        "sounds/voice/Best/vox_set_ow_11.wav",
        "sounds/voice/Best/vox_set_pressstart_01.wav",
        "sounds/voice/Best/vox_set_whoopy_03.wav",
        "sounds/voice/Best/vox_set_yah_01.wav",
        "sounds/voice/Best/vox_set_yeah_04.wav",
        "sounds/voice/Best/vox_set_yes_11.wav",
        "sounds/voice/Best/vox_ben_ay_01.wav",
        "sounds/voice/Best/vox_ben_ay_04.wav",
        "sounds/voice/Best/vox_ben_ay_10.wav",
        "sounds/voice/Best/vox_ben_ay_hi_01.wav",
        "sounds/voice/Best/vox_ben_ow_01.wav",
        "sounds/voice/Best/vox_ben_ow_02.wav",
        "sounds/voice/Best/vox_ben_ow_03.wav",
        "sounds/voice/Best/vox_ben_ow_04.wav",
        "sounds/voice/Best/vox_ben_ow_10.wav",
        "sounds/voice/Best/vox_ben_ow_11.wav",
        "sounds/voice/Best/vox_ben_yah_01.wav",
        "sounds/voice/Best/vox_ben_yah_10.wav",
        "sounds/voice/Best/vox_ben_yeah_01.wav",
        "sounds/voice/Best/vox_ben_yeah_02.wav",
        "sounds/voice/Best/vox_ben_yeah_10.wav",
        "sounds/voice/Best/vox_ben_yes_01.wav",
        "sounds/voice/Best/vox_ben_yes_10.wav",
        "sounds/voice/Best/vox_ben_yes_11.wav",
        "sounds/voice/Best/vox_ben_gotit_01.wav",
        "sounds/voice/Best/vox_ben_gotit_04.wav",
        "sounds/voice/Best/vox_ben_gotit_10.wav",
        "sounds/voice/Best/vox_ben_gotit_11.wav",
        "textures/player_Ben.jpg",
        "textures/player_Ben_mask.jpg",
        "textures/player_Christina.jpg",
        "textures/player_Christina_mask.jpg",
        "textures//font2Ar24.txt",
        "textures/Spanish_button_big_play.jpg",
        "textures/Spanish_button_big_play.jpg",
        "textures/Spanish_button_big_play_mask.jpg",
        "textures/Spanish_button_you_win.jpg",
        "textures/Spanish_button_you_win.jpg",
        "textures/Spanish_button_you_win_mask.jpg",
        "textures/Spanish_button_donation.jpg",
        "textures/Spanish_button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
        "textures/screen_title3.jpg",
        "textures/language_English.txt",
        "textures/Spanish_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Spanish_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Spanish_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Spanish_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Spanish_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Spanish_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Spanish_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Spanish_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Spanish_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/English_button_big_play.jpg",
        "textures/button_big_play.jpg",
        "textures/button_big_play_mask.jpg",
        "textures/English_button_you_win.jpg",
        "textures/button_you_win.jpg",
        "textures/button_you_win_mask.jpg",
        "textures/English_button_donation.jpg",
        "textures/button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
        "textures/language_English.txt",
        "textures/English_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/English_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/English_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/English_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/English_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/English_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/English_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/English_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/English_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Dutch_button_big_play.jpg",
        "textures/Dutch_button_big_play.jpg",
        "textures/Dutch_button_big_play_mask.jpg",
        "textures/Dutch_button_you_win.jpg",
        "textures/Dutch_button_you_win.jpg",
        "textures/Dutch_button_you_win_mask.jpg",
        "textures/Dutch_button_donation.jpg",
        "textures/Dutch_button_donation.jpg",
        "textures/Dutch_font3Ar24000.jpg",
        "textures//Dutch_font3Ar24.txt",
        "textures/language_English.txt",
        "textures/Dutch_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Dutch_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Dutch_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Dutch_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Dutch_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Dutch_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Dutch_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Dutch_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Dutch_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/French_button_big_play.jpg",
        "textures/French_button_big_play.jpg",
        "textures/French_button_big_play_mask.jpg",
        "textures/French_button_you_win.jpg",
        "textures/French_button_you_win.jpg",
        "textures/French_button_you_win_mask.jpg",
        "textures/French_button_donation.jpg",
        "textures/French_button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
        "textures/language_English.txt",
        "textures/French_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/French_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/French_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/French_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/French_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/French_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/French_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/French_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/French_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/German_button_big_play.jpg",
        "textures/German_button_big_play.jpg",
        "textures/German_button_big_play_mask.jpg",
        "textures/German_button_you_win.jpg",
        "textures/German_button_you_win.jpg",
        "textures/German_button_you_win_mask.jpg",
        "textures/German_button_donation.jpg",
        "textures/German_button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
        "textures/language_English.txt",
        "textures/German_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/German_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/German_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/German_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/German_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/German_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/German_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/German_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/German_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Greek_button_big_play.jpg",
        "textures/Greek_button_big_play.jpg",
        "textures/Greek_button_big_play_mask.jpg",
        "textures/Greek_button_you_win.jpg",
        "textures/Greek_button_you_win.jpg",
        "textures/Greek_button_you_win_mask.jpg",
        "textures/Greek_button_donation.jpg",
        "textures/button_donation.jpg",
        "textures/Greek_font3Ar24000.jpg",
        "textures//Greek_font3Ar24.txt",
        "textures/language_English.txt",
        "textures/Greek_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Greek_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Greek_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Greek_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Greek_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Greek_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Greek_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Greek_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Greek_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Italian_button_big_play.jpg",
        "textures/Italian_button_big_play.jpg",
        "textures/Italian_button_big_play_mask.jpg",
        "textures/Italian_button_you_win.jpg",
        "textures/Italian_button_you_win.jpg",
        "textures/Italian_button_you_win_mask.jpg",
        "textures/Italian_button_donation.jpg",
        "textures/Italian_button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
        "textures/language_English.txt",
        "textures/Italian_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Italian_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Italian_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Italian_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Italian_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Italian_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Italian_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Italian_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Italian_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Japanese_button_big_play.jpg",
        "textures/Japanese_button_big_play.jpg",
        "textures/Japanese_button_big_play_mask.jpg",
        "textures/Japanese_button_you_win.jpg",
        "textures/button_you_win.jpg",
        "textures/button_you_win_mask.jpg",
        "textures/Japanese_button_donation.jpg",
        "textures/button_donation.jpg",
        "textures/Japanese_font3Ar24000.jpg",
        "textures//Japanese_font3Ar24.txt",
        "textures/language_English.txt",
        "textures/Japanese_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Japanese_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Japanese_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Japanese_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Japanese_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Japanese_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Japanese_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Japanese_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Japanese_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Russian_button_big_play.jpg",
        "textures/Russian_button_big_play.jpg",
        "textures/Russian_button_big_play_mask.jpg",
        "textures/Russian_button_you_win.jpg",
        "textures/Russian_button_you_win.jpg",
        "textures/Russian_button_you_win_mask.jpg",
        "textures/Russian_button_donation.jpg",
        "textures/button_donation.jpg",
        "textures/Russian_font3Ar24000.jpg",
        "textures//Russian_font3Ar24.txt",
        "textures/language_English.txt",
        "textures/Russian_English_flag.jpg",
        "textures/English_flag.jpg",
        "textures/language_Dutch.txt",
        "textures/Russian_Dutch_flag.jpg",
        "textures/Dutch_flag.jpg",
        "textures/language_French.txt",
        "textures/Russian_French_flag.jpg",
        "textures/French_flag.jpg",
        "textures/language_German.txt",
        "textures/Russian_German_flag.jpg",
        "textures/German_flag.jpg",
        "textures/language_Greek.txt",
        "textures/Russian_Greek_flag.jpg",
        "textures/Greek_flag.jpg",
        "textures/language_Italian.txt",
        "textures/Russian_Italian_flag.jpg",
        "textures/Italian_flag.jpg",
        "textures/language_Japanese.txt",
        "textures/Russian_Japanese_flag.jpg",
        "textures/Japanese_flag.jpg",
        "textures/language_Russian.txt",
        "textures/Russian_Russian_flag.jpg",
        "textures/Russian_flag.jpg",
        "textures/language_Spanish.txt",
        "textures/Russian_Spanish_flag.jpg",
        "textures/Spanish_flag.jpg",
        "textures/Spanish_button_big_play.jpg",
        "textures/Spanish_button_big_play.jpg",
        "textures/Spanish_button_big_play_mask.jpg",
        "textures/Spanish_button_you_win.jpg",
        "textures/Spanish_button_you_win.jpg",
        "textures/Spanish_button_you_win_mask.jpg",
        "textures/Spanish_button_donation.jpg",
        "textures/Spanish_button_donation.jpg",
        "textures/font2Ar24000.jpg",
        "textures//font2Ar24.txt",
NULL
};
#endif

void tsuInit(void)
{
#if TSU_MAKE
	tsuMake();
#endif

#if TSU_USE
	tsuLoad();
#endif
}

void tsuTerminate(void)
{
}

void tsuMarkFile(char *fileName)
{
#if TSU_MAKE
	char *cp;
	cp = strrchr(fileName, '\\');
	if (cp) cp++;
	else {
		cp = strrchr(fileName, '/');
		if (cp) cp++;
		else {
			cp = fileName;
		}
	}

	cp = fileName;
	printf("\t\"%s\",\n", cp);
#endif
}

typedef struct tsuTag {
	int32 dataSize; // data size not including this tag 
	int32 dataCheck; // checksum 
	char name[256]; // file name 
} tsuTag;

int32 gTsuChecksum = 0;

void tsuEncrypt(unsigned char *data, int size)
{
	char *crypt = gTsuFileEncryptStr;

	while (size--) {
		*data++ ^= *crypt++;
		if (*crypt == 0) crypt = gTsuFileEncryptStr;
	}
}

int32 tsuChecksum(unsigned char *data, int size)
{
	int32 sum = 0;

	while (size--) {
		sum += *data++;
	}
	gTsuChecksum += sum;
	return(sum);
}

#if TSU_MAKE
void tsuMake(void)
{
	int i, j;
	FILE *fp;
	unsigned char *data;
	tsuTag tag;
	char *padding = "pad";
	bool	unique;

	gTsuFile = fopen(gTsuFileName, "wb");
	if (gTsuFile) {
		strcpy(tag.name, gTsuFileBeginStr);
		tag.dataSize = 0;
		tag.dataCheck = gTsuChecksum;
		fwrite(&tag, 1, sizeof(tag), gTsuFile);
		for (i = 0; gTsuFileList[i]; i++) {
			/**** strip out duplicates ****/
			unique = true;
			for (j = 0; j < i && unique; j++) {
				if (!strcmp(gTsuFileList[i], gTsuFileList[j])) unique = false;
			}
			if (unique) {
				fp = fopen(gTsuFileList[i], "rb");
				if (fp) {
					fseek(fp, 0, SEEK_END);
					tag.dataSize = ftell(fp);
					fseek(fp, 0, SEEK_SET);
					if (tag.dataSize > 0) {
						data = new unsigned char[tag.dataSize];
						fread(data, 1, tag.dataSize, fp);
						strcpy(tag.name, gTsuFileList[i]);

						tag.dataCheck = tsuChecksum(data, tag.dataSize);
						tsuEncrypt(data, tag.dataSize);

						fwrite(&tag, 1, sizeof(tag), gTsuFile);
						fwrite(data, 1, tag.dataSize, gTsuFile);
						if (tag.dataSize & 3) {
							fwrite(padding, 1, 4-(tag.dataSize & 3), gTsuFile);
						}
						delete[] data; 
					}
					fclose(fp);
				}
			}
		}
		strcpy(tag.name, "");
		tag.dataSize = 0;
		tag.dataCheck = gTsuChecksum;
		fwrite(&tag, 1, sizeof(tag), gTsuFile);

		fclose(gTsuFile);
	}
}
#endif

void *tsuGetMemFile(char *fileName, uint32 *psize)
{
	tsuTag *tag;

	tag = (tsuTag *)gTsuData;
	if (!tag) return(NULL);
	tag++;

	while (tag && tag->dataSize) {
        	if (!strcmp(tag->name, fileName)) {
			if (psize) *psize = tag->dataSize;
			return((void*)(tag + 1));
		}
		tag = (tsuTag*)((tag->dataSize + sizeof(tsuTag) + (size_t)tag + 3) & (~3));
	}
	if (psize) *psize = 0;
	return(NULL);
}

#if TSU_USE
void tsuLoad(void)
{
	int i, size;
	int32 check;
	FILE *fp;
	unsigned char *data;
	tsuTag *tag;
	bool	tsuReport = false;
	FILE	*rfp = NULL;

	if (tsuReport) rfp = fopen("tsuReport.csv", "w");

	gTsuFile = fopen(gTsuFileName, "rb");

#if MAC_BUILD
	if (!gTsuFile)
	{
		gTsuFile = mac_open_resource_file(gTsuFileName, "rb");
	}
#endif

	if (gTsuFile) {
		fseek(gTsuFile, 0, SEEK_END);
		size = ftell(gTsuFile);
		fseek(gTsuFile, 0, SEEK_SET);
		if (size > 0) {
			gTsuData = new unsigned char[size];
			fread(gTsuData, 1, size, gTsuFile);

			tag = (tsuTag *)gTsuData;
			if (strcmp(tag->name, gTsuFileBeginStr)) ShutdownApplication(1);	// exit(1);
			tag++;

#if MAC_BUILD
			// ENDIAN we hate thee
			TWIST(tag->dataSize);
			TWIST(tag->dataCheck);
#endif
			if (rfp) fprintf(rfp, "%s,%d\n", tag->name, tag->dataSize);

			while (tag && tag->dataSize) 
			{
				data = (unsigned char *)(tag + 1);
				tsuEncrypt(data, tag->dataSize);
				check = tsuChecksum(data, tag->dataSize);
				if (tag->dataCheck != check) exit(1);
				tag = (tsuTag*)((tag->dataSize + sizeof(tsuTag) + (size_t)tag + 3) & (~3));

#if MAC_BUILD
				// ENDIAN we hate thee
				TWIST(tag->dataSize);
				TWIST(tag->dataCheck);
#endif
				if (rfp) fprintf(rfp, "%s,%d\n", tag->name, tag->dataSize);
			}
			if (tag->dataCheck != gTsuChecksum) ShutdownApplication(1);		// exit(1);
		}
		fclose(gTsuFile);
	}
	if (rfp) fclose(rfp);
}
#endif

