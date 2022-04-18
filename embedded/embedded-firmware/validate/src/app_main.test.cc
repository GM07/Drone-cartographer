#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/app_main.h"

extern "C" {
#include "components/crazyflie_task.h"
}
#include "components/drone.h"
#include "mock_functions.h"

using ::testing::_;
using ::testing::Return;

TEST(ValidateAppMain, CommunicationManagerInitShouldCreateTask) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _)).Times(1);
  communicationManagerInit();
  EXPECT_EQ(true, communicationManagerTest());
  delete mock;
}

TEST(ValidateAppMain, GetDroneShouldAlwaysReturnTheSameInstance) {
  mock = new FunctionsMock;
  EXPECT_EQ(&Drone::getEmbeddedDrone(), &Drone::getEmbeddedDrone());
  delete mock;
}

TEST(ValidateAppMain, updateCrashStatusFun) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, supervisorIsTumbled())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  Drone::getEmbeddedDrone().getController()->m_state = State::kIdle;

  updateCrashStatus();
  EXPECT_EQ(Drone::getEmbeddedDrone().getController()->m_state, State::kIdle);

  updateCrashStatus();
  EXPECT_EQ(Drone::getEmbeddedDrone().getController()->m_state, State::kCrash);
  delete mock;
}