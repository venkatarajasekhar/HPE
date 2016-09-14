/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#ifndef NETWORK_HIERARCHYHYPERX_VALIANTROUTINGFUNCTION_H_
#define NETWORK_HIERARCHYHYPERX_VALIANTROUTINGFUNCTION_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "network/hierarchyhyperx/DimOrderRoutingFunction.h"
#include "router/Router.h"

namespace HierarchyHyperX {

class ValiantRoutingFunction : public DimOrderRoutingFunction {
 public:
  ValiantRoutingFunction(const std::string& _name, const Component* _parent,
                          u64 _latency, Router* _router, u32 _numVcs,
                          const std::vector<u32>& _globalDimensionWidths,
                          const std::vector<u32>& _globalDimensionWeights,
                          const std::vector<u32>& _localDimensionWidths,
                          const std::vector<u32>& _localDimensionWeights,
                          u32 _concentration, u32 _globalLinksPerRouter,
                          bool _allVcs);
  ~ValiantRoutingFunction();

 protected:
  void processRequest(
      Flit* _flit, RoutingFunction::Response* _response) override;
};

}  // namespace HierarchyHyperX

#endif  // NETWORK_HIERARCHYHYPERX_VALIANTROUTINGFUNCTION_H_
