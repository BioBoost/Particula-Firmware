#include "mbed.h"
#include "BME280.h"
//#include "i2cTPH.h"

//For IC2 intilization
BME280 TphIc2 = BME280(D1, D0, DEFAULT_SLAVE_ADDRESS); 
//DO en D1 voor NUCLEO_L432KC
//D14 en D15 voor NUCLEO_F042K6

//PC serial initilization
Serial pc(USBTX, USBRX); //tx, rx


int main(){
    pc.printf("\n IC2");
    printf("TEST");
    pc.printf("\n Humidity = %lf", TphIc2.getHumidity());
    pc.printf("\n Temperature = %lf", TphIc2.getTemperature());
    pc.printf("\n Presure = %lf", TphIc2.getPressure());
}