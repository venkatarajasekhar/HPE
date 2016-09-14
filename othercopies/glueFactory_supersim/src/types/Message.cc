/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "types/Message.h"
#include "types/Packet.h"
#include "application/Terminal.h"

Message::Message(u32 _numPackets, void* _data)
    : data_(_data) {
  packets_.resize(_numPackets);
}

Message::~Message() {
  for (u32 p = 0; p < packets_.size(); p++) {
    if (packets_.at(p)) {
      delete packets_.at(p);
    }
  }
}

Terminal* Message::getOwner() const {
  return owner_;
}

void Message::setOwner(Terminal* _owner) {
  owner_ = _owner;
}

u32 Message::getId() const {
  return id_;
}

void Message::setId(u32 _id) {
  id_ = _id;
}

u32 Message::numPackets() const {
  return packets_.size();
}

u32 Message::numFlits() const {
  u32 numFlits = 0;
  for (u32 p = 0; p < packets_.size(); p++) {
    numFlits += packets_.at(p)->numFlits();
  }
  return numFlits;
}

Packet* Message::getPacket(u32 _index) const {
  return packets_.at(_index);
}

void Message::setPacket(u32 _index, Packet* _packet) {
  packets_.at(_index) = _packet;
}

void* Message::getData() const {
  return data_;
}

void Message::setData(void* _data) {
  data_ = _data;
}

u32 Message::getSourceId() const {
  return sourceId_;
}

void Message::setSourceId(u32 _sourceId) {
  sourceId_ = _sourceId;
}

u32 Message::getDestinationId() const {
  return destinationId_;
}

void Message::setDestinationId(u32 _destinationId) {
  destinationId_ = _destinationId;
}

void Message::setSourceAddress(const std::vector<u32>* _address) {
  sourceAddress_ = _address;
}

const std::vector<u32>* Message::getSourceAddress() const {
  return sourceAddress_;
}

void Message::setDestinationAddress(const std::vector<u32>* _address) {
  destinationAddress_ = _address;
}

const std::vector<u32>* Message::getDestinationAddress() const {
  return destinationAddress_;
}
