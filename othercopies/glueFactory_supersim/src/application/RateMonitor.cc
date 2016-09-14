/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/RateMonitor.h"

RateMonitor::RateMonitor(const std::string& _name, const Component* _parent)
    : Component(_name, _parent),
      flitCount_(0), startTime_(U64_MAX), endTime_(U64_MAX), running_(false) {}

RateMonitor::~RateMonitor() {}

void RateMonitor::setMessageReceiver(MessageReceiver* _receiver) {
  messageReceiver_ = _receiver;
}

void RateMonitor::receiveMessage(Message* _message) {
  if (running_) {
    flitCount_ += _message->numFlits();
  }
  messageReceiver_->receiveMessage(_message);
}

void RateMonitor::start() {
  if (!running_) {
    startTime_ = gSim->time();
    flitCount_ = 0;
  }
  running_ = true;
}

void RateMonitor::end() {
  if (running_) {
    endTime_ = gSim->time();
  }
  running_ = false;
}

f64 RateMonitor::rate() const {
  if ((startTime_ == U64_MAX) || (endTime_ == U64_MAX) || (running_)) {
    return F64_NAN;
  } else {
    f64 cycles = (f64)(endTime_ - startTime_) / gSim->cycleTime();
    return flitCount_ / cycles;
  }
}
