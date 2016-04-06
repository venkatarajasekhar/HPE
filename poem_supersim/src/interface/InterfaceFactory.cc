/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/InterfaceFactory.h"

#include <cassert>

#include "interface/poem/Interface.h"
#include "interface/standard/Interface.h"

Interface* InterfaceFactory::createInterface(
    const std::string& _name, const Component* _parent, u32 _id,
    Json::Value _settings) {
  std::string type = _settings["type"].asString();
  if (type == "standard") {
    return new Standard::Interface(_name, _parent, _id, _settings);
  } else if (type == "poem") {
    return new Poem::Interface(_name, _parent, _id, _settings);
  } else {
    fprintf(stderr, "unknown interface type: %s\n", type.c_str());
    assert(false);
  }
}
