/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_ENTERNOTIFIER_H_
#define APPLICATION_ENTERNOTIFIER_H_

#include "types/Message.h"
#include "types/MessageReceiver.h"

class EnterNotifier : public MessageReceiver {
 public:
  EnterNotifier();
  ~EnterNotifier();
  void setMessageReceiver(MessageReceiver* _receiver);
  void receiveMessage(Message* _message) override;
 private:
  MessageReceiver* receiver_;
};

#endif  // APPLICATION_ENTERNOTIFIER_H_
