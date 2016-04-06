/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemmono/CrossbarScheduler.h"

#include <cassert>
#include <cstring>

#include "allocator/AllocatorFactory.h"

namespace PoemMono {

CrossbarScheduler::Client::Client() {}

CrossbarScheduler::Client::~Client() {}

CrossbarScheduler::CrossbarScheduler(
    const std::string& _name, const Component* _parent, u32 _numClients,
    u32 _numVcs, u32 _numPorts, Json::Value _settings)
    : Component(_name, _parent), numClients_(_numClients), numVcs_(_numVcs),
      numPorts_(_numPorts), latency_(_settings["latency"].asUInt()) {
  assert(numClients_ > 0 && numClients_ != U32_MAX);
  assert(numVcs_ > 0 && numVcs_ != U32_MAX);
  assert(numPorts_ > 0 && numPorts_ != U32_MAX);
  assert(latency_ > 0);

  // create Client pointers
  clients_.resize(numClients_, nullptr);
}

CrossbarScheduler::~CrossbarScheduler() {}

u32 CrossbarScheduler::numClients() const {
  return numClients_;
}

u32 CrossbarScheduler::numVcs() const {
  return numVcs_;
}

u32 CrossbarScheduler::numPorts() const {
  return numPorts_;
}

u32 CrossbarScheduler::latency() const {
  return latency_;
}

void CrossbarScheduler::setClient(u32 _id, Client* _client) {
  assert(clients_.at(_id) == nullptr);
  clients_.at(_id) = _client;
}

}  // namespace PoemMono
