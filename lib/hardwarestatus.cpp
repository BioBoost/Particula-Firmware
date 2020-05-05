#include "hardwarestatus.h"

using namespace Particula;

HardwareStatus::HardwareStatus(void) {

}

void HardwareStatus::setParticleWakeupSuccess(void) {
    error_values |= (1u); 
}

void HardwareStatus::setParticleReadSuccess(void) {
    error_values |= (1u << 1);
}

void HardwareStatus::setParticleSleepSuccess(void) {
    error_values |= (1u << 2);
}

void HardwareStatus::setTphWakeupSuccess(void) {
    error_values |= (1u << 5);
}

void HardwareStatus::setTphReadSuccess(void) {
    error_values |= (1u << 6);
}

void HardwareStatus::setStat1(void) {
    error_values |= (1u << 10);
}

void HardwareStatus::setStat2(void) {
    error_values |= (1u << 11);
}

void HardwareStatus::setPg(void) {
    error_values |= (1u << 12);
}

char HardwareStatus::getStatus(void) {
    return error_values;
}
