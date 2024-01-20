#ifndef REMOTE_LED_HPP
#define REMOTE_LED_HPP 

#ifdef UNIT_TEST_CPP
#include "../../test/include/HardwareInterfaceMock.hpp"
#else
#include "HardwareIntarface.hpp"
#endif

#define TRUE    1   //True value
#define FALSE   0   //False value

// 1 is the basic led strip remote controller with 24 button
// 2 is my TV remote controller
#define REMOTE_CONTROLLER 1

//size of the store buttons
#define STORE_BUTTONS_SIZE 12

#if REMOTE_CONTROLLER == 1
    #define RED_BUTTON      0x20 //RED button
    #define GREEN_BUTTON    0xA0 //GREEN button
    #define BLUE_BUTTON     0x60 //BLUE button
    #define ON_OFF_BUTTON   0xC0 //ON button
    #define SAVE_BUTTON     0xE0 //W button
    #define LOAD_BUTTON     0xD0 //FLASH button
    #define CH_UP_BUTTON    0x40 //OFF button
    #define CH_DOWN_BUTTON  0x80 //ˇ button
    // Buttons to save and load the LED values
    extern const uint8_t STORE_BUTTONS[STORE_BUTTONS_SIZE];
    //Buttons under the R button:
    //R-x = 0x10 0x30 0x8 0x28
    //Buttons under the G button:
    //G-x = 0x90 0xB0 0x88 0xA8
    //Buttons under the B button:
    //B-x = 0x50 0x70 0x48 0x68
    //Other Buttons:
    //STROBE = 0xF0
    //FADE = 0xC8
    //SMOOTH = 0xE8
#elif REMOTE_CONTROLLER == 2
    #define RED_BUTTON       0x98    //Red button code
    #define GREEN_BUTTON     0x28    //Green button code
    #define BLUE_BUTTON      0xC8    //Blue button code
    #define ON_OFF_BUTTON    0x48    //On-Off button code
    #define SAVE_BUTTON      0x84    //OK button code
    #define LOAD_BUTTON      0XE7    //AV button code
    #define CH_UP_BUTTON     0x24    //P+ button code
    #define CH_DOWN_BUTTON   0xC4    //P- button code
    extern const uint8_t STORE_BUTTONS[3] = {0x80, 0x40, 0xC0};
#endif

/////////////////////////////////////LED variables
#define RED_LED      9    //Red LED pin
#define BLUE_LED    11    //Blue LED pin
#define GREEN_LED   10    //Green LED pin

#define LED_DEFAULT_VALUE         200                     //Default value of the LEDs
#define MAX_LED_VALUE             250                     //Max value of the LEDs
#define MIN_LED_VALUE               0                     //Min led value of the LEDs (0 is not illuminating)
#define LED_STEP_VALUE             10                     //Value of one step
#define MIXED_COLOR_MIN_LED_VALUE  10                     //Min led value of the mixed colors

#define COLOR_EEPROM_ALLOCATION_SIZE 4

// Default: HIGH_LED (1) is not illuminate, LOW_LED (0) is illuminate
// If your LED has inverse logic change the values
#define HIGH_LED    0
#define LOW_LED     1

extern uint8_t OnOffButtonState;
extern uint8_t SaveButtonState;
extern uint8_t LoadButtonState;

extern uint8_t RedLedState;
extern uint8_t BlueLedState;
extern uint8_t GreenLedState;

extern uint8_t RedLedValue;
extern uint8_t BlueLedValue;
extern uint8_t GreenLedValue;

uint8_t GetEEPROM_Address(uint8_t index);

void SetLedState(uint8_t* led);

void SetLedStates(uint8_t state);

uint8_t GetActiveLedNumber();

uint8_t** GetActiveLedPointers();

void OnOffButtonEvent();

#endif // REMOTE_LED_HPP
