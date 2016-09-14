/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_EXITNOTIFIER_H_
#define APPLICATION_EXITNOTIFIER_H_

#include "types/Message.h"
#include "types/MessageReceiver.h"

class ExitNotifier : public MessageReceiver {
 public:
  ExitNotifier();
  ~ExitNotifier();
  void setMessageReceiver(MessageReceiver* _receiver);
  void receiveMessage(Message* _message) override;

 private:
  MessageReceiver* receiver_;
};

#endif  // APPLICATION_EXITNOTIFIER_H_
