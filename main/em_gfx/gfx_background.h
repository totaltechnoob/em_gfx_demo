#ifndef GFX_BACKGROUND_H
    #define GFX_BACKGROUND_H

#ifndef GFX_MAIN_H
    #include "gfx_main.h"
#endif

typedef struct gfx_object_background{
    gfx_object common;
}gfx_background;

void draw_background(gfx_object *bg, int layer);

gfx_object *create_background(color bg_color, char *name);

#endif //GFX_BACKGROUND_H