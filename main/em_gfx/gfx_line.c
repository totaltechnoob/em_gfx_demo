#include "gfx_line.h"
#include <stdbool.h>

void draw_line(gfx_object *object, int layer){
    gfx_line *line = (gfx_line*) object;
    
    coords p1 = {
        .x = line->endpoint[0].x + object->anchor.x,
        .y = line->endpoint[0].y + object->anchor.y
    };
    coords p2 = {
        p2.x = line->endpoint[1].x + object->anchor.x,
        p2.y = line->endpoint[1].y + object->anchor.y
    };
    bool reverse = false;
    if(p2.x < p1.x){
        reverse = true;
        coords temp = p1;
        p1 = p2;
        p2 = temp;
    }
    int dx = p2.x-p1.x;
    int dy = abs(p2.y-p1.y);
    
    //current coordinate being calculated. Set it to the first point initially.
    int y = p1.y;
    int x = p1.x;
    
    if(dy <= dx){
        int D = 2*dy - dx;
        for(int i=0; i<dx; i++){
            set_pixel(x,y,object->color,layer);
            if(D <= 0){
                D = D + 2*dy;
            } else {
                (p2.y>=p1.y)?y++:y--;
                D = D + 2*dy - 2*dx;
            }
            x++;
        }
    } else {
        int D = 2*dx - dy;
        for(int i=0; i<dy; i++){
            set_pixel(x,y,object->color,layer);
            if(D <= 0){
                D = D + 2*dx;
            } else {
                x = x+1;
                D = D + 2*dx - 2*dy;
            }
            (p2.y>=p1.y)?y++:y--;
        }
    }
}

gfx_object *create_line(coords p1, coords p2, char *name){
    gfx_line *line = malloc(sizeof(gfx_line)); //free this while deleting line
    
    line->common.anchor = p1;
    line->endpoint[0] = (coords){.x = 0, .y= 0};
    line->endpoint[1] = (coords){.x = p2.x - p1.x, .y = p2.y - p1.y};
    line->common.obj_constructor = draw_line;
    line->common.animate = 0;
    line->common.isPath = 0;
    line->common.name = name;
    line->common.color.rgb = 0b1111111111111111;
    activate_object((gfx_object*)line);
    return (gfx_object*) line;
}