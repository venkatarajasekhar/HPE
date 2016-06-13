/*
 * Copyright 2016 Hewlett Packard Enterprise Development LP
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "types/Packet.h"

#include <cassert>

#include "types/Flit.h"
#include "types/Message.h"

Packet::Packet(u32 _id, u32 _numFlits, Message* _message)
  : id_(_id), message_(_message), hopCount_(0),
    metadata_(U64_MAX), routingExtension_(nullptr),
    localDst_(nullptr), localDstPorts_(nullptr),
    globalHopCount_(0), intermediateDone_(false),
    localDetour_(0) {
  flits_.resize(_numFlits);
}

Packet::~Packet() {
  for (u32 f = 0; f < flits_.size(); f++) {
    if (flits_.at(f)) {
      delete flits_.at(f);
    }
  }
  assert(routingExtension_ == nullptr);
  assert(localDst_ == nullptr);
  assert(localDstPorts_ == nullptr);
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

u64 Packet::getMetadata() const {
  assert(metadata_ != U64_MAX);
  return metadata_;
}

void Packet::setMetadata(u64 _metadata) {
  assert(_metadata != U64_MAX);
  metadata_ = _metadata;
}

void* Packet::getRoutingExtension() const {
  return routingExtension_;
}

void Packet::setRoutingExtension(void* _ext) {
  routingExtension_ = _ext;
}

void* Packet::getLocalDst() const {
  return localDst_;
}

void Packet::setLocalDst(void* _localDst) {
  localDst_ = _localDst;
}

void* Packet::getLocalDstPort() const {
  return localDstPorts_;
}

void Packet::setLocalDstPort(void* _localDstPort) {
  localDstPorts_ = _localDstPort;
}

void Packet::incrementGlobalHopCount() {
  globalHopCount_++;
}

u32 Packet::getGlobalHopCount() const {
  return globalHopCount_;
}

void Packet::setIntermediate(bool _intermediateDone) {
  intermediateDone_ = _intermediateDone;
}

bool Packet::getIntermediateDone() {
  return intermediateDone_;
}

u32 Packet::getDetour() const {
  return localDetour_;
}

void Packet::setDetour(u32 _localDetour) {
  localDetour_ = _localDetour;
}
