#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011_Particle::SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

int main(void) {
<<<<<<< HEAD

    const int PART_SENSOR_WAKEUP_ERROR = 0x00;
    const int PART_SENSOR_READ_ERROR = 0x01;
    const int PART_SENSOR_SLEEP_ERROR = 0x02;


    while (true) {
        ParticulaLora::AmbiantSensorMessage message;

        double percentage = readBattery();

        if(percentage >= 20.0){   
            if( part_sensor.wakeUp() == WAKEUP_NOT_SUCCESFULL ){}        
                message.addError(PART_SENSOR_WAKEUP_ERROR);
            }

            ThisThread::sleep_for(30000);     
            tph_sensor.awake();
            
            if(part_sensor.read() == READ_SUCCESSFULL){
                message.addPM(part_sensor.getPM25Value());
                message.addPM(part_sensor.getPM10Value());
            } else {
                message.addError(PART_SENSOR_READ_ERROR);
            }

            message.addTemperature((double) tph_sensor.getTemperature());
            message.addHumidity((double) tph_sensor.getHumidity());
            message.addPressure((double) tph_sensor.getPressure());

            if(part_sensor.sleep() == SLEEP_NOT_SUCCESFULL){
            } 
                message.addError(PART_SENSOR_SLEEP_ERROR);
            }
            tph_sensor.sleep();
            node.send(message.getMessage(), message.getLength());           
                  
        } else if(percentage <= 20.0){
            ThisThread::sleep_for(30000);
        }
        ThisThread::sleep_for(270000); 
=======
    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");

    while (true) {
        AmbiantSensorMessage message;    // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");
        
        if(part_sensor.wakeUp() == WAKEUP_SUCCESFULL){
            pc.printf("[Particle sensor] wake up has been succesfull \r\n");
        } else {
            pc.printf("[Particle sensor] wake up hasn't been succesfull \r\n");
        }

        if(part_sensor.read() == READ_SUCCESSFULL){
            pc.printf("[Particle sensor] read has been succesfull \r\n");
        } else {
            pc.printf("[Particle sensor] read hasn't been succesfull \r\n");
        }
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

        if(part_sensor.sleep() == SLEEP_SUCCESSFULL){
            pc.printf("[Particle sensor] sleep has been succesfull \r\n");
        } else {
            pc.printf("[Particle sensor] sleep hasn't been succesfull \r\n");
        }
        
        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(30000);
>>>>>>> ce05b5b521b62c2ab00df670884ba8f3e6ec30ca
    }
    return 0;
}

double readBattery(){
    return 80.0;
}