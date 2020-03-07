#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"

int main(void) {
    SimpleLoRaWAN::Node node(keys, pins);

    char addr = 0x76 << 1;  // I2C address of TPH sensor
    BME280 tph_sensor = BME280(D14, D15, addr);

    while (true) {
        double temperature = (double) tph_sensor.getTemperature();  // value in °C
        double humidity = (double) tph_sensor.getHumidity();        // value in %
        double pressure = (double) tph_sensor.getPressure();        // value in hPa
        double pm25 = 12.3;          // value in µg/m³
        double pm10 = 23.4;          // value in µg/m³

        ParticulaLora::AmbiantSensorMessage message;

        temperature = (double) tph_sensor.getTemperature();
        humidity = (double) tph_sensor.getHumidity();
        pressure = (double) tph_sensor.getPressure();

        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);

        node.send(message.getMessage(), message.getLength());
        wait(30.0);
    }
    return 0;
}