#include "driver/gpio.h"

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

// ERROR CODES
#define E_onewire_reset            1
#define E_onewire_skip_rom         2
#define E_onewire_write            3
#define E_onewire_read_crc         4

