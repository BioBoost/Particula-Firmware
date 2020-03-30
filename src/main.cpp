#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

Serial pc(USBTX, USBRX);

SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // D4 en D5 voor kleine nucleo
SDS011_Particle::SDS011 part_sensor(D1, D0);  // D1 en D0 voor kleine nucleo

// Timeout timeout;
// bool check = false;
// void stop_loop() {
//     check = true;
//     // pc.printf("\r\n[Particula] stop_loop function is called ...\r\n");
// }

// Implement a watchdog timer

int main(void) {
    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");

    while (true) {
        ParticulaLora::AmbiantSensorMessage message;    // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");
        double temperature = (double) tph_sensor.getTemperature();  // value in °C
        double humidity = (double) tph_sensor.getHumidity();        // value in %
        double pressure = (double) tph_sensor.getPressure();        // value in hPa

        double pm25 = 0.0;
        double pm10 = 0.0;

        // timeout.attach(&stop_loop, 5.0);
        try
        {
            pc.printf("\r\n[Particula] Try to wake up Particle Sensor ...\r\n");
            part_sensor.wakeUp();
            pc.printf("[Particula] Particle Sensor Woke Up\r\n");
        }
        catch(const std::exception& e)
        {
            // std::cerr << e.what() << '\n';
            pc.printf("\r\n[Particula] Could not wake up Particle Sensor.\r\n");
        }
        

            while(!part_sensor.read());   // makes sure it has read a correct value
            pc.printf("[Particula] Particle Sensor Values Are Being Read\r\n");

            pm25 = part_sensor.getPM25Value();          // value in µg/m³
            pm10 = part_sensor.getPM10Value();          // value in µg/m³


        

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

        node.send(message.getMessage(), message.getLength());
        part_sensor.sleep();
        pc.printf("[Particula] Going to sleep, deep sleep possible: %i\r\n", sleep_manager_can_deep_sleep());
        ThisThread::sleep_for(30000);
    }
    return 0;
}