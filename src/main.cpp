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

using namespace Particula;

mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

int main(void) {

    consoleMessage("\r\n\r\n[Particula] Loading Firmware ...\r\n");

    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    BME280 tph_sensor(&i2c_com);
    HardwareStatus hardwareStatus;
    BatteryManagement batterymanager(CHARGE_STATUS_1,CHARGE_STATUS_2,POWER_GOOD);
    ParticulaApp particulaApp;
    SDS011 part_sensor(UART_TX_PIN,UART_RX_PIN);  // D1 en D0 voor kleine nucleo

    AmbiantSensorMessage versionMessage;
    versionMessage.addVersionNumber(VERSION);
    node.send(versionMessage.getMessage(), versionMessage.getLength());

    while (true) {
        HardwareStatus hardwareStatus;
        const int SLEEP_TIME = MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME;

        if (!batterymanager.BatterySufficient(&hardwareStatus)) {
            printf("it's looping \r\n");
            continue;
        }

        AmbiantSensorMessage message;   // Must be placed here, new values will otherwise be added to the same message
        consoleMessage("\r\n[Particula] Taking measurements ...\r\n");


        /**
         * Particle sensor wakeup
         */
        char * partSensorWakeup = particulaApp.partSensorWake(&part_sensor, &hardwareStatus);
        consoleMessage(partSensorWakeup);        
 
        /**
         * Sleep 30 sec. After this time particle sensor measurements are considered correct
         */
        ThisThread::sleep_for(PART_SENS_WARMUP_TIME);             
             
        /**
         * Particle sensor takes measurements
         */
        char * partSensorRead = particulaApp.partSensorRead(&part_sensor, &hardwareStatus);
        consoleMessage(partSensorRead);

        /**
         * Particle sensor goes to sleep
         */
        char * partSensorSleep = particulaApp.partSensorSleep(&part_sensor, &hardwareStatus);
        consoleMessage(partSensorSleep);

        /**
         * TPH sensor wakeup
         */
        char * tphSensorWakeup = particulaApp.tphSensorWake(&tph_sensor, &hardwareStatus);
        consoleMessage(tphSensorWakeup);

        /**
         * TPH sensor save measurements to add to LoRa message and check if measurements are valid
         */
        char * tphSensorRead = particulaApp.tphSensorRead(&tph_sensor, &hardwareStatus);
        consoleMessage(tphSensorRead);      

        /**
         * TPH sensor goes to sleep
         */
        char * tphSensorSleep = particulaApp.tphSensorSleep(&tph_sensor);
        consoleMessage(tphSensorSleep);

        /**
         * All sensor measurements and binary coded errors added to LoRa message
         */
        char * addToLoraMessage = particulaApp.addToLoRaMessage(&message, &hardwareStatus);
        consoleMessage(addToLoraMessage);
        consoleMessage("[Particula] Hardware status (hex): %X \r\n", hardwareStatus.get_state());

        /**
         * Print out measurements to console for development purposes
         */
        consoleMessage("[Particula] Measered temperature:  %4.2f °C\r\n", particulaApp.returnTemperature());
        consoleMessage("[Particula] Measered humidity:     %4.2f %%\r\n", particulaApp.returnHumidity());
        consoleMessage("[Particula] Measered pressure:     %4.2f hPa\r\n", particulaApp.returnPressure());
        consoleMessage("[Particula] Measered PM25:         %4.2f µg/m3\r\n", particulaApp.returnPm25());
        consoleMessage("[Particula] Measered PM10:         %4.2f µg/m3\r\n", particulaApp.returnPm10());

        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(SLEEP_TIME);            
    }
    return 0;
}

