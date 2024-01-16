#ifndef REMOTE_LED_HPP
#define REMOTE_LED_HPP 

#ifdef UNIT_TEST_CPP
#include <cstdint>
#else
#include <Arduino.h>
#endif

uint8_t GetEEPROM_Address(uint8_t index);

#endif // REMOTE_LED_HPP
