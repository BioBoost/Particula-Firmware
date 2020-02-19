#include "mbed.h"
#include "BME280.h"
#include "tphInterface.h"

//For IC2 intilization
PinName sda = D0;
PinName scl = D1;
//SDA=DO and SCK=D1 for NUCLEO_L432KC
//SDA=D14 and SCK=D15 for NUCLEO_F042K6

//PC serial initilization
Serial pc(USBTX, USBRX);                        //tx, rx

void tphInterface(char addr){                   //Input of a 7BIT I2C address
    addr << 1;                                  //Shifting the address 1 bit to get an 8BIT addres that embed is using
    BME280 sensorTPH = BME280(sda, scl, addr);  //Initializing sensor with library
}

double temperature(){                           //returns Termperature as a double in °C    
    return (double)sensorTPH.getTeperature();
}

double presure(){                               //returns Presure as a double in hPa
    return (double)sensorTPH.getPresure();
}

double humidity(){                              //returns Humidity as a doubel in %
    return (double)sensorTPH.getHumidity();
}

void sleep(){
    //WIP
}

void awake(){
    //WIP
}