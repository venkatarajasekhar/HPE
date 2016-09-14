/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_CROSSBARSCHEDULERFACTORY_H_
#define ROUTER_POEMMONO_CROSSBARSCHEDULERFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "router/poemmono/CrossbarScheduler.h"

namespace PoemMono {

class CrossbarSchedulerFactory {
 public:
  static CrossbarScheduler* createCrossbarScheduler(
      const std::string& _name, const Component* _parent, u32 _numClients,
      u32 _numVcs, u32 _numPorts, Json::Value _settings);
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_CROSSBARSCHEDULERFACTORY_H_
