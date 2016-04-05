/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/poem/Ejector.h"

#include <cassert>

#include "interface/poem/Interface.h"

namespace Poem {

Ejector::Ejector(const std::string& _name, Interface* _interface)
    : Component(_name, _interface), interface_(_interface) {
  lastSetTime_ = U32_MAX;
}

Ejector::~Ejector() {}

void Ejector::receiveFlit(u32 _port, Flit* _flit) {
  // this is overkill checking!
  u64 nextTime = gSim->futureCycle(1);
  assert((lastSetTime_ != nextTime) || (lastSetTime_ == U32_MAX));
  interface_->sendFlit(_flit);
  lastSetTime_ = nextTime;
}

}  // namespace Poem
