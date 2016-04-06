/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/ComparingArbiter.h"

#include "random/Random.h"

ComparingArbiter::ComparingArbiter(
    const std::string& _name, const Component* _parent,
    u32 _size, Json::Value _settings)
    : Arbiter(_name, _parent, _size) {
  greater_ = _settings["greater"].asBool();
}

ComparingArbiter::~ComparingArbiter() {}

u32 ComparingArbiter::arbitrate() {
  u32 winner = U32_MAX;
  u64 best = U64_MAX;
  for (u32 client = 0; client < size_; client++) {
    if (*requests_[client]) {
      u64 cmeta = *metadatas_[client];
      // input enabled
      if (winner == U32_MAX) {
        // first contender
        best = cmeta;
        winner = client;
      } else {
        // secondary contender
        if (((greater_) && (cmeta > best)) ||
            ((!greater_) && (cmeta < best)) ||
            ((cmeta == best) && (gRandom->randomBool()))) {
          // better metadata or won tie breaker
          best = cmeta;
          winner = client;
        }
      }
    }
  }
  if (winner != U32_MAX) {
    *grants_[winner] = true;
  }
  return winner;
}
