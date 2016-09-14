/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/standard/MessageReassembler.h"

#include <cassert>
#include <cstdio>

namespace Standard {

MessageReassembler::MessageReassembler(const std::string& _name,
                                       const Component* _parent)
    : Component(_name, _parent) {}

MessageReassembler::~MessageReassembler() {}

Message* MessageReassembler::receivePacket(Packet* _packet) {
  // get the message and determine unqiue id
  Message* message = _packet->getMessage();
  u32 sourceId = message->getSourceId();
  u32 messageId = message->getId();
  u64 mid = ((u64)sourceId) << 32 | ((u64)messageId);

  // if non-existent, add a new table entry
  bool midExists = messages_.count(mid) == 1;
  if (!midExists) {
    messages_[mid] = MessageData();
    messages_[mid].message = message;
    messages_[mid].packetsReceived.resize(message->numPackets(), false);
    messages_[mid].receivedCount = 0;
  }

  // retrieve the message data
  MessageData& messageData = messages_.at(mid);
  assert(messageData.message == message);

  // mark the packet as received
  assert(messageData.packetsReceived.at(_packet->getId()) == false);
  messageData.packetsReceived.at(_packet->getId()) = true;
  messageData.receivedCount++;

  // check if the full message has been received
  if (messageData.receivedCount == message->numPackets()) {
    // remove the message from the map
    s32 erased = messages_.erase(mid);
    assert(erased == 1);
    return message;
  } else {
    // return nullptr to signify more packets are needed for the message
    return nullptr;
  }
}

}  // namespace Standard
