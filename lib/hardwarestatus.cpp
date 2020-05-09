#include "hardwarestatus.h"

using namespace Particula;

HardwareStatus::HardwareStatus(void) {

}

void HardwareStatus::particle_wakeup_failed(void) {
    hardware_state &= ~(1u << 0); 
}

void HardwareStatus::particle_read_failed(void) {
    hardware_state &= ~(1u << 1);
}

void HardwareStatus::particle_sleep_failed(void) {
    hardware_state &= ~(1u << 2);
}

void HardwareStatus::tph_wakeup_failed(void) {
    hardware_state &= ~(1u << 5);
}

void HardwareStatus::tph_read_failed(void) {
    hardware_state &= ~(1u << 6);
}

void HardwareStatus::set_stat1(void) {
    hardware_state |= (1u << 10);
}

void HardwareStatus::set_stat2(void) {
    hardware_state |= (1u << 11);
}

void HardwareStatus::set_pg(void) {
    hardware_state |= (1u << 12);
}

char HardwareStatus::get_state(void) {
    return hardware_state;
}

bool HardwareStatus::errors(void) {
    return (hardware_state != successful_state);
}
