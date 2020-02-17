#include <mbed.h>
#include "BME280_SPI.h"

//For SPI Initalization
int PinSpiTphMosi = null;   // #CONF
int PinSpiTphMiso = null;   // #CONF
int PinSpiTphSclk = null;   // #CONF
int PinSpiTphCs = null;     // #CONF
BME280_SPI TphSpi = BME280_SPI(PinSpiTphMosi, PinSpiTphMiso, PinSpiTphSclk, PinSpiTphCs);

//PC serial initilization
Serial pc(USBTX, USBRX); //tx, rx


int main(){
    pc.fprint("SPI");
    pc.fprint("Humidity = " + TphSpi.getHumidity());
    pc.fprint("Temperature = " + TphSpi.getTemperature());
    pc.fprint("Presure = " = TphSpi.getPressure());
}