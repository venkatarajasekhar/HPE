/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/LoopbackTrafficPattern.h"

LoopbackTrafficPattern::LoopbackTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _self,
    u32 _numTerminals, Json::Value _settings)
    : TrafficPattern(_name, _parent, _numTerminals, _self) {}

LoopbackTrafficPattern::~LoopbackTrafficPattern() {}

u32 LoopbackTrafficPattern::nextDestination() {
  return self_;
}
