#ifndef _SHADOWBOX_H_
#define _SHADOWBOX_H_

// Quality settings
#define SHADOWBOX_TILES_X        9  // The number of columns in the quilt
#define SHADOWBOX_TILES_Y        5  // The number of rows in the quilt
#define SHADOWBOX_TILE_SIZE_X  512  // Width in pixels of each tile
#define SHADOWBOX_TILE_SIZE_Y  640  // Height in pixels of each tile
#define SHADOWBOX_SHADER_SAMPLES 3  // 1 for low-quality sampling, 3 for high-quality

// These settings are specific to each LGP unit
// They're provided by the driver software.
#define SHADOWBOX_PITCH 246.848f
#define SHADOWBOX_TILT    0.184957f
#define SHADOWBOX_CENTER -0.05f
#define SHADOWBOX_SUBP    0.000217014f


void toggle_shadowbox();
void shadowbox_begin_render_quilt();
void shadowbox_end_render_quilt();
void shadowbox_draw_quilt_to_screen();
void shadowbox_save_screen();
void shadowbox_save_quilt();

extern bool do_shadowbox_quilt;
extern int  shadowbox_tiles_x;
extern int  shadowbox_tiles_y;
extern int  shadowbox_tile_size_x;
extern int  shadowbox_tile_size_y;
extern float shadowbox_left_right;
extern bool shadowbox_save_screens;
#endif _SHADOWBOX_H_
