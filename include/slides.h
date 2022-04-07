/************************************************************\
	slides.h
	Files for controlling the slides in Ben's project
\************************************************************/

enum {
//	SLIDE_LEGAL,
	SLIDE_PRE_TITLE,
	SLIDE_TITLE,
	SLIDE_WISH,
//	SLIDE_OPTIONS,
	SLIDE_START,
	SLIDE_BUTTON_PLAY,
	SLIDE_BUTTON_SAVE,
	SLIDE_BUTTON_NUMPLAYERS,
	SLIDE_BUTTON_CHOOSE1PLAYER,
	SLIDE_BUTTON_CHOOSE2PLAYER,
	SLIDE_BUTTON_DIFFICULTY,
	SLIDE_PAUSE,
	SLIDE_YOU_WIN,
//	SLIDE_BENS_SECRET_SLIDE,
	SLIDE_HOWMANY	/**** just to count and terminate ****/
};

enum {
	BUTTON_ID_NONE,
	BUTTON_ID_BIGPLAY,

	BUTTON_ID_MAW,
	BUTTON_ID_HELP,
	BUTTON_ID_THANKS,
	BUTTON_ID_OPTIONS,
	BUTTON_ID_FORTUNE,
	BUTTON_ID_QUIT,

	BUTTON_ID_SCORES,

	BUTTON_ID_1PLAYER,
	BUTTON_ID_2PLAYER,
	BUTTON_ID_BACK,
	BUTTON_ID_PLAY,

	BUTTON_ID_SAVE1,
	BUTTON_ID_SAVE2,
	BUTTON_ID_SAVE3,
	BUTTON_ID_SAVE4,

	BUTTON_ID_EASY,
	BUTTON_ID_MEDIUM,
	BUTTON_ID_HARD,

	BUTTON_ID_BODY_1A,
	BUTTON_ID_BODY_1B,
	BUTTON_ID_BODY_2A,
	BUTTON_ID_BODY_2B,
	BUTTON_ID_BOARD_1A,
	BUTTON_ID_BOARD_1B,
	BUTTON_ID_BOARD_2A,
	BUTTON_ID_BOARD_2B,
	BUTTON_ID_NAME_1,
	BUTTON_ID_NAME_2,

	BUTTON_ID_PROCEED1,

	BUTTON_ID_FLAG_00,
	BUTTON_ID_FLAG_01,
	BUTTON_ID_FLAG_02,
	BUTTON_ID_FLAG_03,
	BUTTON_ID_FLAG_04,
	BUTTON_ID_FLAG_05,
	BUTTON_ID_FLAG_06,
	BUTTON_ID_FLAG_07,
	BUTTON_ID_FLAG_08,
	BUTTON_ID_FLAG_09,
	BUTTON_ID_FLAG_10,
	BUTTON_ID_FLAG_11,
	BUTTON_ID_FLAG_12,
	BUTTON_ID_FLAG_13,
	BUTTON_ID_FLAG_14,
	BUTTON_ID_FLAG_15,
	BUTTON_ID_FLAG_16,
	BUTTON_ID_FLAG_17,
	BUTTON_ID_FLAG_18,
	BUTTON_ID_FLAG_19,
	BUTTON_ID_FLAG_20,
	BUTTON_ID_FLAG_21,
	BUTTON_ID_FLAG_22,
	BUTTON_ID_FLAG_23,
	BUTTON_ID_FLAG_24,
	BUTTON_ID_FLAG_25,
	BUTTON_ID_FLAG_26,
	BUTTON_ID_FLAG_27,
	BUTTON_ID_FLAG_28,
	BUTTON_ID_FLAG_29,

	/**** alert buttons start here ****/
	BUTTON_ID_ALERT_OK,
	BUTTON_ID_ALERT_QUIT,
	BUTTON_ID_ALERT_NOQUIT,
	BUTTON_ID_ALERT_BACK,
	BUTTON_ID_ALERT_NEXT,
	BUTTON_ID_ALERT_1,

	BUTTON_ID_ALERT_WEB_DONATION_SITE,
	BUTTON_ID_ALERT_WEB_BENSGAME_SITE,
	BUTTON_ID_ALERT_WEB_MAKEWISH_SITE,
	BUTTON_ID_ALERT_WEB_INSTRUCTIONS_SITE,

