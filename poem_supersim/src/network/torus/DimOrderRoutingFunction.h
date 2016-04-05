/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_TORUS_DIMORDERROUTINGFUNCTION_H_
#define NETWORK_TORUS_DIMORDERROUTINGFUNCTION_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/Router.h"

namespace Torus {

class DimOrderRoutingFunction : public RoutingFunction {
 public:
  DimOrderRoutingFunction(const std::string& _name, const Component* _parent,
                          u64 _latency, Router* _router, u32 _numVcs,
                          std::vector<u32> _dimensionWidths,
                          u32 _concentration, u32 _inputPort);
  ~DimOrderRoutingFunction();

 protected:
  void processRequest(
      Flit* _flit, RoutingFunction::Response* _response) override;

 private:
  Router* router_;
  u32 numVcs_;
  u32 numPorts_;
  std::vector<u32> dimensionWidths_;
  u32 concentration_;
  u32 inputPort_;
  bool isTerminalPort_;
  u32 inputPortDim_;
};

}  // namespace Torus

#endif  // NETWORK_TORUS_DIMORDERROUTINGFUNCTION_H_
