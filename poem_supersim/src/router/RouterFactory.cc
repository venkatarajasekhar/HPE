/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/RouterFactory.h"

#include <cassert>

#include "router/inputoutputqueued/Router.h"
#include "router/inputqueued/Router.h"
#include "router/poemmono/Router.h"

Router* RouterFactory::createRouter(
    const std::string& _name, const Component* _parent,
    RoutingFunctionFactory* _routingFunctionFactory,
    Json::Value _settings) {
  std::string architecture = _settings["architecture"].asString();

  if (architecture == "input_queued") {
    return new InputQueued::Router(
        _name, _parent, _routingFunctionFactory, _settings);
  } else if (architecture == "input_output_queued") {
    return new InputOutputQueued::Router(
        _name, _parent, _routingFunctionFactory, _settings);
  } else if (architecture == "poemmono") {
    return new PoemMono::Router(
        _name, _parent, _routingFunctionFactory, _settings);
  } else {
    fprintf(stderr, "unknown router architecture: %s\n", architecture.c_str());
    assert(false);
  }
}
