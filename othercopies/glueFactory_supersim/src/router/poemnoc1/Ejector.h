/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMNOC1_EJECTOR_H_
#define ROUTER_POEMNOC1_EJECTOR_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"
#include "network/Channel.h"

namespace PoemNoc1 {

class Ejector : public Component, public FlitReceiver {
 public:
  Ejector(const std::string& _name, const Component* _parent);
  ~Ejector();

  void setChannel(Channel* _channel);

  // called by crossbar (FlitReceiver)
  void receiveFlit(u32 _port, Flit* _flit) override;

 private:
  Channel* channel_;
  u64 lastSetTime_;
};

}  // namespace PoemNoc1

#endif  // ROUTER_POEMNOC1_EJECTOR_H_
