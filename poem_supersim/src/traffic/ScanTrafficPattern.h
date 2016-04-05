/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TRAFFIC_SCANTRAFFICPATTERN_H_
#define TRAFFIC_SCANTRAFFICPATTERN_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "traffic/TrafficPattern.h"
#include "event/Component.h"

class ScanTrafficPattern : public TrafficPattern {
 public:
  ScanTrafficPattern(const std::string& _name, const Component* _parent,
                     u32 _numTerminals, u32 _self, Json::Value _settings);
  ~ScanTrafficPattern();
  u32 nextDestination() override;

 private:
  u32 next_;
};

#endif  // TRAFFIC_SCANTRAFFICPATTERN_H_
