/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/NetworkFactory.h"

#include <cassert>

#include "network/foldedclos/Network.h"
#include "network/hyperx/Network.h"
#include "network/hyperxpoemnoc1/Network.h"
#include "network/hyperxpoemnoc2/Network.h"
#include "network/torus/Network.h"
#include "network/uno/Network.h"
#include "network/hierarchyhyperx/Network.h"

Network* NetworkFactory::createNetwork(
    const std::string& _name, const Component* _parent,
    Json::Value _settings) {
  std::string topology = _settings["topology"].asString();

  if (topology == "folded_clos") {
    return new FoldedClos::Network(_name, _parent, _settings);
  } else if (topology == "hyperx") {
    return new HyperX::Network(_name, _parent, _settings);
  } else if (topology == "torus") {
    return new Torus::Network(_name, _parent, _settings);
  } else if (topology == "uno") {
    return new Uno::Network(_name, _parent, _settings);
  } else if (topology == "hyperxpoemnoc1") {
    return new HyperXPoemNoc1::Network(_name, _parent, _settings);
  } else if (topology == "hyperxpoemnoc2") {
    return new HyperXPoemNoc2::Network(_name, _parent, _settings);
  } else if (topology == "hierarchyhyperx") {
    return new HierarchyHyperX::Network(_name, _parent, _settings);
  } else {
    fprintf(stderr, "unknown netwok topology: %s\n", topology.c_str());
    assert(false);
  }
}
