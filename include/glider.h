
/************************************************************\
	glider.h
	Files for controlling the glider in Ben's project
\************************************************************/
#include "glowSpark.h"

#define GLIDER_FLAG_ON_HALFPIPE	0x00000001
#define GLIDER_FLAG_ACTIVE		0x00000002

#define GLIDER_ACTIVE(_g) (((_g)->mFlags & GLIDER_FLAG_ACTIVE) != 0)

enum {
	GLIDER_KEY_TURN_LEFT,
	GLIDER_KEY_TURN_RIGHT,
	GLIDER_KEY_THRUST,
	GLIDER_KEY_BRAKE,
	GLIDER_KEY_FIRE1,
	GLIDER_KEY_SWITCH,

	GLIDER_KEY_HOWMANY
};

#define MAX_NUM_GLIDERS	2

#define GLIDER_TRAIL_LENGTH	50

class Glider {
public:
	Glider();
	~Glider();
	void Thrust(float amount);
	void Turn(float amount);
	void Fire(int32 weapon);
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);
	void DrawDude(void);
	void DrawWeapon(int32 weapon);
	void SimpleGameVoice(int boyVoice, int girlVoice, bool ignoreTimer = false);
	void ManageTrail(void);
	void DrawTrail(void);
	void ManageTripleWeapons(void);
	
	int32		mFlags;
	pfMatrix	mMatrix;
	pfMatrix	mDrawMatrix;
	pfMatrix	mLastMatrix;
	pfVec3		mVelocity;
	float		mVoiceTimer;
	float		mRapidFireTimer;
	float		mSpeed;
	float		mHoverHeight;
	bool		mFlying;
	float		mTurning;
	float		mThrusting;
	float		mBraking;
	float		mGliderCamDist;
	int32		mLightningIndex;
	int32		mCurrentWeapon[2];
	GlowSpark	*mWeaponGlowSpark;
	GlowSpark	*mWeaponGlowSpark2;
	int			mPlayerNum;
	int			mPlayerImageNum;
	int			mBoardImageNum;
	char		mName[256];
	Glider		*mNext;
	pfVec3		mTrailVerts[2][GLIDER_TRAIL_LENGTH];
	int			mTrailNext;
};

extern Glider	*gActiveGliderList;
extern Glider	*gGliders[MAX_NUM_GLIDERS];

extern unsigned char		gGliderKeys[MAX_NUM_GLIDERS][GLIDER_KEY_HOWMANY];

