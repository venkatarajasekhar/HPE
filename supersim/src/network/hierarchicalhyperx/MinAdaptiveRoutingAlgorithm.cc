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
#include "network/hierarchicalhyperx/MinAdaptiveRoutingAlgorithm.h"
#include <strop/strop.h>
#include <cassert>

#include <unordered_set>
#include <unordered_map>
#include <set>
#include "types/Message.h"
#include "types/Packet.h"

namespace HierarchicalHyperX {

MinAdaptiveRoutingAlgorithm::MinAdaptiveRoutingAlgorithm(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs,
    const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _concentration, u32 _globalLinksPerRouter)
    : RoutingAlgorithm(_name, _parent, _router, _latency),
      numVcs_(_numVcs), globalDimWidths_(_globalDimensionWidths),
      globalDimWeights_(_globalDimensionWeights),
      localDimWidths_(_localDimensionWidths),
      localDimWeights_(_localDimensionWeights),
      concentration_(_concentration),
      globalLinksPerRouter_(_globalLinksPerRouter) {
  assert(numVcs_ >= globalDimWidths_.size() + 1);
}

MinAdaptiveRoutingAlgorithm::~MinAdaptiveRoutingAlgorithm() {}

void MinAdaptiveRoutingAlgorithm::processRequest(
    Flit* _flit, RoutingAlgorithm::Response* _response) {
  // ex: [c,1,...,m,1,...,n]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();

  dbgprintf("Destination address is %s \n",
         strop::vecString<u32>(*destinationAddress).c_str());

  // perform routing
  std::unordered_set<u32> outputPorts = routing(_flit, destinationAddress);
  assert(outputPorts.size() == 1);

  // figure out which VC set to use
  u32 vcSet = _flit->getGlobalHopCount();

  // format the response
  for (auto it = outputPorts.cbegin(); it != outputPorts.cend(); ++it) {
    u32 outputPort = *it;
    // select VCs in the corresponding set
    for (u32 vc = vcSet; vc < numVcs_; vc += globalDimWidths_.size() + 1) {
      _response->add(outputPort, vc);
    }
  }
}

std::unordered_set<u32> MinAdaptiveRoutingAlgorithm::routing(Flit* _flit,
  const std::vector<u32>* destinationAddress) {
  // ex: [1,...,m,1,...,n]
  const std::vector<u32>& routerAddress = router_->getAddress();
  dbgprintf("Router address is %s \n",
         strop::vecString<u32>(routerAddress).c_str());
  assert(routerAddress.size() == destinationAddress->size() - 1);
  u32 globalDimensions = globalDimWidths_.size();
  u32 localDimensions = localDimWidths_.size();
  u32 numRoutersPerGlobalRouter = 1;
  for (u32 tmp = 0; tmp < localDimensions; tmp++) {
    numRoutersPerGlobalRouter *= localDimWidths_.at(tmp);
  }

  std::vector<u32> diffGlobalDims;
  bool atGlobalDst = true;
  // determine if already at destination virtual global router
  for (u32 globalDim = 0; globalDim < globalDimensions; globalDim++) {
    if (routerAddress.at(localDimensions + globalDim)
        != destinationAddress->at(localDimensions + globalDim + 1)) {
      diffGlobalDims.push_back(globalDim);
      atGlobalDst = false;
    }
  }

  // first perform routing at the global level
  std::unordered_set<u32> outputPorts;

  // if at different global router
  if (atGlobalDst == false) {
    std::unordered_map< u32, f64 > globalPortAvailability;
    // find the all ports of the virtual global router
    for (auto itr = diffGlobalDims.begin(); itr != diffGlobalDims.end();
         itr++) {
      std::unordered_set<u32> globalOutputPorts;
      u32 src = routerAddress.at(localDimensions + *itr);
      u32 dst = destinationAddress->at(localDimensions + *itr + 1);
      if (dst < src) {
        dst += globalDimWidths_.at(*itr);
      }
      u32 globalPortBase = 0;
      for (u32 globalDim = 0; globalDim < *itr; globalDim++) {
        globalPortBase += ((globalDimWidths_.at(globalDim) - 1)
                          * globalDimWeights_.at(globalDim));
      }
      u32 offset = (dst - src - 1) * globalDimWeights_.at(*itr);
      // add all ports where the two global routers are connecting
      for (u32 weight = 0; weight < globalDimWeights_.at(*itr); weight++) {
        u32 globalPort = globalPortBase + offset + weight;
        bool res = globalOutputPorts.insert(globalPort).second;
        (void)res;
        assert(res);
      }
      assert(globalOutputPorts.size() > 0);

      // translate global router port number to local router
      std::set< std::vector<u32> > routerLinkedToGlobalDst;

      for (auto itr = globalOutputPorts.begin();
           itr != globalOutputPorts.end(); itr++) {
        std::vector<u32> localRouter(localDimensions);
        u32 connectedPort;
        globalPortToLocalAddress(*itr, &localRouter, &connectedPort);
        routerLinkedToGlobalDst.insert(localRouter);
        // find the availability for each of the links in each of the diff dims
        f64 availability = 0.0;
        for (u32 vc = _flit->getGlobalHopCount(); vc < numVcs_;
             vc += globalDimWidths_.size() + 1) {
          u32 vcIdx = router_->vcIndex(getPortBase() + connectedPort, vc);
          availability += router_->congestionStatus(vcIdx);
        }
        globalPortAvailability.insert(std::make_pair(*itr, availability));
      }
    }
    // find the global port with max availability
    u32 highestGlobalPort = findHighestPort(globalPortAvailability);

    // doing a traslation from globalPort to localRouterAddress again
    std::vector<u32> highestRouter(localDimensions);
    u32 connectedPort;
    globalPortToLocalAddress(highestGlobalPort, &highestRouter, &connectedPort);

    // test if router has a global link to destination global router
    bool hasGlobalLinkToDst = false;
    if (std::equal(highestRouter.begin(), highestRouter.end(),
                   routerAddress.begin())) {
      // found direct global link to destation global router
      hasGlobalLinkToDst = true;
      // set output ports to those links
      bool res = outputPorts.insert(getPortBase() + connectedPort).second;
      (void)res;
      assert(res);
    }

    // if router has no direct global link to destination global router
    if (hasGlobalLinkToDst != true) {
      // route to the local router with highest availability
      // determine the local dimension to work on
      std::vector<u32> diffLocalDims;
      std::unordered_map<u32, f64> portAvailability;
      for (u32 localDim = 0; localDim < localDimensions; localDim++) {
        if (routerAddress.at(localDim) != highestRouter.at(localDim)) {
          diffLocalDims.push_back(localDim);
        }
      }
      findPortAvailability(diffLocalDims, &portAvailability, &highestRouter,
                           _flit);
      // find the port with max availability
      u32 highestPort = findHighestPort(portAvailability);
      bool res = outputPorts.insert(highestPort).second;
      (void)res;
      assert(res);
    } else {
      _flit->incrementGlobalHopCount();
    }
  } else {
    // if at the same global virtual router
    // find the port with most availability of all offset dimensions

    // determine all local dimensions src and dst are different
    std::vector<u32> diffDims;
    std::unordered_map<u32, f64> portAvailability;
    bool atDst = true;
    for (u32 localDim = 0; localDim < localDimensions; localDim++) {
      if (routerAddress.at(localDim) != destinationAddress->at(localDim+1)) {
        diffDims.push_back(localDim);
        atDst = false;
      }
    }
    // test if already at destination local router
    if (atDst == true) {
      bool res = outputPorts.insert(destinationAddress->at(0)).second;
      (void)res;
      assert(res);
    } else {
      // more local router-to-router hops needed
      std::vector<u32> dstRouter(destinationAddress->begin() + 1,
                                 destinationAddress->end());
      findPortAvailability(diffDims, &portAvailability, &dstRouter, _flit);
      // find the port with max availability
      u32 availablePort = findHighestPort(portAvailability);

      bool res = outputPorts.insert(availablePort).second;
      (void)res;
      assert(res);
    }
  }
  return outputPorts;
}

u32 MinAdaptiveRoutingAlgorithm::getPortBase() {
  u32 localDimensions = localDimWidths_.size();
  u32 portBase = concentration_;
  for (u32 i = 0; i < localDimensions; i++) {
    portBase += ((localDimWidths_.at(i) - 1) * localDimWeights_.at(i));
  }
  return portBase;
}

void MinAdaptiveRoutingAlgorithm::globalPortToLocalAddress(u32 globalPort,
  std::vector<u32>* localAddress, u32* localPortWithoutBase) {
  u32 localDimensions = localDimWidths_.size();
  u32 numRoutersPerGlobalRouter = 1;
  for (u32 tmp = 0; tmp < localDimensions; tmp++) {
    numRoutersPerGlobalRouter *= localDimWidths_.at(tmp);
  }
  u32 product = 1;
  for (u32 tmp = 0; tmp < localDimensions - 1; tmp++) {
    product *= localDimWidths_.at(tmp);
  }
  for (s32 localDim = localDimensions - 1; localDim >= 0; localDim--) {
       localAddress->at(localDim) = (globalPort / product)
                                    % localDimWidths_.at(localDim);
    globalPort %= product;
    if (localDim != 0) {
      product /= localDimWidths_.at(localDim - 1);
    }
  }
  assert(localAddress->size() == localDimensions);
  *localPortWithoutBase = globalPort / numRoutersPerGlobalRouter;
  assert(*localPortWithoutBase < globalLinksPerRouter_);
  dbgprintf("Connected local router address is %s \n",
            strop::vecString<u32>(*localAddress).c_str());
}

u32 MinAdaptiveRoutingAlgorithm::findHighestPort(
  std::unordered_map< u32, f64 > portAvailability) {
  assert(portAvailability.size() >= 1);
  u32 highestPort = 0;;
  f64 highest = 0.0;
  for (auto const& port : portAvailability) {
    if (port.second >= highest) {
      highestPort = port.first;
      highest = port.second;
    }
  }
  return highestPort;
}

void MinAdaptiveRoutingAlgorithm::findPortAvailability(
  std::vector<u32> diffDims, std::unordered_map<u32, f64>* portAvailability,
  std::vector<u32>* destinationAddress, Flit* _flit) {
  const std::vector<u32>& routerAddress = router_->getAddress();
  for (auto itr = diffDims.begin(); itr != diffDims.end(); itr++) {
    u32 src = routerAddress.at(*itr);
    u32 dst = destinationAddress->at(*itr);
    if (dst < src) {
      dst += localDimWidths_.at(*itr);
    }
    u32 portBase = concentration_;
    for (u32 localDim = 0; localDim < *itr; localDim++) {
      portBase += ((localDimWidths_.at(localDim) - 1)
                  * localDimWeights_.at(localDim));
    }
    u32 offset = (dst - src - 1) * localDimWeights_.at(*itr);
    // all ports where the two routers are connecting
    for (u32 weight = 0; weight < localDimWeights_.at(*itr); weight++) {
      u32 port = portBase + offset + weight;
      f64 availability = 0.0;
      for (u32 vc = _flit->getGlobalHopCount(); vc < numVcs_;
           vc += globalDimWidths_.size() + 1) {
        u32 vcIdx = router_->vcIndex(port, vc);
        availability += router_->congestionStatus(vcIdx);
      }
      portAvailability->insert(std::make_pair(port, availability));
    }
  }
}
}  // namespace HierarchicalHyperX
