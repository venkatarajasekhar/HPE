/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_MESSAGERECEIVER_H_
#define TYPES_MESSAGERECEIVER_H_

class Message;

class MessageReceiver {
 public:
  MessageReceiver();
  virtual ~MessageReceiver();
  virtual void receiveMessage(Message* _message) = 0;
};

#endif  // TYPES_MESSAGERECEIVER_H_
