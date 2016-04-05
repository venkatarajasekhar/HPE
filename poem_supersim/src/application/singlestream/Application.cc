/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/singlestream/Application.h"

#include <cassert>

#include <vector>

#include "application/singlestream/Terminal.h"
#include "event/Simulator.h"
#include "network/Network.h"
#include "random/Random.h"

namespace SingleStream {

Application::Application(const std::string& _name, const Component* _parent,
                         Json::Value _settings)
    : ::Application(_name, _parent, _settings), doMonitoring_(true) {
  // the index of the pair of communicating terminals
  s32 src = _settings["source_terminal"].asInt();
  if (src < 0) {
    sourceTerminal_ = gRandom->randomU64(0, numTerminals() - 1);
  } else {
    sourceTerminal_ = src;
  }
  dbgprintf("source terminal is %u", sourceTerminal_);

  s32 dst = _settings["destination_terminal"].asInt();
  if (dst < 0) {
    do {
      destinationTerminal_ = gRandom->randomU64(0, numTerminals() - 1);
    } while ((numTerminals() != 1) &&
             (destinationTerminal_ == sourceTerminal_));
  } else {
    destinationTerminal_ = dst;
  }
  dbgprintf("destination terminal is %u", destinationTerminal_);
  assert(sourceTerminal_ < numTerminals());
  assert(destinationTerminal_ < numTerminals());

  // all terminals are the same
  for (u32 t = 0; t < numTerminals(); t++) {
    std::string tname = "Terminal_" + std::to_string(t);
    std::vector<u32> address;
    gSim->getNetwork()->translateIdToAddress(t, &address);
    Terminal* terminal = new Terminal(tname, this, t, address, this,
                                      _settings["terminal"]);
    setTerminal(t, terminal);
  }
}

Application::~Application() {}

f64 Application::percentComplete() const {
  Terminal* t = reinterpret_cast<Terminal*>(getTerminal(destinationTerminal_));
  return t->percentComplete();
}

u32 Application::getSource() const {
  return sourceTerminal_;
}

u32 Application::getDestination() const {
  return destinationTerminal_;
}

void Application::receivedFirst(u32 _id) {
  dbgprintf("receivedFirst(%u)", _id);
  assert(_id == destinationTerminal_);

  // upon first received, start monitoring
  printf("starting monitoring\n");
  if (doMonitoring_) {
    gSim->startMonitoring();
  }
}

void Application::sentLast(u32 _id) {
  dbgprintf("sentLast(%u)", _id);
  assert(_id == sourceTerminal_);

  // after last sent message, stop monitoring
  printf("ending monitoring\n");
  if (gSim->getMonitoring() == false) {
    doMonitoring_ = false;
    printf("*** monitoring attempted to end before starting.\n"
           "*** you need to have a larger num_messages value.\n"
           "*** simulation will continue without monitoring.\n");
  } else {
    gSim->endMonitoring();
  }
}

}  // namespace SingleStream
