
/************************************************************\
	boss.h
	Files for controlling the boss in Ben's project
\************************************************************/

enum {
	BOSS_TYPE_EVILCHICKEN,
	BOSS_TYPE_ICEMAN,
	BOSS_TYPE_TORNADO,
	BOSS_TYPE_ROBARF,
	BOSS_TYPE_FIREMAN,
	BOSS_TYPE_QBALL,
	BOSS_TYPE_VAMPIRE,

	BOSS_TYPE_HOWMANY	/**** just to count and terminate ****/
};

#define BOSS_MAX_LAUNCH	3
enum {
	BOSS_LAUNCH_STATE_OFF,
	BOSS_LAUNCH_STATE_AIRBORNE,
	BOSS_LAUNCH_STATE_LANDED,
	BOSS_LAUNCH_STATE_CHASE,
};

class Boss {
public:
	Boss();
	~Boss();
	void Damage(float amount);
	void Reset(int32 type);
	void Fire(int32 weapon);
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);
	void ManageLaunches(void);
	void DrawLaunches(void);
	void SimpleVoice(void);

	float		mVoiceTimer;
	int32		mBossType;
	pfMatrix	mMatrix;
	pfMatrix	mLastMatrix;
	pfVec3		mTargetPos;
	pfVec3		mVelocity;
	int32		mInnerRange;
	int32		mOuterRange;
	float		mHealth;
	bool		mDead;
	bool		mRelocate;
	float		mDesiredFacing;
	float		mCurrentFacing;

	float		mNextLaunchTimer;
	float		mLaunchUpSpeed;
	float		mLaunchGravity;
	float		mLaunchTravelTime;
	float		mLaunchFromHeight;
	int			mLaunchTexID;
	int			mLaunchStage[BOSS_MAX_LAUNCH];
	pfMatrix	mLaunchMat[BOSS_MAX_LAUNCH];
	pfVec3		mLaunchVel[BOSS_MAX_LAUNCH];
	float		mLaunchRadius[BOSS_MAX_LAUNCH];
	float		mLaunchThickness[BOSS_MAX_LAUNCH];
	float		mLaunchLifeTimer[BOSS_MAX_LAUNCH];
};

extern Boss	*gBoss;

