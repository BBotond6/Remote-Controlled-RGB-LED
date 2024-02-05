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

void SetOneLedValue(uint8_t* led, uint8_t mode)
{
    if (mode == TRUE && *led > (MAX_LED_VALUE - LED_STEP_VALUE)) {
        *led = MAX_LED_VALUE;
    }
    else if (mode == TRUE && *led != MAX_LED_VALUE) {
        *led += LED_STEP_VALUE;
    }
    else if (mode == FALSE && *led < (MIN_LED_VALUE + LED_STEP_VALUE)) {
        *led = MIN_LED_VALUE;
    }
    else if (mode == FALSE && *led != MIN_LED_VALUE) {
        *led -= LED_STEP_VALUE;
    }
}

void SetColorValues(uint8_t mode)
{
    uint8_t   i;
    float     ratio;
    uint8_t   ReferenceLedValue = 0;
    uint8_t   ReferenceIndex    = 0;
    uint8_t** LedPointers       = GetActiveLedPointers();
    uint8_t   ActiveLedNumber   = GetActiveLedNumber();

    if (mode) {
        for (i = 0; i < ActiveLedNumber; i++) {
            if (*LedPointers[i] > ReferenceLedValue) {
                ReferenceLedValue = *LedPointers[i];
                ReferenceIndex    = i;
            }
        }

        if (*LedPointers[ReferenceIndex] > (MAX_LED_VALUE - LED_STEP_VALUE)) {
            ReferenceLedValue = MAX_LED_VALUE;
        }
        else {
            ReferenceLedValue = *LedPointers[ReferenceIndex] + LED_STEP_VALUE;
        }

        ratio = (float)((float)ReferenceLedValue / (float)(*LedPointers[ReferenceIndex]));
    }
    else {
        ReferenceLedValue = UINT8_MAX;
        for (i = 0; i < ActiveLedNumber; i++) {
            if (*LedPointers[i] < ReferenceLedValue) {
                ReferenceLedValue = *LedPointers[i];
                ReferenceIndex    = i;
            }
        }

        if (*LedPointers[ReferenceIndex] < (MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE)) {
            ReferenceLedValue = MIXED_COLOR_MIN_LED_VALUE;
        }
        else {
            ReferenceLedValue = *LedPointers[ReferenceIndex] - LED_STEP_VALUE;
        }

        ratio = (float)((float)ReferenceLedValue / (float)(*LedPointers[ReferenceIndex]));
    }

    *LedPointers[ReferenceIndex] = ReferenceLedValue;

    for (i = 0; i < ActiveLedNumber; i++) {
        if (i != ReferenceIndex) {
            *LedPointers[i] = (uint8_t)round((float)(*LedPointers[i]) * ratio);
        }
    }

    delete[] LedPointers;
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
