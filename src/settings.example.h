#pragma once

#define PRODUCTION_ENVIRONMENT true

#if PRODUCTION_ENVIRONMENT
#define consoleMessage(string, value1)
#undef LORAWAN_DEBUGGING
#else
#define consoleMessage(string, value1) printf(string, value1)
#endif

#include "config.h"
#include "BME280.h"
#include "SDS011.h"
#include <exception>
#include "mbed.h"
#include "AmbiantSensorMessage.h"
#include "Simple-LoRaWAN.h"

SimpleLoRaWAN::LoRaWANKeys keys = { 
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },         // devEui
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },         // appEui
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } // appKey
};

SimpleLoRaWAN::Pinmapping pins = { D11, D12, D13, LORA_NSS_PIN, LORA_RESET_PIN, D2, D3 };  // mosi, miso, clk, nss, reset, dio0, dio1

namespace Particula {

        const unsigned int PART_SENS_WARMUP_TIME = (PRODUCTION_ENVIRONMENT ? 30000 : 30000);   // ms
        const unsigned int MEASUREMENT_INTERVAL = (PRODUCTION_ENVIRONMENT ? 300000 : 60000);   // ms

};
