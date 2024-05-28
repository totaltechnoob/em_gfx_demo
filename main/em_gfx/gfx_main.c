#include "gfx_main.h"

static uint16_t *screen;
int screen_w;
int screen_l;

void set_pixel(int x, int y, color color, int layer)
{   
    if(x<0 || x>=screen_l || y<0 || y>=screen_w ) return;
    *(screen+(screen_l * y + x)) = color.rgb;
}

gfx_object_node active_objects_head = {
    .object = NULL,
    .next = NULL,
    .previous = NULL
};

gfx_object_node active_objects_tail = {
    .object = NULL,
    .next = NULL,
    .previous = NULL
};

void gfx_activate_object(gfx_object *obj){
    gfx_object_node *current_node;
    gfx_object_node *new_node = malloc(sizeof(gfx_object_node)); //free this while deleting object
    new_node->object = obj;

    current_node = &active_objects_head;

    while(current_node->next != &active_objects_tail){
        current_node = current_node->next;
    }
    current_node->next = new_node;
    new_node->previous = current_node;
    new_node->next = &active_objects_tail;
    active_objects_tail.previous = new_node;
}

void gfx_list_active_objects(){
    gfx_object_node *current_node;
    current_node = active_objects_head.next;
    int i = 1;
    while(current_node->object != NULL){
        current_node = current_node->next;
        i++;
    }
}

void gfx_delete_object(gfx_object *obj){ //delete the object as well as the node referring to it
    gfx_object_node *current;
    current = &active_objects_head;
    while(current->object != obj){
        current = current->next;
    }
    current->previous->next = current->next;
    current->next->previous = current->previous;
    free(current);
    free(obj);
}

void gfx_render_frame(){
    int i=0;
    gfx_object_node *current = active_objects_head.next;
    while(current->object != NULL){
        current->object->obj_constructor(current->object, i);
        i++;
        current = current->next;
    }
}

void gfx_initlib(int length, int width, uint16_t *buf){
    screen_l = length;
    screen_w = width;
    screen = buf;
    active_objects_head.next = &active_objects_tail;
    active_objects_tail.previous = &active_objects_head;
    register_function_to_timer(gfx_render_frame);
}

void gfx_clear_screen(){
    color black;
    black.rgb = 0b0000000000000000;
    for(int i=0; i<screen_w; i++){
        for(int j=0; j<screen_l; j++){
            set_pixel(i, j, black, 0);
        }
    }
}