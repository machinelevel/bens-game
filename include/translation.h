
/************************************************************\
	translation.h
	Files_for_international_translation_in_Ben's_project
\************************************************************/

#define MAX_TRANSLATION_LINE_LENGTH	4096

enum {
	TXT_Bens_Game,
	TXT_Great_job__keep_it_up,
	TXT_Dont_be_afraid,
	TXT_Take_it_one_day_at_a_time,
	TXT_Dont_give_up__be_strong,
	TXT_Every_day_is_a_great_day,
	TXT_Dont_forget_to_take_your_pills,
	TXT_A_nap_a_day_keeps_the_blahs_away,
	TXT_Never_think_bad_thoughts,
	TXT_If_you_have_a_tummyache_make_yourself_a_milkshake,
	TXT_Dont_stop_now_youre_doing_great,
	TXT_Keep_going__zoom_to_the_finish,
	TXT_So_youre_bald__big_deal,
	TXT_noosllewteg,
	TXT_Youre_a_star,

	TXT_Welcome_to_Bens_Game,
	TXT_This_is_a_game_about_fighting_cancer_In_Bens_Game_you_play_a_high_speed_hero_with_good_friends_and_all_the_right_tools,
	TXT_For_more_detailed_instructions_visit_wwwmakewishorg_ben,
	TXT_Find_Seven_Shields,
	TXT_There_are_SEVEN_SHIELDS_to_collect_These_are_shields_for_protection_against_some_yucky_side_effects,
	TXT_Chicken_Pox,
	TXT_Fever,
	TXT_Bleeding,
	TXT_Hair_Loss,
	TXT_Barf,
	TXT_Rash,
	TXT_Colds,
	TXT_Defeat_Monsters,
	TXT_Each_shield_is_guarded_by_a_MONSTER_who_generates_MUTATED_CELLS,
	TXT_Staying_Healthy,
	TXT_Your_hero_has_HEALTH_from_the_hospital_AMMO_from_the_pharmacy_ATTITUDE_from_home_When_you_bump_a_MUTATED_CELL_you_lose_HEALTH_To_get_it_back_fly_to_the_HEALTH_BUBBLE_in_the_corner_of_the_board,
	TXT_Setbacks,
	TXT_Electrical_barriers_in_the_game_are_called_SETBACKS_When_you_hit_them_you_lose_ATTITUDE,
	TXT_Weapons_and_Ammo,
	TXT_There_are_WEAPONS_floating_in_bubbles_in_the_game_To_get_a_weapon_just_run_into_it,
	TXT_Customize_Bens_Game,
	TXT_The_character_can_look_like_anything_you_want_CONT,
	TXT_Make_A_Wish,
	TXT_How_to_Play,
	TXT_Thanks,
	TXT_Options,
	TXT_Message_from_Ben,
	TXT_Quit,
	TXT_One_Player,
	TXT_Two_Players,
	TXT_Go_Back,
	TXT_Play,
	TXT_Start_a_brand_new_game,
	TXT_Continue_where_you_left_off,
	TXT_Yes_bye_bye,
	TXT_No_keep_playing,
	TXT_Next,

	TXT_The_Greater_Bay_Area,
	TXT_Make_A_Wish_Foundation,
	TXT_This_game_was_made_possible_when_Ben_and_Eric_were_introduced_CONT,
	TXT_These_buttons_will_take_you_to_the_Make_A_Wish_web_pages,
	TXT_Make_a_Donation,
	TXT_Visit_Make_A_Wish,
	TXT_Bens_Game_Page,
	TXT_A_Message_From_Ben,
	TXT_Game_Options,
	TXT_Start_up_Bens_Game_in_a_window_next_time,
	TXT_Use_higher_quality_graphics__may_be_slower_,
	TXT_Enable_sound_effects,
	TXT_Enable_character_voices,
	TXT_Wavy_motion_for_cells,
	TXT_Auto_save_my_progress,
	TXT_Ben_and_Eric_wish_to_say_THANKS_to_the_following_people,
	TXT_Are_you_sure_you_want_to_quit,
	TXT_Version_Release_1__6_29_04,
	TXT_Welcome_Bens_Game_To_get_an_updated_version_or_to_make_comments_and_report_bugs_please_visit_wwwmakewishorg_ben,
	TXT_New_Game,
	TXT_Continue_Game,

	TXT_fontAr16,
	TXT_fontAr16000,

	TXT_Checkpoint1,

	TXT_Thank_you,
	TXT_COMPLETE,
	TXT_Play_Button,
	TXT_PLAY_EASY,
	TXT_PLAY_MEDIUM,
	TXT_PLAY_HARD,
	TXT_You_Win,
	TXT_Player_1,
	TXT_Player_2,
	TXT_Move,
	TXT_Stop,
	TXT_Shoot,
	TXT_Switch_Weapon,
	TXT_Adjust_Camera,

	MAX_NUM_TRANSLATION_LINES
};

#define MAX_LANGUAGES	30

extern char	gTranslationTable[MAX_NUM_TRANSLATION_LINES][MAX_TRANSLATION_LINE_LENGTH];

extern int	gNumLanguages;
extern char	gLanguageList[MAX_LANGUAGES][256];

#define TRANSLATE(id)	(gTranslationTable[id])

void WriteTranslationFile(void);
void ReadTranslationFile(void);
void TextSetLanguage(int whichLanguage);
void FullSetLanguage(void);
void FindLanguages(void);
void InitTranslation(void);
bool SetLanguageByName(char *name);
char *GetLanguageName(void);

