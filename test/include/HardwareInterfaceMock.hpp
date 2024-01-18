#ifndef HARDWARE_INTERFACE_MOCK_HPP
#define HARDWARE_INTERFACE_MOCK_HPP

#include <cstdint>

#define LED_NUMBER      3
#define RED_LED_INDEX   0
#define GREEN_LED_INDEX 1
#define BLUE_LED_INDEX  2

/////////////////////////////////////LED variables
#define RED_LED_M      9    //Red LED pin
#define BLUE_LED_M    11    //Blue LED pin
#define GREEN_LED_M   10    //Green LED pin

struct digitalWriteValues
{
    uint8_t pin;
    uint8_t val;
};

extern digitalWriteValues dWriteValues[LED_NUMBER];

void digitalWrite_fn(uint8_t pin, uint8_t val);

#endif // HARDWARE_INTERFACE_MOCK_HPP
