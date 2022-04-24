#ifndef _SHADOWBOX_H_
#define _SHADOWBOX_H_

void toggle_shadowbox();
void shadowbox_begin_render_quilt();
void shadowbox_end_render_quilt();
void shadowbox_draw_quilt_to_screen();

extern bool do_shadowbox_quilt;
extern int  shadowbox_tiles_x;
extern int  shadowbox_tiles_y;
extern int  shadowbox_tile_size_x;
extern int  shadowbox_tile_size_y;

#endif _SHADOWBOX_H_
