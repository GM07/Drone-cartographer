#ifndef MATH_H
#define MATH_H

#include <functional>

constexpr float kThreshold = 10.0e-3;

namespace Math {

constexpr float kPi = 3.14159265358979323846F;

///////////////////////////////////////////////
template <class T>
[[nodiscard]] constexpr inline bool areAlmostEqual(
    const T& first, const T& second, float threshold = kThreshold) {
  T const diff = std::minus<>{}(first, second);
  return diff < threshold && diff > -threshold;
}

template <typename T>
constexpr T pi = static_cast<T>(3.14159265358979323846L);  // NOLINT

constexpr std::size_t kMaxDepth = 10;

///////////////////////////////////////////////
// n!
template <typename T>
[[nodiscard]] constexpr T factorial(T n) {
  T res = 1;
  for (T i = 2; i <= n; ++i) {
    res *= i;
  }
  return res;
}

///////////////////////////////////////////////
template <typename T>
[[nodiscard]] constexpr T isEven(T number) {
  return !(number % 2);
}

///////////////////////////////////////////////
template <typename T>
[[nodiscard]] constexpr T abs(T number) {
  return (number < 0) ? -number : number;
}

///////////////////////////////////////////////
// https://blogs.sas.com/content/iml/2016/05/16/babylonian-square-roots.html
///////////////////////////////////////////////
template <typename T>
[[nodiscard]] constexpr T __sqrt(T number, T guess) {
  return areAlmostEqual(number, guess * guess)
             ? guess
             : __sqrt(number, (guess + number / guess) / 2);
}

///////////////////////////////////////////////
template <typename T>
[[nodiscard]] constexpr T sqrt(T number) {
  constexpr T kInitialGuess = 10;
  return __sqrt(number, kInitialGuess);
}

///////////////////////////////////////////////
// Fortement inspiré de ce lien :
// https://stackoverflow.com/questions/26860574/pow-implementation-in-cmath-and-efficient-replacement
// Divide and conquer ;)
///////////////////////////////////////////////
template <typename Number, typename Exponent>
[[nodiscard]] constexpr std::common_type_t<Number, Exponent> powRecursif(
    Number num, Exponent exponent) {
  if (exponent == 0) {
    return 1;
  };
  if (exponent % 2 == 0) {
    return powRecursif(num * num, (exponent / 2));
  };
  return num * powRecursif(num * num, (exponent / 2));
}

///////////////////////////////////////////////
template <typename Number, typename Exponent>
[[nodiscard]] constexpr std::common_type_t<Number, Exponent> pow(
    Number number, Exponent exponent) {
  if (exponent > 0) {
    return powRecursif(number, exponent);
  };
  return 1 / powRecursif(number, -exponent);
}

///////////////////////////////////////////////
// x^n / (n!)
template <typename T>
[[nodiscard]] constexpr T xnNFac(T x, int n) {
  return Math::pow(x, n) / factorial(n);
}

///////////////////////////////////////////////
template <std::size_t N, typename T>
[[nodiscard]] constexpr T taylorSin(T value) {
  if constexpr (N == 0) {
    return 0;
  }

  if constexpr (isEven((N - 1) / 2)) {
    return xnNFac(value, N) + taylorSin < (N > kMaxDepth) ? 0 : N + 2 > (value);
  } else {
    return -xnNFac(value, N) + taylorSin < (N > kMaxDepth) ? 0
                                                           : N + 2 > (value);
  }
}

///////////////////////////////////////////////
template <std::size_t N, typename T>
[[nodiscard]] constexpr T taylorCos(T value) {
  if constexpr (N == 0) {
    return 0;
  }

  if constexpr (isEven((N) / 2)) {
    return xnNFac(value, N) + taylorCos < (N > kMaxDepth) ? 0 : N + 2 > (value);
  } else {
    return -xnNFac(value, N) + taylorCos < (N > kMaxDepth) ? 0
                                                           : N + 2 > (value);
  }
}

/////////////////////////////////////////////////////////
// Implementation du sin(x) à partir de la série de Taylor
template <typename T>
[[nodiscard]] constexpr T sin(T x) {
  return x + taylorSin<3>(x);
}

/////////////////////////////////////////////////////////
// Implementation du cos(x) à partir de la série de Taylor
template <typename T>
[[nodiscard]] constexpr T cos(T value) {
  return 1 + taylorCos<2>(value);
}

}  // namespace Math

#endif
