#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>

Serial pc(USBTX, USBRX);
mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);
double readBattery(void);

using namespace Particula;

int main(void) {
    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");
    
    /**
     * Binary coded error values
     * bit 0    Particle Sensor Wake-up Successful
     * bit 1    Particle Sensor Read Successful
     * bit 2    Particle Sensor Sleep Successful
     * bit 3    not used
     * bit 4    not used
     * bit 5    TPH Sensor Wake-up Successful
     * bit 6    TPH Sensor Read Successful
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

    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    BME280 tph_sensor(&i2c_com);
    SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);  // D1 en D0 voor kleine nucleo

    while (true) {
        AmbiantSensorMessage message;   // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");

        double percentage = readBattery();

        if (percentage >= 20.0) {

            /**
             * Particle sensor wakeup
             */
            if(part_sensor.wakeUp() == WAKEUP_SUCCESSFULL){
                pc.printf("[Particle sensor] wake up has been successfull \r\n");
                error_values |= (1u);       // 1 for successfull wakeup
            } else {
                pc.printf("[Particle sensor] wake up hasn't been successfull \r\n");
                error_values &= ~(1u);      // 0 for unsuccessfull wakeup
            }


            /**
             * Sleep 30 sec. After this time particle sensor measurements are considered correct
             */
            ThisThread::sleep_for(30000);     
            
        
            /**
             * Particle sensor takes measurements
             */
            if(part_sensor.read() == READ_SUCCESSFULL){
                pc.printf("[Particle sensor] read has been successfull \r\n");
                error_values |= (1u << 1);  // 1 for successfull read
            } else {
                pc.printf("[Particle sensor] read hasn't been successfull \r\n");
                error_values &= ~(1u << 1); // 0 for unsuccessfull read
            }


            /**
             * Particle sensor measurements added to LoRa message
             */
            double pm25 = part_sensor.getPM25Value();          // value in µg/m³
            double pm10 = part_sensor.getPM10Value();          // value in µg/m³
            message.addPM(pm25);
            message.addPM(pm10);


            /**
             * Particle sensor goes to sleep
             */
            if (part_sensor.sleep() == SLEEP_SUCCESSFULL) {
                pc.printf("[Particle sensor] sleep has been successfull \r\n");
                error_values |= (1u << 2);  // Set bit 1: 1 for successfull sleep
            } else {
                pc.printf("[Particle sensor] sleep hasn't been successfull \r\n");
                error_values &= ~(1u << 2); // Set bit 1: 0 for unsuccessfull sleep
            }

            error_values |= (1u << 5);  // 1 for successfull wakeup

            /**
             * TPH sensor wakeup
             */
            tph_sensor.awake();
            if (tph_sensor.present()) {
                pc.printf("[TPH sensor] sensor is present \r\n");
                error_values |= (1u << 5);  // 1 for successfull wakeup
            } else {
                pc.printf("[TPH sensor] sensor is not present \r\n");
                error_values &= ~(1u << 5); // 0 for unsuccessfull wakeup
            }


            /**
             * TPH sensor taking measurements
             */
            double temperature = tph_sensor.temperature();  // value in °C
            double humidity = tph_sensor.humidity();        // value in %
            double pressure = tph_sensor.presure();        // value in hPa


            /**
             * TPH sensor goes to sleep
             */
            tph_sensor.sleep();


            /**
             * TPH sensor measurements added to LoRa message
             */
            message.addTemperature(temperature);
            message.addHumidity(humidity);
            message.addPressure(pressure);


            /**
             * Print out measurements to console for development purposes
             */
            pc.printf("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
            pc.printf("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
            pc.printf("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
            pc.printf("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
            pc.printf("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);


            /**
             * Add binary coded error to LoRa message and send the message
             */
            message.addStatus(error_values);
            node.send(message.getMessage(), message.getLength());           
                  
        } else if(percentage <= 20.0){
            ThisThread::sleep_for(30000);
        }
        ThisThread::sleep_for(270000); 
    }
    return 0;
}

double readBattery(void){
    // class yet to be made 
    return 80.0;
}