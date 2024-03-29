/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/standard/Interface.h"

#include <cassert>

#include "application/Application.h"
#include "interface/standard/InputQueue.h"
#include "interface/standard/MessageReassembler.h"
#include "interface/standard/Ejector.h"
#include "interface/standard/PacketReassembler.h"
#include "types/MessageOwner.h"
#include "types/Credit.h"

namespace Standard {

Interface::Interface(const std::string& _name, const Component* _parent,
                     u32 _id, Json::Value _settings)
    : ::Interface(_name, _parent, _id, _settings) {
  u32 initCredits = _settings["init_credits"].asUInt();
  assert(initCredits > 0);

  inputQueues_.resize(numVcs_);
  crossbar_ = new Crossbar("Crossbar", this, numVcs_, 1, _settings["crossbar"]);
  crossbarScheduler_ = new CrossbarScheduler(
      "CrossbarScheduler", this, numVcs_, numVcs_, 1,
      _settings["crossbar_scheduler"]);

  for (u32 vc = 0; vc < numVcs_; vc++) {
    // initialize the credit count in the CrossbarScheduler
    crossbarScheduler_->initCreditCount(vc, initCredits);

    // create the input queue
    inputQueues_.at(vc) = new InputQueue(
        "InputQueue_" + std::to_string(vc), this, crossbarScheduler_, vc,
        crossbar_, vc, vc);

    // link queue to scheduler
    crossbarScheduler_->setClient(vc, inputQueues_.at(vc));
  }

  ejector_ = new Ejector("Ejector", this);
  crossbar_->setReceiver(0, ejector_, 0);

  // create packet reassemblers
  packetReassemblers_.resize(numVcs_);
  for (u32 vc = 0; vc < numVcs_; vc++) {
    std::string tname = "PacketReassembler_" + std::to_string(vc);
    packetReassemblers_.at(vc) = new PacketReassembler(tname, this);
  }

  // create message reassembler
  messageReassembler_ = new MessageReassembler("MessageReassembler", this);
}

Interface::~Interface() {
  delete ejector_;
  delete crossbarScheduler_;
  delete crossbar_;
  for (u32 i = 0; i < numVcs_; i++) {
    delete inputQueues_.at(i);
    delete packetReassemblers_.at(i);
  }
  delete messageReassembler_;
}

void Interface::setInputChannel(Channel* _channel) {
  inputChannel_ = _channel;
  _channel->setSink(this, 0);
}

void Interface::setOutputChannel(Channel* _channel) {
  outputChannel_ = _channel;
  _channel->setSource(this, 0);
}

void Interface::receiveMessage(Message* _message) {
  dbgprintf("received message from terminal");
  assert(_message != nullptr);
  u64 now = gSim->time();

  // push all flits into the corresponding input queue
  for (u32 p = 0; p < _message->numPackets(); p++) {
    Packet* packet = _message->getPacket(p);
    u32 pktVc = packet->getFlit(0)->getVc();
    for (u32 f = 0; f < packet->numFlits(); f++) {
      Flit* flit = packet->getFlit(f);
      flit->setSendTime(now);
      u32 vc = flit->getVc();
      assert(vc == pktVc);
      inputQueues_.at(vc)->receiveFlit(flit);
    }
  }
}

void Interface::receiveFlit(u32 _port, Flit* _flit) {
  assert(_port == 0);
  assert(_flit != nullptr);

  // send credit
  Control* ctrl = inputChannel_->getNextControl();
  Credit* cred;
  if (ctrl == nullptr) {
    cred = new Credit(numVcs_);
    inputChannel_->setNextControl(cred);
  } else {
    cred = dynamic_cast<Credit*>(ctrl);
    assert(cred);
  }
  cred->putNum(_flit->getVc());

  // check destination is correct
  u32 dest = _flit->getPacket()->getMessage()->getDestinationId();
  assert(dest == getId());

  // mark the receive time
  _flit->setReceiveTime(gSim->time());

  // process flit, attempt to create packet
  Packet* packet = packetReassemblers_.at(_flit->getVc())->receiveFlit(_flit);
  // if a packet was completed, process it
  if (packet) {
    // process packet, attempt to create message
    Message* message = messageReassembler_->receivePacket(packet);
    if (message) {
      getMessageReceiver()->receiveMessage(message);
    }
  }
}

void Interface::receiveControl(u32 _port, Control* _control) {
  assert(_port == 0);
  Credit* cred = dynamic_cast<Credit*>(_control);
  assert(cred);
  while (cred->more()) {
    u32 vc = cred->getNum();
    crossbarScheduler_->incrementCreditCount(vc);
  }
  delete _control;
}

void Interface::sendFlit(Flit* _flit) {
  assert(outputChannel_->getNextFlit() == nullptr);
  outputChannel_->setNextFlit(_flit);
}

}  // namespace Standard
