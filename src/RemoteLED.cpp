#include "include/RemoteLED.hpp"

uint8_t OnOffButtonState = TRUE;
uint8_t SaveButtonState  = FALSE;
uint8_t LoadButtonState  = FALSE;

uint8_t RedLedState   = FALSE;
uint8_t GreenLedState = FALSE;
uint8_t BlueLedState  = FALSE;

uint8_t RedLedValue   = MIN_LED_VALUE;
uint8_t GreenLedValue = MIN_LED_VALUE;
uint8_t BlueLedValue  = MIN_LED_VALUE;

// Get EEPROM address from the button index
uint8_t GetEEPROM_Address(uint8_t index)
{
    return (COLOR_EEPROM_ALLOCATION_SIZE * index) + 1;
}

void SetLedState(uint8_t* led)
{
    SetLedStates(FALSE);
    *led = TRUE;
}

void SetLedStates(uint8_t state)
{
    RedLedState   = state;
    BlueLedState  = state;
    GreenLedState = state;
}

uint8_t GetActiveLedNumber()
{
    uint8_t ActiveLedNumber = 0;
    if (RedLedValue != MIN_LED_VALUE) {
        ActiveLedNumber++;
    }

    if (BlueLedValue != MIN_LED_VALUE) {
        ActiveLedNumber++;
    }

    if (GreenLedValue != MIN_LED_VALUE) {
        ActiveLedNumber++;
    }

    return ActiveLedNumber;
}

uint8_t** GetActiveLedPointers()
{
    uint8_t** pointers = new uint8_t*[GetActiveLedNumber() + 1];
    uint8_t   index    = 0;

    if (RedLedValue != 0) {
        pointers[index] = &RedLedValue;
        index++;
    }

    if (GreenLedValue != 0) {
        pointers[index] = &GreenLedValue;
        index++;
    }

    if (BlueLedValue != 0) {
        pointers[index] = &BlueLedValue;
        index++;
    }

    pointers[index] = nullptr;

    return pointers;
}

void OnOffButtonEvent()
{
    if (OnOffButtonState == TRUE) {    //Off state
        SetLedStates(FALSE);

        digitalWrite_fn(RED_LED, HIGH_LED);
        digitalWrite_fn(GREEN_LED, HIGH_LED);
        digitalWrite_fn(BLUE_LED, HIGH_LED);

        SaveButtonState = FALSE;
        LoadButtonState = FALSE;
    }
}
