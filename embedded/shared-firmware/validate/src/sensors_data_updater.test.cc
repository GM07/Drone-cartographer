#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stub_firmware_controller.h"

using ::testing::_;
using ::testing::Return;

TEST(ValidateSensorsDataUpdater, UpdateSensorsDataShouldForward) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));

  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              updateSensorsData())
      .Times(1);

  drone.updateSensorsData();
}

TEST(ValidateSensorsDataUpdater, updateCrashStatusIfNotCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, isDroneCrashed()).Times(1).WillOnce(Return(false));

  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.getController()->state = State::kIdle;

  drone.updateCrashStatus();

  EXPECT_EQ(drone.getController()->state, State::kIdle);
}

TEST(ValidateSensorsDataUpdater, updateCrashStatusIfCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, isDroneCrashed()).Times(1).WillOnce(Return(true));

  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  drone.updateCrashStatus();

  EXPECT_EQ(drone.getController()->state, State::kCrash);
}