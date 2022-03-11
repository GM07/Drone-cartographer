#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "components/ccommunication_manager.h"
}
#include "components/drone.h"
#include "mock_functions.h"

using ::testing::_;

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
