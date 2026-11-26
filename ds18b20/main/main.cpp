#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "ds18b20.h"

static const char *TAG = "main";
static uint8_t ledState = 0;
static gpio_num_t BLINK_GPIO = GPIO_NUM_17;




extern "C" void app_main(void)
{
    int pt = portTICK_PERIOD_MS;
    ESP_LOGI(TAG, "***** APP STARTED *****" );
    ESP_LOGI(TAG, "portTICK_PERIOD_MS: %u", pt);

    // CONFIG BLINK GPIO
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT); // Set the GPIO as a push/pull output

    // DS18B20
    DS18B20 sensor(GPIO_NUM_5);
    sensor.init();

    while(1){
        // READ TEMPERATURE
        try{
            float temp = sensor.readTempereture();
            ESP_LOGI(TAG, "Sensor [%d] temp: %.2f", sensor.getLineGPIO(), temp);
        }catch(int e){
            ESP_LOGE(TAG,"error code: %d",e);
        }

        // Blink LED
        ledState = !ledState;
        ESP_LOGI(TAG, "ledState: %s!", ledState == true ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, ledState);

        
        vTaskDelay( 3000 / portTICK_PERIOD_MS );
    }

}

