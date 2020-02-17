#include <mbed.h>
#include "BME280.h"

//For IC2 intilization
int PinIC2TphSda = null;    // #CONF
int PinIC2TphScl = null;    // #CONF
char SlaveAddress = null;   // #CONF
BME280 TphIc2 = BME280(PinIC2TphSda, PinIC2TphScl, SlaveAddress);

//PC serial initilization
Serial pc(USBTX, USBRX); //tx, rx


int main(){
    pc.fprint("IC2");
    pc.fprint("Humidity = " + TphIc2.getHumidity());
    pc.fprint("Temperature = " + TphIc2.getTemperature());
    pc.fprint("Presure = " = TphIc2.getPressure());
}