#ifndef UNIT_TEST_CPP
#define UNIT_TEST_CPP
#endif

#include <gtest/gtest.h>
#include "RemoteLED.hpp"

TEST(GetEEPROM_Address_TEST, RemoteLED) {
  uint8_t TestNumber = 2;
  EXPECT_EQ(GetEEPROM_Address(TestNumber), (TestNumber * COLOR_EEPROM_ALLOCATION_SIZE) + 1);
  EXPECT_NE(GetEEPROM_Address(TestNumber), (TestNumber * COLOR_EEPROM_ALLOCATION_SIZE) + 2);
  EXPECT_NE(GetEEPROM_Address(TestNumber), TestNumber * COLOR_EEPROM_ALLOCATION_SIZE);
}

TEST(SetLedState_TEST, RemoteLED) {
  SetLedStates(TRUE);
  RedLedState = FALSE;
  SetLedState(&RedLedState);
  EXPECT_EQ(TRUE, RedLedState);
  EXPECT_EQ(FALSE, GreenLedState);
  EXPECT_EQ(FALSE, BlueLedState);

  SetLedStates(TRUE);
  GreenLedState = FALSE;
  SetLedState(&GreenLedState);
  EXPECT_EQ(FALSE, RedLedState);
  EXPECT_EQ(TRUE, GreenLedState);
  EXPECT_EQ(FALSE, BlueLedState);

  SetLedStates(TRUE);
  BlueLedState  = FALSE;
  SetLedState(&BlueLedState);
  EXPECT_EQ(FALSE, RedLedState);
  EXPECT_EQ(FALSE, GreenLedState);
  EXPECT_EQ(TRUE, BlueLedState);
}

TEST(SetLedStates_TEST, RemoteLED) {
  RedLedState   = FALSE;
  BlueLedState  = FALSE;
  GreenLedState = FALSE;

  SetLedStates(TRUE);
  EXPECT_EQ(TRUE, RedLedState);
  EXPECT_EQ(TRUE, GreenLedState);
  EXPECT_EQ(TRUE, BlueLedState);

  SetLedStates(FALSE);
  EXPECT_EQ(FALSE, RedLedState);
  EXPECT_EQ(FALSE, GreenLedState);
  EXPECT_EQ(FALSE, BlueLedState);
}

TEST(GetActiveLedNumber_TEST, RemoteLED) {
  for (RedLedValue = MIN_LED_VALUE; RedLedValue <= MIN_LED_VALUE + 1; RedLedValue++) {
    for (GreenLedValue = MIN_LED_VALUE; GreenLedValue <= MIN_LED_VALUE + 1; GreenLedValue++) {
      for (BlueLedValue = MIN_LED_VALUE; BlueLedValue <= MIN_LED_VALUE + 1; BlueLedValue++) {
        EXPECT_EQ(GetActiveLedNumber(), (RedLedValue + GreenLedValue + BlueLedValue) - (MIN_LED_VALUE * 3));
      }
    }
  }
}

TEST(GetActiveLedPointers_TEST, RemoteLED) {
  for (RedLedValue = MIN_LED_VALUE; RedLedValue <= MIN_LED_VALUE + 1; RedLedValue++) {
    for (GreenLedValue = MIN_LED_VALUE; GreenLedValue <= MIN_LED_VALUE + 1; GreenLedValue++) {
      for (BlueLedValue = MIN_LED_VALUE; BlueLedValue <= MIN_LED_VALUE + 1; BlueLedValue++) {
        uint8_t** pointers = GetActiveLedPointers();

        if (GetActiveLedNumber() == 0) {
          EXPECT_EQ(pointers[0], nullptr);
        }
        else if (GetActiveLedNumber() == 1) {
          if (RedLedValue) {
            EXPECT_EQ(pointers[0], &RedLedValue);
          }
          else if (GreenLedValue) {
            EXPECT_EQ(pointers[0], &GreenLedValue);
          }
          else {
            EXPECT_EQ(pointers[0], &BlueLedValue);
          }
          EXPECT_EQ(pointers[1], nullptr);
        }
        else if (GetActiveLedNumber() == 2) {
          if (!RedLedValue) {
            EXPECT_EQ(pointers[0], &GreenLedValue);
            EXPECT_EQ(pointers[1], &BlueLedValue);
          }
          else if (!GreenLedValue) {
            EXPECT_EQ(pointers[0], &RedLedValue);
            EXPECT_EQ(pointers[1], &BlueLedValue);
          }
          else {
            EXPECT_EQ(pointers[0], &RedLedValue);
            EXPECT_EQ(pointers[1], &GreenLedValue);
          }
          EXPECT_EQ(pointers[2], nullptr);
        }
        else {
          EXPECT_EQ(pointers[0], &RedLedValue);
          EXPECT_EQ(pointers[1], &GreenLedValue);
          EXPECT_EQ(pointers[2], &BlueLedValue);
          EXPECT_EQ(pointers[3], nullptr);
        }

        delete[] pointers;
      }
    }
  }
}

TEST(OnOffButtonEvent_TEST, RemoteLED) {
  OnOffButtonState                  = TRUE;
  SaveButtonState                   = TRUE;
  LoadButtonState                   = TRUE;
  dWriteValues[RED_LED_INDEX].val   = LOW_LED;
  dWriteValues[GREEN_LED_INDEX].val = LOW_LED;
  dWriteValues[BLUE_LED_INDEX].val  = LOW_LED;

  OnOffButtonEvent();
  EXPECT_EQ(dWriteValues[RED_LED_INDEX].val, HIGH_LED);
  EXPECT_EQ(dWriteValues[GREEN_LED_INDEX].val, HIGH_LED);
  EXPECT_EQ(dWriteValues[BLUE_LED_INDEX].val, HIGH_LED);
  EXPECT_EQ(SaveButtonState, FALSE);
  EXPECT_EQ(LoadButtonState, FALSE);

  OnOffButtonState                  = FALSE;
  SaveButtonState                   = TRUE;
  LoadButtonState                   = TRUE;
  dWriteValues[RED_LED_INDEX].val   = LOW_LED;
  dWriteValues[GREEN_LED_INDEX].val = LOW_LED;
  dWriteValues[BLUE_LED_INDEX].val  = LOW_LED;

  OnOffButtonEvent();
  EXPECT_EQ(dWriteValues[RED_LED_INDEX].val, LOW_LED);
  EXPECT_EQ(dWriteValues[GREEN_LED_INDEX].val, LOW_LED);
  EXPECT_EQ(dWriteValues[BLUE_LED_INDEX].val, LOW_LED);
  EXPECT_EQ(SaveButtonState, TRUE);
  EXPECT_EQ(LoadButtonState, TRUE);
}
