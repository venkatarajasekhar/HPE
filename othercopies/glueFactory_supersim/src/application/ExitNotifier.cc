/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/ExitNotifier.h"

#include "application/Terminal.h"

ExitNotifier::ExitNotifier() {}

ExitNotifier::~ExitNotifier() {}

void ExitNotifier::setMessageReceiver(MessageReceiver* _receiver) {
  receiver_ = _receiver;
}

void ExitNotifier::receiveMessage(Message* _message) {
  // notify source of message exit
  _message->getOwner()->messageExitedNetwork(_message);

  // deliver to next recipient
  receiver_->receiveMessage(_message);
}
