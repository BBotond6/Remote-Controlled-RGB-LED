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

TEST(SetOneLedValue_TEST, RemoteLED) {
  // Mode TRUE case
  RedLedValue   = MIN_LED_VALUE;
  GreenLedValue = MIN_LED_VALUE;
  BlueLedValue  = MIN_LED_VALUE;
  SetOneLedValue(&RedLedValue, TRUE);
  EXPECT_EQ(RedLedValue, MIN_LED_VALUE + LED_STEP_VALUE);
  EXPECT_EQ(GreenLedValue, MIN_LED_VALUE);
  EXPECT_EQ(BlueLedValue, MIN_LED_VALUE);

  // Mode FALSE case
  RedLedValue   = MAX_LED_VALUE;
  GreenLedValue = MIN_LED_VALUE;
  BlueLedValue  = MIN_LED_VALUE;
  SetOneLedValue(&RedLedValue, FALSE);
  EXPECT_EQ(RedLedValue, MAX_LED_VALUE - LED_STEP_VALUE);
  EXPECT_EQ(GreenLedValue, MIN_LED_VALUE);
  EXPECT_EQ(BlueLedValue, MIN_LED_VALUE);

  // Mode TRUE case when LedValue is bigger then MAX_LED_VALUE - LED_STEP_VALUE
  RedLedValue   = MAX_LED_VALUE - 1;
  GreenLedValue = MIN_LED_VALUE;
  BlueLedValue  = MIN_LED_VALUE;
  SetOneLedValue(&RedLedValue, TRUE);
  EXPECT_EQ(RedLedValue, MAX_LED_VALUE);
  EXPECT_EQ(GreenLedValue, MIN_LED_VALUE);
  EXPECT_EQ(BlueLedValue, MIN_LED_VALUE);

  // Mode FALSE case when LedValue is smaller then MIN_LED_VALUE + LED_STEP_VALUE
  RedLedValue   = MIN_LED_VALUE + 1;
  GreenLedValue = MIN_LED_VALUE;
  BlueLedValue  = MIN_LED_VALUE;
  SetOneLedValue(&RedLedValue, FALSE);
  EXPECT_EQ(RedLedValue, MIN_LED_VALUE);
  EXPECT_EQ(GreenLedValue, MIN_LED_VALUE);
  EXPECT_EQ(BlueLedValue, MIN_LED_VALUE);
}

