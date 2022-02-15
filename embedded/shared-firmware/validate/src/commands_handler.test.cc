#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "utils/commands.h"

using ::testing::_;

TEST(ValidateCommandsHandler, handleCommandIdentify) {
  Drone drone = Drone::getEmbeddedDrone();
  EXPECT_EQ(drone.handleCommand(Command::kIdentify, nullptr, 0), true);
  EXPECT_EQ(drone.getController()->state, State::kIdentify);
}

TEST(ValidateCommandsHandler, handleCommandTakeOff) {
  Drone drone = Drone::getEmbeddedDrone();
  EXPECT_EQ(drone.handleCommand(Command::kTakeOff, nullptr, 0), true);
  EXPECT_EQ(drone.getController()->state, State::kTakingOff);
}

TEST(ValidateCommandsHandler, handleCommandLand) {
  Drone drone = Drone::getEmbeddedDrone();
  EXPECT_EQ(drone.handleCommand(Command::kLand, nullptr, 0), true);
  EXPECT_EQ(drone.getController()->state, State::kLanding);
}

TEST(ValidateCommandsHandler, handleCommandInvalid) {
  Drone drone = Drone::getEmbeddedDrone();
  EXPECT_EQ(drone.handleCommand(static_cast<Command>(0x00), nullptr, 0), false);
}
