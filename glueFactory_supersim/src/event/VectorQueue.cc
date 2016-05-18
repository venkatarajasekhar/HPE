/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "event/VectorQueue.h"

#include <cassert>

VectorQueue::VectorQueue(Json::Value _settings)
    : Simulator(_settings) {}

VectorQueue::~VectorQueue() {}

void VectorQueue::addEvent(u64 _time, u8 _epsilon, Component* _component,
                           void* _event, s32 _type) {
  assert((_time > time_) ||  // future by time
         ((_time == time_) && (_epsilon > epsilon_)) ||  // future by epsilon
         (initial()));  // has not yet run

  // create a bundle object
  VectorQueue::EventBundle bundle;
  bundle.time      = _time;
  bundle.epsilon   = _epsilon;
  bundle.component = _component;
  bundle.event     = _event;
  bundle.type      = _type;

  // push into queue
  eventQueue_.push(bundle);
}

u64 VectorQueue::queueSize() const {
  return eventQueue_.size();
}

void VectorQueue::runNextEvent() {
  // if there is an event to run, run it
  if (eventQueue_.size() > 0) {
    // process the next event
    VectorQueue::EventBundle bundle = eventQueue_.top();
    time_ = bundle.time;
    epsilon_ = bundle.epsilon;
    bundle.component->processEvent(bundle.event, bundle.type);
    eventQueue_.pop();
  }

  // set the quit_ status
  quit_ = eventQueue_.size() < 1;
}

/** EventBundleComparator sub-class **/
VectorQueue::EventBundleComparator::EventBundleComparator() {}

VectorQueue::EventBundleComparator::~EventBundleComparator() {}

bool VectorQueue::EventBundleComparator::operator()(
    const VectorQueue::EventBundle _lhs,
    const VectorQueue::EventBundle _rhs) const {
  return (_lhs.time == _rhs.time) ?
      (_lhs.epsilon > _rhs.epsilon) :
      (_lhs.time > _rhs.time);
}
