
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <stdarg.h>
#include <time.h>
#include "genincludes.h"
#include "camera.h"
#include "timer.h"
#include "guideway.h"
#include "texture.h"
#include "draw.h"
#include "controls.h"
#include "sound.h"
#include "cellgrid.h"
#include "cellport.h"
#include "glider.h"
#include "projectile.h"
#include "hud.h"
#include "slides.h"
#include "font.h"
#include "translation.h"
#include "shadowbox.h"

char	KeysDown[256];
char	PrevKeysDown[256];
char	KeysPressed[256];
char	KeysReleased[256];

int32	sNumJoysticks = 0;		// how many are connected
Boolean GotJoystickData;
float	JoystickVals[4];
uint32	JoystickButtons = 0;
uint32	JoystickButtonsPrevious = 0;

void CheckKeyDiff(int key)
{
	KeysPressed[key] = false;
	KeysReleased[key] = false;
	if (PrevKeysDown[key] != KeysDown[key]) {
		if (KeysDown[key]) {
			KeysPressed[key] = true;
		} else {
			KeysReleased[key] = true;
		}
		PrevKeysDown[key] = KeysDown[key];
	}
}

void ControlPressKey(int key, bool down)
{
	int		i;
	bool	done = FALSE;
	char	*unshiftTable = "`1234567890-=qwertyuiop[]asdfghjkl;zxcvbnm,./'\\ ~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\"|";
	char	*shiftTable   = "~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\"| `1234567890-=qwertyuiop[]asdfghjkl;zxcvbnm,./'\\";
	/**** only one-byte keys for now ****/
	key &= 0x00ff;

	if (down) {
		HandleKeyboard(key, 0, 0);
	}

	KeysDown[key] = down;

	/**** If a key gets released, release the shifted version too! ****/
	if (1 || !down) {	/**** always do this, to account for caps lock ****/
		i = 0;
		while (unshiftTable[i] != 0 && unshiftTable[i] != key) i++;
		if (unshiftTable[i] != 0) {
			KeysDown[shiftTable[i]] = down;
		}
	}
}

void HandleKeyboard(unsigned char key, int x, int y)
{
	Glider	*g;
//	printf("key %d\n", key);
	switch (key) {
	case 27:	/**** esc ****/
		if (gGameMode == GAME_MODE_PLAY || gGameMode == GAME_MODE_PORT) {
			gPaused = !gPaused;
		}
		break;
#if 0
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		FindLanguages();
		TextSetLanguage(key - '0');
		FullSetLanguage();
		break;
#endif
//	case '1':
//		if (gGameMode == GAME_MODE_SLIDES) {
//			gSlides->NewGame(1);
//		}
//		break;
//	case '2':
//		if (gGameMode == GAME_MODE_SLIDES) {
//			gSlides->NewGame(2);
//		}
//		break;
	case '\\':
		toggle_shadowbox();
		break;
	case 'C':
		if (0) {
			if (gGameMode != GAME_MODE_SLIDES) {
				gLevels[gCurrentLevel].mComplete = true;
				gCells->Clear();
				for (g = gActiveGliderList; g; g = g->mNext) {
					pfSetVec3(g->mMatrix[PF_T], 0, 0, 0);
					pfSetVec3(g->mVelocity, 0, 0, 0);
					pfCopyMat(g->mLastMatrix, g->mMatrix);
					g->mTrailNext = -1;
				}
				gGameMode = GAME_MODE_PORT;
				gCells->mNumWalls = 0;
			}
		}
		break;
//	case 'W':
//		gGameMode = GAME_MODE_SLIDES;
//		gCells->mNumWalls = 0;
//		gSlides->mCurrentSlide = SLIDE_YOU_WIN;
//		break;
//	case 'm': gCells->Mutate(); break;
//	case 'c': 
//		if (gChipmunkFactor > 1.1f) gChipmunkFactor = 1.0f;
//		else gChipmunkFactor = 1.5f;
//		break;
//	case '-':
//		gChipmunkFactor -= 0.05f;
//		if (gChipmunkFactor < 0.05f) gChipmunkFactor = 0.05f;
//		printf("Chipmunk factor: %.2f\n", gChipmunkFactor);
//		break;
//	case '=':
//		gChipmunkFactor += 0.05f;
//		if (gChipmunkFactor > 4.0f) gChipmunkFactor = 4.0f;
//		printf("Chipmunk factor: %.2f\n", gChipmunkFactor);
//		break;
//	case 'R':
//		ReloadAllTextures();
//		gFontSys->LoadAll();
//		break;
	default: break;
	}

//	for (g = gActiveGliderList; g; g = g->mNext) {
//		if (g->mPlayerNum == 0) {
//			switch (key) {
//			case '4': g->mCurrentWeapon[0] = PROJECTILE_TYPE_BLASTER; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '5': g->mCurrentWeapon[0] = PROJECTILE_TYPE_SLINGSHOT; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '6': g->mCurrentWeapon[0] = PROJECTILE_TYPE_SWORD; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '7': g->mCurrentWeapon[0] = PROJECTILE_TYPE_CROSSBOW; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '8': g->mCurrentWeapon[1] = PROJECTILE_TYPE_MISSILE; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '9': g->mCurrentWeapon[1] = PROJECTILE_TYPE_BALLISTIC; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			case '0': g->mCurrentWeapon[1] = PROJECTILE_TYPE_PARTICLE_ACCELERATOR; gHealthLevel[HEALTH_TYPE_AMMO] = 1.0f; break;
//			default:
//				break;
//			}
//		}
//	}
}

void ControlJoystickData(uint32 buttons, float x, float y, float z, float r)
{
	uint32 TButtons;

	GotJoystickData=1;
	JoystickVals[0]=x;
	JoystickVals[1]=y;
	JoystickVals[2]=z;
	JoystickVals[3]=r;

	/* JoystickVals need to be -1 to 1 */

	/* And do button mapping */
//	TButtons=~buttons;
//
//	JoystickCData= (((TButtons & 0x0000ff00) >> 8) | ((TButtons & 0x000000f0)<<4) | ((TButtons & 0x0000000f)<<12)) << 16;

	JoystickButtonsPrevious = JoystickButtons;
	JoystickButtons = buttons;
//	if (buttons != 0) printf("Joystick: %f %f %x\n",JoystickVals[0],JoystickVals[1],buttons);
}

long	MouseDownLoc[2] = {0,0};
long	MouseWhichButtonDown = -1;
pfVec3	CameraSwingVeloc = {0.0f, 0.0f, 0.0f};

void HandleMouse(int button, int state, int x, int y)
{
	if (state == 1) {
		if (gGameMode == GAME_MODE_SLIDES) {
			gSlides->Click(x, y);
		} else if (gGameMode == GAME_MODE_PLAY || gGameMode == GAME_MODE_PORT) {
			if (gPaused) {
				gSlides->Click(x, y);
			} else {
				gPaused = true;
			}
		}


		if (MouseWhichButtonDown != -1) return;
		MouseDownLoc[0] = x;
		MouseDownLoc[1] = y;
		MouseWhichButtonDown = button;
	}
	else {
		MouseWhichButtonDown = -1;
	}
}

void HandleMouseMotion(int x, int y)
{
	if (gGameMode == GAME_MODE_SLIDES) {
		gSlides->MousePos(x, y);
	} else if (gPaused) {
		gSlides->MousePos(x, y);
	}
}

