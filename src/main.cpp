#pragma once

#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"

SimpleLoRaWAN::Node node(keys, pins);

int main(void) {

    double temperature = 22.64;  // value in °C
    double humidity = 9.55;      // value in %
    double pressure = 1013.5;    // value in hPa
    double pm25 = 12.3;          // value in µg/m³
    double pm10 = 23.4;          // value in µg/m³

    ParticulaLora::AmbiantSensorMessage message;

    message.addTemperature(temperature);
    message.addHumidity(humidity);
    message.addPressure(pressure);
    message.addPM(pm25);
    message.addPM(pm10);

    node.send(message.getMessage(), message.getLength());
    wait(30.0);

    return 0;
}