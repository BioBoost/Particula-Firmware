# Particula-Firmware


## Getting started
Clone the project and get the dependencies installed:

```
git clone git@github.com:vives-projectwerk-2-2020/Particula-Firmware.git
cd Particula-Firmware

// If your project is not an mbed project yet
// mbed new .

mbed deploy
```

Setup an TTN application:



The conglomeration of all the firmeware from the Particula Project
 
## Temperature Presure Humidity sensor
These measurements are taken with the BME280 over I2C.
And is located under the folder ./TPH/

## EEPROM
This device is used over I2C 

## LoraWAN
This device is used over SPI

## Particel sensor
This device is used over UART

## Related repos
### TPH: "https://github.com/vives-projectwerk-2-2020/ParticulaTPH"
### EEPROM: "PlaceHolder"
### LoraWAN: "https://github.com/vives-projectwerk-2-2020/LoRaWAN-example"
### Particel: "https://github.com/vives-projectwerk-2-2020/SDS011_Library" 