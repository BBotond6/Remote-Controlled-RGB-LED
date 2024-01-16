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
  uint8_t RedLedState;
  uint8_t GreenLedState;
  uint8_t BlueLedState;

  SetLedStates(TRUE, &RedLedState, &GreenLedState, &BlueLedState);
  EXPECT_EQ(TRUE, RedLedState);
  EXPECT_EQ(TRUE, GreenLedState);
  EXPECT_EQ(TRUE, BlueLedState);

  SetLedStates(FALSE, &RedLedState, &GreenLedState, &BlueLedState);
  EXPECT_EQ(FALSE, RedLedState);
  EXPECT_EQ(FALSE, GreenLedState);
  EXPECT_EQ(FALSE, BlueLedState);
}
