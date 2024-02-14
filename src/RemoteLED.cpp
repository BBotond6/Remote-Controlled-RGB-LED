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

//Memory protection
const uint8_t CRC_8_TABLE[256] =
{
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

//Memory protection
uint8_t Calc_CRC_8(uint8_t  DataArray[], uint16_t Length)
{
    uint16_t i;
    uint8_t  CRC;

    CRC = 0;
    for (i = 0; i < Length; i++) {
        CRC = CRC_8_TABLE[CRC ^ DataArray[i]];
    }

    return CRC;
}

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

void SetLedValue(uint8_t mode)
{
    if (RedLedState == TRUE || BlueLedState == TRUE || GreenLedState == TRUE) {

        if (RedLedState == TRUE) {
            SetOneLedValue(&RedLedValue, mode);
        }
        else if (GreenLedState == TRUE) {
            SetOneLedValue(&GreenLedValue, mode);
        }
        else if (BlueLedState == TRUE) {
            SetOneLedValue(&BlueLedValue, mode);
        }
    }
    else if(GetActiveLedNumber() > 1) {
        SetColorValues(mode);
    }
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

// Return with the indx of the value or return UINT8_MAX if the array does not contain the value
uint8_t GetIndexInArray(const uint8_t* array, uint8_t size, uint8_t value)
{
    uint8_t i;

    for (i = 0; i < size; i++) {
        if (value == array[i]) {
            return i;
        }
    }

    return UINT8_MAX;
}
