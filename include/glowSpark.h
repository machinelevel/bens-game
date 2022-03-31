
/************************************************************\
	GlowSpark.h
	Files for controlling the GlowSparks in Ben's project
\************************************************************/

#ifndef GLOWSPARK_H
#define GLOWSPARK_H

enum {
	GLOWSPARK_TYPE_NORMAL,
	GLOWSPARK_TYPE_SWORD,
	GLOWSPARK_TYPE_BIGSOFT,

	GLOWSPARK_TYPE_HOWMANY	/**** just to count and terminate ****/
};

enum {
	MINISPARK_TYPE_FIREFLY,
	MINISPARK_TYPE_FIREWORKS,

	MINISPARK_TYPE_HOWMANY	/**** just to count and terminate ****/
};

#define SPARK_FLAG_ACTIVE		0x00000001

#define GLOWSPARK_FLAG_ACTIVE	0x00000001

typedef struct Spark {
	int32	mFlags;
	float	mLifeTimer;
	pfVec3	mPosition;
	pfVec3	mLastPosition;
	pfVec4	mColor;
	pfVec3	mVelocity;
	float	mSize;
} Spark;

class GlowSpark {
public:
	GlowSpark(int32 type, int32 numSparks);
	~GlowSpark();
	
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);

	static GlowSpark *msFirst;
	static void ThinkAll(void);
	static void MoveAll(void);
	static void ReactAll(void);
	static void DrawAll(void);

	int32		mType;
	int32		mFlags;
	float		mLifeTimer;

	float		mGlowSize;
	pfVec4		mGlowColor;
	int32		mGlowTexture;
	float		mSparkSize;
	pfVec4		mSparkColor;
	int32		mSparkTexture;
	pfVec3		mPosition;
	pfVec3		mLastPosition;
	pfVec3		mVelocity;

	int32		mNumSparks;
	Spark		*mSparks;

	class GlowSpark	*mPrev;
	class GlowSpark	*mNext;
};

class MiniSparks {
public:
	MiniSparks(int32 type, int32 numSparks);
	~MiniSparks();
	
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);
	static void UpdateFireworks(void);


	bool		mBurst;
	float		mAcc;
	float		mBuddyAcc;
	float		mDrag;

	int32		mType;
	int32		mFlags;
	float		mLifeTimer;

	pfVec3		mCenter;
	float		mSpread;
	pfVec4		mColor;
	int			mSparkTexture;

	int32		mNumSparks;
	Spark		*mSparks;
};

#define NUM_FIREWORKS	3
extern MiniSparks *gFireFlies;
extern MiniSparks *gFireworks[NUM_FIREWORKS];

#endif	// GLOWSPARK_H

