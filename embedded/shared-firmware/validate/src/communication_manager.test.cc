
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>

#include "components/drone.h"
#include "stubs/stub_firmware_controller.h"

using ::testing::_;
using ::testing::Return;

TEST(validateCommunicationManager, shouldReturnIfDead) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  EXPECT_CALL(*controller, receiveMessage(_, _)).Times(1);
  drone.getController()->m_state = State::kDead;
  auto asyncFuture = std::async(std::launch::async,
                                [&drone] { drone.communicationManagerTask(); });

  const int64_t kRandomThreadDelay = 125;

  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) != std::future_status::timeout);
}

TEST(validateCommunicationManager, shouldNotHandleCommandIfNoMessage) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));

  EXPECT_CALL(*controller, sendMessage(_, _)).Times(0);
  EXPECT_CALL(*controller, receiveMessage(_, _)).Times(testing::AtLeast(1));

  auto asyncFuture = std::async(std::launch::async,
                                [&drone] { drone.communicationManagerTask(); });

  const int64_t kRandomThreadDelay = 100;
  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) == std::future_status::timeout);

  drone.getController()->m_state = State::kDead;
}

TEST(validateCommunicationManager, shouldHandleCommandIfMessage) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, receiveMessage(_, _))
      .Times(2)
      .WillOnce(Return(true))
      .WillRepeatedly([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 0;
      });

  Drone drone(std::dynamic_pointer_cast<AbstractController>(controller));
  auto asyncFuture = std::async(std::launch::async,
                                [&drone] { drone.communicationManagerTask(); });
  const int64_t kRandomThreadDelay = 50;
  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) == std::future_status::timeout);

  drone.getController()->m_state = State::kDead;
}
