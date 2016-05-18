/*
 * Copyright 2016 Hewlett Packard Enterprise Development LP
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "network/hierarchicalhyperx/DimOrderRoutingAlgorithm.h"
#include <strop/strop.h>
#include <cassert>

#include <unordered_set>
#include <set>
#include "types/Message.h"
#include "types/Packet.h"

namespace HierarchicalHyperX {

DimOrderRoutingAlgorithm::DimOrderRoutingAlgorithm(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs,
    const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _concentration, u32 _globalLinksPerRouter, bool _allVcs)
    : RoutingAlgorithm(_name, _parent, _router, _latency),
      numVcs_(_numVcs), globalDimensionWidths_(_globalDimensionWidths),
      globalDimensionWeights_(_globalDimensionWeights),
      localDimensionWidths_(_localDimensionWidths),
      localDimensionWeights_(_localDimensionWeights),
      concentration_(_concentration),
      globalLinksPerRouter_(_globalLinksPerRouter), allVcs_(_allVcs) {
}

DimOrderRoutingAlgorithm::~DimOrderRoutingAlgorithm() {}

void DimOrderRoutingAlgorithm::processRequest(
    Flit* _flit, RoutingAlgorithm::Response* _response) {
  // ex: [c,1,...,m,1,...,n]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();

  dbgprintf("Destination address is %s \n",
         strop::vecString<u32>(*destinationAddress).c_str());

  // perform routing
  std::unordered_set<u32> outputPorts = routing(destinationAddress);
  assert(outputPorts.size() > 0);

  // format the response
  for (auto it = outputPorts.cbegin(); it != outputPorts.cend(); ++it) {
    u32 outputPort = *it;
    if (allVcs_) {
      // select all VCs in the output port
      for (u32 vc = 0; vc < numVcs_; vc++) {
        _response->add(outputPort, vc);
      }
    } else {
      // use the current VC
      _response->add(outputPort, _flit->getVc());
    }
  }
}

std::unordered_set<u32> DimOrderRoutingAlgorithm::routing(
     const std::vector<u32>* destinationAddress) {
  // ex: [1,...,m,1,...,n]
  const std::vector<u32>& routerAddress = router_->getAddress();
  dbgprintf("Router address is %s \n",
         strop::vecString<u32>(routerAddress).c_str());
  assert(routerAddress.size() == destinationAddress->size() - 1);
  u32 globalDimensions = globalDimensionWidths_.size();
  u32 localDimensions = localDimensionWidths_.size();
  u32 numRoutersPerGlobalRouter = 1;
  for (u32 tmp = 0; tmp < localDimensions; tmp++) {
    numRoutersPerGlobalRouter *= localDimensionWidths_.at(tmp);
  }

  // determine if already at destination virtual global router
  u32 globalDim;
  u32 globalPortBase = 0;
  for (globalDim = 0; globalDim < globalDimensions; globalDim++) {
    if (routerAddress.at(localDimensions + globalDim)
        != destinationAddress->at(localDimensions + globalDim + 1)) {
      break;
    }
    globalPortBase += ((globalDimensionWidths_.at(globalDim) - 1)
                 * globalDimensionWeights_.at(globalDim));
  }

  // first perform routing at the global level
  std::unordered_set<u32> globalOutputPorts;
  std::unordered_set<u32> outputPorts;

  // if at different global router
  if (globalDim != globalDimensions) {
    // find the right port of the virtual global router
    u32 src = routerAddress.at(localDimensions + globalDim);
    u32 dst = destinationAddress->at(localDimensions + globalDim + 1);
    if (dst < src) {
      dst += globalDimensionWidths_.at(globalDim);
    }
    u32 offset = (dst - src - 1) * globalDimensionWeights_.at(globalDim);

    // add all ports where the two global routers are connecting
    for (u32 weight = 0; weight < globalDimensionWeights_.at(globalDim);
         weight++) {
      u32 globalPort = globalPortBase + offset + weight;
      bool res = globalOutputPorts.insert(globalPort).second;
      (void)res;
      assert(res);
    }
    assert(globalOutputPorts.size() > 0);

    // translate global router port number to local router
    std::set< std::vector<u32> > routerLinkedToGlobalDst;
    bool hasGlobalLinkToDst = false;
    u32 portBase = concentration_;
    for (u32 i = 0; i < localDimensions; i++) {
      portBase += ((localDimensionWidths_.at(i) - 1) *
                     localDimensionWeights_.at(i));
    }

    for (auto itr = globalOutputPorts.begin();
         itr != globalOutputPorts.end(); itr++) {
      std::vector<u32> localRouter(localDimensions);
      u32 product = 1;
      for (u32 tmp = 0; tmp < localDimensions - 1; tmp++) {
        product *= localDimensionWidths_.at(tmp);
      }
      u32 globalOutputPort = *itr;
      for (s32 localDim = localDimensions - 1; localDim >= 0; localDim--) {
        localRouter.at(localDim) = (globalOutputPort / product)
          % localDimensionWidths_.at(localDim);
        globalOutputPort %= product;
        if (localDim != 0) {
          product /= localDimensionWidths_.at(localDim - 1);
        }
      }
      assert(localRouter.size() == localDimensions);
      routerLinkedToGlobalDst.insert(localRouter);
      dbgprintf("Connected local router address is %s \n",
         strop::vecString<u32>(localRouter).c_str());
      u32 connectedPort = (*itr) / numRoutersPerGlobalRouter;
      assert(connectedPort < globalLinksPerRouter_);
      // test if router has a global link to destination global router
      if (std::equal(localRouter.begin(), localRouter.end(),
          routerAddress.begin())) {
        // found direct global link to destation global router
        hasGlobalLinkToDst = true;
        // set output ports to those links
        bool res = outputPorts.insert(portBase + connectedPort).second;
        (void)res;
        assert(res);
      }
    }

    // if router has no direct global link to destination global router
    if (hasGlobalLinkToDst != true) {
      // route to all the local routers which have a link to global dst
      for (auto itr = routerLinkedToGlobalDst.begin();
           itr != routerLinkedToGlobalDst.end(); itr++) {
        // determine the next local dimension to work on
        u32 localDim;
        u32 portBase = concentration_;
        for (localDim = 0; localDim < localDimensions; localDim++) {
          if (routerAddress.at(localDim) != itr->at(localDim)) {
            break;
          }
        portBase += ((localDimensionWidths_.at(localDim) - 1)
                     * localDimensionWeights_.at(localDim));
        }
        // more local router-to-router hops needed
        u32 src = routerAddress.at(localDim);
        u32 dst = itr->at(localDim);
        if (dst < src) {
          dst += localDimensionWidths_.at(localDim);
        }
        u32 offset = (dst - src - 1) * localDimensionWeights_.at(localDim);
        // add all ports where the two routers are connecting
        for (u32 weight = 0; weight < localDimensionWeights_.at(localDim);
             weight++) {
          bool res = outputPorts.insert(portBase + offset + weight).second;
          (void)res;
          // there are cases where the same output is inserted multiple
          // times, so should not assert res
          // assert(res);
        }
      }
    }
  } else {
    // if at the same global virtual router
    // use the regular dimension order routing of HyperX

    // determine the next local dimension to work on
    u32 localDim;
    u32 portBase = concentration_;
    for (localDim = 0; localDim < localDimensions; localDim++) {
      if (routerAddress.at(localDim) != destinationAddress->at(localDim+1)) {
        break;
      }
      portBase += ((localDimensionWidths_.at(localDim) - 1)
                   * localDimensionWeights_.at(localDim));
    }
    // test if already at destination local router
    if (localDim == localDimensions) {
      bool res = outputPorts.insert(destinationAddress->at(0)).second;
      (void)res;
      assert(res);
    } else {
      // more local router-to-router hops needed
      u32 src = routerAddress.at(localDim);
      u32 dst = destinationAddress->at(localDim + 1);
      if (dst < src) {
        dst += localDimensionWidths_.at(localDim);
      }
      u32 offset = (dst - src - 1) * localDimensionWeights_.at(localDim);
      // add all ports where the two routers are connecting
      for (u32 weight = 0; weight < localDimensionWeights_.at(localDim);
           weight++) {
        bool res = outputPorts.insert(portBase + offset + weight).second;
        (void)res;
        assert(res);
      }
    }
  }
  return outputPorts;
}

}  // namespace HierarchicalHyperX
