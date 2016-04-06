/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/torus/RoutingFunctionFactory.h"

#include <cassert>

#include "network/torus/DimOrderRoutingFunction.h"
#include "network/RoutingFunction.h"

namespace Torus {

RoutingFunctionFactory::RoutingFunctionFactory(
    u32 _numVcs, std::vector<u32> _dimensionWidths, u32 _concentration)
    : ::RoutingFunctionFactory(), numVcs_(_numVcs),
      dimensionWidths_(_dimensionWidths), concentration_(_concentration) {}

RoutingFunctionFactory::~RoutingFunctionFactory() {}

RoutingFunction* RoutingFunctionFactory::createRoutingFunction(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort, Json::Value _settings) {
  std::string algorithm = _settings["algorithm"].asString();
  u32 latency = _settings["latency"].asUInt();

  if (algorithm == "dimension_ordered") {
    return new Torus::DimOrderRoutingFunction(
        _name, _parent, latency, _router, numVcs_,
        dimensionWidths_, concentration_, _inputPort);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace Torus
