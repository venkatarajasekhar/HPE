/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemnoc1/Ejector.h"

#include <cassert>

#include <string>

#include "router/poemnoc1/Router.h"
#include "types/Packet.h"
#include "types/Message.h"

namespace PoemNoc1 {

Ejector::Ejector(const std::string& _name, const Component* _parent)
    : Component(_name, _parent) {
  lastSetTime_ = U32_MAX;
}

Ejector::~Ejector() {}

void Ejector::setChannel(Channel* _channel) {
  channel_ = _channel;
}

void Ejector::receiveFlit(u32 _port, Flit* _flit) {
  dbgprintf("ejecting flit (msgId=%u)",
            _flit->getPacket()->getMessage()->getId());

  // verify one flit per cycle
  u64 nextTime = gSim->time();
  assert((lastSetTime_ != nextTime) || (lastSetTime_ == U32_MAX));
  lastSetTime_ = nextTime;

  // send flit
  assert(channel_->getNextFlit() == nullptr);
  channel_->setNextFlit(_flit);
}

}  // namespace PoemNoc1
