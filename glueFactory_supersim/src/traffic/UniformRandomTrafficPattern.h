/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TRAFFIC_UNIFORMRANDOMTRAFFICPATTERN_H_
#define TRAFFIC_UNIFORMRANDOMTRAFFICPATTERN_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "traffic/TrafficPattern.h"
#include "event/Component.h"

class UniformRandomTrafficPattern : public TrafficPattern {
 public:
  UniformRandomTrafficPattern(
      const std::string& _name, const Component* _parent, u32 _numTerminals,
      u32 _self, Json::Value _settings);
  ~UniformRandomTrafficPattern();
  u32 nextDestination() override;
};

#endif  // TRAFFIC_UNIFORMRANDOMTRAFFICPATTERN_H_
