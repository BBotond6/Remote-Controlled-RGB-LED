#include "include/RemoteLED.hpp"

// Get EEPROM address from the button index
uint8_t GetEEPROM_Address(uint8_t index)
{
    return (4 * index) + 1;
}
