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

#define SHADOWBOX_OFFSCREEN

extern SDL_Window* main_sdl_window;

bool do_shadowbox_quilt = false;
int  shadowbox_tiles_x = 1;
int  shadowbox_tiles_y = 1;
int  shadowbox_tile_size_x = 0;
int  shadowbox_tile_size_y = 0;
bool shadowbox_initialized = false;
GLuint shadowbox_quilt_framebuffer = 0;
GLuint shadowbox_quilt_to_screen_shader = 0;
GLuint shadowbox_quilt_tex_id = 0;
GLuint shadowbox_quilt_quad_vbo = 0;

static PFNGLUSEPROGRAMPROC        glUseProgram       ;
static PFNGLCREATESHADERPROC      glCreateShader     ;
static PFNGLSHADERSOURCEPROC      glShaderSource     ;
static PFNGLCOMPILESHADERPROC     glCompileShader    ;
static PFNGLGETSHADERIVPROC       glGetShaderiv      ;
static PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog ;
static PFNGLATTACHSHADERPROC      glAttachShader     ;
static PFNGLCREATEPROGRAMPROC     glCreateProgram    ;
static PFNGLLINKPROGRAMPROC       glLinkProgram      ;
static PFNGLGETPROGRAMIVPROC      glGetProgramiv     ;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLVALIDATEPROGRAMPROC   glValidateProgram  ;
static PFNGLBINDBUFFERPROC        glBindBuffer       ;
static PFNGLGENBUFFERSPROC        glGenBuffers       ;
static PFNGLBUFFERDATAPROC        glBufferData       ;
static PFNGLUNIFORM1IPROC         glUniform1i        ;
static PFNGLUNIFORM4FPROC         glUniform4f        ;
static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation      ;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray ;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray ;
static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer     ;
static PFNGLGETATTRIBLOCATIONPROC     glGetAttribLocation     ;
static PFNGLTEXSTORAGE2DPROC     glTexStorage2D     ;
static PFNGLBINDFRAMEBUFFERPROC     glBindFramebuffer     ;
static PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture     ;
static PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D     ;
static PFNGLDRAWBUFFERSPROC     glDrawBuffers     ;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC     glCheckFramebufferStatus     ;
static PFNGLGENFRAMEBUFFERSPROC     glGenFramebuffers     ;
static PFNGLGENRENDERBUFFERSPROC           glGenRenderbuffers           ;
static PFNGLBINDRENDERBUFFERPROC           glBindRenderbuffer           ;
static PFNGLRENDERBUFFERSTORAGEPROC        glRenderbufferStorage         ;
static PFNGLFRAMEBUFFERRENDERBUFFERPROC     glFramebufferRenderbuffer     ;

static SDL_Rect restore_window_bounds;
static SDL_Rect shadowbox_window_bounds;

