/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_INPUTQUEUED_EJECTOR_H_
#define ROUTER_INPUTQUEUED_EJECTOR_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace InputQueued {

class Router;

class Ejector : public Component, public FlitReceiver {
 public:
  Ejector(const std::string& _name, Router* _router, u32 _portId);
  ~Ejector();

  // called by crossbar (FlitReceiver)
  void receiveFlit(u32 _port, Flit* _flit) override;

 private:
  Router* router_;
  u32 portId_;
  u64 lastSetTime_;
};

}  // namespace InputQueued

#endif  // ROUTER_INPUTQUEUED_EJECTOR_H_
