#include "gfx_background.h"

void draw_background(gfx_object *bg, int layer){
    for(int i=0; i<128; i++){
        for(int j=0; j<128; j++){
            set_pixel(i, j, bg->color, layer);
        }
    }
}

gfx_object *create_background(color bg_color, char *name){
    gfx_background *ret = malloc(sizeof(gfx_background));
    ret->common.animate = 0;
    ret->common.name = name;
    ret->common.color = bg_color;
    ret->common.isPath = 0;
    ret->common.obj_constructor = draw_background;
    activate_object((gfx_object*)ret);
    return (gfx_object*) ret;
}