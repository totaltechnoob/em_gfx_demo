#ifndef GFX_MAIN_H
   #define GFX_MAIN_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef union{
    struct{
        unsigned short b : 5;
        unsigned short g : 6;
        unsigned short r : 5;
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

void activate_object(gfx_object *);

typedef struct node{
    //Flag: Needs refresh
    gfx_object *object;
    struct node *next;
    struct node *previous;
}gfx_object_node;

void set_pixel(int x, int y, color color, int layer);

void list_active_objects();

void delete_object(gfx_object *obj);

void refresh_screen();

uint16_t get_pixel_color(int y, int x);

void init_emgfx(int, int, pixel*);

void clear_screen();

#endif //GFX_MAIN_H