/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_FOLDEDCLOS_ROUTINGFUNCTIONFACTORY_H_
#define NETWORK_FOLDEDCLOS_ROUTINGFUNCTIONFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "network/RoutingFunctionFactory.h"

namespace FoldedClos {

class RoutingFunctionFactory : public ::RoutingFunctionFactory {
 public:
  RoutingFunctionFactory(u32 _numVcs, u32 _numPorts, u32 _numLevels, u32 level);
  ~RoutingFunctionFactory();
  RoutingFunction* createRoutingFunction(
      const std::string& _name, const Component* _parent, Router* _router,
      u32 inputPort, Json::Value _settings);

 private:
  u32 numVcs_;
  u32 numPorts_;
  u32 numLevels_;
  u32 level_;
};

}  // namespace FoldedClos

#endif  // NETWORK_FOLDEDCLOS_ROUTINGFUNCTIONFACTORY_H_
