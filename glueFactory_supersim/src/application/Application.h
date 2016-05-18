/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_APPLICATION_H_
#define APPLICATION_APPLICATION_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "stats/MessageLog.h"
#include "stats/RateLog.h"

class Messenger;
class Terminal;

class Application : public Component {
 public:
  Application(const std::string& _name, const Component* _parent,
              Json::Value _settings);
  ~Application();
  u32 numTerminals() const;
  Terminal* getTerminal(u32 _id) const;
  virtual f64 percentComplete() const = 0;
  MessageLog* getMessageLog() const;
  u64 cyclesToSend(u32 _numFlits) const;
  void startMonitoring();
  void endMonitoring();

 protected:
  void setTerminal(u32 _id, Terminal* _terminal);

 private:
  std::vector<Terminal*> terminals_;
  std::vector<Messenger*> messengers_;
  f64 maxInjectionRate_;
  MessageLog* messageLog_;
  RateLog* rateLog_;
};

#endif  // APPLICATION_APPLICATION_H_
