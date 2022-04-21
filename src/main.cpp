
/**************************************************\
	Release 1 Completion List

	//Finish translation pics
	Boss shots harm player?
	Skate park!
	x  Detect or message for "Ordinal 11"?
	//Fix small texture button problem??
	//Spawn health sooner? Or invincibility timer?
	//Power link?
	//Fly into port
	//permanent score
	New characters!
	New boards!
	x  Translate!!
	x  Fix savegame tunnels active.
	x  Fix spinning character
	x  Fireworks!
	x  Beta thanks and credits
	x  More sounds?

\**************************************************/


/**************************************************\
	Beta 4 Completion List

	x   save/load!!!
	x   boss too small bug
	x  fix sword
	port redesign?
	x  super sword?
	x  super crossbow!
	x  super repeater?
	new textures?
	ballistic?
	particle accelerator? 
	boss launches affect player
\**************************************************/

/**************************************************\
	Beta 3 Completion List

	port redesign?
	X   port camera fix
	X   2 player weapon select?
	X   caps lock fix
	super sword?
	super repeater?
	ballistic?
	boss launches affect player
	X   Try Release Build
	X   stray keys
	X   out-of-ammo = simpleshot
	X   crossbow freeze?
	X   stop missile beeps
	X   new shields
	X   mac leak
	X   mac zip
\**************************************************/

/**************************************************\
Notes from Katherine's machine:

CPU: P4M 1.7 GHz 256MB
Video: S3 16MB

Problems noticed:
1. Very slow. <<<Stopped mipping, reduced txtr bit depth
2. Additive seems to malfunction <<<SRC_COLOR mode doesn't work!
3. Buttons stop showing up. <<<Stopped font projection mtx from being set
4. AFTER going to player screen and back, very slow <<<Stopped mipping, reduced txtr bit depth
5. Corner arrows are BLUE?

Things to try:
Removing background pic (fixed 1 in menus)

\**************************************************/

