/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/BitComplementTrafficPattern.h"

#include <cassert>

BitComplementTrafficPattern::BitComplementTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self, Json::Value _settings)
    : TrafficPattern(_name, _parent, _numTerminals, _self) {}

BitComplementTrafficPattern::~BitComplementTrafficPattern() {}

u32 BitComplementTrafficPattern::nextDestination() {
  return (self_ + (numTerminals_ / 2)) % numTerminals_;
}
