#include "mbed.h"
#include "BME280.h"
//#include "i2cTPH.h"

//For IC2 intilization
BME280 TphIc2 = BME280(D14, D15, DEFAULT_SLAVE_ADDRESS);

//PC serial initilization
Serial pc(USBTX, USBRX); //tx, rx


int main(){
    pc.printf("\n IC2");
    printf("TEST");
    pc.printf("\n Humidity = %lf", TphIc2.getHumidity());
    pc.printf("\n Temperature = %lf", TphIc2.getTemperature());
    pc.printf("\n Presure = %lf", TphIc2.getPressure());
}