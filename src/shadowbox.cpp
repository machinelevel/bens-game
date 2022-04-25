#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include "types.h"
#include "foxpf.h"
#include "draw.h"
#include "shadowbox.h"

extern SDL_Window* main_sdl_window;

bool do_shadowbox_quilt = false;
int  shadowbox_tiles_x = 1;
int  shadowbox_tiles_y = 1;
int  shadowbox_tile_size_x = 0;
int  shadowbox_tile_size_y = 0;
float shadowbox_left_right = 0.0f;

void* shadowbox_quilt_tex32_pixels = NULL;
bool shadowbox_initialized = false;
GLuint shadowbox_quilt_framebuffer = 0;
GLuint shadowbox_quilt_depthrenderbuffer = 0;
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
static PFNGLUNIFORM1FPROC         glUniform1f        ;
static PFNGLUNIFORM2FPROC         glUniform2f        ;
static PFNGLUNIFORM3FPROC         glUniform3f        ;
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

void add_shader(GLuint program, const char* shader_src, GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);
    if (!shader_obj) {
        printf("Error creating shader type %d\n", shader_type);
        return ;
    }

    const GLchar* p[1];
    p[0] = shader_src;
    GLint lengths[1];
    lengths[0]= strlen(shader_src);

    glShaderSource(shader_obj, 1, p, lengths);
    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
        printf("Error compiling shader type %d: '%s'\n", shader_type, info_log);
        return;
    }
    glAttachShader(program, shader_obj);
}

GLuint compile_one_shader(const char* vshader, const char* pshader)
{
    GLuint shader_program = glCreateProgram();
    if (shader_program == 0)
    {
        printf("Error creating shader program\n");
        return shader_program;
    }
    add_shader(shader_program, vshader, GL_VERTEX_SHADER);
    add_shader(shader_program, pshader, GL_FRAGMENT_SHADER);
    GLint success = 0;
    GLchar error_log[1024] = { 0 };
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        printf("Error linking shader program: '%s'\n", error_log);
        return shader_program;
    }
    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        printf("Invalid shader program: '%s'\n", error_log);
        return shader_program;
    }
    return shader_program;
}

static const char* quilt_to_screen_vshader = 
"attribute vec4 vertPos_data;\n"
"\n"
"varying vec2 z_interp;\n"
"varying vec2 texCoords;\n"
"uniform sampler2D screenTex;\n"
"uniform float pitch;\n"
"uniform float tilt;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"    texCoords = gl_MultiTexCoord0.st;\n"
"    z_interp = (vec2(gl_Position.x, -gl_Position.y) + 1.0) * 0.5;\n"
"    z_interp = vec2(z_interp.x, z_interp.y * tilt) * pitch;\n"
"}";

// This shader is faster, but has confetti turds, because it cheats.
static const char* quilt_to_screen_pshader_1sample = 
"#line 155\n"
"varying vec2 z_interp;\n"
"varying vec2 texCoords;\n"
"uniform sampler2D screenTex;\n"
"uniform sampler2D screenTex;\n"
"uniform float pitch;\n"
"uniform float center;\n"
"uniform float subp;\n"
"uniform vec3 subp_step;\n"
"uniform vec2 num_tiles;\n"
"uniform vec2 inv_num_tiles;\n"
"void main()\n"
"{\n"
"   vec3 z = fract(z_interp.x + subp_step + z_interp.y);\n"
"   vec2 uv = texCoords * inv_num_tiles;\n"
"   vec3 tile_id = floor(z * num_tiles.x * num_tiles.y);\n"
"   vec3 tilex = inv_num_tiles.x * mod(tile_id, num_tiles.x);\n"
"   vec3 tiley = inv_num_tiles.y * floor(tile_id / num_tiles.x);\n"
"   uv.x += tilex.y;\n"
"   uv.y += tiley.y;\n"
"   vec3 out_color = texture2D(screenTex, uv).rgb;\n"
"   gl_FragColor = vec4(out_color, 1.0);\n"
"}\n"
"";

