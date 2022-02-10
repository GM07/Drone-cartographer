
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>

#include "components/drone.h"
#include "stubs/stub_firmware_controller.h"

using ::testing::_;

TEST(validateCommunicationManager, shouldReturnIfDead) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              receiveMessage(_, _))
      .Times(1);
  drone.getController()->state = State::kDead;
  auto asyncFuture = std::async(std::launch::async,
                                [&drone] { drone.communicationManagerTask(); });

  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(100)) !=
              std::future_status::timeout);
}

TEST(validateCommunicationManager, shouldNotHandleCommandIfNoMessage) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              sendMessage(_, _))
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              receiveMessage(_, _))
      .Times(testing::AtLeast(1));

  auto asyncFuture = std::async(std::launch::async,
                                [&drone] { drone.communicationManagerTask(); });
  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(100)) ==
              std::future_status::timeout);

  drone.getController()->state = State::kDead;
}
