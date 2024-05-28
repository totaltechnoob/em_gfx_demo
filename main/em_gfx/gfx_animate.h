#ifndef GFX_ANIMATE_H
    #define GFX_ANIMATE_H
    #ifndef GFX_TIMER_H
        #include "gfx_timer.h"
    #endif //GFX_TIMER_H
    #ifndef GFX_MAIN_H
        #include "gfx_main.h"
    #endif //GFX_MAIN_H

typedef struct gfx_animation{
    gfx_object *object;
    int start_time;
    int time_elapsed;
    int duration;
    void(*animation_cb)(gfx_object *obj, int value);
    int start_value;
    int end_value;
    int active_flag;
}gfx_animation_t;

typedef struct gfx_animation_t_node{
    gfx_animation_t *animation;
    struct gfx_animation_t_node *next;
}gfx_animation_t_node;

void gfx_activate_animation(gfx_animation_t *anim);

void gfx_init_animations();

void update_animations();

#endif //GFX_ANIMATE_H