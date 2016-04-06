/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/ArbiterFactory.h"

#include <cassert>

#include "arbiter/ComparingArbiter.h"
#include "arbiter/LslpArbiter.h"
#include "arbiter/RandomArbiter.h"

Arbiter* ArbiterFactory::createArbiter(
    const std::string& _name, const Component* _parent,
    u32 _size, Json::Value _settings) {
  std::string type = _settings["type"].asString();

  if (type == "lslp") {
    return new LslpArbiter(_name, _parent, _size, _settings);
  } else if (type == "comparing") {
    return new ComparingArbiter(_name, _parent, _size, _settings);
  } else if (type == "random") {
    return new RandomArbiter(_name, _parent, _size, _settings);
  } else {
    fprintf(stderr, "unknown Arbiter 'type': %s\n", type.c_str());
    assert(false);
  }
}
