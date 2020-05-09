#include "settings.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>
#include "mbed.h"
#include "Simple-LoRaWAN.h"
#include "AmbiantSensorMessage.h"
#include "../lib/hardwarestatus.h"
#include "BatteryManagement.h"
#include "ParticulaApp.h"


//mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

/* disclaimer pins are not right yet */
// DigitalIn stat1(D5);
// DigitalIn stat2(D4);
// DigitalIn PG(D3);

using namespace Particula;



int main(void) {

    consoleMessage("\r\n\r\n[Particula] Loading Firmware ...", 0);

    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    BME280 tph_sensor(&i2c_com);
    HardwareStatus hardwareStatus;
    BatteryManagement batterymanager(stat1,stat2,PG);
    ParticulaApp particulaApp;
    SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);  // D1 en D0 voor kleine nucleo

    AmbiantSensorMessage versionMessage;
    versionMessage.addVersionNumber(VERSION);
    node.send(versionMessage.getMessage(), versionMessage.getLength());

    while (true) {
        HardwareStatus hardwareStatus;
        const int SLEEP_TIME = MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME;

        if (!batterymanager.BatterySufficient(&hardwareStatus)) {
            continue;
        }

        AmbiantSensorMessage message;   // Must be placed here, new values will otherwise be added to the same message
        consoleMessage("\r\n[Particula] Taking measurements ...\r\n", 0);


        /**
         * Particle sensor wakeup
         */
        if(particulaApp.partSensorWake(&part_sensor,&hardwareStatus)){
            consoleMessage("[Particle sensor] wake up has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] wake up hasn't been successfull \r\n", 0);
        }


        /**
         * Sleep 30 sec. After this time particle sensor measurements are considered correct
         */
        ThisThread::sleep_for(PART_SENS_WARMUP_TIME);     
        
    
        /**
         * Particle sensor takes measurements
         */
        if(particulaApp.partSensorRead(&part_sensor,&hardwareStatus)){
            consoleMessage("[Particle sensor] read has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] read hasn't been successfull \r\n", 0);
        }


        /**
         * Particle sensor goes to sleep
         */
        if (particulaApp.partSensorSleep(&part_sensor,&hardwareStatus)) {
            consoleMessage("[Particle sensor] sleep has been successfull \r\n", 0);
        } else {
            consoleMessage("[Particle sensor] sleep hasn't been successfull \r\n", 0);
        }


        /**
         * TPH sensor wakeup
         */
        if (particulaApp.tphSensorWake(&tph_sensor,&hardwareStatus)) {
            consoleMessage("[TPH sensor] sensor is present \r\n", 0);
        } else {
            consoleMessage("[TPH sensor] sensor is not present \r\n", 0);
        }


        /**
         * TPH sensor save measurements to add to LoRa message and check if measurements are valid
         */

        
        if (particulaApp.tphSensorRead(&tph_sensor,&hardwareStatus)){
            consoleMessage("[TPH sensor] read has been successful \r\n", 0);
        } else {
            consoleMessage("[TPH sensor] read has been unsuccessful \r\n", 0);
        }


        /**
         * TPH sensor goes to sleep
         */
        particulaApp.tphSensorSleep(&tph_sensor);


        /**
         * All sensor measurements added to LoRa message
         */
        particulaApp.addToLoRaMessage(&message);

        /**
         * Print out measurements to console for development purposes
         */

        // !!!!to be remade!!!! //
        // consoleMessage("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
        // consoleMessage("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
        // consoleMessage("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
        // consoleMessage("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
        // consoleMessage("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);


        /**
         * Add binary coded errors to LoRa message and send the message
         */
        // !!!!to be remade!!!! //
        if (hardwareStatus.errors()) {
            consoleMessage("[Particula] Errors detected, adding them to lora message \r\n", 0);
            consoleMessage("[Particula] Hardware status (hex): %X \r\n", hardwareStatus.get_state());
            message.addStatus(hardwareStatus.get_state());
        } else {
            consoleMessage("[Particula] No errors detected \r\n", 0);
        }
        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(SLEEP_TIME);            
    }
    return 0;
}

