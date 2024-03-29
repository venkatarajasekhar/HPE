/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/Channel.h"

#include <cassert>

#include "types/Flit.h"
#include "types/FlitReceiver.h"
#include "types/Control.h"
#include "types/ControlReceiver.h"
#include "types/Packet.h"
#include "event/Simulator.h"

#define FLIT 0xBE
#define CTRL 0xEF

Channel::Channel(const std::string& _name, const Component* _parent,
                 Json::Value _settings)
    : Component(_name, _parent) {
  latency_ = _settings["latency"].asUInt();
  assert(latency_ > 0);

  nextFlitTime_ = U64_MAX;
  nextFlit_ = nullptr;
  nextControlTime_ = U64_MAX;
  nextControl_ = nullptr;

  monitoring_ = false;
  monitorTime_ = U64_MAX;
}

Channel::~Channel() {}

u32 Channel::latency() const {
  return latency_;
}

void Channel::setSource(ControlReceiver* _source, u32 _port) {
  source_ = _source;
  sourcePort_ = _port;
}

void Channel::setSink(FlitReceiver* _sink, u32 _port) {
  sink_ = _sink;
  sinkPort_ = _port;
}

void Channel::startMonitoring() {
  assert(monitoring_ == false);
  assert(monitorTime_ == U64_MAX);
  monitoring_ = true;
  monitorTime_ = gSim->time();  // start time
  monitorCount_ = 0;
}

void Channel::endMonitoring() {
  assert(monitoring_ == true);
  assert(monitorTime_ != U64_MAX);
  monitoring_ = false;
  monitorTime_ = gSim->time() - monitorTime_;  // delta time
}

f64 Channel::utilization() const {
  assert(monitoring_ == false);
  assert(monitorTime_ != U64_MAX);
  return (f64)monitorCount_ / ((f64)monitorTime_ / gSim->cycleTime());
}

void Channel::processEvent(void* _event, s32 _type) {
  assert(gSim->epsilon() == 1);
  switch (_type) {
    case FLIT:
      {
        Flit* flit = reinterpret_cast<Flit*>(_event);
        if (flit->isHead()) {
          flit->getPacket()->incrementHopCount();
        }
        sink_->receiveFlit(sinkPort_, flit);
      }
      break;
    case CTRL:
      {
        source_->receiveControl(sourcePort_,
                                reinterpret_cast<Control*>(_event));
      }
      break;
    default:
      assert(false);
  }
}

Flit* Channel::getNextFlit() const {
  // determine the next time slot to send a flit
  u64 nextSlot = gSim->futureCycle(1);

  // return nullptr if the next flit hasn't been set
  if (nextFlitTime_ != nextSlot) {
    return nullptr;
  } else {
    // if it was set, return it
    return nextFlit_;
  }
}

u64 Channel::setNextFlit(Flit* _flit) {
  // determine the next time slot to send a flit
  u64 nextSlot = gSim->futureCycle(1);
  assert(nextSlot != nextFlitTime_);

  // set the time and value
  nextFlitTime_ = nextSlot;
  nextFlit_ = _flit;

  // add the event of when the flit will arrive on the other end
  u64 nextTime = gSim->futureCycle(latency_);
  addEvent(nextTime, 1, _flit, FLIT);

  // increment the count when monitoring
  if (monitoring_) {
    monitorCount_++;
  }

  // return the injection time
  return nextFlitTime_;
}

Control* Channel::getNextControl() const {
  // determine the next time slot to send a control
  u64 nextSlot = gSim->futureCycle(1);

  // return nullptr if the next control hasn't been set
  if (nextControlTime_ != nextSlot) {
    return nullptr;
  } else {
    // if it was set, return it
    return nextControl_;
  }
}

u64 Channel::setNextControl(Control* _control) {
  // determine the next time slot to send a control
  u64 nextSlot = gSim->futureCycle(1);
  assert(nextSlot != nextControlTime_);

  // set the time and value
  nextControlTime_ = nextSlot;
  nextControl_ = _control;

  // add the event of when the control will arrive on the other end
  u64 nextTime = gSim->futureCycle(latency_);
  addEvent(nextTime, 1, _control, CTRL);

  // return the injection time
  return nextControlTime_;
}
