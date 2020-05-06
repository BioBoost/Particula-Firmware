#include "settings.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>
#include "mbed.h"
#include "Simple-LoRaWAN.h"
#include "AmbiantSensorMessage.h"
#include "../lib/hardwarestatus.h"

mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

/* disclaimer pins are not right yet */
DigitalIn stat1(D5);
DigitalIn stat2(D4);
DigitalIn PG(D3);

using namespace Particula;

bool readBatteryStatus(HardwareStatus *);


int main(void) {

    consoleMessage("\r\n\r\n[Particula] Loading Firmware ...", 0);

    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    BME280 tph_sensor(&i2c_com);
    SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);  // D1 en D0 voor kleine nucleo

    AmbiantSensorMessage versionMessage;
    versionMessage.addVersionNumber(VERSION);
    node.send(versionMessage.getMessage(), versionMessage.getLength());

    while (true) {
        HardwareStatus hardwareStatus;
        ThisThread::sleep_for(MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME); 

        if (!readBatteryStatus(&hardwareStatus)) {
            continue;
        }

        AmbiantSensorMessage message;   // Must be placed here, new values will otherwise be added to the same message
        consoleMessage("\r\n[Particula] Taking measurements ...\r\n", 0);


        /**
         * Particle sensor wakeup
         */
        if(part_sensor.wakeUp() == WAKEUP_SUCCESSFULL){
            consoleMessage("[Particle sensor] wake up has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] wake up hasn't been successfull \r\n", 0);
            hardwareStatus.particle_wakeup_failed();
        }


        /**
         * Sleep 30 sec. After this time particle sensor measurements are considered correct
         */
        ThisThread::sleep_for(PART_SENS_WARMUP_TIME);     
        
    
        /**
         * Particle sensor takes measurements
         */
        if(part_sensor.read() == READ_SUCCESSFULL){
            consoleMessage("[Particle sensor] read has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] read hasn't been successfull \r\n", 0);
            hardwareStatus.particle_read_failed();
        }


        /**
         * Particle sensor save measurements to add to LoRa message
         */
        double pm25 = part_sensor.getPM25Value();          // value in µg/m³
        double pm10 = part_sensor.getPM10Value();          // value in µg/m³


        /**
         * Particle sensor goes to sleep
         */
        if (part_sensor.sleep() == SLEEP_SUCCESSFULL) {
            consoleMessage("[Particle sensor] sleep has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] sleep hasn't been successfull \r\n", 0);
            hardwareStatus.particle_sleep_failed();
        }


        /**
         * TPH sensor wakeup
         */
        tph_sensor.awake();
        if (tph_sensor.present()) {
            consoleMessage("[TPH sensor] sensor is present \r\n", 0);
        } else {
            consoleMessage("[TPH sensor] sensor is not present \r\n", 0);
            hardwareStatus.tph_wakeup_failed();
        }


        /**
         * TPH sensor save measurements to add to LoRa message
         */
        bool temperatureValueCorrect = false;
        bool humidityValueCorrect = false;
        bool pressureValueCorrect = false;
        double temperature = tph_sensor.temperature(&temperatureValueCorrect);  // value in °C
        double humidity = tph_sensor.humidity(&humidityValueCorrect);           // value in %
        double pressure = tph_sensor.presure(&pressureValueCorrect);            // value in hPa


        /**
         *  TPH sensor check if measurements are valid
         */
        if (temperatureValueCorrect && humidityValueCorrect && pressureValueCorrect) {
            consoleMessage("[TPH sensor] read has been successful \r\n", 0);
        } else {
            consoleMessage("[TPH sensor] read has been unsuccessful \r\n", 0);
            hardwareStatus.tph_read_failed();
        }


        /**
         * TPH sensor goes to sleep
         */
        tph_sensor.sleep();


        /**
         * All sensor measurements added to LoRa message
         */
        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);


        /**
         * Print out measurements to console for development purposes
         */
        consoleMessage("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
        consoleMessage("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
        consoleMessage("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
        consoleMessage("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
        consoleMessage("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);


        /**
         * Add binary coded errors to LoRa message and send the message
         */
        if (hardwareStatus.errors()) {
            consoleMessage("[Particula] Errors detected, adding them to lora message \r\n", 0);
            message.addStatus(hardwareStatus.get_state());
        } else {
            consoleMessage("[Particula] No errors detected \r\n", 0);
        }
        node.send(message.getMessage(), message.getLength());           
    }
    return 0;
}

bool readBatteryStatus(HardwareStatus * hardwareStatus){
    if(stat1 == 1){
        (*hardwareStatus).set_stat1();
    }

    if(stat2 == 1){
        (*hardwareStatus).set_stat2();
    }

    if(PG == 1){
        (*hardwareStatus).set_pg();
    }

    if (stat1 == 0 && stat2 == 1 && PG==1){
        return false;
    }

    return true;
}