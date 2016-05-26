/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#include "types/Flit.h"
#include <strop/strop.h>

#include <cassert>

#include "types/Packet.h"

Flit::Flit(u32 _id, bool _isHead, bool _isTail, Packet* _packet)
    : id_(_id), head_(_isHead), tail_(_isTail), packet_(_packet),
      sendTime_(U64_MAX), receiveTime_(U64_MAX),
      globalHopCount_(0), intermediateDone_(false) {}

Flit::~Flit() {}

u32 Flit::getId() const {
  return id_;
}

bool Flit::isHead() const {
  return head_;
}

bool Flit::isTail() const {
  return tail_;
}

Packet* Flit::getPacket() const {
  return packet_;
}

u32 Flit::getVc() const {
  return vc_;
}

void Flit::setVc(u32 _vc) {
  vc_ = _vc;
}

void Flit::setSendTime(u64 _time) {
  assert(_time != U64_MAX);
  sendTime_ = _time;
}

u64 Flit::getSendTime() const {
  assert(sendTime_ != U64_MAX);
  return sendTime_;
}

void Flit::setReceiveTime(u64 _time) {
  assert(_time != U64_MAX);
  receiveTime_ = _time;
}

u64 Flit::getReceiveTime() const {
  assert(receiveTime_ != U64_MAX);
  return receiveTime_;
}

void Flit::incrementGlobalHopCount() {
  globalHopCount_++;
}

u32 Flit::getGlobalHopCount() const {
  return globalHopCount_;
}

void Flit::recordHop(std::vector<u32> hop) {
  hops.push_back(hop);
}

std::vector<std::vector<u32> > Flit::getHops() const {
  // for (auto itr = hops.begin(); itr != hops.end(); itr++) {
  //  printf("flit path is %s \n", strop::vecString<u32>(*itr).c_str());
  // }
  return hops;
}

void Flit::setIntermediate(bool _intermediateDone) {
  intermediateDone_ = _intermediateDone;
}
bool Flit::getIntermediateDone() {
  return intermediateDone_;
}
