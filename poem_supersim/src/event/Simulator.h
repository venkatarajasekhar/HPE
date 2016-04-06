/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef EVENT_SIMULATOR_H_
#define EVENT_SIMULATOR_H_

#include <json/json.h>
#include <prim/prim.h>

class Component;
class Network;
class Application;

class Simulator {
 public:
  explicit Simulator(Json::Value _settings);
  virtual ~Simulator();

  // this adds an event to the queue
  virtual void addEvent(u64 _time, u8 _epsilon, Component* _component,
                        void* _event, s32 _type) = 0;

  // this function must return the current size of the queue
  virtual u64 queueSize() const = 0;

  void simulate();
  void stop();
  bool initial() const;
  bool running() const;
  u64 time() const;
  u8 epsilon() const;
  u64 cycle() const;
  u64 cycleTime() const;
  u64 futureCycle(u32 _cycles) const;
  void setNetwork(Network* _network);
  Network* getNetwork() const;
  void setApplication(Application* _app);
  Application* getApplication() const;
  bool getMonitoring() const;
  void startMonitoring();
  void endMonitoring();

 protected:
  u64 time_;
  u8 epsilon_;
  bool quit_;
  bool printProgress_;
  f64 printInterval_;

  // this function must set time_, epsilon_, and quit_ on every call
  virtual void runNextEvent() = 0;

 private:
  bool initial_;
  bool running_;
  u64 cycleTime_;

  Network* net_;
  Application* app_;

  bool monitoring_;
};

extern Simulator* gSim;

#endif  // EVENT_SIMULATOR_H_
