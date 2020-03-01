#pragma once


#include "./../mbed.h"

//This does all the procedures to write and read from slaves on the i2c bus For the project particula
//for the moment it only does the TPH sensor


namespace i2cCom{
    class I2c_particula{
        public:
            BME280(PinName sda, PinName sck);
        private:
            I2C *i2c_p;
            I2C &i2c;
            char address_tph;
            char address_eeprom;
    };
}