#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

Serial pc(USBTX, USBRX);

using namespace Particula;

int main(void) {
    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    
    BME280 tph_sensor = BME280(
        I2C_SDA_PIN,
        I2C_SCK_PIN,
        0x76 << 1
    );

    SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);
    
    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");

    while (true) {
        AmbiantSensorMessage message;    // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");

        int sds011WakeUpStatus = part_sensor.wakeUp();
        int sds011ReadStatus = part_sensor.read();

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

        // Add status (error) codes
        message.addError(sds011WakeUpStatus);
        message.addError(sds011ReadStatus);

        node.send(message.getMessage(), message.getLength());
        part_sensor.sleep();
        pc.printf("[Particula] Going to sleep, deep sleep possible (1: yes, 0: no): %i\r\n", sleep_manager_can_deep_sleep());
        ThisThread::sleep_for(30000);
    }
    return 0;
}