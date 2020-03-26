#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011_Particle::SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

int main(void) {
    while (true) {

        /* read battery voltage */
        /* all values are to be determined after closer evaluation */
        if(percentage >= 40.0){   
            // depending of current percentage, determine sleeping time to come
        } else if(percentage >= 20.0){
            // disable part sensor, read only tph, send with lorawan
        } else if(percentage >= 10.0){
            // disable lorawan, write all data to eeprom
        } else {
            // read nothing, send low battery code vb (0 0 0 0 0)
            ParticulaLora::AmbiantSensorMessage message;
            message.addTemperature(0.0);
            message.addHumidity(0.0);
            message.addPressure(0.0);
            message.addPM(0.0);
            message.addPM(0.0);
            node.send(message.getMessage(), message.getLength());
            /* put particula to deep sleep */
            ThisThread::sleep_for(43200000);       //12h = 12 * 60 * 60 *1000 = 43200 000
        }
    }
    return 0;
}