/**************************************************\

Two days till it goes live, completion list:

			-LEC Logo in Thanks
-Fix web links in game
-Change instructions to point at web
			-Mipping Option, window option, sound option, voice option

			-Remove 'of America' from the title screen
			-Fix nasty gfx bug
			-Fix thanks: Victoria, Katherine Wallen, ranch folks, Josh?

-VTune
-Write for web
-GLUT32 Note
-Spiffy Icon!

Bosses affect player
Turn shields off
Test built-in player images

Tornado spin?
Whirlwinds?
_____________________________________________


Night-before-launch 3-hour completion list

xx	MAW changes
xx	Vamp changes
xx	Tornado face
xx	EULA
xx	Thanks
	MAW web link
xx	Final keys
--	Options
	Instructions
--	boss-barf finish
--	Tornado spin

	Turn shields off
--	Make images built-in
	burn discs
\**************************************************/
/*********************************************************\
Completion List:

Priorities:
1 - Must have
2 - High
3 - Low
4 - Add after launch?

Difficulties:
A - Real programming
B - Casual work
C - No-brainer

Pri Dif Item
--------------------------------- 1   A  Choose player
 1   A  Bosses launch stuff
--------------------------------- 1   A  Real level layout and progression
 1   B  End-user license agreement
 1   A  Content for UI bubbles
 1   A  New weapons
 1   B  Wacky bubbles
--------------------------------- 1   B  Winner Congratulaions
xxxxxxxx 1   B  Final HUD
--------------------------------- 1   C  Backdrop
--------------------------------- 1   B  Sound Effects
--------------------------------- 1   B  New Player Images
--------------------------------- 1   B  Which tube leads where?
--------------------------------- 1   C  Shields should appear on screen in order of levels
--------------------------------- 1   C  Easy/Med/Hard screen too busy
--------------------------------- 1   B  Vampire color
--------------------------------- 1   C  Health should refill on each new level
--------------------------------- 1   C  Pop-up button text
 2   A  Pop-up button voice
 2   C  Tornado twisted face
--------------------------------- 1   C  Sounds in TSUs
********************************* 2   A  Control config
--------------------------------- 2   B  Fireflies (to finish intro screen)
--------------------------------- 2   B  2-player glow trails
 2   B  chia fins
 2   C  screen aspect fix
 2   C  Color change per level
 2   B  Allow Player Name Setting
 2   B  Allow Player Color Setting
 3   B  Permanent totals
 3   A  Music
--------------------------------- 3   B  Corner Indicator
--------------------------------- 3   C  bug:Tube squeeze

 4   A  Save/Load?
 4   B  Body Map?
 4   A  Network play?

\*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
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
#include "cellgrid.h"
#include "CellPort.h"
#include "glider.h"
#include "projectile.h"
#include "boss.h"
#include "glowSpark.h"
#include "bubble.h"
#include "slides.h"
#include "sound.h"
#include "font.h"
#include "hud.h"
#include "tsufile.h"
#include "translation.h"
#include "shadowbox.h"

char	    *main_window_title = TRANSLATE(TXT_Bens_Game);
uint64_t    main_sdl_window_flags = SDL_WINDOW_OPENGL;
SDL_Window* main_sdl_window = NULL;

static int32 CurrentMouseButtons = 0;
int32	MainWindowSize[2] = {640, 480};
bool	doneFlag = false;

void InitApplication(void)
{
	int	i;

	tsuInit();
	InitTranslation();
	InitAllTextures();
	InitCamera();
	initAllSounds();
	gPort = new CellPort();
	gCells = new CellGrid(200, 200);
	gBoss = new Boss();
	gGliders[0] = new Glider();
	gGliders[0]->mPlayerNum = 0;
	gGliders[1] = new Glider();
	gGliders[1]->mPlayerNum = 1;
	gSlides = new Slides();
	gFontSys = new FontSystem();
	gFontSys->LoadAll();
	gFireFlies = new MiniSparks(MINISPARK_TYPE_FIREFLY, 200);
	for (i = 0; i < NUM_FIREWORKS; i++) {
		gFireworks[i] = new MiniSparks(MINISPARK_TYPE_FIREWORKS, 50);
	}
	SetLanguageByName(gOptionLanguage);

}

void ShutdownApplication(int exitError)
{
	SDL_DestroyWindow(main_sdl_window);
	SDL_Quit();
	exit( exitError);	
}


void HandleIdle(void)
{
	Glider	*g;
	int		i;
	bool	wonAll;

	UpdateDeltaTime();

	for (i = 0; i < 255; i++) {
		CheckKeyDiff(i);
	}

	if (gGameMode == GAME_MODE_SLIDES) {
		gPaused = false;
		gSlides->Think();
		gFireFlies->Think();
		gFireFlies->Move();
		gFireFlies->React();
		Bubble::RemoveDead();
	} if (gGameMode == GAME_MODE_INTRO) {
		gPaused = false;
		gPort->Think();
		MoveCamera();
		gPort->IntroCamera();
		Bubble::RemoveDead();
	} else if (gGameMode == GAME_MODE_PORT) {
		gPort->Think();
		for (g = gActiveGliderList; g; g = g->mNext) g->Think();
		for (g = gActiveGliderList; g; g = g->mNext) g->Move();
		for (g = gActiveGliderList; g; g = g->mNext) g->React();
		MoveCamera();
		gPort->HandleZoom();
		GliderCam();
		Bubble::RemoveDead();
	} else {
		gCells->Think();
		gBoss->Think();
		for (g = gActiveGliderList; g; g = g->mNext) g->Think();
		Projectile::ThinkAll();
		GlowSpark::ThinkAll();
		Bubble::ThinkAll();

		gBoss->Move();
		for (g = gActiveGliderList; g; g = g->mNext) g->Move();
		Projectile::MoveAll();
		GlowSpark::MoveAll();
		Bubble::MoveAll();

		gBoss->React();
		for (g = gActiveGliderList; g; g = g->mNext) g->React();
		Projectile::ReactAll();
		GlowSpark::ReactAll();
		Bubble::ReactAll();

		MoveCamera();
		GliderCam();
		Projectile::RemoveDead();
		Bubble::RemoveDead();

		if (gWinTimer > 0.0f) {
			gWinTimer -= DeltaTime;
			if (gWinTimer <= 0.0f) {
				gCells->Clear();
				for (g = gActiveGliderList; g; g = g->mNext) {
					pfSetVec3(g->mMatrix[PF_T], 0, 0, 0);
					pfSetVec3(g->mVelocity, 0, 0, 0);
					pfCopyMat(g->mLastMatrix, g->mMatrix);
					g->mTrailNext = -1;
				}
				wonAll = true;
				for (i = 0; wonAll && i < SHIELD_HOWMANY; i++) {
					if (!gLevels[i].mComplete) {
						wonAll = false;
					}
				}
				if (wonAll) {
					gGameMode = GAME_MODE_SLIDES;
					gSlides->mCurrentSlide = SLIDE_YOU_WIN;
					gCells->mNumWalls = 0;
					gSave.slots[0].numPlayers = 0;
					WriteSaves();
				} else {
					gGameMode = GAME_MODE_PORT;
					gCells->mNumWalls = 0;
					GameToSaveSlot();
					WriteSaves();
				}
			}
		}
	}

	if (doneFlag) 
	{
		doneFlag = false;
		ShutdownApplication( 0 );
		// exit(0);
	}
}

void mainReshape(int width, int height)
{
	MainWindowSize[0] = width;
	MainWindowSize[1] = height;
}

void mainKeyboardPress(unsigned char key, int x, int y)
{
	ControlPressKey(key, TRUE);
}

void mainKeyboardRelease(unsigned char key, int x, int y)
{
	ControlPressKey(key, FALSE);
}


void mainSpecialPress(int key, int x, int y)
{
	switch (key) {
//	  case 0x1b: mainShutDown(); break; /**** esc to quit ****/
	case SDLK_LEFT:
		ControlPressKey(0xe2, TRUE); break;

	case SDLK_RIGHT:
		ControlPressKey(0xe1, TRUE); break;

	case SDLK_UP:
		ControlPressKey(0xe4, TRUE); break;

	case SDLK_DOWN:
		ControlPressKey(0xe3, TRUE); break;

