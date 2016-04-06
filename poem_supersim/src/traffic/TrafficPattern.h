/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TRAFFIC_TRAFFICPATTERN_H_
#define TRAFFIC_TRAFFICPATTERN_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"

class TrafficPattern : public Component {
 public:
  TrafficPattern(const std::string& _name, const Component* _parent,
                 u32 _numTerminals, u32 _self);
  virtual ~TrafficPattern();
  virtual u32 nextDestination() = 0;

 protected:
  const u32 numTerminals_;
  const u32 self_;
};

#endif  // TRAFFIC_TRAFFICPATTERN_H_
