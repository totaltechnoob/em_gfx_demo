#include "gfx_timer.h"

static uint32_t system_on_time = 0;

timer_task_node_t head = {
    .next = NULL,
    .callback = NULL
};

void increment_time(uint32_t tick_period){
    system_on_time += tick_period;
}

void register_function_to_timer(timer_cb_t callback){
    timer_task_node_t *current = &head;
    timer_task_node_t *new = malloc(sizeof(timer_task_node_t)); //free this when removing a function from timers [probably won't ever happen]
    new->callback = callback;
    new->next = NULL;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = new;
}

uint32_t get_current_time(){
    return system_on_time;
}

void execute_timed_tasks(){
    timer_task_node_t *current = &head;
    
    while(current->next != NULL){
        current = current->next;
        current->callback();
    }
}

