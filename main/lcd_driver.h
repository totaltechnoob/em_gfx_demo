#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>
#include <math.h>
#include <inttypes.h>


typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;

void lcd_spi_pre_transfer_callback(spi_transaction_t *t);

void lcd_data(spi_device_handle_t lcd, const uint8_t *data, int length);

void lcd_cmd(spi_device_handle_t lcd, const uint8_t cmd, bool keep_cs_active);

void lcd_init(spi_device_handle_t lcd, int dc, int cs, int rst, int bckl, int lines, int width, int length);

void print_screen_to_lcd(spi_device_handle_t spi, uint16_t *buffer);
