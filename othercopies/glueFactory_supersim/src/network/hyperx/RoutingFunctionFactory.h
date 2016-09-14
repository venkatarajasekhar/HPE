/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_HYPERX_ROUTINGFUNCTIONFACTORY_H_
#define NETWORK_HYPERX_ROUTINGFUNCTIONFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunctionFactory.h"

namespace HyperX {

class RoutingFunctionFactory : public ::RoutingFunctionFactory {
 public:
  RoutingFunctionFactory(u32 _numVcs, const std::vector<u32>& _dimensionWidths,
                         const std::vector<u32>& _dimensionWeights,
                         u32 _concentration);
  ~RoutingFunctionFactory();
  RoutingFunction* createRoutingFunction(
      const std::string& _name, const Component* _parent, Router* _router,
      u32 inputPort, Json::Value _settings);

 private:
  u32 numVcs_;
  const std::vector<u32> dimensionWidths_;
  const std::vector<u32> dimensionWeights_;
  u32 concentration_;
};

}  // namespace HyperX

#endif  // NETWORK_HYPERX_ROUTINGFUNCTIONFACTORY_H_
