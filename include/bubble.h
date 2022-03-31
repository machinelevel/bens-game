
/************************************************************\
	Bubble.h
	Files for controlling the Bubbles in Ben's project
\************************************************************/

#ifndef BUBBLE_H
#define BUBBLE_H

enum {
	BUBBLE_TYPE_EMPTY,

	BUBBLE_TYPE_HEALTH,
	BUBBLE_TYPE_AMMO,
	BUBBLE_TYPE_ATTITUDE,

	BUBBLE_TYPE_BLASTER,
	BUBBLE_TYPE_SPLIT,
	BUBBLE_TYPE_SLINGSHOT,
	BUBBLE_TYPE_SWORD,
	BUBBLE_TYPE_MISSILE,
	BUBBLE_TYPE_BALLISTIC,
	BUBBLE_TYPE_CROSSBOW,
	BUBBLE_TYPE_PARTICLE_ACCELERATOR,

	BUBBLE_TYPE_SHIELD_POX,
	BUBBLE_TYPE_SHIELD_COLDS,
	BUBBLE_TYPE_SHIELD_RASH,
	BUBBLE_TYPE_SHIELD_BARF,
	BUBBLE_TYPE_SHIELD_FEVER,
	BUBBLE_TYPE_SHIELD_HAIR,
	BUBBLE_TYPE_SHIELD_BANDAID,

	BUBBLE_TYPE_HOWMANY	/**** just to count and terminate ****/
};

#define BUBBLE_FLAG_ACTIVE	0x00000001

class Bubble {
public:
	Bubble(int32 type);
	~Bubble();
	
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);

	static Bubble *msFirst;

	static void RemoveDead(void);
	static bool CheckGliderCollisions(Glider *glide);
	static int32 CountTypes(int32 type);
	static void SpawnType(int32 type);
	static void ThinkAll(void);
	static void MoveAll(void);
	static void ReactAll(void);
	static void DrawAll(void);

	int32		mType;
	int32		mFlags;
	float		mLifeTimer;

	float		mSize;
	pfVec4		mColor;
	int32		mBubbleTexture;
	int32		mPrizeTexture;
	pfVec3		mPosition;
	pfVec3		mVelocity;

	float		mBobTimer;
	float		mSpinTimer;
	float		mSpawnDrop;
	
	class Bubble	*mPrev;
	class Bubble	*mNext;
};

#endif	// BUBBLE_H

