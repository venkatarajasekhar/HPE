/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/standard/PacketReassembler.h"

#include <cassert>
#include <cstdio>

#include "types/Message.h"

namespace Standard {

PacketReassembler::PacketReassembler(const std::string& _name,
                                     const Component* _parent)
    : Component(_name, _parent) {
  expSourceId_  = U32_MAX;
  expPacketId_  = U32_MAX;
  expFlitId_    = 0;
}

PacketReassembler::~PacketReassembler() {}

Packet* PacketReassembler::receiveFlit(Flit* _flit) {
  Packet* packet = nullptr;
  u32 sourceId = _flit->getPacket()->getMessage()->getSourceId();
  u32 packetId = _flit->getPacket()->getId();
  u32 flitId   = _flit->getId();

  // if expected packet id isn't yet set, set it
  if (expPacketId_ == U32_MAX) {
    assert(flitId == 0);
    expSourceId_ = sourceId;
    expPacketId_ = packetId;
  }

  // check sourceId, packetId, and flitId
  if (sourceId != expSourceId_) {
    assert(false);
  }
  if (packetId != expPacketId_) {
    assert(false);
  }
  if (flitId != expFlitId_) {
    assert(false);
  }

  assert(flitId < _flit->getPacket()->numFlits());
  assert(packetId < _flit->getPacket()->getMessage()->numPackets());

  // if this is the last flit of the packet
  if (flitId == (_flit->getPacket()->numFlits() - 1)) {
    expSourceId_ = U32_MAX;  // clear expected source id
    expPacketId_ = U32_MAX;  // clear expected packet id
    expFlitId_ = 0;  // expect flit 0 on the next packet
    packet = _flit->getPacket();
  } else {
    expFlitId_ = flitId + 1;
  }
  return packet;
}

}  // namespace Standard
