/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <cmath>

#include <numeric>
#include <vector>

template <typename T>
f64 arithmeticMean(const std::vector<T>& _vals) {
  f64 sum = 0.0;
  for (auto it = _vals.cbegin(); it != _vals.cend(); ++it) {
    sum += *it;
  }
  return sum / _vals.size();
}

template <typename T>
f64 variance(const std::vector<T>& _vals, f64 _mean) {
  f64 diffSum = 0.0;
  for (auto it = _vals.cbegin(); it != _vals.cend(); ++it) {
    f64 diff = *it - _mean;
    diffSum += (diff * diff);
  }
  return diffSum / _vals.size();
}

template <typename T>
f64 standardDeviation(f64 _variance) {
  return sqrt(_variance);
}

template <typename T>
f64 slope(const std::vector<T>& _x, const std::vector<T>& _y) {
  assert(_x.size() == _y.size());

  // get a size
  const f64 n = _x.size();

  // sums the vectors
  const f64 xs = std::accumulate(_x.cbegin(), _x.cend(), 0.0);
  const f64 ys = std::accumulate(_y.cbegin(), _y.cend(), 0.0);

  // compute the inner products
  const f64 xx = std::inner_product(_x.cbegin(), _x.cend(), _x.cbegin(), 0.0);
  const f64 xy = std::inner_product(_x.cbegin(), _x.cend(), _y.cbegin(), 0.0);

  // compute the slope
  return (n * xy - xs * ys) / (n * xx - xs * xs);
}
