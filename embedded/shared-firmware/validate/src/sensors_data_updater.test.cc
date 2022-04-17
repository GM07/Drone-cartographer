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

  drone.getController()->updateSensorsData();
}

TEST(ValidateSensorsDataUpdater, updateCrashStatusIfNotCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, isDroneCrashed()).Times(1).WillOnce(Return(false));

  Drone drone(controller);
  drone.getController()->m_state = State::kIdle;

  drone.updateCrashStatus();

  EXPECT_EQ(drone.getController()->m_state, State::kIdle);
}

TEST(ValidateSensorsDataUpdater, updateCrashStatusIfCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, isDroneCrashed()).Times(1).WillOnce(Return(true));

  Drone drone(controller);
  drone.updateCrashStatus();

  EXPECT_EQ(drone.getController()->m_state, State::kCrash);
}

TEST(ValidateSensorsDataUpdater, updateCrashStatusIfNotCrashedButWasCrashed) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, isDroneCrashed()).Times(1).WillOnce(Return(false));

  Drone drone(controller);
  drone.getController()->m_state = State::kCrash;
  drone.updateCrashStatus();

  EXPECT_EQ(drone.getController()->m_state, State::kIdle);
}

TEST(ValidateSensorsDataUpdater, updateStatusFromBatteryFalse) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, hasLowBattery()).Times(1).WillOnce(Return(false));
  Drone drone(controller);
  controller->m_state = State::kExploring;

  drone.updateStatusFromBattery();
  EXPECT_EQ(controller->m_state, State::kExploring);
}

TEST(ValidateSensorsDataUpdater, updateStatusFromBatteryTrue) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, hasLowBattery()).Times(1).WillOnce(Return(true));
  Drone drone(controller);
  controller->m_state = State::kExploring;

  drone.updateStatusFromBattery();
  EXPECT_EQ(controller->m_state, State::kReturningToBase);
}