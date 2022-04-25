
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
#include "cellgrid.h"
#include "cellport.h"
#include "glider.h"
#include "projectile.h"
#include "hud.h"
#include "boss.h"
#include "glowSpark.h"
#include "bubble.h"
#include "slides.h"
#include "random.h"
#include "sound.h"
#include "shadowbox.h"

extern SDL_Window* main_sdl_window;

void drawFloorSquare(float x, float y, float z, float size)
{
    glBegin(GL_LINE_LOOP);
    glVertex3f(x - (size * 0.5f), y - (size * 0.5f), z);
	glVertex3f(x + (size * 0.5f), y - (size * 0.5f), z);
    glVertex3f(x + (size * 0.5f), y + (size * 0.5f), z);
	glVertex3f(x - (size * 0.5f), y + (size * 0.5f), z);
    glEnd();
}

void drawFloor(void)
{
	long	i, j;
	
	glDisable( GL_BLEND );
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	for (i = -5; i <= 5; i++) {
		for (j = -5; j <= 5; j++) {
			drawFloorSquare(i*2.0f, j*2.0f, -1.0f, 1.0f);
		}
	}
	return;
}

/* 

   general purpose text routine.  draws a string according to the
   format in a stroke font at x, y after scaling it by the scale
   specified.  x, y and scale are all in window-space [i.e., pixels]
   with origin at the lower-left.

 */

void AppDrawText(GLuint x, GLuint y, GLuint scale, char* format, ...)
{
    va_list args;
    char buffer[255], *p;
    GLfloat font_scale = 119.05F + 33.33F;
	int	win_width, win_height;
	
	va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

	SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, win_width, 0, win_height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
   	glDisable(GL_BLEND);
    glTranslatef((GLfloat) x, (GLfloat) y, 0.0F);

    glScalef(scale/font_scale, scale/font_scale, scale/font_scale);

#if 0
@@ convert to SDL? Is this used?
    for(p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
#endif

    glPopAttrib();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void PreDraw(int tile_h, int tile_v)
{
	float	aspect;
	extern int32	MainWindowSize[2];

//	glClearColor( 0.0F, 0.0F, 0.2F, 1.0F );
	glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );

	if (do_shadowbox_quilt)
	{
		if (tile_h == 0 && tile_v == 0)
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}
		float w = shadowbox_tile_size_x;
		float h = shadowbox_tile_size_y;
		glViewport(w * tile_h, h * tile_v, w, h);
	}
	else
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glViewport(0, 0, MainWindowSize[0], MainWindowSize[1]);
	}
	/* set viewing projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspect = ((float)MainWindowSize[1]) / ((float)MainWindowSize[0]);
	glFrustum(-0.5f, 0.5f, -0.5f*aspect, 0.5f*aspect, 1.0f, 1000.0f);

	/* position viewer */
	SetGLViewToCamera();

	glEnable(GL_DEPTH_TEST);

//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

void DrawQuickModel(quickModel *qm)
{
	int32		prim;
	pfMatrix	sm;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	pfMakeScaleMat(sm, qm->scale[PF_X], qm->scale[PF_Y], qm->scale[PF_Z]);
	glMultMatrixf((float*)qm->matrix);
	glMultMatrixf((float*)sm);
	
	glVertexPointer (3, GL_FLOAT, 12, qm->vList);
	glColorPointer (4, GL_FLOAT, 16, qm->cList);
	glNormalPointer (GL_FLOAT, 12, qm->nList);
	glTexCoordPointer (2, GL_FLOAT, 8, qm->tList);
			
	glEnableClientState(GL_VERTEX_ARRAY);
	if (qm->nList) glEnableClientState(GL_NORMAL_ARRAY);
	else glDisableClientState(GL_NORMAL_ARRAY);
	if (qm->cList) glEnableClientState(GL_COLOR_ARRAY);
	else glDisableClientState(GL_COLOR_ARRAY);
	if (qm->tList) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	else glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	prim = GL_LINE_STRIP;
	glDrawElements (prim, qm->indexNum, GL_UNSIGNED_SHORT, qm->iList);

	glPopMatrix();
}


