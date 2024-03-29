/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_MESSENGER_H_
#define APPLICATION_MESSENGER_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "stats/RateLog.h"

class Application;
class InjectionLimiter;
class RateMonitor;
class EnterNotifier;
class ExitNotifier;

class MessageReceiver;
class Interface;
class Terminal;

class Messenger : public Component {
 public:
  Messenger(const std::string& _name, const Component* _parent,
            Application* _app, u32 _id);
  ~Messenger();
  void linkInterface(Interface* _interface);
  void linkTerminal(Terminal* _terminal);
  void startRateMonitors();
  void endRateMonitors();
  void logRates(RateLog* _rateLog);

 private:
  Application* app_;
  u32 id_;
  InjectionLimiter* injectionLimiter_;
  RateMonitor* supplyMonitor_;
  RateMonitor* injectionMonitor_;
  RateMonitor* ejectionMonitor_;
  EnterNotifier* enterNotifier_;
  ExitNotifier* exitNotifier_;
};

#endif  // APPLICATION_MESSENGER_H_
