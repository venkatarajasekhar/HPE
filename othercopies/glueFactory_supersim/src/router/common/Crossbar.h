/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_COMMON_CROSSBAR_H_
#define ROUTER_COMMON_CROSSBAR_H_

#include <json/json.h>
#include <prim/prim.h>

#include <list>
#include <string>
#include <utility>
#include <vector>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

class Crossbar : public Component {
 public:
  Crossbar(const std::string& _name, const Component* _parent,
           u32 _numInputs, u32 _numOutputs,
           Json::Value _settings);
  ~Crossbar();
  u32 numInputs() const;
  u32 numOutputs() const;
  void setReceiver(u32 _destId, FlitReceiver* _receiver, u32 _destPort);
  // call multiple times for multicast
  void inject(Flit* _flit, u32 _srcId, u32 _destId);
  void processEvent(void* _event, s32 _type) override;

 private:
  u64 latency_;
  u32 numInputs_;
  u32 numOutputs_;
  std::vector<std::pair<u32, FlitReceiver*> > receivers_;
  u64 nextTime_;
  std::list<std::vector<Flit*> > destMaps_;
};

#endif  // ROUTER_COMMON_CROSSBAR_H_
