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
#include "network/hierarchicalhyperx/GlobalRandomRoutingAlgorithm.h"
#include <strop/strop.h>
#include <cassert>

#include <unordered_set>
#include <set>
#include "types/Message.h"
#include "types/Packet.h"

namespace HierarchicalHyperX {

GlobalRandomRoutingAlgorithm::GlobalRandomRoutingAlgorithm(
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

GlobalRandomRoutingAlgorithm::~GlobalRandomRoutingAlgorithm() {}

void GlobalRandomRoutingAlgorithm::processRequest(
    Flit* _flit, RoutingAlgorithm::Response* _response) {
  // ex: [c,1,...,m,1,...,n]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();

  // perform routing
  std::unordered_set<u32> outputPorts = routing(_flit, destinationAddress);
  assert(outputPorts.size() >= 1);
  if (*outputPorts.begin() >= getPortBase()) {
    _flit->getPacket()->incrementGlobalHopCount();
    // delete local router
    _flit->getPacket()->setLocalDst(nullptr);
    _flit->getPacket()->setLocalDstPort(nullptr);
  }
  // figure out which VC set to use
  u32 vcSet = _flit->getPacket()->getHopCount() - 1;
  dbgprintf("current vcset %u \n", vcSet);

  // format the response
  for (auto it = outputPorts.cbegin(); it != outputPorts.cend(); ++it) {
    dbgprintf("output port is %u \n", *it);
    u32 outputPort = *it;
    if (outputPort < concentration_) {
      for (u32 vc = 0; vc < numVcs_; vc++) {
        _response->add(outputPort, vc);
      }
    } else {
    // select VCs in the corresponding set
      for (u32 vc = vcSet; vc < numVcs_; vc += (globalDimWidths_.size() + 1)
                     * localDimWidths_.size() + globalDimWidths_.size()) {
        _response->add(outputPort, vc);
      }
    }
  }
  assert(_response->size() > 0);
}

std::unordered_set<u32> GlobalRandomRoutingAlgorithm::routing(Flit* _flit,
  const std::vector<u32>* destinationAddress) {
  // ex: [1,...,m,1,...,n]
  const std::vector<u32>& routerAddress = router_->getAddress();
  dbgprintf("\nDim: Router address is %s \n",
           strop::vecString<u32>(routerAddress).c_str());
  dbgprintf("Dim: Destination address is %s \n",
           strop::vecString<u32>(*destinationAddress).c_str());
  assert(routerAddress.size() == destinationAddress->size() - 1);

  Packet* packet = _flit->getPacket();

  u32 globalDimensions = globalDimWidths_.size();
  u32 localDimensions = localDimWidths_.size();
  u32 numRoutersPerGlobalRouter = 1;
  for (u32 tmp = 0; tmp < localDimensions; tmp++) {
    numRoutersPerGlobalRouter *= localDimWidths_.at(tmp);
  }

  // determine if already at destination virtual global router
  std::vector<u32> diffGlobalDims;
  assert(diffGlobalDims.size() == 0);
  bool atGlobalDst = true;
  for (u32 globalDim = 0; globalDim < globalDimensions; globalDim++) {
    if (routerAddress.at(localDimensions + globalDim)
        != destinationAddress->at(localDimensions + globalDim + 1)) {
      diffGlobalDims.push_back(globalDim);
      atGlobalDst = false;
    }
  }

  // first perform routing at the global level
  std::vector<u32> globalOutputPorts;
  std::unordered_set<u32> outputPorts;

  // if at different global router
  if (atGlobalDst == false) {
    if (packet->getLocalDst() == nullptr) {
      // pick a random global dimension
      u32 globalDim = diffGlobalDims.at(gSim->rnd.nextU64(0,
                                        diffGlobalDims.size() - 1));
      u32 globalPortBase = 0;
      for (u32 tmp = 0; tmp < globalDim; tmp++) {
        globalPortBase += ((globalDimWidths_.at(tmp) - 1)
                           * globalDimWeights_.at(tmp));
      }
      // find the right port of the virtual global router
      u32 src = routerAddress.at(localDimensions + globalDim);
      u32 dst = destinationAddress->at(localDimensions + globalDim + 1);
      if (dst < src) {
        dst += globalDimWidths_.at(globalDim);
      }
      u32 offset = (dst - src - 1) * globalDimWeights_.at(globalDim);

      // add all ports where the two global routers are connecting
      for (u32 weight = 0; weight < globalDimWeights_.at(globalDim);
           weight++) {
        u32 globalPort = globalPortBase + offset + weight;
        globalOutputPorts.push_back(globalPort);
      }
      assert(globalOutputPorts.size() > 0);

      bool hasGlobalLinkToDst = false;
      std::vector<u32>* dstPort = new std::vector<u32>;

      // set local dst to self if has global link
      for (auto itr = globalOutputPorts.begin();
           itr != globalOutputPorts.end(); itr++) {
        std::vector<u32>* localRouter = new std::vector<u32>(localDimensions);
        u32 connectedPort;
        globalPortToLocalAddress(*itr, localRouter, &connectedPort);
        if (std::equal(localRouter->begin(), localRouter->end(),
                       routerAddress.begin())) {
          hasGlobalLinkToDst = true;
          packet->setLocalDst(localRouter);
          dstPort->push_back(connectedPort);
          packet->setLocalDstPort(dstPort);
        }
      }

      // if no global link, pick a random one
      if (hasGlobalLinkToDst == false) {
        // pick a random global port
        u32 globalPort = globalOutputPorts.at(gSim->rnd.nextU64(
                         0, globalOutputPorts.size() - 1));

        // translate global router port number to local router
        std::vector<u32>* localRouter = new std::vector<u32>(localDimensions);
        u32 connectedPort;
        globalPortToLocalAddress(globalPort, localRouter, &connectedPort);
        dstPort->push_back(connectedPort);
        packet->setLocalDst(localRouter);
        packet->setLocalDstPort(dstPort);
      }
    }

    const std::vector<u32>* localDst =
      reinterpret_cast<const std::vector<u32>*>(packet->getLocalDst());
    const std::vector<u32>* localDstPort =
      reinterpret_cast<const std::vector<u32>*>(packet->getLocalDstPort());
    dbgprintf("Connected local dst is %s \n",
               strop::vecString<u32>(*localDst).c_str());

    u32 portBase = getPortBase();
    // if router has a global link to destination global router
    if (std::equal(localDst->begin(), localDst->end(),
          routerAddress.begin())) {
      // set output ports to those links
      for (auto itr = localDstPort->begin();
           itr != localDstPort->end(); itr++) {
        bool res = outputPorts.insert(portBase + *itr).second;
        (void)res;
        assert(res);
      }
    } else {
      // determine the next local dimension to work on
      u32 localDim;
      u32 portBase = concentration_;
      for (localDim = 0; localDim < localDimensions; localDim++) {
        if (routerAddress.at(localDim) != localDst->at(localDim)) {
          break;
        }
        portBase += ((localDimWidths_.at(localDim) - 1)
                     * localDimWeights_.at(localDim));
      }
      // more local router-to-router hops needed
      u32 src = routerAddress.at(localDim);
      u32 dst = localDst->at(localDim);
      if (dst < src) {
        dst += localDimWidths_.at(localDim);
      }
      u32 offset = (dst - src - 1) * localDimWeights_.at(localDim);
      // add all ports where the two routers are connecting
      for (u32 weight = 0; weight < localDimWeights_.at(localDim);
           weight++) {
        bool res = outputPorts.insert(portBase + offset + weight).second;
        (void)res;
        assert(res);
      }
    }
  } else {
    // if at the same global virtual router
    // use the regular dimension order routing of HyperX
    packet->setLocalDst(nullptr);
    packet->setLocalDstPort(nullptr);
    // determine the next local dimension to work on
    u32 localDim;
    u32 portBase = concentration_;
    for (localDim = 0; localDim < localDimensions; localDim++) {
      if (routerAddress.at(localDim) != destinationAddress->at(localDim+1)) {
        break;
      }
      portBase += ((localDimWidths_.at(localDim) - 1)
                   * localDimWeights_.at(localDim));
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
        dst += localDimWidths_.at(localDim);
      }
      u32 offset = (dst - src - 1) * localDimWeights_.at(localDim);
      // add all ports where the two routers are connecting
      for (u32 weight = 0; weight < localDimWeights_.at(localDim);
           weight++) {
        bool res = outputPorts.insert(portBase + offset + weight).second;
        (void)res;
        assert(res);
      }
    }
  }
  return outputPorts;
}

u32 GlobalRandomRoutingAlgorithm::getPortBase() {
  u32 localDimensions = localDimWidths_.size();
  u32 portBase = concentration_;
  for (u32 i = 0; i < localDimensions; i++) {
    portBase += ((localDimWidths_.at(i) - 1) * localDimWeights_.at(i));
  }
  return portBase;
}

void GlobalRandomRoutingAlgorithm::globalPortToLocalAddress(u32 globalPort,
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
  u32 globalPortCopy = globalPort;
  for (s32 localDim = localDimensions - 1; localDim >= 0; localDim--) {
    localAddress->at(localDim) = (globalPortCopy / product)
                                  % localDimWidths_.at(localDim);
    globalPortCopy %= product;
    if (localDim != 0) {
      product /= localDimWidths_.at(localDim - 1);
    }
  }
  assert(localAddress->size() == localDimensions);
  *localPortWithoutBase = globalPort / numRoutersPerGlobalRouter;
  assert(*localPortWithoutBase < globalLinksPerRouter_);
  dbgprintf("Computed local router address is %s \n",
            strop::vecString<u32>(*localAddress).c_str());
}

}  // namespace HierarchicalHyperX
