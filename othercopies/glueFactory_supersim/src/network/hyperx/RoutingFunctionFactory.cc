/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/hyperx/RoutingFunctionFactory.h"

#include <cassert>

#include "network/hyperx/DimOrderRoutingFunction.h"
#include "network/RoutingFunction.h"

namespace HyperX {

RoutingFunctionFactory::RoutingFunctionFactory(
    u32 _numVcs, const std::vector<u32>& _dimensionWidths,
    const std::vector<u32>& _dimensionWeights, u32 _concentration)
    : ::RoutingFunctionFactory(), numVcs_(_numVcs),
      dimensionWidths_(_dimensionWidths), dimensionWeights_(_dimensionWeights),
      concentration_(_concentration) {}

RoutingFunctionFactory::~RoutingFunctionFactory() {}

RoutingFunction* RoutingFunctionFactory::createRoutingFunction(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort, Json::Value _settings) {
  std::string algorithm = _settings["algorithm"].asString();
  u32 latency = _settings["latency"].asUInt();
  bool allVcs = _settings["all_vcs"].asBool();

  if (algorithm == "dimension_ordered") {
    return new HyperX::DimOrderRoutingFunction(
        _name, _parent, latency, _router, numVcs_, dimensionWidths_,
        dimensionWeights_, concentration_, allVcs);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace HyperX
