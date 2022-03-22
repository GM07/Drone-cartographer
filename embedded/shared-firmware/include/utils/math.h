#ifndef MATH_H
#define MATH_H

#include <functional>

constexpr float kThreshold = 10.0e-3;

template <class T>
[[nodiscard]] inline bool areAlmostEqual(const T& first, const T& second,
                                         float threshold = kThreshold) {
  T const diff = std::minus<>{}(first, second);
  return diff < threshold && diff > -threshold;
}

#endif
