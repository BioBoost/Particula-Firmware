#include "settings.h"

mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

/* disclaimer pins are not right yet */
DigitalIn stat1(D5);
DigitalIn stat2(D4);
DigitalIn PG(D3);

using namespace Particula;

bool readBatteryStatus(char*);


int main(void) {

    consoleMessage("\r\n\r\n[Particula] Loading Firmware ...", 0);
    
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
        consoleMessage("\r\n[Particula] Taking measurements ...\r\n", 0);

        if (readBatteryStatus(&error_values)) {

            /**
             * Particle sensor wakeup
             */
            if(part_sensor.wakeUp() == WAKEUP_SUCCESSFULL){
                consoleMessage("[Particle sensor] wake up has been successfull \r\n", 0);
                error_values |= (1u);       // 1 for successfull wakeup
            } else {
                consoleMessage("[Particle sensor] wake up hasn't been successfull \r\n", 0);
                error_values &= ~(1u);      // 0 for unsuccessfull wakeup
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
                error_values |= (1u << 1);  // 1 for successfull read
            } else {
                consoleMessage("[Particle sensor] read hasn't been successfull \r\n", 0);
                error_values &= ~(1u << 1); // 0 for unsuccessfull read
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
                error_values |= (1u << 2);  // Set bit 2: 1 for successfull sleep
            } else {
                consoleMessage("[Particle sensor] sleep hasn't been successfull \r\n", 0);
                error_values &= ~(1u << 2); // Set bit 2: 0 for unsuccessfull sleep
            }


            /**
             * TPH sensor wakeup
             */
            tph_sensor.awake();
            if (tph_sensor.present()) {
                consoleMessage("[TPH sensor] sensor is present \r\n", 0);
                error_values |= (1u << 5);  // 1 for successfull wakeup
            } else {
                consoleMessage("[TPH sensor] sensor is not present \r\n", 0);
                error_values &= ~(1u << 5); // 0 for unsuccessfull wakeup
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
                error_values |= (1u << 6);  // 1 for successfull read
            } else {
                consoleMessage("[TPH sensor] read has been unsuccessful \r\n", 0);
                error_values &= ~(1u << 6);  // 0 for unsuccessfull read
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
            message.addStatus(error_values);
            node.send(message.getMessage(), message.getLength());           
                  
        } else {
            ThisThread::sleep_for(PART_SENS_WARMUP_TIME);
        }
        ThisThread::sleep_for(MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME); 
    }
    return 0;
}

bool readBatteryStatus(char * error_values){
    if(stat1 == 1){
        (*error_values) |= (1u << 10);
    } else {
        (*error_values) &= ~(1u << 10);
    }

    if(stat2 == 1){
        (*error_values) |= (1u << 11);
    } else {
        (*error_values) &= ~(1u << 11);
    }

    if(PG == 1){
        (*error_values) |= (1u << 12);
    } else {
        (*error_values) &= ~(1u << 12);
    }

    if (stat1 == 0 && stat2 == 1 && PG==1){
        return false;
    }

    return true;
}