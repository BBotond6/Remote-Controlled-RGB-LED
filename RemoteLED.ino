#include <EEPROM.h>
#include "src/include/RemoteLED.hpp"

/////////////////////////////////////Remote controller variables and constans
#define PHOTOTRANSISTOR      2    //I/O port of the phototransistor.
#define NUM_OF_GAPS         33    //In one shot there are 33 gaps between pulses.

#define RECEPTION_TIMEOUT   70    //Time slot for the code bytes to arrive after the first rising edge (of the header).

#define MIN_H     4000  //Minimum value of the header gap in microseconds.
#define MAX_H     5500  //Maximum value of the header gap in microseconds.
#define MIN_0      450  //Minimum value of a gap meaning '0', in microseconds.
#define MAX_0      650  //Maximum value of a gap meaning '0', in microseconds.
#define MIN_1     1550  //Minimum value of a gap meaning '1', in microseconds.
#define MAX_1     1750  //Maximum value of a gap meaning '1', in microseconds.

#define MB_MASK     (uint8_t)0x80  //0x80 = 0%10000000 - Mask for the most significant bit in a byte.
#define MOD8_MASK   (uint8_t)0x07  //0x07 = 0%00000111 - Mask for the least 3 bits in a byte.
//With this mask the modulo division by 8 can be substituted.
//Note that (NUM % 8) is equivalent to (NUM & 0x07).

#define IR_ON   LOW     //Constants for the states of the phototransistor. The ON and OFF terms refer to
#define IR_OFF  HIGH    //the IR ray. When the infra LED of the remote controller is ON or OFF.

#define SAVED_COLOR_SIZE    3   //Size of the saved color in bytes -1

uint8_t previous_state;         //Global variable of the previous state of the infra communication.
uint8_t actual_state;           //Global variable of the actual state of the infra communication.
uint8_t reception_started;      //Flag to signalize that the reception has been started.
uint8_t reception_completed;    //Flag to signalize that the reception has been completed.
uint8_t length_index;           //Index for the array of gap length data.

unsigned long timestamp_gap;            //Timestamp for a falling edge. Here starts the next gap between pulses.
                                        //Unit: microseconds.
unsigned long timestamp_reception;      //Timestamp for the beginning of the reception process. Milliseconds.

unsigned long gap_length[NUM_OF_GAPS];  //Time values of the gap lengths in microseconds.

const uint8_t STORE_BUTTONS[STORE_BUTTONS_SIZE] = {0x10, 0x90, 0x50, 0x30, 0x08, 0x28,
                                                   0xB0, 0x88, 0xA8, 0x70, 0x48, 0x68};

uint8_t illuminate;

//Configuration and initialization.
void setup()
{
    Serial.begin(19200);

    /////////////////////////////////////Remote controller setup
    uint8_t idx;

    pinMode(PHOTOTRANSISTOR, INPUT);    //Configuring the phototransistor's pin as INPUT pin.

    previous_state = IR_OFF;            //Initial values of the state variables.
    actual_state   = IR_OFF;

    reception_started   = FALSE;        //Inital values for the process flags.
    reception_completed = FALSE;

    for (idx = 0; idx < NUM_OF_GAPS; idx++) {
        gap_length[idx] = 0;            //Initializing the array of gap lengths.
    }

    /////////////////////////////////////LED setup
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    digitalWrite_fn(RED_LED, HIGH_LED);
    digitalWrite_fn(BLUE_LED, HIGH_LED);
    digitalWrite_fn(GREEN_LED, HIGH_LED);

    illuminate = 0;

    LoadLedFromEEPROM(1, &RedLedValue, &GreenLedValue, &BlueLedValue);
}

