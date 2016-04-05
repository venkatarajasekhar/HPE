/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/RandomArbiter.h"

#include "random/Random.h"

RandomArbiter::RandomArbiter(const std::string& _name, const Component* _parent,
                             u32 _size, Json::Value _settings)
    : Arbiter(_name, _parent, _size) {}

RandomArbiter::~RandomArbiter() {}

u32 RandomArbiter::arbitrate() {
  u32 winner = U32_MAX;
  for (u32 client = 0; client < size_; client++) {
    if (*requests_[client]) {  // requesting
      // input enabled
      if (winner == U32_MAX) {
        // first contender
        winner = client;
      } else if (gRandom->randomBool()) {
        // won random tie-breaker
        winner = client;
      }
    }
  }
  if (winner != U32_MAX) {
    *grants_[winner] = true;
  }
  return winner;
}