//	  default: ControlPressKey(key+100, TRUE); break;
	default:
		printf("special key: %d\n", key);
		break;
	}
}

void mainSpecialRelease(int key, int x, int y)
{
	switch (key) {
	case SDLK_LEFT:
		ControlPressKey(0xe2, FALSE); break;

	case SDLK_RIGHT:
		ControlPressKey(0xe1, FALSE); break;

	case SDLK_UP:
		ControlPressKey(0xe4, FALSE); break;

	case SDLK_DOWN:
		ControlPressKey(0xe3, FALSE); break;

//	  default: ControlPressKey(key+100, TRUE); break;
	default: break;
	}
}

void mainMouseClick(int button, int state, int x, int y)
{
	HandleMouse(button, state, x, y);
}

void mainMouseMotion(int x, int y)
{
	HandleMouseMotion(x, y);
}

void mainJoystick(unsigned int buttonMask, int x, int y, int z)
{
	int xx,yy,zz,rr;
	float xxx,yyy,zzz,rrr;
	int	bb;


//#ifdef WIN32
//	_asm {
//		mov         eax,dword ptr [ebp]
//		mov         ecx,dword ptr [eax-2ch]
//		mov         dword ptr [xx], ecx
//		mov         ecx,dword ptr [eax-28h]
//		mov         dword ptr [yy], ecx
//		mov         ecx,dword ptr [eax-24h]
//		mov         dword ptr [zz], ecx
//		mov         ecx,dword ptr [eax-20h]
//		mov         dword ptr [rr], ecx
//	}
//#else
	xx = x;
	yy = y;
	zz = 0;
	rr = 0;
//#endif


#define SCALE(v)  ((float) ((v - 32767)/32768.0f))

	xxx=SCALE(xx);
	yyy=SCALE(yy);
	zzz=SCALE(zz);
	rrr=SCALE(rr);
buttonMask=0;
xxx=0;
yyy=0;
zzz=0; //fix a joystick problem 
rrr=0;
//	if (buttonMask != 0x00) printf("AAAA: %x [%d %d %d] [%f %f %f %f]\n",buttonMask,x,y,z,xxx,yyy,zzz,rrr);
//	if (buttonMask != 0x00) printf("BBBB: %x\n",buttonMask);

	ControlJoystickData(buttonMask,xxx,yyy,zzz,rrr);
}