/////////////////////////////////////Remote signal process
uint8_t decoding(unsigned long* p_gap_length, uint8_t * p_code_byte)
{
//Gap length data (32 pieces) are converted into 4 code bytes.
    uint8_t byte_idx;    //Index for the code bytes: 0, 1, 2, 3.
    uint8_t len_idx;     //Index for the gap length values: 1, 2, ..., 31, 32.

    for (byte_idx = 0; byte_idx < 4; byte_idx++) {  //There are 4 code bytes.
        p_code_byte[byte_idx] = 0;    //Clearing the code bytes.
    }

    for (len_idx = 1; len_idx < NUM_OF_GAPS; len_idx++) { //Processing the array of pulse distances.
        byte_idx = (len_idx - 1) / 8;   //With fixpoint arithmetics this expression gives back the correct
                                        //byte index from the length index.
                                        //  1, 2, 3, 4, 5, 6, 7, 8 |-> 0
                                        //  9,10,11,12,13,14,15,16 |-> 1
                                        // 17,18,19,20,21,22,23,24 |-> 2
                                        // 25,26,27,28,29,30,31,32 |-> 3

        //Shall digit 1 be decoded?
        if ((MIN_1 < p_gap_length[len_idx]) && (p_gap_length[len_idx] < MAX_1)) {
            //Assembly of the right code byte, bit by bit. Various algebraic expressions are given here.

            //p_code_byte[byte_idx] = p_code_byte[byte_idx] | ( 1 << (7 - (len_idx - 1) % 8) );
            //p_code_byte[byte_idx] = p_code_byte[byte_idx] | ( MB_MASK >> ((len_idx - 1) % 8) );
            p_code_byte[byte_idx] = p_code_byte[byte_idx] | (MB_MASK >> ((len_idx - 1) & MOD8_MASK));
        }
    }

    return p_code_byte[2];
}

uint8_t GetRemoteSignal()
{
    uint8_t       retur = 0;
    uint8_t       code_byte[4];             //Local array of the code bytes.
    unsigned long pulse_distance;           //Time difference between two consecutive pulses.

    actual_state = digitalRead(PHOTOTRANSISTOR);    //Reading the actual state of the phototransistor.

    //Hunting for the FIRST rising edge. This is the starting event of data reception.
    if ((previous_state == IR_OFF) && (actual_state == IR_ON) && (reception_started == FALSE)) {
        reception_started   = TRUE;       //The reception has been started.
        reception_completed = FALSE;      //Of course the reception has not been completed yet.
        length_index        = 0;          //Clearing the index of gap lenght data.
        timestamp_reception = millis();   //Taking the timestamp of the start of the reception. Milliseconds.
    }
    //Hunting for a falling edge during data reception.
    else if ((previous_state == IR_ON) && (actual_state == IR_OFF) && (reception_started == TRUE)) {
        timestamp_gap = micros();     //Taking the timestamp at the start of a gap between pulses. Microseconds.
    }
    //Hunting for a rising edge during data reception.
    else if ((previous_state == IR_OFF) && (actual_state == IR_ON) && (reception_started == TRUE)) {
        pulse_distance = micros() - timestamp_gap;   //Calculating the length of the gap between pulses.

        if (((MIN_H < pulse_distance) && (pulse_distance < MAX_H))   //Is the gap length in one of the
            || ((MIN_0 < pulse_distance) && (pulse_distance < MAX_0))   //dedicated ranges?
            || ((MIN_1 < pulse_distance) && (pulse_distance < MAX_1))
            ) {
            gap_length[length_index] = pulse_distance;    //Storing the actual pulse distance.
            length_index++;     //Increasing the index of pulse distances.

            if (length_index == NUM_OF_GAPS) {  //Do we have ALL the expected length values?
                reception_completed = TRUE;     //Yes, the reception ha been completed.
            }
        }
        else {  //The actual pulse distance is not in any of the given ranges.
            reception_started = FALSE;    //Aborting the reception process.
            Serial.println("\n\nInvalid length");   //Sending an error message.
        }
    }

    //If the timeout value of the reception is reached but it has not been completed yet...
    if ((millis() - timestamp_reception > RECEPTION_TIMEOUT)
        && (reception_started == TRUE) && (reception_completed == FALSE)) {
        reception_started = FALSE;      //Aborting the reception process.
        Serial.println("\nTimeout");    //Sending an error message.
    }

    if (reception_completed == TRUE) {  //If the reception has been completed with success...
        reception_started   = FALSE;    //Clearing the process flags.
        reception_completed = FALSE;

        retur = decoding(gap_length, code_byte);     //Decoding the sequence of pulse distances.
    }

    previous_state = actual_state;    //Shifting the states.
    return retur;
}

void SaveLedToEEPROM(uint8_t address, uint8_t redled, uint8_t greenled, uint8_t blueled)
{
    if (address > 0 && address <= 60) {
        EEPROM.write(address++, redled);
        EEPROM.write(address++, greenled);
        EEPROM.write(address++, blueled);
        uint8_t protec[] = { redled,greenled,blueled };
        EEPROM.write(address, Calc_CRC_8(protec, 3));
    }
}

