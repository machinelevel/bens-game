


/************************************************************\
	hud.h
	Functions for drawing the HUD in Ben's project
\************************************************************/

enum {
	HEALTH_TYPE_HEALTH,
	HEALTH_TYPE_AMMO,
	HEALTH_TYPE_ATTITUDE,

	HEALTH_TYPE_HOWMANY
};

const int	gHealthIcons[] = {
	TEXID_ICON_HEALTH,
	TEXID_ICON_AMMO,
	TEXID_ICON_ATTITUDE,
};

const pfVec4	healthFillColor[] = {
	{ 1.0, 0.3, 0.0, 0.3 },
	{ 0.0, 1.0, 0.6, 0.3 },
	{ 0.8, 1.0, 0.0, 0.3 },
};

extern float	gHealthLevel[HEALTH_TYPE_HOWMANY];
extern float	gScreenFlashSpeed;
extern float	gScreenFlashTimer;
extern pfVec4	gScreenFlashColor;
extern bool		gPaused;
extern float	gPauseFader;

typedef struct Shield {
	int32	icon;
	bool	have;
} Shield;

extern Shield	gShields[7];
extern int		gTripleWeaponTexIDs[2][3];
extern float	gTripleWeaponShift[2];
extern bool		gTripleWeaponReady[2];

void hudDraw(void);
void hudQuickRect(float x, float y, float w, float h);
void getShield(int texID);
void hudSwapTripleWeapon(Glider *player);
void hudGetTripleWeapon(Glider *player, int texID);

