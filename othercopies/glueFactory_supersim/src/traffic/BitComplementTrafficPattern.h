/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TRAFFIC_BITCOMPLEMENTTRAFFICPATTERN_H_
#define TRAFFIC_BITCOMPLEMENTTRAFFICPATTERN_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "traffic/TrafficPattern.h"
#include "event/Component.h"

class BitComplementTrafficPattern : public TrafficPattern {
 public:
  BitComplementTrafficPattern(
      const std::string& _name, const Component* _parent, u32 _numTerminals,
      u32 _self, Json::Value _settings);
  ~BitComplementTrafficPattern();
  u32 nextDestination() override;
};

#endif  // TRAFFIC_BITCOMPLEMENTTRAFFICPATTERN_H_
