#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "em_gfx/gfx.h"
#include "lcd_driver.h"
#include "esp_timer.h"

static char *TAG = "MAIN";

#define PIN_NUM_MISO 2
#define PIN_NUM_CLK  4
#define PIN_NUM_MOSI 16
#define PIN_NUM_CS   21
#define PIN_NUM_DC   23
#define PIN_NUM_BCKL 15
#define PIN_NUM_RST  22

#define PARALLEL_LINES 32  


// void vAnimationTask(void *pvParameters){
//     animation_data *data = (animation_data*)pvParameters;
//     int steps = (abs(data->target.x-data->obj->anchor.x)>=abs(data->target.y-data->obj->anchor.y))?abs(data->target.x-data->obj->anchor.x)+1:abs(data->target.y-data->obj->anchor.y)+1;
//     coords path[steps];
//     get_line(data->obj->anchor,data->target,path,steps);

//     for(;;){
//         for(int i=0; i<steps; i += 1){
//             data->obj->anchor = path[i];
//             data->obj->obj_constructor(data->obj,5);
//             update_screen_array();
//             print_screen_to_lcd(data->spi_handle);
//             vTaskDelay(10/portTICK_PERIOD_MS);
//         }
//         vTaskDelay(10/portTICK_PERIOD_MS);
//     }
// }

void app_main(void)
{   
    uint16_t *buffer = malloc(sizeof(uint16_t) *  SCREEN_L * SCREEN_W);
    
    spi_device_handle_t lcd;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=PARALLEL_LINES*128*2+8
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=21,               //CS pin
        .queue_size=15,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);    
    spi_bus_add_device(HSPI_HOST, &devcfg, &lcd);

    lcd_init(lcd, PIN_NUM_DC, PIN_NUM_CS, PIN_NUM_RST, PIN_NUM_BCKL, PARALLEL_LINES, SCREEN_L, SCREEN_W);
    gfx_initlib(SCREEN_L, SCREEN_W, buffer);

    color bgcolor = {
        .rgb = 0b0000000000000000
    };

    gfx_object *bg = create_background(bgcolor, "bg");
    gfx_object *line1 = create_line((coords){1,1},(coords){127,127},"line1");
    gfx_object *line2 = create_line((coords){87,54}, (coords){22,125}, "line2");
    
    gfx_init_animations();

    gfx_animation_t line1_move = {
        .animation_cb = gfx_set_x,
        .duration = 500,
        .object = line1,
        .start_value = line1->anchor.x,
        .end_value = line1_move.start_value + 50,
        .start_time = get_current_time(),
        .time_elapsed = 0
    };
    
    gfx_animation_t line2_move = {
        .animation_cb = gfx_set_x,
        .duration = 700,
        .object = line2,
        .start_value = line2->anchor.x,
        .end_value = line2_move.start_value - 30,
        .start_time = get_current_time(),
        .time_elapsed = 0
    };

    gfx_activate_animation(&line1_move);
    gfx_activate_animation(&line2_move);
    
    while(1){            
        increment_time(10);
        execute_timed_tasks();
        print_screen_to_lcd(lcd, buffer);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
