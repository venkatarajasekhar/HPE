/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_HYPERXPOEMNOC1_ROUTINGFUNCTIONFACTORY_H_
#define NETWORK_HYPERXPOEMNOC1_ROUTINGFUNCTIONFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunctionFactory.h"

namespace HyperXPoemNoc1 {

class RoutingFunctionFactory : public ::RoutingFunctionFactory {
 public:
  RoutingFunctionFactory(u32 _numVcs, const std::vector<u32>& _dimensionWidths,
                         const std::vector<u32>& _dimensionWeights,
                         u32 _concentration, u32 _nocWidth, u32 _nocWeight,
                         u32 _nocConcentration);
  ~RoutingFunctionFactory();
  RoutingFunction* createRoutingFunction(
      const std::string& _name, const Component* _parent, Router* _router,
      u32 inputPort, Json::Value _settings);

 private:
  u32 numVcs_;
  const std::vector<u32> dimensionWidths_;
  const std::vector<u32> dimensionWeights_;
  u32 concentration_;
  u32 nocWidth_;
  u32 nocWeight_;
  u32 nocConcentration_;
};

}  // namespace HyperXPoemNoc1

#endif  // NETWORK_HYPERXPOEMNOC1_ROUTINGFUNCTIONFACTORY_H_
