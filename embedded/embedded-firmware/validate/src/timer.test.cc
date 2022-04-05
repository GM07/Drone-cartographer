#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_functions.h"
#include "utils/time.h"

using ::testing::_;

TEST(validateTimer, delayMSShouldCallVTaskDelay) {
  const uint32_t kRandomTimerDelay = 25;
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, vTaskDelay(_)).Times(1);
  Time::delayMs(kRandomTimerDelay);
  delete mock;
}
