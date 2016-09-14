/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/singlestream/Terminal.h"

#include <cassert>
#include <cmath>

#include "application/singlestream/Application.h"
#include "network/Network.h"
#include "random/Random.h"
#include "stats/MessageLog.h"
#include "stats/calc.h"
#include "types/Flit.h"
#include "types/Packet.h"

namespace SingleStream {

Terminal::Terminal(const std::string& _name, const Component* _parent,
                   u32 _id, std::vector<u32> _address, Application* _app,
                   Json::Value _settings)
    : ::Terminal(_name, _parent, _id, _address), lastSendTime_(0) {
  app_ = _app;

  // message quantity limition
  numMessages_ = _settings["num_messages"].asUInt();

  // apply the routines only if this terminal is targeted
  if (_id == app_->getSource()) {
    remainingMessages_ = numMessages_;

    // message and packet sizes
    minMessageSize_ = _settings["min_message_size"].asUInt();
    maxMessageSize_ = _settings["max_message_size"].asUInt();
    maxPacketSize_  = _settings["max_packet_size"].asUInt();

    // choose a random number of cycles in the future to start
    // make an event to start the Terminal in the future
    u64 cycles = app_->cyclesToSend(maxMessageSize_);
    cycles = gRandom->randomU64(1, cycles);
    u64 time = gSim->futureCycle(1) + ((cycles - 1) * gSim->cycleTime());
    dbgprintf("start time is %lu", time);
    addEvent(time, 0, nullptr, 0);
    remainingMessages_--;  // decrement for first message
  } else {
    remainingMessages_ = 0;
  }
  recvdMessages_ = 0;
}
Terminal::~Terminal() {}

void Terminal::processEvent(void* _event, s32 _type) {
  sendNextMessage();
}

void Terminal::handleMessage(Message* _message) {
  dbgprintf("received message %u", _message->getId());

  // determine when complete
  recvdMessages_++;

  // on first received message, start monitoring
  if (recvdMessages_ == 1) {
    app_->receivedFirst(getId());
  }

  delete _message;  // don't need this anymore
}

void Terminal::messageEnteredInterface(Message* _message) {
  // determine if more messages should be created and sent
  u64 now = gSim->time();
  assert(lastSendTime_ <= now);
  if (remainingMessages_ > 0) {
    remainingMessages_--;
    if (now == lastSendTime_) {
      addEvent(gSim->futureCycle(1), 0, nullptr, 0);
    } else {
      sendNextMessage();
    }
  } else {
    app_->sentLast(getId());
  }
}

void Terminal::messageExitedNetwork(Message* _message) {
  // any override of this function must call the base class's function
  ::Terminal::messageExitedNetwork(_message);

  // log the message
  app_->getMessageLog()->logMessage(_message);
}

f64 Terminal::percentComplete() const {
  return (f64)recvdMessages_ / (f64)numMessages_;
}

void Terminal::sendNextMessage() {
  u64 now = gSim->time();
  lastSendTime_ = now;

  // pick a destination
  u32 destination = app_->getDestination();

  // pick a random message length
  u32 messageLength = gRandom->randomU64(minMessageSize_, maxMessageSize_);
  u32 numPackets = messageLength / maxPacketSize_;
  if ((messageLength % maxPacketSize_) > 0) {
    numPackets++;
  }

  // create the message object
  Message* message = new Message(numPackets, nullptr);

  // create the packets
  u32 flitsLeft = messageLength;
  for (u32 p = 0; p < numPackets; p++) {
    u32 packetLength = flitsLeft > maxPacketSize_ ?
                       maxPacketSize_ : flitsLeft;

    Packet* packet = new Packet(p, packetLength, message);
    message->setPacket(p, packet);

    packet->setDeadline(now + 10000);

    // pick a random starting VC
    u32 numVcs = gSim->getNetwork()->numVcs();
    u32 vc = gRandom->randomU64(0, numVcs - 1);

    // create flits
    for (u32 f = 0; f < packetLength; f++) {
      bool headFlit = f == 0;
      bool tailFlit = f == (packetLength - 1);
      Flit* flit = new Flit(f, headFlit, tailFlit, packet);
      flit->setVc(vc);
      packet->setFlit(f, flit);
    }
    flitsLeft -= packetLength;
  }

  // send the message
  sendMessage(message, destination);
}

}  // namespace SingleStream
