/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/Application.h"

#include <cassert>
#include <cmath>

#include "application/Messenger.h"
#include "application/Terminal.h"
#include "network/Network.h"
#include "random/Random.h"

Application::Application(const std::string& _name, const Component* _parent,
                         Json::Value _settings)
    : Component(_name, _parent) {
  Network* network = gSim->getNetwork();

  u32 radix = network->numInterfaces();
  terminals_.resize(radix, nullptr);
  messengers_.resize(radix, nullptr);

  // extract maximum injection rate
  maxInjectionRate_ = _settings["max_injection_rate"].asDouble();
  if (maxInjectionRate_ <= 0.0) {
    maxInjectionRate_ = INFINITY;
  }

  // instantiate the messengers
  for (u32 id = 0; id < radix; id++) {
    // create the messenger
    std::string cname = "Messenger_" + std::to_string(id);
    messengers_.at(id) = new Messenger(cname, this, this, id);

    // link the messenger to the network
    messengers_.at(id)->linkInterface(network->getInterface(id));
  }

  // create a MessageLog
  messageLog_ = new MessageLog(_settings["message_log"]);

  // create the rate log
  rateLog_ = new RateLog(_settings["rate_log"]);
}

Application::~Application() {
  for (u32 idx = 0; idx < terminals_.size(); idx++) {
    delete terminals_.at(idx);
    delete messengers_.at(idx);
  }
  delete messageLog_;
  delete rateLog_;
}

u32 Application::numTerminals() const {
  return terminals_.size();
}

Terminal* Application::getTerminal(u32 _id) const {
  return terminals_.at(_id);
}

MessageLog* Application::getMessageLog() const {
  return messageLog_;
}

u64 Application::cyclesToSend(u32 _numFlits) const {
  if (isinf(maxInjectionRate_)) {
    return 0;  // infinite injection rate
  }
  f64 cycles = _numFlits / maxInjectionRate_;

  // if the number of cycles is not even, probablistic cycles must be computed
  f64 fraction = modf(cycles, &cycles);
  if (fraction != 0.0) {
    assert(fraction > 0.0);
    assert(fraction < 1.0);
    f64 rnd = gRandom->randomF64();
    if (fraction > rnd) {
      cycles += 1.0;
    }
  }
  return (u64)cycles;
}

void Application::startMonitoring() {
  for (u32 i = 0; i < messengers_.size(); i++) {
    messengers_.at(i)->startRateMonitors();
  }
}

void Application::endMonitoring() {
  for (u32 i = 0; i < messengers_.size(); i++) {
    messengers_.at(i)->endRateMonitors();
  }
  for (u32 i = 0; i < messengers_.size(); i++) {
    messengers_.at(i)->logRates(rateLog_);
  }
}

void Application::setTerminal(u32 _id, Terminal* _terminal) {
  terminals_.at(_id) = _terminal;
  messengers_.at(_id)->linkTerminal(terminals_.at(_id));
}
