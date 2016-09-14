/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_COMMON_FLITDISTRIBUTOR_H_
#define ROUTER_COMMON_FLITDISTRIBUTOR_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

class FlitDistributor : public Component, public FlitReceiver {
 public:
  FlitDistributor(const std::string& _name, const Component* _parent,
                  u32 _outputs);
  ~FlitDistributor();

  void setReceiver(u32 _vc, FlitReceiver* _receiver);

  void receiveFlit(u32 _port, Flit* _flit) override;

 private:
  std::vector<FlitReceiver*> receivers_;
};

#endif  // ROUTER_COMMON_FLITDISTRIBUTOR_H_
