/*
 * Copyright (c) 2012-2015, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MUT_MUT_H_
#error "do not include this file, use the .h instead"
#else  // MUT_MUT_H_

#include <cassert>
#include <cmath>

#include <numeric>
#include <vector>

namespace mut {

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

}  // namespace mut

#endif  // MUT_MUT_H_
