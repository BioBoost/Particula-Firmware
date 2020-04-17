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

int main(void) {

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
    }
    return 0;
}

double readBattery(){
    return 80.0;
}