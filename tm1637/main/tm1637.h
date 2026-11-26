#include "driver/gpio.h"

typedef enum{
    E_GPIO_CLK  = 1,
    E_GPIO_DIO  = 2,
    E_WRONG_ACK = 3,
} TM1637_error;

class TM1637{
    private:
        gpio_num_t clkPin;
        gpio_num_t dioPin;

        void set_clk(uint8_t level);
        void set_dio(uint8_t level);
        void start();
        void stop();
        int read_ack();
        int write_byte(unsigned char b);

    public:
        TM1637(gpio_num_t clkGPIO, gpio_num_t dioGPIO);
        void init();
        void set_brightness(unsigned char brightness);
        void write_string(char* string, int colon);
};