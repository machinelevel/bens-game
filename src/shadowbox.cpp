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

void toggle_shadowbox()
{
	do_shadowbox_quilt = !do_shadowbox_quilt;
	if (do_shadowbox_quilt)
	{
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
	        glGenBuffers(1, &quilt->shadowbox_quilt_quad_vbo);
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
		shadowbox_tiles_x = 1;
		shadowbox_tiles_y = 1;
	}
}

void shadowbox_begin_render_quilt()
{
#ifdef SHADOWBOX_OFFSCREEN
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbox_quilt_framebuffer);
        // glBindRenderbuffer(GL_RENDERBUFFER, quilt->depthrenderbuffer);
        // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, quilt->depthrenderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowbox_quilt_tex_id, 0);
#endif
}

void shadowbox_end_render_quilt()
{
    glFinish();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void shadowbox_draw_quilt_to_screen()
{
#ifdef SHADOWBOX_OFFSCREEN
    GLint old_program;
    glDisable(GL_BLEND);
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
//    glUseProgram(shadowbox_quilt_to_screen_shader);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
    glUniform1i(glGetUniformLocation(quilt->plain_shader, "tx_color"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(old_program);
    glDisable(GL_TEXTURE_2D);
    glViewport(0.0, 0.0, MainWindowSize[0], MainWindowSize[1]);
#endif
}
