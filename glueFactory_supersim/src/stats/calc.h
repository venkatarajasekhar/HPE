/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef STATS_CALC_H_
#define STATS_CALC_H_

#include <prim/prim.h>

#include <vector>

template <typename T>
f64 arithmeticMean(const std::vector<T>& _vals);

template <typename T>
f64 variance(const std::vector<T>& _vals, f64 _mean);

template <typename T>
f64 standardDeviation(f64 _variance);

template <typename T>
f64 slope(const std::vector<T>& _x, const std::vector<T>& _y);

#include "stats/calc.tcc"

#endif  // STATS_CALC_H_
