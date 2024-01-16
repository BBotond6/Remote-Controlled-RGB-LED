#include "include/RemoteLED.hpp"

// Get EEPROM address from the button index
uint8_t GetEEPROM_Address(uint8_t index)
{
    return (4 * index) + 1;
}

void SetLedStates(uint8_t state, uint8_t* RedLedState, uint8_t* GreenLedState, uint8_t* BlueLedState)
{
    *RedLedState   = state;
    *BlueLedState  = state;
    *GreenLedState = state;
}