void LoadLedFromEEPROM(uint8_t address ,uint8_t* redled, uint8_t* greenled, uint8_t* blueled)
{
    *redled              = EEPROM.read(address++);
    *greenled            = EEPROM.read(address++);
    *blueled             = EEPROM.read(address++);
    uint8_t saved_protec = EEPROM.read(address);
    uint8_t protec[]     = { *redled,*greenled,*blueled };
    uint8_t calc_protec  = Calc_CRC_8(protec, 3);

    //Memory protection
    if (saved_protec != calc_protec) {
        RedLedValue   = LED_DEFAULT_VALUE;
        BlueLedValue  = LED_DEFAULT_VALUE;
        GreenLedValue = LED_DEFAULT_VALUE;

        SaveLedToEEPROM(address - SAVED_COLOR_SIZE, *redled, *greenled, *blueled);
    }

    if (*redled != MIN_LED_VALUE && *greenled == MIN_LED_VALUE && *blueled == MIN_LED_VALUE) {
        SetLedState(&RedLedState);
    }
    else if (*redled == MIN_LED_VALUE && *greenled != MIN_LED_VALUE && *blueled == MIN_LED_VALUE) {
        SetLedState(&GreenLedState);
    }
    else if (*redled == MIN_LED_VALUE && *greenled == MIN_LED_VALUE && *blueled != MIN_LED_VALUE) {
        SetLedState(&BlueLedState);
    }
}

void ButtonEventManager(uint8_t button)
{
    if (button == ON_OFF_BUTTON) {
        OnOffButtonEvent();
        OnOffButtonState = abs(OnOffButtonState - 1);
    }
    else if (OnOffButtonState == TRUE) {

        if (button == SAVE_BUTTON && SaveButtonState == FALSE) {
            SaveButtonState = TRUE;
            LoadButtonState = FALSE;
            SetLedStates(FALSE);
        }
        else if (button == LOAD_BUTTON && LoadButtonState == FALSE) {
            SaveButtonState = FALSE;
            LoadButtonState = TRUE;
            SetLedStates(FALSE);
        }
        else if (SaveButtonState == FALSE && LoadButtonState == FALSE) {
            if (button == RED_BUTTON) {
                SetLedState(&RedLedState);
            }
            else if (button == BLUE_BUTTON) {
                SetLedState(&BlueLedState);
            }
            else if (button == GREEN_BUTTON) {
                SetLedState(&GreenLedState);
            }
            else if (button == CH_UP_BUTTON) {
                SetLedValue(TRUE);  //Set active LED value up
            }
            else if (button == CH_DOWN_BUTTON) {
                SetLedValue(FALSE); //Set active LED value down
            }
        }
        else if (SaveButtonState == TRUE && LoadButtonState == FALSE) {
            uint8_t ButtonIndex = GetIndexInArray(STORE_BUTTONS, STORE_BUTTONS_SIZE, button);

            if (ButtonIndex != UINT8_MAX) {
                SaveLedToEEPROM(GetEEPROM_Address(ButtonIndex), RedLedValue, GreenLedValue, BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
        }
        else if (SaveButtonState == FALSE && LoadButtonState == TRUE) {
            uint8_t ButtonIndex = GetIndexInArray(STORE_BUTTONS, STORE_BUTTONS_SIZE, button);

            if (ButtonIndex != UINT8_MAX) {
                LoadLedFromEEPROM(GetEEPROM_Address(ButtonIndex), &RedLedValue, &GreenLedValue, &BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
        }
    }
}

void Illumination()
{
    if (OnOffButtonState != FALSE) {
        if (illuminate < (RedLedValue / LED_STEP_VALUE)) {
            digitalWrite_fn(RED_LED, LOW_LED);
        }
        else {
            digitalWrite_fn(RED_LED, HIGH_LED);
        }

        if (illuminate < (BlueLedValue / LED_STEP_VALUE)) {
            digitalWrite_fn(BLUE_LED, LOW_LED);
        }
        else {
            digitalWrite_fn(BLUE_LED, HIGH_LED);
        }

        if (illuminate < (GreenLedValue / LED_STEP_VALUE)) {
            digitalWrite_fn(GREEN_LED, LOW_LED);
        }
        else {
            digitalWrite_fn(GREEN_LED, HIGH_LED);
        }

        if (illuminate != 25) {
            illuminate++;
        }
        else {
            illuminate = 0;
        }
    }
}

void loop()
{
    uint8_t CurrentButton= GetRemoteSignal();

    ButtonEventManager(CurrentButton);
    Illumination();

}
