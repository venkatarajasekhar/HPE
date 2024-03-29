/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_INJECTIONLIMITER_H_
#define APPLICATION_INJECTIONLIMITER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "types/Message.h"
#include "types/MessageReceiver.h"

class Application;

class InjectionLimiter : public Component, public MessageReceiver {
 public:
  InjectionLimiter(const std::string& _name, const Component* _parent,
                   Application* _app, u32 _id);
  ~InjectionLimiter();

  void setMessageReceiver(MessageReceiver* _receiver);
  void receiveMessage(Message* _message) override;
  void processEvent(void* _event, s32 _type) override;

 private:
  Application* app_;
  u32 id_;
  bool enabled_;
  f64 injectionRate_;
  MessageReceiver* receiver_;
  u64 lastTime_;
};

#endif  // APPLICATION_INJECTIONLIMITER_H_
