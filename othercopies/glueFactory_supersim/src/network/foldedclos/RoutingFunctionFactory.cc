/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/foldedclos/RoutingFunctionFactory.h"

#include <cassert>

#include "network/foldedclos/McaRoutingFunction.h"
#include "network/foldedclos/LcaRoutingFunction.h"
#include "network/RoutingFunction.h"

namespace FoldedClos {

RoutingFunctionFactory::RoutingFunctionFactory(
    u32 _numVcs, u32 _numPorts, u32 _numLevels, u32 _level)
    : ::RoutingFunctionFactory(), numVcs_(_numVcs), numPorts_(_numPorts),
      numLevels_(_numLevels), level_(_level) {}

RoutingFunctionFactory::~RoutingFunctionFactory() {}

RoutingFunction* RoutingFunctionFactory::createRoutingFunction(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort, Json::Value _settings) {

  std::string algorithm = _settings["algorithm"].asString();
  u32 latency = _settings["latency"].asUInt();
  bool allVcs = _settings["all_vcs"].asBool();

  if (algorithm == "most_common_ancestor") {
    return new McaRoutingFunction(_name, _parent, latency, _router,
                                  numVcs_, numPorts_, numLevels_, level_,
                                  _inputPort, allVcs);
  } else if (algorithm == "least_common_ancestor") {
    return new LcaRoutingFunction(_name, _parent, latency, _router,
                                  numVcs_, numPorts_, numLevels_, level_,
                                  _inputPort, allVcs);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace FoldedClos
