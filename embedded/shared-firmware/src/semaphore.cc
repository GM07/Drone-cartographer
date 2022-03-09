#include "utils/semaphore.h"

Semaphore::Semaphore() : m_count(0), m_maxSize(-1) {}
Semaphore::Semaphore(size_t maxSize) : m_count(0), m_maxSize(maxSize) {}

void Semaphore::release() {
  std::lock_guard<decltype(m_mutex)> lock(m_mutex);
  if (m_count < m_maxSize) {
    ++m_count;
  }
  m_condition.notify_one();
}

void Semaphore::acquire() {
  std::unique_lock<decltype(m_mutex)> lock(m_mutex);
  while (m_count == 0U) {  // Handle spurious wake-ups.
    m_condition.wait(lock);
  }
  --m_count;
}

bool Semaphore::tryAcquire() {
  std::lock_guard<decltype(m_mutex)> lock(m_mutex);

  if (m_count != 0U) {
    --m_count;
    return true;
  }
  return false;
}
