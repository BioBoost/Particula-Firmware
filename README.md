# Particula-Firmware

![Markdown Linter](../../workflows/Markdown%20Linter/badge.svg?branch=master)

This is the firmware required to make the Particula project work. The particula project is a Smart Self-Sufficient Open Wireless Air Quality Sensor. This IoT device makes use of a particle sensor, a tph sensor and LoRaWAN to transmit its data. 

The particle sensor reads the particles of 2.5 and 10 µm using a laser sensor.

The tph sensor reads Temperature, Pressure and Humidity.

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

Looking for more information about how to setup or add your device to The Things Network? Have a look [here](./TTN_README.md).

## Code example: main

```cpp
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>
#include "mbed.h"
#include "Simple-LoRaWAN.h"
#include "AmbiantSensorMessage.h"
#include "../lib/hardwarestatus.h"

mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);

using namespace Particula;

int main(void) {

    printf("\r\n\r\n[Particula] Loading Firmware ...");

    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)
    BME280 tph_sensor(&i2c_com);
    SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);  // D1 en D0 voor kleine nucleo
    HardwareStatus hardwareStatus;
    AmbiantSensorMessage versionMessage;
    versionMessage.addVersionNumber(VERSION);
    node.send(versionMessage.getMessage(), versionMessage.getLength());

    while (true) {

        const int SLEEP_TIME = MEASUREMENT_INTERVAL - PART_SENS_WARMUP_TIME;

        AmbiantSensorMessage message;   // Must be placed here, new values will otherwise be added to the same message
        printf("\r\n[Particula] Taking measurements ...\r\n");


        /**
         * Particle sensor wakeup
         */
        if(part_sensor.wakeUp() == WAKEUP_SUCCESSFULL){
            printf("[Particle sensor] wake up has been successfull \r\n", 0);
        } else {
            printf("[Particle sensor] wake up hasn't been successfull \r\n", 0);
            hardwareStatus.particle_wakeup_failed();
        }


        /**
         * Sleep 30 sec. After this time particle sensor measurements are considered correct
         */
        ThisThread::sleep_for(PART_SENS_WARMUP_TIME);     
        
    
        /**
         * Particle sensor takes measurements
         */
        if(part_sensor.read() == READ_SUCCESSFULL){
            printf("[Particle sensor] read has been successfull \r\n", 0);
        } else {
            printf("[Particle sensor] read hasn't been successfull \r\n", 0);
            hardwareStatus.particle_read_failed();
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
            printf("[Particle sensor] sleep has been successfull \r\n", 0);
        } else {
            printf("[Particle sensor] sleep hasn't been successfull \r\n", 0);
            hardwareStatus.particle_sleep_failed();
        }


        /**
         * TPH sensor wakeup
         */
        tph_sensor.awake();
        if (tph_sensor.present()) {
            printf("[TPH sensor] sensor is present \r\n", 0);
        } else {
            printf("[TPH sensor] sensor is not present \r\n", 0);
            hardwareStatus.tph_wakeup_failed();
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
            printf("[TPH sensor] read has been successful \r\n", 0);
        } else {
            printf("[TPH sensor] read has been unsuccessful \r\n", 0);
            hardwareStatus.tph_read_failed();
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
        printf("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
        printf("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
        printf("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
        printf("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
        printf("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);


        /**
         * Add binary coded errors to LoRa message and send the message
         */
        if (hardwareStatus.errors()) {
            printf("[Particula] Errors detected, adding them to lora message \r\n", 0);
            printf("[Particula] Hardware status (hex): %X \r\n", hardwareStatus.get_state());
            message.addStatus(hardwareStatus.get_state());
        } else {
            printf("[Particula] No errors detected \r\n", 0);
        }
        node.send(message.getMessage(), message.getLength());
        ThisThread::sleep_for(SLEEP_TIME);            
    }
    return 0;
}

```

## Pinout

### LoRaWAN + EEPROM Arduino compatible shield

We use the default pinmap of the shield, make sure the dip switches on the back of the shield are configured correctly.

| Signal | Default pin
|--|--|
| MOSI | D11
| MISO | D12
| CLK | D13
| NSS | A0
| RESET | A1
| DIO 0 | D2
| DIO 1 | D3

On the NUCLEO_L432KC the A3 pin is used for NSS and A4 is used for RESET.

### BME280 (TPH) sensor

| From    | To       | Description         |
|------------|------------|---------------------|
| VDD        | 3V3        | power supply        |
| GND        | GND        | common ground       |
| SDO        | GND        | Selects 0X76 as the address on the I2C buss    |
| CSB        | VDD        | Selects I2C for communication
| SCK        | SCL        | I2C bus clock
| SDI        | SDA        | I2C data bus

SDA and SCK lines for the different development boards:
| Board     | SDA       | SCK       |
|-----------|-----------|-----------|
| Nucleo_L432KC | D4 | D5 |
| Nucleo_L476RG | D14 | D15 |

### SDS011 (Particle) sensor

TX and RX lines for the different development boards:
| Board     | TX     | RX      |
|-----------|-----------|-----------|
| Nucleo_L432KC | D1 | D0 |
| Nucleo_L476RG | A4 | A5 |

(remember to connect sensor-RX to nucleo-TX and vice versa)

## What pins should I use with a different board

Choose UART RX and TX pins of your choice for the SDS011 particle sensor and I2C SDA and SCK pins for the BME280 TPH sensor.
You can add your board with it's preferred pins for serial communication to the `target_overrides` section in the `mbed_app.json` file.

### Example `target_overrides` section

```json
"target_overrides": {
        "NUCLEO_L476RG": {
            "i2c_sda": "D14",
            "i2c_sck": "D15",
            "uart_tx": "A4",
            "uart_rx": "A5",
            "lora_nss": "A0",
            "lora_reset": "A1"
        },
        "NUCLEO_L432KC": {
            "i2c_sda": "D4",
            "i2c_sck": "D5",
            "uart_tx": "D1",
            "uart_rx": "D0",
            "lora_nss": "A3",
            "lora_reset": "A4"
        }
```

## TTN Payload Decoder Output Example

```javascript
{
  "hardwareStatus": {
    "BatteryIndicator": {
      "chargeComplete": true,
      "chargeStatus": true,
      "lowBattery": true,
      "timerTemperatureFault": true
    },
    "ParticleSensor": {
      "readSuccessful": true,
      "sleepSuccessful": true,
      "wakeUpSuccessful": true
    },
    "TphSensor": {
      "readSuccessful": true,
      "wakeUpSuccessful": true
    }
  },
  "humidity": 54.97,
  "pm10": 22.4,
  "pm25": 20.3,
  "pressure": 1007.5,
  "temperature": 16.18
}
```

## Used modules and libraries

### LoRaWAN RFM95W Transceiver (over ISP)

- [Mbed-Simple-LoRaWAN](https://github.com/sillevl/mbed-Simple-LoRaWAN)
- [AmbientSensorMessage](https://github.com/vives-projectwerk-2-2020/AmbiantSensorMessage)

### Bosch BME280 TPH Sensor (over I2C)

- [ParticulaTPH](https://github.com/vives-projectwerk-2-2020/ParticulaTPH)

### SDS011 Particle sensor (over UART)

- [SDS011_Library](https://github.com/vives-projectwerk-2-2020/SDS011_Library)

### EEPROM (over I2C)

## Credits
