/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "random/Random.h"

#include <cassert>

Random::Random(Json::Value _settings) {
  u64 s = _settings["seed"].asUInt64();
  assert(s != 0);
  this->seed(s);
}

Random::~Random() {}

void Random::seed(u64 _seed) {
  std::seed_seq seq = {(u32)((_seed >> 32) & 0xFFFFFFFFlu),
                       (u32)((_seed >>  0) & 0xFFFFFFFFlu)};
  prng_.seed(seq);
}

u64 Random::randomU64() {
  return intDist_(prng_);
}

u64 Random::randomU64(u64 _min, u64 _max) {
  assert(_max >= _min);
  if (_min == _max) {
    return _min;
  }
  u64 rand = intDist_(prng_);
  u64 span = _max - _min;
  if (span == U64_MAX) {
    return rand;
  } else {
    return (rand % (span + 1)) + _min;
  }
}

f64 Random::randomF64() {
  return realDist_(prng_);
}

f64 Random::randomF64(f64 _min, f64 _max) {
  assert(_max >= _min);
  f64 r = realDist_(prng_);
  r *= (_max - _min);
  r += _min;
  return r;
}

bool Random::randomBool() {
  return static_cast<bool>(intDist_(prng_) & 0x1);
}

/* globals */
Random* gRandom;
