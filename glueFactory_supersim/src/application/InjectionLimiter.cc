/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/InjectionLimiter.h"

#include <cassert>

#include "application/Application.h"

InjectionLimiter::InjectionLimiter(
    const std::string& _name, const Component* _parent,
    Application* _app, u32 _id)
    : Component(_name, _parent), app_(_app), id_(_id),
      lastTime_(0) {}

InjectionLimiter::~InjectionLimiter() {}

void InjectionLimiter::setMessageReceiver(MessageReceiver* _receiver) {
  receiver_ = _receiver;
}

void InjectionLimiter::receiveMessage(Message* _message) {
  u64 cycles = app_->cyclesToSend(_message->numFlits());

  // determine when this message should be delivered to the message receiver
  u64 nextTime = lastTime_ + (cycles * gSim->cycleTime());
  u64 nowTime = gSim->time();
  if (nextTime == nowTime) {
    receiver_->receiveMessage(_message);
  } else {
    if (nextTime < nowTime) {
      dbgprintf("compensating time %lu -> %lu", nextTime,
                gSim->futureCycle(1));
      nextTime = gSim->futureCycle(1);
    }
    addEvent(nextTime, 0, _message, 0);
  }

  // save next for last
  lastTime_ = nextTime;
}

void InjectionLimiter::processEvent(void* _event, s32 _type) {
  receiver_->receiveMessage(reinterpret_cast<Message*>(_event));
}
