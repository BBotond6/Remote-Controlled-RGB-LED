#include "include/RemoteLED.hpp"

uint8_t OnOffButtonState = TRUE;
uint8_t SaveButtonState  = FALSE;
uint8_t LoadButtonState  = FALSE;

uint8_t RedLedState   = FALSE;
uint8_t BlueLedState  = FALSE;
uint8_t GreenLedState = FALSE;

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

void OnOffButtonEvent()
{
    if (OnOffButtonState == TRUE) {    //Off state
        SetLedStates(FALSE, &RedLedState, &GreenLedState, &BlueLedState);

        digitalWrite_fn(RED_LED, HIGH_LED);
        digitalWrite_fn(GREEN_LED, HIGH_LED);
        digitalWrite_fn(BLUE_LED, HIGH_LED);

        SaveButtonState = FALSE;
        LoadButtonState = FALSE;
    }
}