// This shader is slower, but more correct.
static const char* quilt_to_screen_pshader_3samples = 
"#line 155\n"
"varying vec2 z_interp;\n"
"varying vec2 texCoords;\n"
"uniform sampler2D screenTex;\n"
"uniform float pitch;\n"
"uniform float center;\n"
"uniform float subp;\n"
"uniform vec3 subp_step;\n"
"uniform vec2 num_tiles;\n"
"uniform vec2 inv_num_tiles;\n"
"void main()\n"
"{\n"
"   vec3 z = fract(z_interp.x + subp_step + z_interp.y);\n"
"   vec2 uv = texCoords * inv_num_tiles;\n"
"   vec3 tile_id = floor(z * num_tiles.x * num_tiles.y);\n"
"   vec3 tilex = inv_num_tiles.x * mod(tile_id, num_tiles.x);\n"
"   vec3 tiley = inv_num_tiles.y * floor(tile_id / num_tiles.x);\n"
"   float r = texture2D(screenTex, vec2(uv.x+tilex.x, uv.y+tiley.x)).r;\n"
"   float g = texture2D(screenTex, vec2(uv.x+tilex.y, uv.y+tiley.y)).g;\n"
"   float b = texture2D(screenTex, vec2(uv.x+tilex.z, uv.y+tiley.z)).b;\n"
"   gl_FragColor = vec4(r, g, b, 1.0);\n"
"}\n"
"";

static const char* quilt_to_screen_pshader_tint_test = 
"#line 155\n"
"varying vec2 z_interp;\n"
"varying vec2 texCoords;\n"
"uniform sampler2D screenTex;\n"
"uniform float pitch;\n"
"uniform float center;\n"
"uniform float subp;\n"
"uniform vec3 subp_step;\n"
"void main()\n"
"{\n"
"   vec3 z = fract(z_interp.x + subp_step + z_interp.y);\n"
"   vec3 out_color = texture2D(screenTex, texCoords.xy).rgb;\n"
//"   out_color += vec3(0.5 * texCoords.xy, 0.0);\n"
"   out_color += vec3(z.y, 1.0-z.y, 0.0);\n"
"   gl_FragColor = vec4(out_color, 1.0);\n"
"}\n"
"";

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
		shadowbox_tiles_x = SHADOWBOX_TILES_X;
		shadowbox_tiles_y = SHADOWBOX_TILES_Y;
		shadowbox_tile_size_x = 256;//512;
		shadowbox_tile_size_y = 320;//640;

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
            glUniform1f         = (PFNGLUNIFORM1FPROC)        SDL_GL_GetProcAddress("glUniform1f");
            glUniform2f         = (PFNGLUNIFORM2FPROC)        SDL_GL_GetProcAddress("glUniform2f");
            glUniform3f         = (PFNGLUNIFORM3FPROC)        SDL_GL_GetProcAddress("glUniform3f");
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

            int total_x = shadowbox_tile_size_x * shadowbox_tiles_x;
            int total_y = shadowbox_tile_size_y * shadowbox_tiles_y;
	        glGenFramebuffers(1, &shadowbox_quilt_framebuffer);
            glGenRenderbuffers(1, &shadowbox_quilt_depthrenderbuffer);
        	glGenTextures(1, &shadowbox_quilt_tex_id);

	        glGenBuffers(1, &shadowbox_quilt_quad_vbo);
	        // Make the offscreen texture
	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
            uint64_t total_bytes = total_x * total_y * 4;
            shadowbox_quilt_tex32_pixels = malloc(total_bytes); // TODO: Might not need this buffer at all.
            // initialize to something obnoxious for debugging
            uint32_t* pix = (uint32_t*)shadowbox_quilt_tex32_pixels;
            for (int y = 0; y < total_y; ++y)
            {
                for (int x = 0; x < total_x; ++x)
                {
                    uint32_t* dst = pix + (y * total_x) + x;
                    uint32_t color = 0xff0000ff;
                    if ((y / 256) & 1)
                        color |= 0x00ff0000;
                    if ((x / 256) & 1)
                        color |= 0x0000ff00;
                    *dst = color;
                }
            }
            printf("Quilt texture is %dx%d, %d MB\n", total_x, total_y, (total_x * total_y * 4) / 1048576);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                         total_x,
                         total_y,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, shadowbox_quilt_tex32_pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
#if SHADOWBOX_SHADER_SAMPLES == 3
            shadowbox_quilt_to_screen_shader = compile_one_shader(quilt_to_screen_vshader,
                                                                  quilt_to_screen_pshader_3samples);
#else
            shadowbox_quilt_to_screen_shader = compile_one_shader(quilt_to_screen_vshader,
                                                                  quilt_to_screen_pshader_1samples);
