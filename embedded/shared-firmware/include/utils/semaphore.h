#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <condition_variable>
#include <mutex>

/**
 * @brief Semaphore class for c++17
 * Inspired from
 * https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
 *
 */
class Semaphore {
 public:
  Semaphore();
  Semaphore(size_t maxSize);
  void release();
  void acquire();
  bool tryAcquire();

 private:
  size_t m_count = 0;  // Initialized as locked.
  std::mutex m_mutex;
  std::condition_variable m_condition;
  size_t m_maxSize = 0;
};

#endif