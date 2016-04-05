/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/poem/InputQueue.h"

#include <cassert>

#include "interface/poem/Interface.h"
#include "types/Packet.h"

namespace Poem {

InputQueue::InputQueue(
    const std::string& _name, Interface* _interface,
    CrossbarScheduler* _crossbarScheduler, u32 _crossbarSchedulerIndex,
    Crossbar* _crossbar, u32 _crossbarIndex, u32 _vc)
    : Component(_name, _interface), crossbarScheduler_(_crossbarScheduler),
      crossbarSchedulerIndex_(_crossbarSchedulerIndex), crossbar_(_crossbar),
      crossbarIndex_(_crossbarIndex), interface_(_interface), vc_(_vc) {
}

InputQueue::~InputQueue() {}

InputQueue::DeadlineComparator::DeadlineComparator() {}

InputQueue::DeadlineComparator::~DeadlineComparator() {}

bool InputQueue::DeadlineComparator::operator()(
    const Flit* _lhs, const Flit* _rhs) const {
  return (_lhs->getPacket()->getDeadline() >
          _rhs->getPacket()->getDeadline());
}

void InputQueue::receiveFlit(Flit* _flit) {
  // POEM is single flit packets
  assert(_flit->isHead() && _flit->isTail());

  u64 now = gSim->time();
  bool wasEmpty = buffer_.empty();

  // put flit/packet into buffer
  _flit->setSendTime(now);
  buffer_.push(_flit);

  // determine if a new event needs to occur
  if (wasEmpty) {
    assert(gSim->epsilon() == 0);
    addEvent(gSim->time(), 1, nullptr, 0);
  }
}

void InputQueue::crossbarSchedulerResponse(u32 _port, u32 _vc) {
  if (_port != U32_MAX) {
    // granted
    assert(vc_ == _vc);
    Flit* flit = buffer_.top();
    buffer_.pop();
    crossbar_->inject(flit, crossbarIndex_, 0);
    crossbarScheduler_->decrementCreditCount(_vc);
  } else {
    // denied
  }

  // request if there is more flits in the queue
  if (buffer_.size() > 0) {
    addEvent(gSim->time(), 1, nullptr, 0);
  }
}

void InputQueue::processEvent(void* _event, s32 _type) {
  u64 metadata = buffer_.top()->getPacket()->getDeadline();
  crossbarScheduler_->request(crossbarSchedulerIndex_, 0, vc_, metadata);
}

}  // namespace Poem
