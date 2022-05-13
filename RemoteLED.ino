/*
    Name:       RemoteLED.ino
    Created:	12/05/2022 21:06:35
    Author:     BBotond6
*/

#include <EEPROM.h>

//#define NUM_OF_BUTTONS      25    //Number of buttons in a list containing the code byte and the name of the button.
// 
//typedef struct      //Type definition of a structure, containing the CODEs and the NAMEs of buttons.
//{
//    byte bcode;
//    char* bname;
//} button_data;
//
////Buttons and their codes in the remote controller
//button_data list_of_buttons[NUM_OF_BUTTONS] = { {0x24 , "p+"},
//                                                   {0xC4 , "p-"},
//                                                   {0x80 , "1"},
//                                                   {0x40 , "2"},
//                                                   {0xC0 , "3"},
//                                                   {0x20 , "4"},
//                                                   {0xA0 , "5"},
//                                                   {0x60 , "6"},
//                                                   {0xE0 , "7"},
//                                                   {0x10 , "8"},
//                                                   {0x90 , "9"},
//                                                   {0x50 , "0"},
//                                                   {0XE7 , "AV"},
//                                                   {0xA4 , "-/--"},
//                                                   {0x84 , "ok"},
//                                                   {0x01 , "^"},
//                                                   {0x81 , "¡"},
//                                                   {0xB2 , ">"},
//                                                   {0x8A , "<"},
//                                                   {0x98 , "red<<"},
//                                                   {0xC8 , "blue>>"},
//                                                   {0xD4 , "yellow>"},
//                                                   {0x00 , "||"},
//                                                   {0x28 , "green"},
//                                                   {0x48 , "on/off"} };
//                                                   

/////////////////////////////////////Remote controller variables and constans
#define PHOTOTRANSISTOR      2    //I/O port of the phototransistor.
#define NUM_OF_GAPS         33    //In one shot there are 33 gaps between pulses.

#define RECEPTION_TIMEOUT   70    //Time slot for the code bytes to arrive after the first rising edge (of the header).

#define MIN_H     4000      //Minimum value of the header gap in microseconds.
#define MAX_H     5500      //Maximum value of the header gap in microseconds.
#define MIN_0      450      //Minimum value of a gap meaning '0', in microseconds.
#define MAX_0      650     //Maximum value of a gap meaning '0', in microseconds.
#define MIN_1     1550      //Minimum value of a gap meaning '1', in microseconds.
#define MAX_1     1750     //Maximum value of a gap meaning '1', in microseconds.

#define MB_MASK     (byte)0x80  //0x80 = 0%10000000 - Mask for the most significant bit in a byte.
#define MOD8_MASK   (byte)0x07  //0x07 = 0%00000111 - Mask for the least 3 bits in a byte.
//With this mask the modulo division by 8 can be substituted.
//Note that (NUM % 8) is equivalent to (NUM & 0x07).

#define TRUE    1       //True value
#define FALSE   0       //False value

#define IR_ON   LOW       //Constants for the states of the phototransistor. The ON and OFF terms refer to
#define IR_OFF  HIGH      //the IR ray. When the infra LED of the remote controller is ON or OFF.

byte previous_state;        //Global variable of the previous state of the infra communication.
byte actual_state;          //Global variable of the actual state of the infra communication.
byte reception_started;     //Flag to signalize that the reception has been started.
byte reception_completed;   //Flag to signalize that the reception has been completed.
byte length_index;               //Index for the array of gap length data.

unsigned long timestamp_gap;            //Timestamp for a falling edge. Here starts the next gap between pulses.
                                        //Unit: microseconds.
unsigned long timestamp_reception;      //Timestamp for the beginning of the reception process. Milliseconds.

unsigned long gap_length[NUM_OF_GAPS];      //Time values of the gap lengths in microseconds.    

#define RED_BUTTON 0x98     //Red button code
#define GREEN_BUTTON 0x28   //Green button code
#define BLUE_BUTTON 0xC8    //Blue button code
#define ON_OFF_BUTTON 0x48  //On-Off button code
#define SAVE_BUTTON 0x84    //OK button code
#define LOAD_BUTTON 0XE7    //AV button code
#define CH_UP_BUTTON 0x24   //P+ button code
#define CH_DOWN_BUTTON 0xC4 //P- button code
#define OneButton 0x80      //1 button code
#define TwoButton 0x40      //2 button code
#define ThreeButton 0xC0    //3 button code

