#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
ParticulaLora::AmbiantSensorMessage message;

int main(void) {
    while (true) {
        double temperature = (double) tph_sensor.getTemperature();  // value in °C
        double humidity = (double) tph_sensor.getHumidity();        // value in %
        double pressure = (double) tph_sensor.getPressure();        // value in hPa
        double pm25 = 12.3;          // value in µg/m³
        double pm10 = 23.4;          // value in µg/m³

        temperature = (double) tph_sensor.getTemperature();
        humidity = (double) tph_sensor.getHumidity();
        pressure = (double) tph_sensor.getPressure();

        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);

        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(30000);
    }
    return 0;
}