/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/LslpArbiter.h"

#include "random/Random.h"

LslpArbiter::LslpArbiter(const std::string& _name, const Component* _parent,
                         u32 _size, Json::Value _settings)
    : Arbiter(_name, _parent, _size) {
  prevPriority_ = static_cast<u32>(gRandom->randomU64(0, size_));
  latch();
}

LslpArbiter::~LslpArbiter() {}

void LslpArbiter::latch() {
  priority_ = prevPriority_;
}

u32 LslpArbiter::arbitrate() {
  u32 winner = U32_MAX;
  for (u32 idx = priority_; idx < (size_ + priority_); idx++) {
    u32 client = idx % size_;
    if (*requests_[client]) {  // requesting
      winner = client;
      *grants_[client] = true;
      break;
    }
  }
  if (winner != U32_MAX) {
    prevPriority_ = (winner + 1) % size_;
  } else {
    prevPriority_ = priority_;
  }
  return winner;
}
