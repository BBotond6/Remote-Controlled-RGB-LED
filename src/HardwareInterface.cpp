#include "include/HardwareIntarface.hpp"

void digitalWrite_fn(uint8_t pin, uint8_t val)
{
    digitalWrite(pin, val);
}
