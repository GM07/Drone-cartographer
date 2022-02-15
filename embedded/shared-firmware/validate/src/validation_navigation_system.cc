#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stub_firmware_controller.h"

using ::testing::_;

TEST(ValidateNavigationSystem, stepTakingOff) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kTakingOff;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              takeOff(_))
      .Times(1);
  drone.step();
}

TEST(ValidateNavigationSystem, stepLanding) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kLanding;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              land())
      .Times(1);
  drone.step();
}

TEST(ValidateNavigationSystem, stepIdentify) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kIdentify;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              setLEDState(_, true, _))
      .Times(1);
  drone.step();
}

TEST(ValidateNavigationSystem, stateIdle) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->state = State::kIdle;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              takeOff(_))
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              land())
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              setLEDState(_, _, _))
      .Times(0);
  drone.step();
}
