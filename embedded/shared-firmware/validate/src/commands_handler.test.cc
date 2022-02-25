#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stubs/stub_firmware_controller.h"
#include "utils/commands.h"

using ::testing::_;

TEST(ValidateCommandsHandler, handleCommandIdentify) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_EQ(drone.handleCommand(Command::kIdentify, nullptr, 0), true);
}

TEST(ValidateCommandsHandler, handleCommandTakeOff) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kTakingOff;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              takeOff(_))
      .Times(1);
  EXPECT_EQ(drone.handleCommand(Command::kTakeOff, nullptr, 0), true);
  EXPECT_EQ(drone.getController()->state, State::kTakingOff);
}

TEST(ValidateCommandsHandler, handleCommandLand) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kLanding;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              land())
      .Times(1);
  EXPECT_EQ(drone.handleCommand(Command::kLand, nullptr, 0), true);
  EXPECT_EQ(drone.getController()->state, State::kLanding);
}

TEST(ValidateCommandsHandler, handleCommandInvalid) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_EQ(drone.handleCommand(static_cast<Command>(0x00), nullptr, 0), false);
}