void toggle_shadowbox()
{
    SDL_Window* win = SDL_GL_GetCurrentWindow();
	do_shadowbox_quilt = !do_shadowbox_quilt;
	if (do_shadowbox_quilt)
	{
        int x = 0, y = 0, w = 100, h = 100;
        SDL_GetWindowPosition(win, &x, &y);
        SDL_GetWindowSize(win, &w, &h);
        restore_window_bounds.x = x;
        restore_window_bounds.y = y;
        restore_window_bounds.w = w;
        restore_window_bounds.h = h;

        int num_displays = SDL_GetNumVideoDisplays();
        SDL_Rect bounds;
        int chosen_screen = 0;
        // stretch it to the correct aspect ratio
        h = (w / 1536.0f) * 2048.0f;
        for (int i = 0; i < num_displays; i++)
        {
            SDL_GetDisplayBounds(i, &bounds);
            printf("[shadowbox]  display %d is %dx%d at (%d,%d)\n", i, bounds.w, bounds.h, bounds.x, bounds.y);
            if (bounds.w == 1536 && bounds.h == 2048)
            {
                chosen_screen = i;
                x = bounds.x;
                y = bounds.y;
                w = bounds.w;
                h = bounds.h;
            }
        }
        printf("[shadowbox] Chosen screen: %d\n", chosen_screen);
        shadowbox_window_bounds.x = x;
        shadowbox_window_bounds.y = y;
        shadowbox_window_bounds.w = w;
        shadowbox_window_bounds.h = h;
        SDL_SetWindowBordered(win, SDL_FALSE);
        SDL_SetWindowPosition(win, shadowbox_window_bounds.x, shadowbox_window_bounds.y);
        SDL_SetWindowSize(win, shadowbox_window_bounds.w, shadowbox_window_bounds.h);
        SDL_GetWindowSize(win, &w, &h);
        glViewport(0, 0, w, h);


        printf("Activating shadowbox mode.\n");
		shadowbox_tiles_x = 9;
		shadowbox_tiles_y = 5;
		shadowbox_tile_size_x = 512;
		shadowbox_tile_size_y = 640;

		if (!shadowbox_initialized)
		{
			shadowbox_initialized = true;
            glUseProgram        = (PFNGLUSEPROGRAMPROC)       SDL_GL_GetProcAddress("glUseProgram");
            glCreateShader      = (PFNGLCREATESHADERPROC)     SDL_GL_GetProcAddress("glCreateShader");
            glShaderSource      = (PFNGLSHADERSOURCEPROC)     SDL_GL_GetProcAddress("glShaderSource");
            glCompileShader     = (PFNGLCOMPILESHADERPROC)    SDL_GL_GetProcAddress("glCompileShader");
            glGetShaderiv       = (PFNGLGETSHADERIVPROC)      SDL_GL_GetProcAddress("glGetShaderiv");
            glGetShaderInfoLog  = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog");
            glAttachShader      = (PFNGLATTACHSHADERPROC)     SDL_GL_GetProcAddress("glAttachShader");
            glCreateProgram     = (PFNGLCREATEPROGRAMPROC)    SDL_GL_GetProcAddress("glCreateProgram");
            glLinkProgram       = (PFNGLLINKPROGRAMPROC)      SDL_GL_GetProcAddress("glLinkProgram");
            glGetProgramiv      = (PFNGLGETPROGRAMIVPROC)     SDL_GL_GetProcAddress("glGetProgramiv");
            glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
            glValidateProgram   = (PFNGLVALIDATEPROGRAMPROC)  SDL_GL_GetProcAddress("glValidateProgram");
            glBindBuffer        = (PFNGLBINDBUFFERPROC)       SDL_GL_GetProcAddress("glBindBuffer");
            glGenBuffers        = (PFNGLGENBUFFERSPROC)       SDL_GL_GetProcAddress("glGenBuffers");
            glBufferData        = (PFNGLBUFFERDATAPROC)       SDL_GL_GetProcAddress("glBufferData");
            glUniform1i         = (PFNGLUNIFORM1IPROC)        SDL_GL_GetProcAddress("glUniform1i");
            glUniform4f         = (PFNGLUNIFORM4FPROC)        SDL_GL_GetProcAddress("glUniform4f");
            glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)      SDL_GL_GetProcAddress("glGetUniformLocation");
            glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glEnableVertexAttribArray");
            glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glDisableVertexAttribArray");
            glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)     SDL_GL_GetProcAddress("glVertexAttribPointer");
            glGetAttribLocation     = (PFNGLGETATTRIBLOCATIONPROC)     SDL_GL_GetProcAddress("glGetAttribLocation");
            glTexStorage2D     = (PFNGLTEXSTORAGE2DPROC)     SDL_GL_GetProcAddress("glTexStorage2D");
            glBindFramebuffer     = (PFNGLBINDFRAMEBUFFERPROC)     SDL_GL_GetProcAddress("glBindFramebuffer");
            glFramebufferTexture     = (PFNGLFRAMEBUFFERTEXTUREPROC)     SDL_GL_GetProcAddress("glFramebufferTexture");
            glFramebufferTexture2D     = (PFNGLFRAMEBUFFERTEXTURE2DPROC)     SDL_GL_GetProcAddress("glFramebufferTexture2D");
            glDrawBuffers     = (PFNGLDRAWBUFFERSPROC)     SDL_GL_GetProcAddress("glDrawBuffers");
            glCheckFramebufferStatus     = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)     SDL_GL_GetProcAddress("glCheckFramebufferStatus");
            glGenFramebuffers     = (PFNGLGENFRAMEBUFFERSPROC)     SDL_GL_GetProcAddress("glGenFramebuffers");
            glGenRenderbuffers     = (PFNGLGENRENDERBUFFERSPROC)            SDL_GL_GetProcAddress("glGenRenderbuffers");
            glBindRenderbuffer     = (PFNGLBINDRENDERBUFFERPROC)            SDL_GL_GetProcAddress("glBindRenderbuffer");
            glRenderbufferStorage     = (PFNGLRENDERBUFFERSTORAGEPROC)         SDL_GL_GetProcAddress("glRenderbufferStorage");
            glFramebufferRenderbuffer     = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)     SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
