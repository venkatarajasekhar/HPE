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
#include <gtest/gtest.h>
#include <prim/prim.h>

#include <cmath>
#include <cstdio>

#include <map>
#include <random>

#include "mut/mut.h"

TEST(arithmeticMean, simple) {
  std::vector<u64> v2 = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  ASSERT_EQ(mut::arithmeticMean(v2), 2.0);
  std::vector<f64> v3 = {1, 2, 3, 1, 2, 3};
  ASSERT_EQ(mut::arithmeticMean(v3), 2.0);
  std::vector<f64> v4 = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  ASSERT_EQ(mut::arithmeticMean(v4), 2.0);
}

TEST(slope, simple) {
  std::vector<f64> t1 = {0, 1, 2, 3, 4, 5};
  std::vector<f64> v1 = {1, 2, 3, 4, 5, 6};
  ASSERT_LT(std::abs(mut::slope(t1, v1) - 1.0), 0.01);
  std::vector<f64> v2 = {0, 2, 4, 6, 8, 10};
  ASSERT_LT(std::abs(mut::slope(t1, v2) - 2.0), 0.01);

  std::vector<f64> t3 = {3, 4, 5, 0, 1, 2};
  std::vector<f64> v3 = {4, 5, 6, 1, 2, 3};
  ASSERT_LT(std::abs(mut::slope(t3, v3) - 1.0), 0.01);
}
