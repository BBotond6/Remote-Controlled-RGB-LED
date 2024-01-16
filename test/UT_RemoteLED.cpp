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
