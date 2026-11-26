#include "driver/gpio.h"

// ERROR CODES
typedef enum {
    E_onewire_reset     = 1,
    E_onewire_skip_rom  = 2,
    E_onewire_write     = 3,
    E_onewire_read_crc  = 4,
} DS18B20_error;

class DS18B20 {
    private:
        gpio_num_t linePin;
        void sendCommand(uint8_t command);
        void readScratchpad(uint8_t *buffer);

    public:
        DS18B20(gpio_num_t lineGPIO);
        gpio_num_t getLineGPIO(){return linePin;}
        void init();
        float readTempereture();
};


