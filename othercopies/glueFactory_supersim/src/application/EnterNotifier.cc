/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/EnterNotifier.h"
#include "application/Terminal.h"

EnterNotifier::EnterNotifier()
    : receiver_(nullptr) {}

EnterNotifier::~EnterNotifier() {}

void EnterNotifier::setMessageReceiver(MessageReceiver* _receiver) {
  receiver_ = _receiver;
}

void EnterNotifier::receiveMessage(Message* _message) {
  _message->getOwner()->messageEnteredInterface(_message);
  receiver_->receiveMessage(_message);
}
