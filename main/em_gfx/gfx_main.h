#ifndef GFX_MAIN_H
#define GFX_MAIN_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef GFX_TIMER_H
    #include "gfx_timer.h"
#endif
#include "esp_log.h"

typedef union{
    struct{
        uint16_t g : 5;
        uint16_t r : 6;
        uint16_t b : 5;
    };
    uint16_t rgb;
}color;

typedef struct pixel_data{
    color color;
    uint8_t layer;
}pixel;

typedef struct{
    int x;
    int y;
}coords;

typedef struct gfx_object{
    char *name;
    coords anchor;
    coords area[2];
    void (*obj_constructor)(struct gfx_object*, int layer);
    color color;
    union{
        struct{
            short animate : 1;
            short isPath : 1;
        };
        uint8_t flag;
    };
}gfx_object;

void gfx_activate_object(gfx_object *);

typedef struct node{
    //Flag: Needs refresh
    gfx_object *object;
    struct node *next;
    struct node *previous;
}gfx_object_node;

void set_pixel(int x, int y, color color, int layer);

void gfx_list_active_objects();

void gfx_delete_object(gfx_object *obj);

void gfx_render_frame();

void gfx_initlib(int, int, uint16_t*);

void gfx_clear_screen();

#endif //GFX_MAIN_H