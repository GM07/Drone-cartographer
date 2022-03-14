#ifndef MATH_H
#define MATH_H

#include <functional>

template <class T>
[[nodiscard]] inline bool areAlmostEqual(const T& first, const T& second,
                                         float threshold = 10e-3) {
  T const diff = std::minus<>{}(first, second);
  return diff < threshold && diff > -threshold;
}

#endif
