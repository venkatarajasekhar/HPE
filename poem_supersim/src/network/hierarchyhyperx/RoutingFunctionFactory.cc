/*
 * Copyright (c) 2013-2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#include "network/hierarchyhyperx/RoutingFunctionFactory.h"

#include <cassert>

#include "network/hierarchyhyperx/DimOrderRoutingFunction.h"
#include "network/RoutingFunction.h"

namespace HierarchyHyperX {

RoutingFunctionFactory::RoutingFunctionFactory(
    u32 _numVcs, const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _globalLinksPerRouter, u32 _concentration)
    : ::RoutingFunctionFactory(), numVcs_(_numVcs),
      globalDimensionWidths_(_globalDimensionWidths),
      globalDimensionWeights_(_globalDimensionWeights),
      localDimensionWidths_(_localDimensionWidths),
      localDimensionWeights_(_localDimensionWeights),
      globalLinksPerRouter_(_globalLinksPerRouter),
      concentration_(_concentration) {}

RoutingFunctionFactory::~RoutingFunctionFactory() {}

RoutingFunction* RoutingFunctionFactory::createRoutingFunction(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort, Json::Value _settings) {
  std::string algorithm = _settings["algorithm"].asString();
  u32 latency = _settings["latency"].asUInt();
  bool allVcs = _settings["all_vcs"].asBool();

  if (algorithm == "dimension_ordered") {
    return new HierarchyHyperX::DimOrderRoutingFunction(
        _name, _parent, latency, _router, numVcs_, globalDimensionWidths_,
        globalDimensionWeights_, localDimensionWidths_, localDimensionWeights_,
        concentration_, globalLinksPerRouter_, allVcs);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace HierarchyHyperX
