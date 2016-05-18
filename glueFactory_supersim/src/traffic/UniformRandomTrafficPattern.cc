/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/UniformRandomTrafficPattern.h"

#include "random/Random.h"

UniformRandomTrafficPattern::UniformRandomTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self, Json::Value _settings)
    : TrafficPattern(_name, _parent, _numTerminals, _self) {}

UniformRandomTrafficPattern::~UniformRandomTrafficPattern() {}

u32 UniformRandomTrafficPattern::nextDestination() {
  return gRandom->randomU64(0, numTerminals_ - 1);
}
