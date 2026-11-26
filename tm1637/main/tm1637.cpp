#include "tm1637.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "tm1637";

#define CLOCK_DELAY 5
#define CMD_WRITE_DATA   0b01000000
#define CMD_SET_ADDRESS  0b11000000 // base address
#define CMD_BRIGHTNESS   0b10001000 // base brightness

static void delay(void)     { vTaskDelay(CLOCK_DELAY / portTICK_PERIOD_MS); }

TM1637::TM1637(gpio_num_t clkGPIO, gpio_num_t dioGPIO){
    clkPin = clkGPIO;
    dioPin = dioGPIO;
}

void TM1637::set_clk(uint8_t level){ 
    esp_err_t e = gpio_set_level(clkPin, level);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_level(CLK) error %X", e);
        throw(E_GPIO_CLK);
    }
}
void TM1637::set_dio(uint8_t level){ 
    esp_err_t e = gpio_set_level(dioPin, level);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_level(DIO) error %X", e);
        throw(E_GPIO_DIO);
    }
}



void TM1637::init(){
    ESP_LOGI(TAG, "TM1637 configuration: CLK=%d   DIO=%d", clkPin, dioPin);

    esp_err_t e = 0;

    e = gpio_reset_pin(clkPin);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_reset_pin(CLK) error %X", e);
        throw(E_GPIO_CLK);
    }
    e = gpio_reset_pin(dioPin);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_reset_pin(DIO) error %X", e);
        throw(E_GPIO_DIO);
    }

    /* Set the GPIO as a push/pull output */
    e = gpio_set_direction(clkPin,   GPIO_MODE_OUTPUT);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_direction(CLK) error %X", e);
        throw(E_GPIO_CLK);
    }
    e = gpio_set_direction(dioPin,   GPIO_MODE_OUTPUT);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_direction(DIO) error %X", e);
        throw(E_GPIO_DIO);
    }

    set_dio(1);
    set_clk(1);
}

void TM1637::start(void){
    set_clk(1);
    set_dio(1);
    delay();
    set_dio(0);
}

void TM1637::stop(void){
    set_clk(0); // clock low
    delay();
    set_dio(0);// data low
    delay();
    set_clk(1);// clock high
    delay();
    set_dio(1);// data high
}

int TM1637::read_ack(){
    esp_err_t e = 0;
    e = gpio_set_direction(dioPin,   GPIO_MODE_INPUT);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_direction(DIO) error %X", e);
        throw(E_GPIO_DIO);
    }
    set_clk(0);
    delay();
    int level = gpio_get_level(dioPin);
    set_clk(1);
    e = gpio_set_direction(dioPin,   GPIO_MODE_OUTPUT);
    if(e!=ESP_OK){
        ESP_LOGE(TAG, "gpio_set_direction(DIO) error %X", e);
        throw(E_GPIO_DIO);
    }
    delay();
    // Check ACK
    if(level!=0) throw(E_WRONG_ACK);
    return level==0;
}

int TM1637::write_byte(unsigned char b){
    // Write bits from low to high
    unsigned char i;
    for (i=0; i<8; i++){
        set_clk(0);
        delay();
        // Write data
        if (b & 1) // LSB to MSB
            set_dio(1);
        else
            set_dio(0);
        delay();
        set_clk(1); // fixes the data level
        delay();
        b >>= 1;
    }
    // Read ACK
    return read_ack();
}

void TM1637::set_brightness(unsigned char brightness){// 0..7
    start();
    //ESP_LOGI(TAG, "brightness: %X", (brightness & 0b00000111));
    write_byte(CMD_BRIGHTNESS + (brightness & 0b00000111));
    stop();
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

void TM1637::write_string(char* string, int colon){
        start();
        write_byte(CMD_WRITE_DATA);
        stop();

        delay();

        start();
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
        stop();
}


