/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_UNO_DIRECTROUTINGFUNCTION_H_
#define NETWORK_UNO_DIRECTROUTINGFUNCTION_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/Router.h"

namespace Uno {

class DirectRoutingFunction : public RoutingFunction {
 public:
  DirectRoutingFunction(const std::string& _name, const Component* _parent,
                        u64 _latency, Router* _router, u32 _numVcs,
                        u32 _concentration, bool _allVcs);
  ~DirectRoutingFunction();

 protected:
  void processRequest(
      Flit* _flit, RoutingFunction::Response* _response) override;

 private:
  Router* router_;
  u32 numVcs_;
  u32 concentration_;
  bool allVcs_;
};

}  // namespace Uno

#endif  // NETWORK_UNO_DIRECTROUTINGFUNCTION_H_
