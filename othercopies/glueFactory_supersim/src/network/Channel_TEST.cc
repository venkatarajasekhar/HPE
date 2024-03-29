/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <gtest/gtest.h>
#include <json/json.h>
#include <prim/prim.h>

#include <cmath>

#include <string>
#include <unordered_set>

#include "event/Component.h"
#include "network/Channel.h"
#include "random/Random.h"
#include "types/Control.h"
#include "types/ControlReceiver.h"
#include "types/Credit.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

#include "test/TestSetup_TEST.h"


const bool DEBUG = false;

class Source;
class Sink;

class Source : public Component, public ControlReceiver {
 public:
  explicit Source(Channel* _channel);
  ~Source();
  void setSink(Sink* _sink);
  void load(u32 _count, u64 _totalCycles);
  void expect(u64 _time);
  void processEvent(void* _event, s32 _type);
  void receiveControl(u32 _port, Control* _control);

 private:
  Channel* channel_;
  u32 port_;
  Sink* sink_;
  std::unordered_set<u64> expected_;
};

class Sink : public Component, public FlitReceiver {
 public:
  explicit Sink(Channel* _channel);
  ~Sink();
  void setSource(Source* _source);
  void load(u32 _count, u64 _totalCycles);
  void expect(u64 _time);
  void processEvent(void* _event, s32 _type);
  void receiveFlit(u32 _port, Flit* _flit);

 private:
  Channel* channel_;
  u32 port_;
  Source* source_;
  std::unordered_set<u64> expected_;
};

/* Source impl */

Source::Source(Channel* _channel)
    : Component("Source", nullptr), channel_(_channel) {
  debug_ = DEBUG;
  channel_->setSource(this, port_ = gRandom->randomU64(0, U32_MAX));
}

Source::~Source() {
  assert(expected_.size() == 0);
}

void Source::setSink(Sink* _sink) {
  sink_ = _sink;
}

void Source::load(u32 _count, u64 _totalCycles) {
  assert(_count <= _totalCycles);

  // list all available times
  std::unordered_set<u64> injectCycles;
  for (u64 cycle = 1; cycle <= _totalCycles; cycle++) {
    injectCycles.insert(cycle);
  }

  // create _count injection events
  while (_count > 0) {
    _count--;
    auto it = injectCycles.begin();
    std::advance(it, gRandom->randomU64(0, injectCycles.size() - 1));
    u64 injectCycle = *it;
    injectCycles.erase(it);
    u64 injectTime = gSim->futureCycle(injectCycle);
    dbgprintf("source inject at %lu(%lu)", injectCycle, injectTime);
    addEvent(injectTime, 0, nullptr, 0);
    sink_->expect(gSim->futureCycle(injectCycle + channel_->latency()));
  }
}

void Source::expect(u64 _time) {
  assert(expected_.count(_time) == 0);
  expected_.insert(_time);
  dbgprintf("source expect at %lu", _time);
}

void Source::processEvent(void* _event, s32 _type) {
  Flit* flit = new Flit(0, false, false, nullptr);
  assert(channel_->getNextFlit() == nullptr);
  channel_->setNextFlit(flit);
  dbgprintf("source injecting at %lu", gSim->time());
}

void Source::receiveControl(u32 _port, Control* _control) {
  dbgprintf("source receiving control at %lu", gSim->time());
  assert(_port == port_);
  assert(expected_.count(gSim->time()) == 1);
  expected_.erase(gSim->time());
  delete _control;
}

/* Sink impl */

Sink::Sink(Channel* _channel)
    : Component("Sink", nullptr), channel_(_channel) {
  debug_ = DEBUG;
  channel_->setSink(this, port_ = gRandom->randomU64(0, U32_MAX));
}

Sink::~Sink() {
  assert(expected_.size() == 0);
}

void Sink::setSource(Source* _source) {
  source_ = _source;
}

void Sink::load(u32 _count, u64 _totalCycles) {
  assert(_count <= _totalCycles);

  // list all available times
  std::unordered_set<u64> injectCycles;
  for (u64 cycle = 1; cycle <= _totalCycles; cycle++) {
    injectCycles.insert(cycle);
  }

  // create _count injection events
  while (_count > 0) {
    _count--;
    auto it = injectCycles.begin();
    std::advance(it, gRandom->randomU64(0, injectCycles.size() - 1));
    u64 injectCycle = *it;
    injectCycles.erase(it);
    u64 injectTime = gSim->futureCycle(injectCycle);
    dbgprintf("sink inject at %lu(%lu)", injectCycle, injectTime);
    addEvent(injectTime, 0, nullptr, 0);
    source_->expect(gSim->futureCycle(injectCycle + channel_->latency()));
  }
}

void Sink::expect(u64 _time) {
  assert(expected_.count(_time) == 0);
  expected_.insert(_time);
  dbgprintf("sink expect at %lu", _time);
}

void Sink::processEvent(void* _event, s32 _type) {
  Control* control = new Control();
  assert(channel_->getNextControl() == nullptr);
  channel_->setNextControl(control);
  dbgprintf("sink injecting at %lu", gSim->time());
}

void Sink::receiveFlit(u32 _port, Flit* _flit) {
  dbgprintf("sink receiving flit at %lu", gSim->time());
  assert(_port == port_);
  assert(expected_.count(gSim->time()) == 1);
  expected_.erase(gSim->time());
  delete _flit;
}

/* Monitoring */

class EndMonitoring : public Component {
 public:
  EndMonitoring(Channel* _channel, u32 _cycles)
      : Component("Timer", nullptr), channel_(_channel) {
    channel_->startMonitoring();
    addEvent(gSim->futureCycle(_cycles), 0, nullptr, 0);
  }

  ~EndMonitoring() {}

  void processEvent(void* _event, s32 _type) {
    channel_->endMonitoring();
  }

 private:
  Channel* channel_;
};


/* Test driver */

TEST(Channel, full) {
  u64 seed = 12345678;
  for (u32 cycleTime = 1; cycleTime <= 100; cycleTime += 26) {
    TestSetup setup(cycleTime, seed++);

    const u32 latency = gRandom->randomU64(1, 5);

    Json::Value settings;
    settings["latency"] = latency;
    Channel c("TestChannel", nullptr, settings);

    Source source(&c);
    Sink sink(&c);
    source.setSink(&sink);
    sink.setSource(&source);

    const u32 clocks = 10000;
    const u32 flits = gRandom->randomU64(1, clocks);
    const u32 controls = gRandom->randomU64(1, clocks);
    source.load(flits, clocks);
    sink.load(controls, clocks);

    EndMonitoring ender(&c, clocks);

    gSim->simulate();

    f64 actUtil = c.utilization();
    f64 expUtil = static_cast<f64>(flits) / clocks;
    f64 absDelta = std::abs(actUtil - expUtil);
    ASSERT_LE(absDelta, 0.0001);
  }
}
