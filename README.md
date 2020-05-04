# Particula-Firmware

![Markdown Linter](../../workflows/Markdown%20Linter/badge.svg?branch=master)

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
#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"
#include "settings.h"
#include "BME280.h"
#include "SDS011.h"

Serial pc(USBTX, USBRX);
mbed::I2C i2c_com(I2C_SDA_PIN, I2C_SCK_PIN);


int main(void) {
    SimpleLoRaWAN::Node node(keys, pins);   // If placed in main, stack size probably too small (Results in Fatal Error)

    BME280 tph_sensor(&i2c_com);

    SDS011_Particle::SDS011 part_sensor(UART_TX_PIN, UART_RX_PIN);

    pc.printf("\r\n\r\n[Particula] Loading Firmware ...");

    while (true) {
        ParticulaLora::AmbiantSensorMessage message;    // Must be placed here, new values will otherwise be added to the same message
        pc.printf("\r\n[Particula] Taking measurements ...\r\n");
        part_sensor.wakeUp();
        part_sensor.read();
        tph_sensor.awake();
        double temperature = tph_sensor.temperature();  // value in °C
        double humidity = tph_sensor.humidity();        // value in %
        double pressure = tph_sensor.presure();        // value in hPa
        tph_sensor.sleep();
        double pm25 = part_sensor.getPM25Value();          // value in µg/m³
        double pm10 = part_sensor.getPM10Value();          // value in µg/m³

        pc.printf("[Particula] Measered temperature:  %4.2f °C\r\n", temperature);
        pc.printf("[Particula] Measered humidity:     %4.2f %%\r\n", humidity);
        pc.printf("[Particula] Measered pressure:     %4.2f hPa\r\n", pressure);
        pc.printf("[Particula] Measered PM25:         %4.2f µg/m3\r\n", pm25);
        pc.printf("[Particula] Measered PM10:         %4.2f µg/m3\r\n", pm10);

        int testError = 0xAA;

        message.addTemperature(temperature);
        message.addHumidity(humidity);
        message.addPressure(pressure);
        message.addPM(pm25);
        message.addPM(pm10);
        message.addError(testError);

        node.send(message.getMessage(), message.getLength());
        part_sensor.sleep();
        pc.printf("[Particula] Going to sleep, deep sleep possible (1: yes, 0: no): %i\r\n", sleep_manager_can_deep_sleep());
        ThisThread::sleep_for(30000);
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
      "chargeComplete": 0,
      "chargeStatus": 0,
      "lowBattery": 0,
      "timerTemperatureFault": 0
    },
    "ParticleSensor": {
      "readSuccessful": 0,
      "sleepSuccessful": 0,
      "wakeUpSuccessful": 0
    },
    "TphSensor": {
      "readSuccessful": 0,
      "wakeUpSuccessful": 0
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

### SDS011 Particel sensor (over UART)

- [SDS011_Library](https://github.com/vives-projectwerk-2-2020/SDS011_Library)

### EEPROM (over I2C)
