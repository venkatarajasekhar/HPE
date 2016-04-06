/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "traffic/TrafficPatternFactory.h"

#include <cassert>

#include "traffic/BitComplementTrafficPattern.h"
#include "traffic/LoopbackTrafficPattern.h"
#include "traffic/ScanTrafficPattern.h"
#include "traffic/UniformRandomTrafficPattern.h"

TrafficPattern* TrafficPatternFactory::createTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self, Json::Value _settings) {
  const std::string& type = _settings["type"].asString();
  if (type == "bit_complement") {
    return new BitComplementTrafficPattern(
        _name, _parent, _numTerminals, _self, _settings);
  } else if (type == "loopback") {
    return new LoopbackTrafficPattern(
        _name, _parent, _numTerminals, _self, _settings);
  } else if (type == "scan") {
    return new ScanTrafficPattern(
        _name, _parent, _numTerminals, _self, _settings);
  } else if (type == "uniform_random") {
    return new UniformRandomTrafficPattern(
        _name, _parent, _numTerminals, _self, _settings);
  } else {
    fprintf(stderr, "unknown traffic pattern: %s\n", type.c_str());
    assert(false);
  }
}
