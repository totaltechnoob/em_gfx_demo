#ifndef GFX_TIMER_H
    #define GFX_TIMER_H
#include <stdint.h>
#include <stdlib.h>

typedef void(*timer_cb_t)(void); 

typedef struct timer_task_node_t{
    timer_cb_t callback;
    struct timer_task_node_t *next;
}timer_task_node_t;

void increment_time(uint32_t time_elapsed_ms);

void register_function_to_timer(timer_cb_t callback);

uint32_t get_current_time();

void execute_timed_tasks();

#endif //GFX_TIMER_H