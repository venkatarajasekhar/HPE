/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_POEM_EJECTOR_H_
#define INTERFACE_POEM_EJECTOR_H_

#include <prim/prim.h>

#include <string>

#include "interface/poem/Interface.h"
#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace Poem {

class Interface;

class Ejector : public Component, public FlitReceiver {
 public:
  Ejector(const std::string& _name, Interface* _interface);
  ~Ejector();

  // called by crossbar (FlitReceiver)
  void receiveFlit(u32 _port, Flit* _flit) override;

 private:
  Interface* interface_;
  u32 portId_;
  u64 lastSetTime_;
};

}  // namespace Poem

#endif  // INTERFACE_POEM_EJECTOR_H_
