/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/Terminal.h"

#include <cassert>

#include "application/Application.h"
#include "event/Simulator.h"

Terminal::Terminal(const std::string& _name, const Component* _parent, u32 _id,
                   std::vector<u32> _address)
    : Component(_name, _parent), id_(_id), address_(_address),
      messagesSent_(0), messagesReceived_(0) {}

Terminal::~Terminal() {
  for (auto it = outstandingMessages_.begin(); it != outstandingMessages_.end();
       ++it) {
    delete *it;
  }
}

u32 Terminal::getId() const {
  return id_;
}

const std::vector<u32>* Terminal::address() const {
  return &address_;
}

u32 Terminal::messagesSent() const {
  return messagesSent_;
}

u32 Terminal::messagesReceived() const {
  return messagesReceived_;
}

void Terminal::setMessageReceiver(MessageReceiver* _receiver) {
  messageReceiver_ = _receiver;
}

void Terminal::receiveMessage(Message* _message) {
  // change the owner of the message to this terminal
  _message->setOwner(this);

  // deliver message to terminal subclass
  handleMessage(_message);
}

void Terminal::messageExitedNetwork(Message* _message) {
  assert(outstandingMessages_.erase(_message) == 1);
  messagesReceived_++;
}

void Terminal::enrouteCount(u32* _messages, u32* _packets, u32* _flits) const {
  *_messages = 0;
  *_packets = 0;
  *_flits = 0;
  for (auto it = outstandingMessages_.cbegin();
       it != outstandingMessages_.cend(); ++it) {
    *_messages += 1;
    *_packets += (*it)->numPackets();
    *_flits += (*it)->numFlits();
  }
}

u32 Terminal::sendMessage(Message* _message, u32 _destinationId) {
  u32 msgId = messagesSent_;
  messagesSent_++;
  _message->setOwner(this);
  _message->setId(msgId);
  _message->setSourceId(id_);
  _message->setSourceAddress(&address_);
  _message->setDestinationId(_destinationId);
  Terminal* dest = gSim->getApplication()->getTerminal(_destinationId);
  _message->setDestinationAddress(&dest->address_);
  messageReceiver_->receiveMessage(_message);
  assert(outstandingMessages_.insert(_message).second);
  return msgId;
}
