#ifndef GFX_LINE_H
    #define GFX_LINE_H
#ifndef GFX_MAIN_H
    #include "gfx_main.h"
#endif //GFX_MAIN_H

typedef struct gfx_object_line {
    gfx_object common;
    coords endpoint[2];
}gfx_line;

void draw_line(gfx_object *object, int layer);

gfx_object *create_line(coords p1, coords p2, char *name);

#endif //GFX_LINE_H