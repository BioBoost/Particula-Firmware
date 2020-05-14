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
        const int SLEEP_TIME = MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME;

        if (!batterymanager.BatterySufficient(&hardwareStatus)) {
            ThisThread::sleep_for(MEASUREMENT_INTERVAL);
            continue;
        }
        AmbiantSensorMessage message;   
        consoleMessage("\r\n[Particula] Taking measurements ...%d\r\n", 0);

        particulaApp.partMeasureCycle(&part_sensor,&hardwareStatus);

        particulaApp.tphMeasureCycle(&tph_sensor,&hardwareStatus);
       
        particulaApp.LoRaWANMakeCycle(&message,&hardwareStatus);
        
        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(SLEEP_TIME);            
    }
    return 0;
}

