# Particula-Firmware


## Getting started
Clone the project, install the dependencies and setup your mbed development environment to compile the firmware and flash it.

```PowerShell
git clone git@github.com:vives-projectwerk-2-2020/Particula-Firmware.git
cd Particula-Firmware

mbed new .              // Create new mbed project and download mbed-os library
mbed deploy             // Install dependencies
mbed toolchain GCC_ARM  // Setting default toolchain (ARM, ARMC5, ARMC6, IAR, GCC_ARM)
mbed target detect      // Setting target to automatically detect connected device
```

Now have a look at the project and use the `settings.example.h` in the `src/` directory as a template for your `settings.h` file. You should add the following keys: `devEui`, `appEui`, `appKey`. All of these can be retrieved from a TTN application.

When this file is set up, run the following command:

```PowerShell
mbed compile -f         // When  device is connected, compile firmware and flash device
```

## Code example

```cpp
#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"

SimpleLoRaWAN::Node node(keys, pins);
BME280 tph_sensor = BME280(D14, D15, 0x76 << 1); // Use pin-names and I2C address of your device(s)
SDS011 part_sensor(D1,D0);
int main(void) {
    while (true) {
        ParticulaLora::AmbiantSensorMessage message;
        part_sensor.wakeUp();
        while(!part_sensor.read());     // makes sure it has read a correct value
        double temperature = (double) tph_sensor.getTemperature();  // value in °C
        double humidity = (double) tph_sensor.getHumidity();        // value in %
        double pressure = (double) tph_sensor.getPressure();        // value in hPa
        double pm25 = part_sensor.getPM25Value();   // value in µg/m³
        double pm10 = part_sensor.getPM10Value();   // value in µg/m³

        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);

        node.send(message.getMessage(), message.getLength());
        part_sensor.sleep();
        ThisThread::sleep_for(30000);
    }
    return 0;
}
```

## Used modules and libraries

### LoRaWAN RFM95W Transceiver (over ISP)
- [Mbed-Simple-LoRaWAN](https://github.com/sillevl/mbed-Simple-LoRaWAN)
- [AmbientSensorMessage](https://github.com/vives-projectwerk-2-2020/AmbiantSensorMessage)

### Bosch BME280 TPH Sensor (over I2C)
- [ParticulaTPH](https://github.com/vives-projectwerk-2-2020/ParticulaTPH)

### SDS011 Particel sensor (over UART)
- [SDS011_Library](https://github.com/vives-projectwerk-2-2020/SDS011_Library)
 
### EEPROM (over I2C)
