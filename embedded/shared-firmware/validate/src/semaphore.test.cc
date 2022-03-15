#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>

#include "utils/semaphore.h"

TEST(ValidateSemaphore, SemaphoreShouldNotThrow) {
  EXPECT_NO_THROW(({
    Semaphore* semaphore = new Semaphore();
    delete semaphore;
  }));
}

TEST(ValidateSemaphore, SemaphoreAcquireYes) {
  Semaphore semaphore(1);
  auto asyncFuture = std::async(std::launch::async, [&semaphore] {
    semaphore.release();
    semaphore.acquire();
  });

  const int64_t kRandomThreadDelay = 5;

  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) != std::future_status::timeout);
}

TEST(ValidateSemaphore, SemaphoreAcquireNo) {
  Semaphore semaphore(1);
  auto asyncFuture =
      std::async(std::launch::async, [&semaphore] { semaphore.acquire(); });

  const int64_t kRandomThreadDelay = 10;

  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) == std::future_status::timeout);

  semaphore.release();
  EXPECT_TRUE(asyncFuture.wait_for(std::chrono::milliseconds(
                  kRandomThreadDelay)) != std::future_status::timeout);
}

TEST(ValidateSemaphore, TryAcquireShouldTellIfValue) {
  Semaphore semaphore;
  EXPECT_FALSE(semaphore.tryAcquire());
  semaphore.release();
  EXPECT_TRUE(semaphore.tryAcquire());
}