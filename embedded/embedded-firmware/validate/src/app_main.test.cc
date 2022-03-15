#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/app_main.h"

extern "C" {
#include "components/ccommunication_manager.h"
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
  EXPECT_EQ(&Drone::getEmbeddedDrone(), &Drone::getEmbeddedDrone());
}

TEST(ValidateAppMain, updateCrashStatusFun) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, supervisorIsTumbled())
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  Drone::getEmbeddedDrone().getController()->state = State::kIdle;

  updateCrashStatus();
  EXPECT_EQ(Drone::getEmbeddedDrone().getController()->state, State::kIdle);

  updateCrashStatus();
  EXPECT_EQ(Drone::getEmbeddedDrone().getController()->state, State::kCrash);
  delete mock;
}

TEST(ValidateAppMain, enableCrtpHighLevelCommand) {
  mock = new FunctionsMock;
  paramVarId_t mockParam;
  EXPECT_CALL(*mock, paramGetVarId(_, _)).WillOnce([&]() { return mockParam; });
  EXPECT_CALL(
      *mock,
      paramSetInt(_, 1));  // Wanted to expect mockParam as first argument but
                           // == operator is not overloaded :(

  enableCrtpHighLevelCommander();
  delete mock;
}
