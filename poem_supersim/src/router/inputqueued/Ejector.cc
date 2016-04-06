/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/inputqueued/Ejector.h"

#include <cassert>

#include <string>

#include "router/inputqueued/Router.h"

namespace InputQueued {

Ejector::Ejector(
    const std::string& _name, Router* _router, u32 _portId)
    : Component(_name, _router),
      router_(_router), portId_(_portId) {
  lastSetTime_ = U32_MAX;
}

Ejector::~Ejector() {}

void Ejector::receiveFlit(u32 _port, Flit* _flit) {
  // verify one flit per cycle
  u64 nextTime = gSim->time();
  assert((lastSetTime_ != nextTime) || (lastSetTime_ == U32_MAX));
  lastSetTime_ = nextTime;

  // send flit using the router
  router_->sendFlit(portId_, _flit);
}

}  // namespace InputQueued
