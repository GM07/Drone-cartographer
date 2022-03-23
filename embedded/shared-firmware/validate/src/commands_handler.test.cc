#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stubs/stub_firmware_controller.h"
#include "utils/commands.h"

using ::testing::_;

TEST(ValidateCommandsHandler, handleCommandIdentify) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, blinkLED(_)).Times(1);

  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));

  EXPECT_EQ(drone.handleCommand(Command::kIdentify), true);
}

TEST(ValidateCommandsHandler, handleCommandTakeOff) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->m_state = State::kTakingOff;
  EXPECT_CALL(*controller, takeOff(_)).Times(1);
  EXPECT_TRUE(drone.handleCommand(Command::kTakeOff));
  EXPECT_EQ(drone.getController()->m_state, State::kTakingOff);
}

TEST(ValidateCommandsHandler, handleCommandLand) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->m_state = State::kLanding;
  EXPECT_CALL(*controller, land()).Times(1);
  EXPECT_TRUE(drone.handleCommand(Command::kLand));
  EXPECT_EQ(drone.getController()->m_state, State::kLanding);
}

TEST(ValidateCOmmandsHandler, shouldNotHandleCommandsIfCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->m_state = State::kCrash;

  EXPECT_FALSE(drone.handleCommand(Command::kIdentify));
  EXPECT_FALSE(drone.handleCommand(Command::kTakeOff));
  EXPECT_FALSE(drone.handleCommand(Command::kLand));

  EXPECT_EQ(drone.getController()->m_state, State::kCrash);
}

TEST(ValidateCommandsHandler, handleCommandInvalid) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_EQ(drone.handleCommand(static_cast<Command>(0x00)), false);
}
