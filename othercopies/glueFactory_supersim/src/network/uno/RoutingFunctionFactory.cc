/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/uno/RoutingFunctionFactory.h"

#include <cassert>

#include "network/uno/DirectRoutingFunction.h"
#include "network/RoutingFunction.h"

namespace Uno {

RoutingFunctionFactory::RoutingFunctionFactory(u32 _numVcs, u32 _concentration)
    : ::RoutingFunctionFactory(), numVcs_(_numVcs),
      concentration_(_concentration) {}

RoutingFunctionFactory::~RoutingFunctionFactory() {}

RoutingFunction* RoutingFunctionFactory::createRoutingFunction(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort, Json::Value _settings) {
  std::string algorithm = _settings["algorithm"].asString();
  u32 latency = _settings["latency"].asUInt();
  bool allVcs = _settings["all_vcs"].asBool();

  if (algorithm == "direct") {
    return new Uno::DirectRoutingFunction(
        _name, _parent, latency, _router, numVcs_, concentration_, allVcs);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace Uno
