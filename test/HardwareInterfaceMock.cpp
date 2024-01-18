#include "include/HardwareInterfaceMock.hpp"

digitalWriteValues dWriteValues[LED_NUMBER];

void digitalWrite_fn(uint8_t pin, uint8_t val)
{
    if (pin == RED_LED_M) {
        dWriteValues[RED_LED_INDEX].pin = pin;
        dWriteValues[RED_LED_INDEX].val = val;
    }
    else if (pin == GREEN_LED_M) {
        dWriteValues[GREEN_LED_INDEX].pin = pin;
        dWriteValues[GREEN_LED_INDEX].val = val;
    }
    else if (pin == BLUE_LED_M) {
        dWriteValues[BLUE_LED_INDEX].pin = pin;
        dWriteValues[BLUE_LED_INDEX].val = val;
    }
}
