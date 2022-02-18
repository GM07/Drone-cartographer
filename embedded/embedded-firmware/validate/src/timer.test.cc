#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_functions.h"
#include "utils/timer.h"

using ::testing::_;

TEST(validateTimer, delayMSShouldCallVTaskDelay) {
  const uint32_t kRandomTimerDelay = 25;
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, vTaskDelay(_)).Times(1);
  Timer::delayMs(kRandomTimerDelay);
  delete mock;
}