#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "mock_functions.h"
#include "utils/led.h"

using ::testing::_;

TEST(ValidateEmbeddedFirmware, setLEDStateShouldRegisterAndRun) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, ledseqRegisterSequence(_)).Times(1);
  EXPECT_CALL(*mock, ledseqRun(_)).Times(1);
  Drone::getEmbeddedDrone().getController()->setLEDState(LED::kLedBlueLeft,
                                                         true, true);
  delete mock;
}
