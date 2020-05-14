#pragma once

#define PRODUCTION_ENVIRONMENT false
#define VERSION 1.0

#include "config.h"

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
