#ifndef GFX_ANIMATE_H
    #define GFX_ANIMATE_H
#ifndef GFX_MAIN_H
    #include "gfx_main.h"
#endif //GFX_MAIN_H
#ifndef GFX_LINE_H
    #include "gfx_line.h"
#endif //GFX_LINE_H

typedef struct gfx_animation{
    gfx_object *object;
    int start_time;
    int current_time;
    int end_time;
    void(*animation_cb)(struct gfx_animation*);
}gfx_animation_t;

#endif //GFX_ANIMATE_H