/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemmono/CrossbarSchedulerFactory.h"

#include <cassert>

#include "router/poemmono/HybridSpecCrossbarScheduler.h"
#include "router/poemmono/PostSpecCrossbarScheduler.h"
#include "router/poemmono/PreSpecCrossbarScheduler.h"

namespace PoemMono {

CrossbarScheduler* CrossbarSchedulerFactory::createCrossbarScheduler(
    const std::string& _name, const Component* _parent, u32 _numClients,
    u32 _numVcs, u32 _numPorts, Json::Value _settings) {
  std::string type = _settings["type"].asString();

  if (type == "pre_spec") {
    return new PreSpecCrossbarScheduler(
        _name, _parent, _numClients, _numVcs, _numPorts, _settings);
  } else if (type == "post_spec") {
    return new PostSpecCrossbarScheduler(
        _name, _parent, _numClients, _numVcs, _numPorts, _settings);
  } else if (type == "hybrid_spec") {
    return new HybridSpecCrossbarScheduler(
        _name, _parent, _numClients, _numVcs, _numPorts, _settings);
  } else {
    fprintf(stderr, "unknown PoemMono::CrossbarScheduler type: %s\n",
            type.c_str());
    assert(false);
  }
}

}  // namespace PoemMono
