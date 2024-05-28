#include "gfx_animate.h"

static gfx_animation_t_node head = {
    .animation = NULL,
    .next = NULL
};

void gfx_activate_animation(gfx_animation_t *anim){
    gfx_animation_t_node *current_node;
    gfx_animation_t_node *new_node = malloc(sizeof(gfx_animation_t_node)); //free this when animation ends
    anim->active_flag = 1;
    new_node->animation = anim;

    current_node = &head;

    while(current_node->next != NULL){
        current_node = current_node->next;
    }
    current_node->next = new_node;
    new_node->next = NULL;
}

int calculate_current_value(gfx_animation_t *anim){
    
    int lib_time = get_current_time();
    anim->time_elapsed = lib_time - anim->start_time;
    
    int current_value = anim->start_value + ((anim->end_value-anim->start_value)*anim->time_elapsed)/anim->duration;
    if((anim->end_value - anim->start_value) * (current_value - anim->end_value) >= 0){
        current_value = anim->end_value;
        anim->active_flag = 0;
    }
    return current_value;
}

void gfx_init_animations(){
    register_function_to_timer(update_animations);
}

void update_animations(){
    gfx_animation_t_node *current = &head;
    int value;
    while(current->next != NULL){
        current = current->next;
        if(current->animation->active_flag == 1){
            value = calculate_current_value(current->animation);
            current->animation->animation_cb(current->animation->object, value);
        }
    }
}