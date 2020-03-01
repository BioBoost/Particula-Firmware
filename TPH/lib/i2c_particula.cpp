#include "./../mbed.h"
#include "i2c_particula.h"

namespace i2cCom{
    I2c_particula::I2c_particula(PinName sdi, PinName sck)
    :i2c_p(new I2C(sda, scl)),i2c(*i2c_p){
        
    }
}