/////////////////////////////////////LED variables
#define RedLed 9    //Red LED pin
#define BlueLed 11  //Blue LED pin
#define GreenLed 10 //Green LED pin

#define LedDefaultValue 200 //Default value of the LEDs

byte OnOffButtonState;
byte SaveButtonState;
byte LoadButtonState;

byte RedLedState;
byte BlueLedState;
byte GreenLedState;

byte RedLedValue;
byte BlueLedValue;
byte GreenLedValue;

byte illuminate;

//Configuration and initialization.
void setup()
{
    Serial.begin(19200);

    /////////////////////////////////////Remote controller setup
    byte idx;

    pinMode(PHOTOTRANSISTOR, INPUT);      //Configuring the phototransistor's pin as INPUT pin.

    previous_state = IR_OFF;        //Initial values of the state variables.
    actual_state = IR_OFF;

    reception_started = FALSE;      //Inital values for the process flags.
    reception_completed = FALSE;

    for (idx = 0; idx < NUM_OF_GAPS; idx++) { gap_length[idx] = 0; }    //Initializing the array of gap lengths.

    /////////////////////////////////////LED setup
    pinMode(RedLed, OUTPUT);
    pinMode(BlueLed, OUTPUT);
    pinMode(GreenLed, OUTPUT);

    digitalWrite(RedLed, HIGH);
    digitalWrite(BlueLed, HIGH);
    digitalWrite(GreenLed, HIGH);

    OnOffButtonState = FALSE;
    SaveButtonState = FALSE;
    LoadButtonState = FALSE;
    
    RedLedState = 0;
    BlueLedState = 0;
    GreenLedState = 0;

    RedLedValue = 0;
    BlueLedValue = 0;
    GreenLedValue = 0;

    illuminate = 0;
}

/////////////////////////////////////Remote signal process
byte decoding(unsigned long* p_gap_length, byte * p_code_byte)
    {
    //Gap length data (32 pieces) are converted into 4 code bytes.
        byte byte_idx;    //Index for the code bytes: 0, 1, 2, 3.
        byte len_idx;     //Index for the gap length values: 1, 2, ..., 31, 32.

        for (byte_idx = 0; byte_idx < 4; byte_idx++)   //There are 4 code bytes.
        {
            p_code_byte[byte_idx] = 0;    //Clearing the code bytes.
        }

        for (len_idx = 1; len_idx < NUM_OF_GAPS; len_idx++)   //Processing the array of pulse distances.
        {
            byte_idx = (len_idx - 1) / 8;   //With fixpoint arithmetics this expression gives back the correct
                                            //byte index from the length index.
                                            //  1, 2, 3, 4, 5, 6, 7, 8 |-> 0
                                            //  9,10,11,12,13,14,15,16 |-> 1
                                            // 17,18,19,20,21,22,23,24 |-> 2
                                            // 25,26,27,28,29,30,31,32 |-> 3

            //Shall digit 1 be decoded?
            if ((MIN_1 < p_gap_length[len_idx]) && (p_gap_length[len_idx] < MAX_1))
            {
                //Assembly of the right code byte, bit by bit. Various algebraic expressions are given here.

                //p_code_byte[byte_idx] = p_code_byte[byte_idx] | ( 1 << (7 - (len_idx - 1) % 8) );
                //p_code_byte[byte_idx] = p_code_byte[byte_idx] | ( MB_MASK >> ((len_idx - 1) % 8) );
                p_code_byte[byte_idx] = p_code_byte[byte_idx] | (MB_MASK >> ((len_idx - 1) & MOD8_MASK));
            }
        }
        /*
        Serial.println("-----------------------------------");
        Serial.println(p_code_byte[2],HEX);
        Serial.println("-----------------------------------");
        */
        return p_code_byte[2];
    }

