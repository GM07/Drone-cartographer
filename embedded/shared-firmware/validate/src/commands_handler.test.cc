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
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->state = State::kTakingOff;
  EXPECT_CALL(*controller, takeOff(_)).Times(1);
  EXPECT_TRUE(drone.handleCommand(Command::kTakeOff, nullptr, 0));
  EXPECT_EQ(drone.getController()->state, State::kTakingOff);
}

TEST(ValidateCommandsHandler, handleCommandLand) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->state = State::kLanding;
  EXPECT_CALL(*controller, land()).Times(1);
  EXPECT_TRUE(drone.handleCommand(Command::kLand, nullptr, 0));
  EXPECT_EQ(drone.getController()->state, State::kLanding);
}

TEST(ValidateCOmmandsHandler, shouldNotHandleCommandsIfCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->state = State::kCrash;

  EXPECT_FALSE(drone.handleCommand(Command::kIdentify, nullptr, 0));
  EXPECT_FALSE(drone.handleCommand(Command::kTakeOff, nullptr, 0));
  EXPECT_FALSE(drone.handleCommand(Command::kLand, nullptr, 0));

  EXPECT_EQ(drone.getController()->state, State::kCrash);
}

TEST(ValidateCommandsHandler, handleCommandInvalid) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_EQ(drone.handleCommand(static_cast<Command>(0x00), nullptr, 0), false);
}
