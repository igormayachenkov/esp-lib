/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "tm1637";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO %d LED!", BLINK_GPIO);
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}
static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

/*----------- TM1637 ------------*/
#define CLK_GPIO 2
#define DIO_GPIO 4
#define CLOCK_DELAY 5
#define CMD_WRITE_DATA   0b01000000
#define CMD_SET_ADDRESS  0b11000000 // base address
#define CMD_BRIGHTNESS   0b10001000 // base brightness

static void delay(void)         { vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS); }
static void clk(uint8_t level){ gpio_set_level(CLK_GPIO, level); }
static void dio(uint8_t level){ gpio_set_level(DIO_GPIO, level); }

static void tm1637_init(void){
    ESP_LOGI(TAG, "TM1637 configuration: CLK-%d   DIO-%d", CLK_GPIO, DIO_GPIO);
    gpio_reset_pin(CLK_GPIO);
    gpio_reset_pin(DIO_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(CLK_GPIO,   GPIO_MODE_OUTPUT);
    gpio_set_direction(DIO_GPIO,   GPIO_MODE_OUTPUT);

    dio(1);
    clk(1);
}

static void tm1637_start(void){
    clk(1);
    dio(1);
    delay();
    dio(0);
}

static void tm1637_stop(void){
    clk(0); // clock low
    delay();
    dio(0);// data low
    delay();
    clk(1);// clock high
    delay();
    dio(1);// data high
}

static int read_ack(){
    gpio_set_direction(DIO_GPIO,   GPIO_MODE_INPUT);
    clk(0);
    delay();
    int level = gpio_get_level(DIO_GPIO);
    clk(1);
    gpio_set_direction(DIO_GPIO,   GPIO_MODE_OUTPUT);
    delay();
    // Check ACK
    if(level!=0)
        ESP_LOGE(TAG, "TM1637: received wrong ACK");
    return level==0;
}

static int write_byte(unsigned char b){
    // Write bits from low to high
    unsigned char i;
    for (i=0; i<8; i++){
        clk(0);
        delay();
        // Write data
        if (b & 1) // LSB to MSB
            dio(1);
        else
            dio(0);
        delay();
        clk(1); // fixes the data level
        delay();
        b >>= 1;
    }
    // Read ACK
    return read_ack();
}
static void set_brightness(unsigned char brightness){// 0..7
    tm1637_start();
    //ESP_LOGI(TAG, "brightness: %X", (brightness & 0b00000111));
    write_byte(CMD_BRIGHTNESS + (brightness & 0b00000111));
    tm1637_stop();
}

// 7dig display segment flags from low bit: GFEDCBA
const unsigned char digit2Seg[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0x6d, // 5
    0x7d, // 6
    0x07, // 7
    0x7f, // 8
    0x6f  // 0
};
#define err2seg    0b00001000
#define minus2seg  0b01000000
#define degree2seg 0b01100011

static void write_string(char* string, int colon){
        tm1637_start();
        write_byte(CMD_WRITE_DATA);
        tm1637_stop();

        delay();

        tm1637_start();
        write_byte(CMD_SET_ADDRESS);
        for(int i=0;i<4;i++){
            // Current char
            char c = string[i];

            // Segments data
            unsigned char segments = err2seg; // default 
            if (c >= '0' && c <= '9')  segments = digit2Seg[c - '0']; // segments data
            else if(c==' ') segments = 0;
            else if(c=='-') segments = minus2seg;
            else if(c=='*') segments = degree2seg;

            // Colon
            if(i==1 && colon)
                segments |= 0b10000000; // second digit high bit controls colon LEDs

            // Write segments data
            write_byte(segments);
        }
        tm1637_stop();
}


void app_main(void){

    /* Configure the peripheral according to the LED type */
    configure_led();
    tm1637_init();

    delay();

    char str[16];

    for (int i = 0; ;i++) {
        ESP_LOGI(TAG, "%d) Turning the LED %s!", i, s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;

        set_brightness(i);

        sprintf(str,"%3d*",i);
        write_string(str, i%2);

        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);

    }
}
