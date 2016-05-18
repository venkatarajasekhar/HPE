/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/TrafficPattern.h"

#include <cassert>

TrafficPattern::TrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self)
    : Component(_name, _parent), numTerminals_(_numTerminals), self_(_self) {
  assert(_self < _numTerminals);
}

TrafficPattern::~TrafficPattern() {}
