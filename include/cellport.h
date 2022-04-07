/************************************************************\
	cellport.h
	Files for controlling the grid of cells in Ben's project
\************************************************************/

enum {
	GAME_MODE_SLIDES,
	GAME_MODE_INTRO,
	GAME_MODE_PORT,

	GAME_MODE_PLAY,

	GAME_MODE_HOWMANY	/**** Just to count and terminate ****/
};

#define SHIELD_HOWMANY	7

typedef struct LevelStruct {
	int			mShieldTexID;
	int			mBossID;
	int			mDifficulty;
	int			mLevelNumber;
	bool		mComplete;
} LevelStruct;

extern LevelStruct	gLevels[SHIELD_HOWMANY];
extern int			gCurrentLevel;
extern int			gNextLevelNumber;

const int kPortCircleSegments = 64;

#define PARKMAP_SIDE_COUNT 20

class CellPort {
public:
	CellPort();
	~CellPort();

	void Think(void);
	void Draw(void);
	void Clear(void);
	void DrawShields(void);
	void HandleZoom(void);

	void IntroCamera(void);
	
	float	mSVals[kPortCircleSegments];
	float	mCVals[kPortCircleSegments];
	float	mRadius;
	pfVec3	mCenter;
	float	mIntroCameraTimer;
	float	mIntroCameraSplineFactor;
	float	mParkMap[PARKMAP_SIDE_COUNT][PARKMAP_SIDE_COUNT];
};

extern CellPort	*gPort;
extern int32	gGameMode;
extern float	gWinTimer;
extern float	gZoomTimer;