void DrawChiaQuad(pfVec3 *verts, pfVec3 extrusion, int layers, int numFins, float alphaScale, uint32 flags)
{
	pfVec3	localVerts[4], step;
	int		i, j;
	float	t;

	static bool		fireInit = false;
	static float	fireSpeed[256];
	static float	fireScale = 2.5f;
	static float	fireT[256];
	static float	fireX[256];
	static float	fireY[256];
	static float	fireAlphaBase[256];
	static float	fireAlpha[256];

	if (!fireInit) {
		fireInit = true;
		for (i = 0; i < 256; i++) {
			fireT[i] = RANDOM_IN_RANGE(-1.0f, 1.0f);
			fireSpeed[i] = RANDOM_IN_RANGE(1.0f, 2.0f);
			fireAlpha[i] = 0.0f;
			fireAlphaBase[i] = 1.0f;
			fireX[i] = RANDOM_IN_RANGE(-1.0f, 1.0f);
			fireY[i] = RANDOM_IN_RANGE(-1.0f, 1.0f);
		}
	}

	pfScaleVec3(step, 1.0f / layers, extrusion);

	for (i = 0; i < 4; i++) {
		pfAddVec3(localVerts[i], verts[i], extrusion);
	}

	if (layers > 1) glAlphaFunc(GL_GREATER, 0.5f);
	else glAlphaFunc(GL_GREATER, 0.0f);
//	glDepthMask(true);
	glBegin(GL_QUADS);

	/**** draw the diagonals ****/
	if ((flags & CHIA_FLAG_DIAGONALS)) {
		glTexCoord2f(0, 0);
		glVertex3fv(verts[0]);
		glTexCoord2f(0, 1);
		glVertex3fv(verts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(localVerts[2]);
		glTexCoord2f(1, 0);
		glVertex3fv(localVerts[3]);
		glTexCoord2f(0, 0);
		glVertex3fv(localVerts[0]);
		glTexCoord2f(0, 1);
		glVertex3fv(localVerts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(verts[2]);
		glTexCoord2f(1, 0);
		glVertex3fv(verts[3]);
	}

	/**** draw the fins ****/
	for (i = 0; i < numFins; i++) {
		t = (float)i+1/(float)(numFins + 1);
		pfCombineVec3(localVerts[0], t, verts[1], 1.0f-t, verts[2]);
		pfAddScaledVec3(localVerts[1], localVerts[0], 1.0f, extrusion);
		pfCombineVec3(localVerts[2], t, verts[3], 1.0f-t, verts[0]);
		pfAddScaledVec3(localVerts[3], localVerts[3], 1.0f, extrusion);

		glTexCoord2f(0, 0);
		glVertex3fv(localVerts[0]);
		glTexCoord2f(1, 0);
		glVertex3fv(localVerts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(verts[2]);
		glTexCoord2f(0, 1);
		glVertex3fv(verts[3]);
	}
	glEnd();

	for (i = 0; i < 4; i++) {
		pfCopyVec3(localVerts[i], verts[i]);
	}

	if (flags & CHIA_FLAG_FIRE_EFFECT) {
		for (i = 0; i < layers; i++) {
			fireT[i] += fireSpeed[i] * DeltaTime;
			if (fireT[i] > 1.0f) {
				fireT[i] = -1.0f;
				fireSpeed[i] = RANDOM_IN_RANGE(1.0f, 2.0f);
				fireAlphaBase[i] = RANDOM_IN_RANGE(0.5f, 1.0f);
				fireX[i] = RANDOM_IN_RANGE(-1.0f, 1.0f);
				fireY[i] = RANDOM_IN_RANGE(-1.0f, 1.0f);
			}
			fireAlpha[i] = 1.0f - PF_ABS(fireT[i]);
			if (fireAlpha[i] < 0.0f) fireAlpha[i] = 0.0f;
			fireAlpha[i] *= fireAlpha[i];
		}
	}

//	if (flags & CHIA_FLAG_REVERSE_ORDER) {
//		for (j = 0; j < 4; j++) {
//			pfAddScaledVec3(localVerts[j], localVerts[j], (layers-1), step);
//		}
//		pfScaleVec3(step, -1.0f, step);
//	}

	/**** draw the layers ****/
	for (i = 0; i < layers; i++) {
		if ((flags & CHIA_FLAG_TAPER_ALPHA) && (layers > 1)) {
			float	af = (float)i/(float)(layers-1);
			
			if (flags & CHIA_FLAG_REVERSE_ORDER) {
				af = 1.0f - af;
			}
			af *= alphaScale;
			glAlphaFunc(GL_GREATER, af);
		}
		if (flags & CHIA_FLAG_FIRE_EFFECT) {
			glColor4f(1.0f, 1.0f, 1.0f, fireAlpha[i] * fireAlphaBase[i]);
			for (j = 0; j < 4; j++) {
				localVerts[j][PF_X] += fireScale * 0.5f * fireX[i];
				localVerts[j][PF_Y] += fireScale * 0.5f * fireY[i];
				localVerts[j][PF_Z] += fireScale * fireT[i];
			}
		}
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3fv(localVerts[0]);
		glTexCoord2f(0, 1);
		glVertex3fv(localVerts[1]);
		glTexCoord2f(1, 1);
		glVertex3fv(localVerts[2]);
		glTexCoord2f(1, 0);
		glVertex3fv(localVerts[3]);
		for (j = 0; j < 4; j++) {
			if (flags & CHIA_FLAG_FIRE_EFFECT) {
				localVerts[j][PF_X] -= fireScale * 0.5f * fireX[i];
				localVerts[j][PF_Y] -= fireScale * 0.5f * fireY[i];
				localVerts[j][PF_Z] -= fireScale * fireT[i];
			}
			pfAddVec3(localVerts[j], localVerts[j], step);
		}
		glEnd();
	}
//	glDepthMask(true);
	glAlphaFunc(GL_GREATER, 0.0f);
}

void PostDraw(void)
{
//	AppDrawText(100, 100, 20, "%.0f Hz", (float)LastFrameRate);
}

void DrawMainWindow(int tile_h, int tile_v)
{
	Glider	*g;
	PreDraw(tile_h, tile_v);

	if (gGameMode == GAME_MODE_SLIDES) {
		gSlides->Draw();
	} else if (gGameMode == GAME_MODE_INTRO) {
		gPort->Draw();
		for (g = gActiveGliderList; g; g = g->mNext) g->Draw();
	} else if (gGameMode == GAME_MODE_PORT) {
		gPort->Draw();
		for (g = gActiveGliderList; g; g = g->mNext) g->Draw();
		hudDraw();
	} else {
		gCells->DrawBackdrop();
		gCells->DrawHalfPipe();
		gCells->Draw();
		gBoss->Draw();
		for (g = gActiveGliderList; g; g = g->mNext) g->Draw();
		gCells->DrawWalls();
		Bubble::DrawAll();
		Projectile::DrawAll();
		GlowSpark::DrawAll();
		hudDraw();

		/*gFireFlies->mLifeTimer = 1.0f;
		pfSetVec3(gFireFlies->mCenter, 0, 0, 0);
		gFireFlies->mSpread = 50.0f;
		gFireFlies->Think();
		gFireFlies->Move();
		gFireFlies->React();
		gFireFlies->Draw();*/
	}

	PostDraw();
	if (do_shadowbox_quilt)
	{
		glFinish();
		glFlush();
		// if (tile_h == shadowbox_tiles_x - 1
		// 	&& tile_v == shadowbox_tiles_y - 1)
		// 	SDL_GL_SwapWindow(main_sdl_window);
	}
	else
	{
		SDL_GL_SwapWindow(main_sdl_window);
	}
}

