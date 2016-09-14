/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_RATEMONITOR_H_
#define APPLICATION_RATEMONITOR_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "types/Message.h"
#include "types/MessageReceiver.h"

class RateMonitor : public Component, public MessageReceiver {
 public:
  RateMonitor(const std::string& _name, const Component* _parent);
  ~RateMonitor();

  void setMessageReceiver(MessageReceiver* _receiver);
  void receiveMessage(Message* _message);
  void start();
  void end();
  f64 rate() const;

 private:
  MessageReceiver* messageReceiver_;
  u64 flitCount_;

  u64 startTime_;
  u64 endTime_;

  bool running_;
};

#endif  // APPLICATION_RATEMONITOR_H_
