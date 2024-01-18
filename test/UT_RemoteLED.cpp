#ifndef UNIT_TEST_CPP
#define UNIT_TEST_CPP
#endif

#include <gtest/gtest.h>
#include "RemoteLED.hpp"

TEST(GetEEPROM_Address_TEST, RemoteLED) {
  EXPECT_EQ(GetEEPROM_Address(2), 9);
  EXPECT_NE(GetEEPROM_Address(2), 10);
  EXPECT_NE(GetEEPROM_Address(2), 8);
}

TEST(SetLedStates_TEST, RemoteLED) {
  SetLedStates(TRUE, &RedLedState, &GreenLedState, &BlueLedState);
  EXPECT_EQ(TRUE, RedLedState);
  EXPECT_EQ(TRUE, GreenLedState);
  EXPECT_EQ(TRUE, BlueLedState);

  SetLedStates(FALSE, &RedLedState, &GreenLedState, &BlueLedState);
  EXPECT_EQ(FALSE, RedLedState);
  EXPECT_EQ(FALSE, GreenLedState);
  EXPECT_EQ(FALSE, BlueLedState);
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
