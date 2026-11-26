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
#include "tm1637.h"

static const char *TAG = "main";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
static gpio_num_t BLINK_GPIO = GPIO_NUM_17;

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

extern "C" void app_main(void){
    TM1637 display(GPIO_NUM_2, GPIO_NUM_4);

    /* Configure the peripheral according to the LED type */
    configure_led();

    try{
        display.init();
    }catch(TM1637_error e){
        ESP_LOGE(TAG,"display.init() error code: %d",e);
    }

    char str[16];

    for (int i = 0; ;i++) {

        // LED BLINKER
        ESP_LOGI(TAG, "%d) Turning the LED %s!", i, s_led_state == true ? "ON" : "OFF");
        blink_led();
        // Toggle the LED state
        s_led_state = !s_led_state;


        // DISPLAY
        try{
            display.set_brightness(i);

            sprintf(str,"%3d*",i);
            display.write_string(str, i%2);
        }catch(TM1637_error e){
            ESP_LOGE(TAG,"error code: %d",e);
        }

        // DELAY
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
