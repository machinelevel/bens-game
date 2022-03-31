
/************************************************************\
	projectile.h
	Files for controlling the projectiles in Ben's project
\************************************************************/

enum {
	PROJECTILE_TYPE_SMALL,
	PROJECTILE_TYPE_SPLIT,
	PROJECTILE_TYPE_BLASTER,
	PROJECTILE_TYPE_SLINGSHOT,
	PROJECTILE_TYPE_SWORD,
	PROJECTILE_TYPE_MISSILE,
	PROJECTILE_TYPE_BALLISTIC,
	PROJECTILE_TYPE_CROSSBOW,
	PROJECTILE_TYPE_PARTICLE_ACCELERATOR,

	PROJECTILE_TYPE_LAUNCH_BARF,
	PROJECTILE_TYPE_LAUNCH_POX,
	PROJECTILE_TYPE_LAUNCH_FIRE,
	PROJECTILE_TYPE_LAUNCH_BALL,
	PROJECTILE_TYPE_LAUNCH_DUST,
	PROJECTILE_TYPE_LAUNCH_SNOW,
	PROJECTILE_TYPE_LAUNCH_BATS,
	PROJECTILE_TYPE_LAUNCH_RUBBERCHICKEN,

	PROJECTILE_TYPE_HOWMANY	/**** just to count and terminate ****/
};

#define PROJECTILE_FLAG_DEAD	0x00000001
#define PROJECTILE_FLAG_TRIPLE	0x00000002

class Projectile {
public:
	Projectile(int32 type);
	~Projectile();
	void Detonate(void);
	
	void Think(void);
	void Move(void);
	void React(void);
	void Draw(void);

	static Projectile *msFirst;
	static Projectile *Launch(int32 type, pfMatrix mat, Glider *owner);
	static void RemoveDead(void);
	static void ThinkAll(void);
	static void MoveAll(void);
	static void ReactAll(void);
	static void DrawAll(void);

	int32		mType;
	int32		mFlags;
	int32		mGeneration;
	float		mLifeTimer;
	float		mSpeed;
	pfMatrix	mMatrix;
	pfVec3		mVelocity;
	Glider		*mOwner;

	GlowSpark	*mGlowSpark;

	class Projectile	*mPrev;
	class Projectile	*mNext;
};

extern float	gTripleCrossbowTimer[2];
extern pfVec3	gTripleCrossbowPos[2];
extern float	gTripleSwordTimer[2];
extern pfVec3	gTripleSwordPos[2];
extern float	gTripleRepeaterTimer[2];
