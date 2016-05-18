/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_CHANNEL_H_
#define NETWORK_CHANNEL_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"

class Flit;
class FlitReceiver;
class Control;
class ControlReceiver;

class Channel : public Component {
 public:
  Channel(const std::string& _name, const Component* _parent,
          Json::Value _settings);
  ~Channel();
  u32 latency() const;
  void setSource(ControlReceiver* _source, u32 _port);
  void setSink(FlitReceiver* _sink, u32 _port);
  void startMonitoring();
  void endMonitoring();
  f64 utilization() const;
  void processEvent(void* _event, s32 _type) override;

  /*
   * This retrieves the flit that exists in the event queue for the next
   * flit time in the future. nullptr is returned if it has not been set.
   */
  Flit* getNextFlit() const;

  /*
   * Sets 'flit' to be the next flit to traverse the channel. This inserts
   * an event into the event queue. If an existing flit is already set for
   * this time, an assertion will fail!
   * This returns the time the flit will be injected into the channel,
   * which is guaranteed to be in the future.
   */
  u64 setNextFlit(Flit* _flit);

  /*
   * This retrieves the control that exists in the event queue for the next
   * control time in the future. nullptr is returned if it has not been set.
   */
  Control* getNextControl() const;

  /*
   * Sets 'control' to be the next control to traverse the channel. This inserts
   * an event into the event queue. If an existing control is already set for
   * this time, an assertion will fail!
   * This returns the time the control will be injected into the channel,
   * which is guaranteed to be in the future.
   */
  u64 setNextControl(Control* _control);

 private:
  u32 latency_;
  u64 nextFlitTime_;
  Flit* nextFlit_;
  u64 nextControlTime_;
  Control* nextControl_;
  bool monitoring_;
  u64 monitorTime_;
  u64 monitorCount_;

  ControlReceiver* source_;  // sends flits, receives controls
  u32 sourcePort_;
  FlitReceiver* sink_;   // receives flits, sends controls
  u32 sinkPort_;
};

#endif  // NETWORK_CHANNEL_H_
