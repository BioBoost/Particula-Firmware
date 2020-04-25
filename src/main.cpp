#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

Serial pc(USBTX, USBRX);

using namespace Particula;

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

int main(void) {
    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");
    
    /**
     * Binary coded error values
     * bit 0    Particle Sensor Wake-up
     * bit 1    Particle Sensor Read
     * bit 2    not used
     * bit 3    not used
     * bit 4    not used
     * bit 5    TPH Sensor Wake-up
     * bit 6    TPH Sensor Read
     * bit 7    not used
     * bit 8    not used
     * bit 9    not used
     * bit 10   Battery Charge Output STAT1/-LBO    // See MCP73871 Datasheet page 20 Table 5-1 for more information
     * bit 11   Battery Charge Output STAT2
     * bit 12   Battery Charge Output -PG
     * bit 13   not used
     * bit 14   not used
     * bit 15   not used
    */
   char error_values = 0x00;
    
    while (true) {
        AmbiantSensorMessage message;    // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");
        
        if(part_sensor.wakeUp() == WAKEUP_SUCCESFULL){
            pc.printf("[Particle sensor] wake up has been successfull \r\n");
            error_values |= (1u);         // Set bit 0: 1 for successfull wakeup
        } else {
            pc.printf("[Particle sensor] wake up hasn't been successfull \r\n");
            error_values &= ~(1u);      // Set bit 0: 0 for unsuccessfull wakeup
        }

        if(part_sensor.read() == READ_SUCCESSFULL){
            pc.printf("[Particle sensor] read has been successfull \r\n");
            error_values |= (1u << 1);  // Set bit 1: 1 for successfull read
        } else {
            pc.printf("[Particle sensor] read hasn't been successfull \r\n");
            error_values &= ~(1u << 1); // Set bit 1: 0 for unsuccessfull read
        }

        // Set bits 5 and 6 to 1 for testing status codes
        error_values |= (1u << 5);  // Set bit 1: 1 for successfull read
        error_values |= (1u << 6);  // Set bit 1: 1 for successfull read

        double temperature = (double) tph_sensor.getTemperature();  // value in °C
        double humidity = (double) tph_sensor.getHumidity();        // value in %
        double pressure = (double) tph_sensor.getPressure();        // value in hPa
        double pm25 = part_sensor.getPM25Value();          // value in µg/m³
        double pm10 = part_sensor.getPM10Value();          // value in µg/m³

        pc.printf("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
        pc.printf("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
        pc.printf("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
        pc.printf("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
        pc.printf("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);


        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);
        message.addError(error_values << 8);
        message.addError(error_values);

        node.send(message.getMessage(), message.getLength());
        if(part_sensor.sleep() == SLEEP_SUCCESSFULL){
            pc.printf("[Particle sensor] sleep has been succesfull \r\n");
        } else {
            pc.printf("[Particle sensor] sleep hasn't been succesfull \r\n");
        }
        ThisThread::sleep_for(30000);
    }
    return 0;
}