#ifdef SHADOWBOX_OFFSCREEN
	        glGenFramebuffers(1, &shadowbox_quilt_framebuffer);
        	glGenTextures(1, &shadowbox_quilt_tex_id);
	        glGenBuffers(1, &shadowbox_quilt_quad_vbo);
	        // Make the offscreen texture
	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, 
	        			 shadowbox_tile_size_x * shadowbox_tiles_x,
	        			 shadowbox_tile_size_y * shadowbox_tiles_y,
	                     0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
#endif
		}
	}
	else
	{
        printf("Deactivating shadowbox mode.\n");
		shadowbox_tiles_x = 1;
		shadowbox_tiles_y = 1;
        SDL_SetWindowBordered(win, SDL_TRUE);
        SDL_SetWindowPosition(win, restore_window_bounds.x, restore_window_bounds.y);
        SDL_SetWindowSize(win, restore_window_bounds.w, restore_window_bounds.h);
	}
}

void shadowbox_begin_render_quilt()
{
#ifdef SHADOWBOX_OFFSCREEN
check_gl_errors(__LINE__);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbox_quilt_framebuffer);
check_gl_errors(__LINE__);
        // glBindRenderbuffer(GL_RENDERBUFFER, quilt->depthrenderbuffer);
        // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, quilt->depthrenderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowbox_quilt_tex_id, 0);
check_gl_errors(__LINE__);
#endif
}

void shadowbox_end_render_quilt()
{
check_gl_errors(__LINE__);
    glFinish();
check_gl_errors(__LINE__);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
check_gl_errors(__LINE__);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
check_gl_errors(__LINE__);
}

void shadowbox_draw_quilt_to_screen()
{
#ifdef SHADOWBOX_OFFSCREEN
    int win_width, win_height;
    GLint old_program;
    glDisable(GL_BLEND);
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
//    glUseProgram(shadowbox_quilt_to_screen_shader);
check_gl_errors(__LINE__);
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
check_gl_errors(__LINE__);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
//    glUniform1i(glGetUniformLocation(shadowbox_quilt_to_screen_shader, "tx_color"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
//    printf("win size %dx%d\n", win_width, win_height);
    glViewport(0.0, 0.0, win_width, win_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
//    glOrtho(0, win_width, 0, win_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
check_gl_errors(__LINE__);

    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    float qsize = 0.5f;
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-qsize, -qsize, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-qsize,  qsize, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( qsize,  qsize, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( qsize, -qsize, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( qsize, -qsize, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( qsize,  qsize, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-qsize,  qsize, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-qsize, -qsize, 0.0f);
    glEnd();

    if (0) {
        const float vsc = 1.0f;
        static float verts_full[] = {
                              // pos
                              vsc*-1.0f, vsc*-1.0f, 0.0f,
                              vsc*1.0, vsc*-1.0, 0.0f,
                              vsc*1.0, vsc*1.0, 0.0f,
                              vsc*-1.0, vsc*1.0, 0.0f,
                              // u,v,pfar,pnear
                               0.0f, 0.0f, 0, 0,
                               1.0f, 0.0f, 0, 0,
                               1.0f, 1.0f, 0, 0,
                               0.0f, 1.0f, 0, 0};
        glBindBuffer(GL_ARRAY_BUFFER, shadowbox_quilt_quad_vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        float* uvfn = verts_full + 3*4;
        uvfn[0*4+0] = 0.0; uvfn[0*4+1] = 1.0;
        uvfn[1*4+0] = 1.0; uvfn[1*4+1] = 1.0;
        uvfn[2*4+0] = 1.0; uvfn[2*4+1] = 0.0;
        uvfn[3*4+0] = 0.0; uvfn[3*4+1] = 0.0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(4 * 3 * sizeof(float)));  
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts_full), verts_full, GL_STATIC_DRAW);
        glDrawArrays(GL_QUADS, 0, 4);
    }

    glFinish();
    glFlush();
    SDL_GL_SwapWindow(main_sdl_window);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(old_program);
    glDisable(GL_TEXTURE_2D);
check_gl_errors(__LINE__);

    extern int32    MainWindowSize[2];
    glViewport(0.0, 0.0, MainWindowSize[0], MainWindowSize[1]);
check_gl_errors(__LINE__);
#endif
}
