/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_HYPERXPOEMNOC1_DIMORDERROUTINGFUNCTION_H_
#define NETWORK_HYPERXPOEMNOC1_DIMORDERROUTINGFUNCTION_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/Router.h"

namespace HyperXPoemNoc1 {

class DimOrderRoutingFunction : public RoutingFunction {
 public:
  DimOrderRoutingFunction(const std::string& _name, const Component* _parent,
                          u64 _latency, Router* _router, u32 _numVcs,
                          const std::vector<u32>& _dimensionWidths,
                          const std::vector<u32>& _dimensionWeights,
                          u32 _concentration, u32 _nocWidth, u32 _nocWeight,
                          u32 _nocConcentration, bool _allVcs);
  ~DimOrderRoutingFunction();

 protected:
  void processRequest(
      Flit* _flit, RoutingFunction::Response* _response) override;

 private:
  Router* router_;
  u32 numVcs_;
  u32 numPorts_;
  const std::vector<u32> dimensionWidths_;
  const std::vector<u32> dimensionWeights_;
  u32 concentration_;
  u32 nocWidth_;
  u32 nocWeight_;
  u32 nocConcentration_;
  bool allVcs_;
};

}  // namespace HyperXPoemNoc1

#endif  // NETWORK_HYPERXPOEMNOC1_DIMORDERROUTINGFUNCTION_H_
