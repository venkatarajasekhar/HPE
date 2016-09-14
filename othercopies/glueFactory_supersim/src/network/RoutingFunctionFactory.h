/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_ROUTINGFUNCTIONFACTORY_H_
#define NETWORK_ROUTINGFUNCTIONFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

class Component;
class Router;
class RoutingFunction;

class RoutingFunctionFactory {
 public:
  RoutingFunctionFactory();
  virtual ~RoutingFunctionFactory();
  virtual RoutingFunction* createRoutingFunction(
      const std::string& _name, const Component* _parent, Router* _router,
      u32 _inputPort, Json::Value _settings) = 0;
};

#endif  // NETWORK_ROUTINGFUNCTIONFACTORY_H_