byte GetRemoteSignal() {
        byte retur = 0;
        byte code_byte[4];               //Local array of the code bytes.
        unsigned long pulse_distance;           //Time difference between two consecutive pulses.

        actual_state = digitalRead(PHOTOTRANSISTOR);    //Reading the actual state of the phototransistor.

        //Hunting for the FIRST rising edge. This is the starting event of data reception.
        if ((previous_state == IR_OFF) && (actual_state == IR_ON) && (reception_started == FALSE))
        {
            reception_started = TRUE;         //The reception has been started.
            reception_completed = FALSE;      //Of course the reception has not been completed yet.
            length_index = 0;                 //Clearing the index of gap lenght data.
            timestamp_reception = millis();   //Taking the timestamp of the start of the reception. Milliseconds.
        }
        //Hunting for a falling edge during data reception.
        else if ((previous_state == IR_ON) && (actual_state == IR_OFF) && (reception_started == TRUE))
        {
            timestamp_gap = micros();     //Taking the timestamp at the start of a gap between pulses. Microseconds.
        }
        //Hunting for a rising edge during data reception.
        else if ((previous_state == IR_OFF) && (actual_state == IR_ON) && (reception_started == TRUE))
        {
            pulse_distance = micros() - timestamp_gap;   //Calculating the length of the gap between pulses.

            if (((MIN_H < pulse_distance) && (pulse_distance < MAX_H))   //Is the gap length in one of the
                || ((MIN_0 < pulse_distance) && (pulse_distance < MAX_0))   //dedicated ranges?
                || ((MIN_1 < pulse_distance) && (pulse_distance < MAX_1))
                )
            {
                gap_length[length_index] = pulse_distance;    //Storing the actual pulse distance.
                length_index++;     //Increasing the index of pulse distances.

                if (length_index == NUM_OF_GAPS)    //Do we have ALL the expected length values?
                {
                    reception_completed = TRUE;     //Yes, the reception ha been completed.
                }
            }
            else    //The actual pulse distance is not in any of the given ranges.
            {
                reception_started = FALSE;    //Aborting the reception process.
                Serial.println("\n\nInvalid length");   //Sending an error message.
            }
        }

        //If the timeout value of the reception is reached but it has not been completed yet...
        if ((millis() - timestamp_reception > RECEPTION_TIMEOUT)
            && (reception_started == TRUE) && (reception_completed == FALSE))
        {
            reception_started = FALSE;      //Aborting the reception process.
            Serial.println("\nTimeout");    //Sending an error message.
        }

        if (reception_completed == TRUE)    //If the reception has been completed with success...
        {
            reception_started = FALSE;          //Clearing the process flags.
            reception_completed = FALSE;

            retur = decoding(gap_length, code_byte);     //Decoding the sequence of pulse distances.
            //display_code_bytes(code_byte);      //Displaying the code bytes.
            //name_of_the_button(code_byte[2]);   //Searching for the name of the button.
        }

        previous_state = actual_state;    //Shifting the states.
        return retur;
    }

void SetLedStates(byte state) {
    RedLedState = state;
    BlueLedState = state;
    GreenLedState = state;
}

void OnOffButtonEvent() {
    if (OnOffButtonState == TRUE) {    //Off state
        SetLedStates(FALSE);

        digitalWrite(RedLed, HIGH);
        digitalWrite(GreenLed, HIGH);
        digitalWrite(BlueLed, HIGH);

        /*RedLedValue = FALSE;
        BlueLedValue = FALSE;
        GreenLedValue = FALSE;*/

        SaveButtonState = FALSE;
        LoadButtonState = FALSE;
    }
    else {
        LoadLedFromEEPROM(1, &RedLedValue, &GreenLedValue, &BlueLedValue);
    }
}

void SetLedState(byte* led) {
    RedLedState = 0;
    BlueLedState = 0;
    GreenLedState = 0;
    *led = 1;
}

void SetLedValue(byte mode) {
    if (RedLedState == TRUE || BlueLedState == TRUE || GreenLedState == TRUE) {
        byte* led;

        if (RedLedState == TRUE) {
            led = &RedLedValue;
        }
        else if (BlueLedState == TRUE) {
            led = &BlueLedValue;
        }
        else if (GreenLedState == TRUE) {
            led = &GreenLedValue;
        }

        if (mode == TRUE && *led != 250) {
            *led += 10;
        }
        else if (mode == FALSE && *led != 0) {
            *led -= 10;
        }
    }
}

void SaveLedToEEPROM(byte address, byte redled, byte greenled, byte blueled) {
    if (address > 0 && address <= 60) {
        EEPROM.write(address, redled);
        EEPROM.write(address + 1, greenled);
        EEPROM.write(address + 2, blueled);
    }
}