TEST(SetColorValues_TEST, RemoteLED) {
  uint8_t* led1;
  uint8_t* led2;
  uint8_t* led3;
  uint8_t  RedLedActive;
  uint8_t  GreenLedActive;
  uint8_t  BlueLedActive;
  uint8_t  TestLedValue1 = 210;
  uint8_t  TestLedValue2 =  20;
  uint8_t  TestLedValue3 =  80;

  // 2 active LED
  for (RedLedActive = 0; RedLedActive < 2; RedLedActive++) {
    for (GreenLedActive = 0; GreenLedActive < 2; GreenLedActive++) {
      for (BlueLedActive = 0; BlueLedActive < 2; BlueLedActive++) {

        if (RedLedActive + GreenLedActive + BlueLedActive == 2) {
          if (RedLedActive && GreenLedActive && !BlueLedActive) {
            led1 = &RedLedValue;
            led2 = &GreenLedValue;
            led3 = &BlueLedValue;
          }
          else if (RedLedActive && !GreenLedActive && BlueLedActive){
            led1 = &BlueLedValue;
            led2 = &RedLedValue;
            led3 = &GreenLedValue;
          }
          else {
            led1 = &GreenLedValue;
            led2 = &BlueLedValue;
            led3 = &RedLedValue;
          }

          // TRUE case
          *led1 = TestLedValue1;
          *led2 = TestLedValue2;
          *led3 = MIN_LED_VALUE;
          SetColorValues(TRUE);
          EXPECT_EQ(*led1, TestLedValue1 + LED_STEP_VALUE);
          EXPECT_EQ(*led2, round(((float)*led1 / (float)TestLedValue1) * TestLedValue2));
          EXPECT_EQ(*led3, MIN_LED_VALUE);

          // FALSE case
          *led1 = TestLedValue1;
          *led2 = TestLedValue2;
          *led3 = MIN_LED_VALUE;
          SetColorValues(FALSE);
          EXPECT_EQ(*led1, round(((float)*led2 / (float)TestLedValue2) * TestLedValue1));
          EXPECT_EQ(*led2, TestLedValue2 - LED_STEP_VALUE);
          EXPECT_EQ(*led3, MIN_LED_VALUE);

          // TRUE case when *led1 bigger then MAX_LED_VALUE + LED_STEP_VALUE
          *led1 = MAX_LED_VALUE - LED_STEP_VALUE + 1;
          *led2 = TestLedValue1;
          *led3 = MIN_LED_VALUE;
          SetColorValues(TRUE);
          EXPECT_EQ(*led1, MAX_LED_VALUE);
          EXPECT_EQ(*led2, round(((float)MAX_LED_VALUE / (float)(MAX_LED_VALUE - LED_STEP_VALUE + 1))
                    * TestLedValue1));
          EXPECT_EQ(*led3, MIN_LED_VALUE);

          // FALSE case when *led1 smaller then MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE
          *led1 = MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE - 1;
          *led2 = TestLedValue1;
          *led3 = MIN_LED_VALUE;
          SetColorValues(FALSE);
          EXPECT_EQ(*led1, MIXED_COLOR_MIN_LED_VALUE);
          EXPECT_EQ(*led2, round(((float)MIXED_COLOR_MIN_LED_VALUE /
                    (float)(MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE - 1)) * TestLedValue1));
          EXPECT_EQ(*led3, MIN_LED_VALUE);
        }
      }
    }
  }

  // 3 LED active
  for (RedLedActive = 0; RedLedActive < LED_NUMBER; RedLedActive++) {
    if (RedLedActive == 0) {
      led1 = &RedLedValue;
      led2 = &GreenLedValue;
      led3 = &BlueLedValue;
    }
    else if (RedLedActive == 1){
      led1 = &BlueLedValue;
      led2 = &RedLedValue;
      led3 = &GreenLedValue;
    }
    else {
      led1 = &GreenLedValue;
      led2 = &BlueLedValue;
      led3 = &RedLedValue;
    }

    // TRUE case
    *led1 = TestLedValue1;
    *led2 = TestLedValue2;
    *led3 = TestLedValue3;
    SetColorValues(TRUE);
    EXPECT_EQ(*led1, TestLedValue1 + LED_STEP_VALUE);
    EXPECT_EQ(*led2, round(((float)*led1 / (float)TestLedValue1) * TestLedValue2));
    EXPECT_EQ(*led3, round(((float)*led1 / (float)TestLedValue1) * TestLedValue3));

    // FALSE case
    *led1 = TestLedValue1;
    *led2 = TestLedValue2;
    *led3 = TestLedValue3;
    SetColorValues(FALSE);
    EXPECT_EQ(*led1, round(((float)*led2 / (float)TestLedValue2) * TestLedValue1));
    EXPECT_EQ(*led2, TestLedValue2 - LED_STEP_VALUE);
    EXPECT_EQ(*led3, round(((float)*led2 / (float)TestLedValue2) * TestLedValue3));

    // TRUE case when *led1 bigger then MAX_LED_VALUE + LED_STEP_VALUE
    *led1 = MAX_LED_VALUE - LED_STEP_VALUE + 1;
    *led2 = TestLedValue1;
    *led3 = TestLedValue3;
    SetColorValues(TRUE);
    EXPECT_EQ(*led1, MAX_LED_VALUE);
    EXPECT_EQ(*led2, round(((float)MAX_LED_VALUE / (float)(MAX_LED_VALUE - LED_STEP_VALUE + 1))
              * TestLedValue1));
    EXPECT_EQ(*led3, round(((float)MAX_LED_VALUE / (float)(MAX_LED_VALUE - LED_STEP_VALUE + 1))
              * TestLedValue3));

    // FALSE case when *led1 smaller then MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE
    *led1 = MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE - 1;
    *led2 = TestLedValue1;
    *led3 = TestLedValue3;
    SetColorValues(FALSE);
    EXPECT_EQ(*led1, MIXED_COLOR_MIN_LED_VALUE);
    EXPECT_EQ(*led2, round(((float)MIXED_COLOR_MIN_LED_VALUE /
              (float)(MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE - 1)) * TestLedValue1));
    EXPECT_EQ(*led3, round(((float)MIXED_COLOR_MIN_LED_VALUE /
              (float)(MIXED_COLOR_MIN_LED_VALUE + LED_STEP_VALUE - 1)) * TestLedValue3));
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
