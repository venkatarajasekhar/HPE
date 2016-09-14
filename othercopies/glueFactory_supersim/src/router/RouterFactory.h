/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_ROUTERFACTORY_H_
#define ROUTER_ROUTERFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "network/RoutingFunctionFactory.h"
#include "router/Router.h"

class RouterFactory {
 public:
  static Router* createRouter(
      const std::string& _name, const Component* _parent,
      RoutingFunctionFactory* _routingFunctionFactory,
      Json::Value _settings);
};

#endif  // ROUTER_ROUTERFACTORY_H_
