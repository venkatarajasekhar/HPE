/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "types/Packet.h"

#include <cassert>

#include "types/Flit.h"
#include "types/Message.h"

Packet::Packet(u32 _id, u32 _numFlits, Message* _message)
    : id_(_id), message_(_message), hopCount_(0), deadline_(U64_MAX) {
  flits_.resize(_numFlits);
}

Packet::~Packet() {
  for (u32 f = 0; f < flits_.size(); f++) {
    if (flits_.at(f)) {
      delete flits_.at(f);
    }
  }
}

u32 Packet::getId() const {
  return id_;
}

u32 Packet::numFlits() const {
  return flits_.size();
}

Flit* Packet::getFlit(u32 _index) const {
  return flits_.at(_index);
}

void Packet::setFlit(u32 _index, Flit* _flit) {
  flits_.at(_index) = _flit;
}

Message* Packet::getMessage() const {
  return message_;
}

void Packet::incrementHopCount() {
  hopCount_++;
}

u32 Packet::getHopCount() const {
  return hopCount_;
}

u64 Packet::headLatency() const {
  Flit* head = flits_.at(0);
  return head->getReceiveTime() - head->getSendTime();
}

u64 Packet::serializationLatency() const {
  Flit* head = flits_.at(0);
  Flit* tail = flits_.at(flits_.size() - 1);
  return tail->getReceiveTime() - head->getReceiveTime();
}

u64 Packet::totalLatency() const {
  Flit* head = flits_.at(0);
  Flit* tail = flits_.at(flits_.size() - 1);
  return tail->getReceiveTime() - head->getSendTime();
}

u64 Packet::getDeadline() const {
  assert(deadline_ != U64_MAX);
  return deadline_;
}

void Packet::setDeadline(u64 _deadline) {
  assert(_deadline != U64_MAX);
  deadline_ = _deadline;
}
