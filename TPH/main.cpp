#include <mbed.h>
#include "BME280_SPI.h"
#include "BME280.h"

//For SPI Initalization
int PinSpiTphMosi = null;   // #CONF
int PinSpiTphMiso = null;   // #CONF
int PinSpiTphSclk = null;   // #CONF
int PinSpiTphCs = null;     // #CONF
BME280_SPI TphSpi = BME280_SPI(PinSpiTphMosi, PinSpiTphMiso, PinSpiTphSclk, PinSpiTphCs);

//For IC2 intilization
int PinIC2TphSda = null;    // #CONF
int PinIC2TphScl = null;    // #CONF
char SlaveAddress = null;   // #CONF
BME280 TphIc2 = BME280(PinIC2TphSda, PinIC2TphScl, SlaveAddress);

//PC serial initilization
Serial pc(USBTX, USBRX); //tx, rx


int main(){
    pc.fprint("IC2");
    pc.fprint("Humidity = " + TphIc2.getHumidity);
    pc.fprint("Temperature = " + TphIc2.getTemperature);
    pc.fprint("Presure = " = TphIc2.getPressure);
    pc.fprint("SPI");
    pc.fprint("Humidity = " + TphSpi.getHumidity);
    pc.fprint("Temperature = " + TphSpi.getTemperature);
    pc.fprint("Presure = " = TphSpi.getPressure);
}