void LoadLedFromEEPROM(byte address ,byte* redled, byte* greenled, byte* blueled) {
    *redled = EEPROM.read(address);
    *greenled = EEPROM.read(address + 1);
    *blueled = EEPROM.read(address + 2);

    //Memory check and repair
    if (*redled < 0 || *redled > 250 || *greenled < 0 || *greenled > 250 || *blueled < 0 || *blueled > 250) {
        if (address == 1) {
            *redled = LedDefaultValue;
            *greenled = FALSE;
            *blueled = FALSE;
        }
        else if (address == 4) {
            *redled = FALSE;
            *greenled = LedDefaultValue;
            *blueled = FALSE;
        }
        else if (address == 7) {
            *redled = FALSE;
            *greenled = FALSE;
            *blueled = LedDefaultValue;
        }
        EEPROM.write(address, *redled);
        EEPROM.write(address + 1, *greenled);
        EEPROM.write(address + 2, *blueled);
    }
}

void ButtonEventManager(byte button) {
    if (button == ON_OFF_BUTTON) {
        OnOffButtonEvent();
        OnOffButtonState = abs(OnOffButtonState - 1);
    }
    else if (OnOffButtonState == TRUE) {
        
        if (button == SAVE_BUTTON && SaveButtonState == FALSE) {
            SaveButtonState = TRUE;
            LoadButtonState = FALSE;
            SetLedStates(FALSE); Serial.println("--------------------------1111111");
        }
        else if (button == LOAD_BUTTON && LoadButtonState == FALSE) {
            SaveButtonState = FALSE;
            LoadButtonState = TRUE;
            SetLedStates(FALSE); Serial.println("--------------------------2222222");
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
            if (button == OneButton) {
                Serial.println("--------------------------One button Save");
                SaveLedToEEPROM(1, RedLedValue, GreenLedValue, BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
            else if (button == TwoButton) {
                Serial.println("--------------------------Two button Save");
                SaveLedToEEPROM(4, RedLedValue, GreenLedValue, BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
            else if (button == ThreeButton) {
                Serial.println("--------------------------Three button Save");
                SaveLedToEEPROM(7, RedLedValue, GreenLedValue, BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
        }
        else if (SaveButtonState == FALSE && LoadButtonState == TRUE) {
            if (button == OneButton) {
                Serial.println("--------------------------One button Load");
                LoadLedFromEEPROM(1, &RedLedValue, &GreenLedValue, &BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
            else if (button == TwoButton) {
                Serial.println("--------------------------Two button Load");
                LoadLedFromEEPROM(4, &RedLedValue, &GreenLedValue, &BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
            else if (button == ThreeButton) {
                Serial.println("--------------------------Three button Load");
                LoadLedFromEEPROM(7, &RedLedValue, &GreenLedValue, &BlueLedValue);
                LoadButtonState = FALSE;
                SaveButtonState = FALSE;
            }
        }
    }
}

void Illumination() {
    if (OnOffButtonState != FALSE) {
        if (illuminate < (RedLedValue / 10)) {
            digitalWrite(RedLed, LOW);
        }
        else {
            digitalWrite(RedLed, HIGH);
        }

        if (illuminate < (BlueLedValue / 10)) {
            digitalWrite(BlueLed, LOW);
        }
        else {
            digitalWrite(BlueLed, HIGH);
        }

        if (illuminate < (GreenLedValue / 10)) {
            digitalWrite(GreenLed, LOW);
        }
        else {
            digitalWrite(GreenLed, HIGH);
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
    byte CurrentButton= GetRemoteSignal();

    if (CurrentButton != 0) {
        Serial.println(CurrentButton, HEX);
        Serial.println("");
        Serial.println("---------------------");
        Serial.print("Red led: ");
        Serial.println(RedLedState, DEC);
        Serial.print("Green led: ");
        Serial.println(GreenLedState, DEC);
        Serial.print("Blue led: ");
        Serial.println(BlueLedState, DEC);
        Serial.println("");
        Serial.print("Red led value: ");
        Serial.println(RedLedValue, DEC);
        Serial.print("Green led value: ");
        Serial.println(GreenLedValue, DEC);
        Serial.print("Blue value: ");
        Serial.println(BlueLedValue, DEC);
        Serial.println("");
    }

    ButtonEventManager(CurrentButton);

    Illumination();

}