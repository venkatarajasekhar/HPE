/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#ifndef NETWORK_HIERARCHICALHYPERX_VALIANTROUTINGALGORITHM_H_
#define NETWORK_HIERARCHICALHYPERX_VALIANTROUTINGALGORITHM_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingAlgorithm.h"
#include "network/hierarchicalhyperx/GlobalDimOrderRoutingAlgorithm.h"
#include "router/Router.h"

namespace HierarchicalHyperX {

class ValiantRoutingAlgorithm : public GlobalDimOrderRoutingAlgorithm {
 public:
  ValiantRoutingAlgorithm(const std::string& _name, const Component* _parent,
                          u64 _latency, Router* _router, u32 _numVcs,
                          const std::vector<u32>& _globalDimensionWidths,
                          const std::vector<u32>& _globalDimensionWeights,
                          const std::vector<u32>& _localDimensionWidths,
                          const std::vector<u32>& _localDimensionWeights,
                          u32 _concentration, u32 _globalLinksPerRouter,
                          bool _randomGroup);
  ~ValiantRoutingAlgorithm();

 protected:
  void processRequest(
      Flit* _flit, RoutingAlgorithm::Response* _response) override;
  bool randomGroup_;
};

}  // namespace HierarchicalHyperX

#endif  // NETWORK_HIERARCHICALHYPERX_VALIANTROUTINGALGORITHM_H_
