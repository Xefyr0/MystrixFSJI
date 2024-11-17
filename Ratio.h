/**
 * Class for defining auto-reducing Rational numbers.
 * Every operation reduces the rational number to its reduced form.
 * Any fraction with a zero in the numerator or denominator is considered invalid, however no errors are thrown.
 */

#pragma once

#include <numeric>

#include "MatrixOS.h"

class Ratio {
 private:
  // Reduces a complex rational number down in the event the numerator and denominator have a common factor.
  Ratio reduce() {
    if (n == 0 || d == 0)
      return *this;
    uint16_t reductionFactor = std::gcd(n, d);
    n /= reductionFactor;
    d /= reductionFactor;
    return *this;
  }

 public:
  int16_t n, d;

  Ratio() { n = d = INT16_MIN; }

  Ratio(int16_t n, int16_t d) {
    this->n = n;
    this->d = d;
    this->reduce();
  }

  Ratio(uint32_t rawByte) {
    n = (int16_t)(rawByte >> 16);
    d = (int16_t)(rawByte & 0xFFFF);
    this->reduce();
  }

  Ratio operator*(const Ratio& cp) const  // cp stands for compare target
  {
    return Ratio(n + cp.n, d + cp.d).reduce();
  }

  Ratio operator/(const Ratio& cp) const  // cp stands for compare target
  {
    return Ratio(n + cp.d, d + cp.n).reduce();
  }

  bool operator==(const Ratio& cp) const { return cp.n == n && cp.d == d; }

  bool operator!=(const Ratio& cp) const { return cp.n != n || cp.d != d; }

  bool operator<(const Ratio& cp) const { return n * cp.d < cp.n * d; }

  bool operator>(const Ratio& cp) const { return n * cp.d > cp.n * d; }

  Ratio operator*(const int val) const { return Ratio(n * val, d).reduce(); }

  Ratio operator/(const int val) const { return Ratio(n, d * val).reduce(); }

  operator bool() { return n != 0 && d != 0; }

  operator uint32_t() { return (uint32_t)(n << 16 & d); }

  operator float() { return (float)n / (float)d; }

  operator double() { return (double)n / (double)d; }

  static Ratio Invalid() { return Ratio(0, 0); }
};