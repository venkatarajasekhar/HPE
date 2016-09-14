/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <gtest/gtest.h>
#include <prim/prim.h>

#include <cmath>
#include <cstdio>

#include "stats/calc.h"

TEST(calc, arithmeticMean) {
  std::vector<u64> v2 = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  ASSERT_EQ(arithmeticMean(v2), 2.0);
  std::vector<f64> v3 = {1, 2, 3, 1, 2, 3};
  ASSERT_EQ(arithmeticMean(v3), 2.0);
  std::vector<f64> v4 = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  ASSERT_EQ(arithmeticMean(v4), 2.0);
}

TEST(calc, slope) {
  std::vector<f64> t1 = {0, 1, 2, 3, 4, 5};
  std::vector<f64> v1 = {1, 2, 3, 4, 5, 6};
  ASSERT_LT(std::abs(slope(t1, v1) - 1.0), 0.01);
  std::vector<f64> v2 = {0, 2, 4, 6, 8, 10};
  ASSERT_LT(std::abs(slope(t1, v2) - 2.0), 0.01);

  std::vector<f64> t3 = {3, 4, 5, 0, 1, 2};
  std::vector<f64> v3 = {4, 5, 6, 1, 2, 3};
  ASSERT_LT(std::abs(slope(t3, v3) - 1.0), 0.01);
}