	BUTTON_ID_ALERT_CHECKBOX_WINDOW,
	BUTTON_ID_ALERT_CHECKBOX_MIPMAP,
	BUTTON_ID_ALERT_CHECKBOX_SOUND,
	BUTTON_ID_ALERT_CHECKBOX_VOICE,
	BUTTON_ID_ALERT_CHECKBOX_WAVES,
	BUTTON_ID_ALERT_CHECKBOX_SAVE,


};

#define NUM_SAVES		4
#define SAVE_VERSION	0x051704
#define WRITE_SAVE_SLOTS(_x) gSave.slots[0]._x, gSave.slots[1]._x, gSave.slots[2]._x, gSave.slots[3]._x
#define READ_SAVE_SLOTS(_x) &(gSave.slots[0]._x), &(gSave.slots[1]._x), &(gSave.slots[2]._x), &(gSave.slots[3]._x)
typedef struct SaveSlot {
	int32		numPlayers;
	char		playerName[2][256];
	char		boardName[2][256];
	char		shields[8];
	int32		difficulty;
	int32		totalCellsDestroyed;
	float		gameTime;
	int32		checkSum;
} SaveSlot;

typedef struct SaveFile {
	int32		version;
	int32		numSaves;
	SaveSlot	slots[NUM_SAVES];
} SaveFile;

extern SaveFile	gSave;
extern int		gsSaveSlot;
extern void		WriteSaves();
extern void		ReadSaves();
extern void		GameToSaveSlot();
extern void		SaveSlotToGame();


class SlideButton {
public:
	int	cx, cy, w, h;
	int tex;
	char *hint;
	int	hintX, hintY;
	float	selectScale;
};


struct OneSlide {
	char	*mTexName;
	int		mTexID;
	int		mSize[2];
	int		*mButtonList;
};

#define BOX_FLAG_END		0x00000001
#define BOX_FLAG_IMAGE		0x00000002
#define BOX_FLAG_TEXT		0x00000004
#define BOX_FLAG_CENTER		0x00000008

struct Box {
	uint32	flags;
	int		cx,cy,w,h;
	int		texID;
	float	scale;
	char	*text;
};

#define SPLAT_NUM	100
typedef struct Splat {
	pfMatrix	mat;
	float		timer;
} Splat;

class Slides {
public:
	Slides();
	~Slides();

	void Think(void);
	void Draw(void);
	void NextSlide(void);
	void NewGame(int numPlayers);
	void Click(int x, int y);
	void MousePos(int x, int y);
	int GetRectButton(int *list, int x, int y, int *secondaryList);
	void StartAlertSlide(int whichSlide);
	void UpdateAlertSlide(void);
	void DrawAlertBubble(void);
	void DrawWobbleButton(float x, float y, float w, float h);
	void DrawStandardUIButton(int buttonID, bool mirror = false);
	void TendButtonSize(int buttonID, float w, float h, float speed);
	void TendButtonPos(int buttonID, float cx, float cy, float speed);
	void DrawSelectionModel(int player, float cx, float cy, float scale);
	void ScanPlayerImages(void);
	int *GetAlertButtonList(void);
	void NewSplat(float x, float y, float size);
	void ManageSplats(void);
	void DrawSplats(void);
	void DrawBoxes(Box *list);
	void DrawBackFan(void);

	int			mPlayStage;
	int			mCurrentSlide;
	int			mCurrentButton;
	int			mNextSlide;
	float		mFadeTimer;
	OneSlide	mSlideList[SLIDE_HOWMANY];
	float		mWobbleX;
	float		mWobbleY;
	float		mWobbleScale;
	float		mWobbleSpeed[4];
	float		mWobbleTimer[4];
	Splat		mSplats[SPLAT_NUM];
	int			mSplatCount;
};

typedef struct PlayerImage {
	char	mFileName[512];
	char	mPlayerName[256];
} PlayerImage;

enum {
	DIFFICULTY_EASY = 0,
	DIFFICULTY_MEDIUM,
	DIFFICULTY_HARD
};

extern Slides	*gSlides;

extern bool gOptionLaunchInWindow;
extern bool gOptionMipMap;
extern bool gOptionSoundEffectsOn;
extern bool gOptionVoicesOn;
extern bool gOptionWaves;
extern bool gOptionAutoSave;
extern char	gOptionLanguage[256];

void LoadOptions(void);
void SaveOptions(void);

extern int		gNumPlayers;
extern int		gDifficultySetting;
