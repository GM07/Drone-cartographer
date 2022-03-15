#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_functions.h"
#include "sensors/firmware_sensors.h"

using ::testing::_;
using ::testing::Return;

TEST(ValidateFirmwareSensors, getFrontDistance) {
  mock = new FunctionsMock;
  constexpr unsigned int kDistanceFront = 25;
  constexpr unsigned int kDistanceLeft = 30;
  constexpr unsigned int kDistanceRight = 35;
  constexpr unsigned int kDistanceBack = 40;
  constexpr unsigned int kPosX = 20;
  constexpr unsigned int kPosY = 15;
  constexpr unsigned int kPosZ = 10;
  constexpr unsigned int kBatteryLevel = 5;
  constexpr uint8_t kSides = 8;

  EXPECT_CALL(*mock, logGetVarId).Times(kSides);
  EXPECT_CALL(*mock, logGetUint)
      .WillOnce(Return(kDistanceFront))
      .WillOnce(Return(kDistanceLeft))
      .WillOnce(Return(kDistanceRight))
      .WillOnce(Return(kDistanceBack));

  EXPECT_CALL(*mock, logGetFloat)
      .WillOnce(Return(kPosX))
      .WillOnce(Return(kPosY))
      .WillOnce(Return(kPosZ))
      .WillOnce(Return(kBatteryLevel));

  FirmwareSensors sensors;
  EXPECT_EQ(sensors.getFrontDistance(), kDistanceFront);
  EXPECT_EQ(sensors.getLeftDistance(), kDistanceLeft);
  EXPECT_EQ(sensors.getRightDistance(), kDistanceRight);
  EXPECT_EQ(sensors.getBackDistance(), kDistanceBack);
  EXPECT_EQ(sensors.getPosX(), kPosX);
  EXPECT_EQ(sensors.getPosY(), kPosY);
  EXPECT_EQ(sensors.getPosZ(), kPosZ);
  EXPECT_EQ(sensors.getBatteryLevel(), kBatteryLevel);

  delete mock;
}
