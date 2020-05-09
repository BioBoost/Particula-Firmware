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


mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

/* disclaimer pins are not right yet */
DigitalIn stat1(D5);
DigitalIn stat2(D4);
DigitalIn PG(D3);

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
        consoleMessage(particulaApp.partSensorWake(&part_sensor,&hardwareStatus),0);        
 
        /**
         * Sleep 30 sec. After this time particle sensor measurements are considered correct
         */
        ThisThread::sleep_for(PART_SENS_WARMUP_TIME);             
    
        /**
         * Particle sensor takes measurements
         */
        consoleMessage(particulaApp.partSensorRead(&part_sensor,&hardwareStatus),0);

        /**
         * Particle sensor goes to sleep
         */
        consoleMessage(particulaApp.partSensorSleep(&part_sensor,&hardwareStatus),0);

        /**
         * TPH sensor wakeup
         */
        consoleMessage(particulaApp.tphSensorWake(&tph_sensor,&hardwareStatus),0);

        /**
         * TPH sensor save measurements to add to LoRa message and check if measurements are valid
         */
        consoleMessage(particulaApp.tphSensorRead(&tph_sensor,&hardwareStatus),0);      

        /**
         * TPH sensor goes to sleep
         */
        consoleMessage(particulaApp.tphSensorSleep(&tph_sensor),0);

        /**
         * All sensor measurements and binary coded errors added to LoRa message
         */
        consoleMessage(particulaApp.addToLoRaMessage(&message,&hardwareStatus),0);
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

