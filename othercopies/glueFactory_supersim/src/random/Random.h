/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef RANDOM_RANDOM_H_
#define RANDOM_RANDOM_H_

#include <json/json.h>
#include <prim/prim.h>

#include <random>

class Random {
 public:
  explicit Random(Json::Value _settings);
  ~Random();
  void seed(u64 _seed);
  u64 randomU64();
  u64 randomU64(u64 _min, u64 _max);
  f64 randomF64();
  f64 randomF64(f64 _min, f64 _max);  // _max is exclusive
  bool randomBool();

 private:
  std::mt19937_64 prng_;
  std::uniform_int_distribution<u64> intDist_;  // this defaults to [0,2^64-1]
  std::uniform_real_distribution<f64> realDist_;  // this defaults to [0,1)
};

extern Random* gRandom;

#endif  // RANDOM_RANDOM_H_
