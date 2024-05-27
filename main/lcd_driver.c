#include "lcd_driver.h"

int pin_miso;
int pin_clk;
int pin_mosi;
int pin_cs;
int pin_dc;
int pin_bckl;
int pin_rst;
int parallel_lines;

int screen_width;
int screen_length;

static uint16_t *screen_array;

DRAM_ATTR static const lcd_init_cmd_t ili_init_cmds[]={
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x26}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x35, 0x3E}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xBE}, 1},
    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
    {0x36, {0x28}, 1},
    /* Pixel format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x08}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    /* Negative gamma correction */
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    /* Column address set, SC=0, EC=0xEF */
    {0x2A, {0x00, 0x00, 0x00, 0x7F}, 4},
    /* Page address set, SP=0, EP=0x013F */
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    /* Memory write */
    {0x2C, {0}, 0},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    /* Sleep out */
    {0x11, {0}, 0x80},
    /* Display on */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(pin_dc, dc);
}

void lcd_data(spi_device_handle_t lcd, const uint8_t *data, int length){
    esp_err_t ret;
    spi_transaction_t t;
    if (length==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=length*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(lcd, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

void lcd_cmd(spi_device_handle_t lcd, const uint8_t cmd, bool keep_cs_active){
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    if (keep_cs_active) {
      t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret=spi_device_polling_transmit(lcd, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues
}

void lcd_init(spi_device_handle_t lcd, int dc, int cs, int rst, int bckl, int lines, int width, int length){
    pin_bckl = bckl;
    pin_rst = rst;
    pin_cs = cs;
    pin_dc = dc;
    parallel_lines = lines;
    screen_width = width;
    screen_length = length;

    screen_array = malloc(width*length*sizeof(uint16_t));
    
    //config non-spi pins
    gpio_config_t lcd_pins_config = {
        .pin_bit_mask = ((1ULL << pin_bckl) | (1ULL << pin_rst) | (1ULL << pin_dc)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };

    gpio_config(&lcd_pins_config);

    //Reset the display
    gpio_set_level(pin_rst, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(pin_rst, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    const lcd_init_cmd_t* lcd_init_cmds;
    lcd_init_cmds = ili_init_cmds;
    int cmd = 0;

    //Send all the commands
    while (lcd_init_cmds[cmd].databytes!=0xff) {
        lcd_cmd(lcd, lcd_init_cmds[cmd].cmd, false);
        lcd_data(lcd, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
        if (lcd_init_cmds[cmd].databytes&0x80) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        cmd++;
    }
    gpio_set_level(pin_bckl, 1);
}

static inline uint16_t get_bgnd_pixel(int x, int y)
{
    return screen_array[(y * screen_width) + x];
}

void calc_lines(uint16_t *dest, int line, int linect)
{
    for (int y=line; y<line+linect; y++) {
        for (int x=0; x<128; x++) {
            *dest=get_bgnd_pixel(x, y);
            dest++;
        }
    }
}

static void send_line_finish(spi_device_handle_t spi)
{
    spi_transaction_t *rtrans;
    esp_err_t ret;
    //Wait for all 6 transactions to be done and get back the results.
    for (int x=0; x<6; x++) {
        ret=spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
        assert(ret==ESP_OK);
        //We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
    }
}

static void send_lines(spi_device_handle_t spi, int ypos, uint16_t *linedata)
{
    esp_err_t ret;
    int x;
    static spi_transaction_t trans[6];

    for (x=0; x<6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x&1)==0) {
            trans[x].length=8;
            trans[x].user=(void*)0;
        } else {
            trans[x].length=8*4;
            trans[x].user=(void*)1;
        }
        trans[x].flags=SPI_TRANS_USE_TXDATA;
    }
    trans[0].tx_data[0]=0x2A;           //Column Address Set
    trans[1].tx_data[0]=0;              //Start Col High
    trans[1].tx_data[1]=0;              //Start Col Low
    trans[1].tx_data[2]=0;       //End Col High
    trans[1].tx_data[3]=127;     //End Col Low
    trans[2].tx_data[0]=0x2B;           //Page address set
    trans[3].tx_data[0]=0;        //Start page high
    trans[3].tx_data[1]=ypos&127;      //start page low
    trans[3].tx_data[2]=(ypos+parallel_lines)>>8;    //end page high
    trans[3].tx_data[3]=(ypos+parallel_lines)&0xff;  //end page low
    trans[4].tx_data[0]=0x2C;           //memory write
    trans[5].tx_buffer=linedata;        //finally send the line data
    trans[5].length=128*2*8*parallel_lines;          //Data length, in bits
    trans[5].flags=0; //undo SPI_TRANS_USE_TXDATA flag

    for (x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    }
}
void print_screen_to_lcd(spi_device_handle_t spi, uint16_t *buffer)
{
    for(int i=0; i<screen_width; i++){
        for(int j=0; j<screen_length; j++){
            *(screen_array+128*i+j) = *(buffer+128*i+j);
        }
    }
    uint16_t *lines[2];
    for (int i=0; i<2; i++) {
        lines[i]=heap_caps_malloc(128*parallel_lines*sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(lines[i]!=NULL);
    }
    
    int sending_line=-1;
    int calc_line=0;
    for (int y=0; y<screen_width; y+=parallel_lines) {
        calc_lines(lines[calc_line], y, parallel_lines);
        if (sending_line != -1) send_line_finish(spi);
        sending_line=calc_line;
        calc_line=(calc_line==1)?0:1;
        send_lines(spi, y, lines[sending_line]);
    }
    send_line_finish(spi);
    heap_caps_free(lines[sending_line]);
    heap_caps_free(lines[calc_line]);
}