#endif
		}
	}
	else
	{
        printf("Deactivating shadowbox mode.\n");
		shadowbox_tiles_x = 1;
		shadowbox_tiles_y = 1;
        shadowbox_left_right = 0.0f;
        SDL_SetWindowBordered(win, SDL_TRUE);
        SDL_SetWindowPosition(win, restore_window_bounds.x, restore_window_bounds.y);
        SDL_SetWindowSize(win, restore_window_bounds.w, restore_window_bounds.h);
	}
}

void shadowbox_begin_render_quilt()
{
    int total_x = shadowbox_tile_size_x * shadowbox_tiles_x;
    int total_y = shadowbox_tile_size_y * shadowbox_tiles_y;

    check_gl_errors(__LINE__);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbox_quilt_framebuffer);
    glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, total_x, total_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowbox_quilt_depthrenderbuffer);
    check_gl_errors(__LINE__);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, total_x, total_y);
    check_gl_errors(__LINE__);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadowbox_quilt_depthrenderbuffer);
    check_gl_errors(__LINE__);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowbox_quilt_tex_id, 0);
    check_gl_errors(__LINE__);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    check_gl_errors(__LINE__);
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    check_gl_errors(__LINE__);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowbox_quilt_framebuffer);
    check_gl_errors(__LINE__);
    glViewport(0,0,total_x, total_y);
    check_gl_errors(__LINE__);
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
    int win_width, win_height;
    GLint old_program;
    glDisable(GL_BLEND);
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);

    GLuint shader = shadowbox_quilt_to_screen_shader;
    glUseProgram(shader);
    // These values are specific to each LGP, and are provided by the driver.
    // TODO: load these from a config file.
    float pitch = SHADOWBOX_PITCH;
    float tilt = SHADOWBOX_TILT;
    float center = SHADOWBOX_CENTER;
    float subp = SHADOWBOX_SUBP;
    glUniform1i(glGetUniformLocation(shader, "screenTex"), 0);
    glUniform1f(glGetUniformLocation(shader, "pitch"), pitch);
    glUniform1f(glGetUniformLocation(shader, "tilt"), tilt);
    glUniform1f(glGetUniformLocation(shader, "center"), center);
    glUniform1f(glGetUniformLocation(shader, "subp"), subp);
    glUniform3f(glGetUniformLocation(shader, "subp_step"), 0.0 * subp * pitch - center,
                                                           1.0 * subp * pitch - center,
                                                           2.0 * subp * pitch - center);
    glUniform2f(glGetUniformLocation(shader, "num_tiles"), shadowbox_tiles_x, shadowbox_tiles_y);
    glUniform2f(glGetUniformLocation(shader, "inv_num_tiles"), 1.0f / shadowbox_tiles_x, 1.0f / shadowbox_tiles_y);

    check_gl_errors(__LINE__);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowbox_quilt_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int total_x = shadowbox_tile_size_x * shadowbox_tiles_x;
    int total_y = shadowbox_tile_size_y * shadowbox_tiles_y;
    if (0)
    {
        // initialize to something obnoxious for debugging
        uint32_t* pix = (uint32_t*)shadowbox_quilt_tex32_pixels;
        for (int y = 0; y < total_y; ++y)
        {
            for (int x = 0; x < total_x; ++x)
            {
                uint32_t* dst = pix + (y * total_x) + x;
                uint32_t color = 0xff0000ff;
                if ((y / 256) & 1)
                    color |= 0x00ff0000;
                if ((x / 256) & 1)
                    color |= 0x0000ff00;
                *dst = color;
            }
        }
    }
    if (0)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                     total_x,
                     total_y,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, shadowbox_quilt_tex32_pixels);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_GetWindowSize(main_sdl_window, &win_width, &win_height);
    glViewport(0.0, 0.0, win_width, win_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    check_gl_errors(__LINE__);

    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    float qsize = 1.0f;
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-qsize, -qsize, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-qsize,  qsize, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( qsize,  qsize, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( qsize, -qsize, 0.0f);
    glEnd();

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
    check_gl_errors(__LINE__);

    extern int32    MainWindowSize[2];
    glViewport(0.0, 0.0, MainWindowSize[0], MainWindowSize[1]);
    check_gl_errors(__LINE__);
}