void main_event_loop()
{

// 	glutSetWindow(MainWindowID);
// 	glutIgnoreKeyRepeat(GLUT_DEVICE_IGNORE_KEY_REPEAT);
// 	glutIdleFunc(HandleIdle);
// 	glutDisplayFunc(DrawMainWindow);
// 	glutKeyboardFunc(mainKeyboardPress);
// 	glutKeyboardUpFunc(mainKeyboardRelease);
// 	glutSpecialFunc(mainSpecialPress);
// 	glutSpecialUpFunc(mainSpecialRelease);
// 	glutMouseFunc(mainMouseClick);
// 	glutMotionFunc(mainMouseMotion);
// 	glutPassiveMotionFunc(mainMouseMotion);

// #ifdef WIN32
// 	glutJoystickFunc(mainJoystick, -1);
// #endif

// 	glutReshapeFunc(mainReshape);
// 	glutShowWindow();


  bool done = false;
  while (!done)
  {
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
      if (Event.type == SDL_KEYDOWN)
      {
      	int x = 0;
      	int y = 0;
      	switch (Event.key.keysym.sym)
      	{
				case SDLK_LEFT:  ControlPressKey(0xe2, TRUE); break;
				case SDLK_RIGHT: ControlPressKey(0xe1, TRUE); break;
				case SDLK_UP:    ControlPressKey(0xe4, TRUE); break;
				case SDLK_DOWN:  ControlPressKey(0xe3, TRUE); break;
				default:         mainKeyboardPress(Event.key.keysym.sym, x, y); break;
				}
      }
      else if (Event.type == SDL_KEYUP)
      {
      	int x = 0;
      	int y = 0;
      	switch (Event.key.keysym.sym)
      	{
				case SDLK_LEFT:  ControlPressKey(0xe2, FALSE); break;
				case SDLK_RIGHT: ControlPressKey(0xe1, FALSE); break;
				case SDLK_UP:    ControlPressKey(0xe4, FALSE); break;
				case SDLK_DOWN:  ControlPressKey(0xe3, FALSE); break;
				default:         mainKeyboardRelease(Event.key.keysym.sym, x, y); break;
				}
      }
      else if (Event.type == SDL_MOUSEBUTTONDOWN)
      {
        mainMouseClick(Event.button.button, 1, Event.button.x, Event.button.y);
      }
      else if (Event.type == SDL_MOUSEBUTTONUP)
      {
        mainMouseClick(Event.button.button, 0, Event.button.x, Event.button.y);
      }
      else if (Event.type == SDL_MOUSEMOTION)
      {
        mainMouseMotion(Event.motion.x, Event.motion.y);
      }
	    else if (Event.type == SDL_WINDOWEVENT)
	    {
	        if (Event.window.event == SDL_WINDOWEVENT_RESIZED
	        		|| Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	        {
	        		mainReshape(Event.window.data1, Event.window.data2);
	        }
	    }
      else if (Event.type == SDL_QUIT)
      {
        done = true;
      }
    }
//printf("time: %f\n", NowTime);
    if (do_shadowbox_quilt)
    {
    	float dt = DeltaTime;
    	float udt = UnscaledDeltaTime;
    	for (int v = 0; v < shadowbox_tiles_y; ++v)
    	{
	    	for (int h = 0; h < shadowbox_tiles_x; ++h)
	    	{
	    		DrawMainWindow(h, v);
	    		DeltaTime = 0.0f;
	    		UnscaledDeltaTime = 0.0f;
	    	}
	    }
  		DeltaTime = dt;
  		UnscaledDeltaTime = udt;
    }
  	else
  	{
			DrawMainWindow(0, 0);
		}
		HandleIdle();

    // glViewport(0, 0, WinWidth, WinHeight);
    // glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    // glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    // glBegin(GL_TRIANGLES);
    // glVertex3f(-1.0f, -1.0f, 0.0f);
    // glVertex3f(-1.0f,  1.0f, 0.0f);
    // glVertex3f( 1.0f,  1.0f, 0.0f);
    // glEnd();

    SDL_GL_SwapWindow(main_sdl_window);
  }
}

int main(int argc, char **argv)
{
#if 0
//#ifdef WIN32
	checkDirectXVersion();
#endif

    if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    IMG_Init(IMG_INIT_JPG);

	main_sdl_window = SDL_CreateWindow(main_window_title, 0, 0, MainWindowSize[0], MainWindowSize[1], main_sdl_window_flags);
    if (!main_sdl_window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return 1;
    }
	SDL_GLContext Context = SDL_GL_CreateContext(main_sdl_window);

	LoadOptions();
	SaveOptions();
	ReadSaves();
	WriteSaves();
	if (!gOptionLaunchInWindow) {
		SDL_SetWindowFullscreen(main_sdl_window, main_sdl_window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	//SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	InitApplication();
	main_event_loop();

	glFlush();
	glFinish();
	SDL_DestroyWindow(main_sdl_window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}

