#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011_Particle::SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

int main(void) {
    while (true) {
        double percentage = readBattery();

        if(percentage >= 20.0){   
            try{
                part_sensor.wakeUp();
            } catch(exception e){
                // add error to lora message
            }

            ThisThread::sleep_for(30000);     
            tph_sensor.awake();
            ParticulaLora::AmbiantSensorMessage message;
            
            try {
                while(!part_sensor.read());
                message.addPM(part_sensor.getPM25Value());
                message.addPM(part_sensor.getPM10Value());
            } catch(Exception e){
                // add error to Lora message 
            }

            message.addTemperature((double) tph_sensor.getTemperature());
            message.addHumidity((double) tph_sensor.getHumidity());
            message.addPressure((double) tph_sensor.getPressure());

            try {
                part_sensor.sleep();
            } catch(Exception e){
                // add error to Lora message
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