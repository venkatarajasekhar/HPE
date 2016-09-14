/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/ScanTrafficPattern.h"

#include <cassert>

ScanTrafficPattern::ScanTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self, Json::Value _settings)
    : TrafficPattern(_name, _parent, _numTerminals, _self) {}

ScanTrafficPattern::~ScanTrafficPattern() {}

u32 ScanTrafficPattern::nextDestination() {
  u32 next = next_;
  next_ = (next_ + 1) % numTerminals_;
  return next;
}
