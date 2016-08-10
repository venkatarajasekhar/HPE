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
#include "network/hierarchicalhyperx/PAR.h"
#include <strop/strop.h>
#include <cassert>

#include <unordered_set>
#include <unordered_map>
#include <set>
#include "types/Message.h"
#include "types/Packet.h"

namespace HierarchicalHyperX {

PAR::PAR(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs,
    const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _concentration, u32 _globalLinksPerRouter,
    f64 _threshold)
    : GlobalDimOrderRoutingAlgorithm(_name, _parent,
      _latency, _router, _numVcs, _globalDimensionWidths,
      _globalDimensionWeights, _localDimensionWidths, _localDimensionWeights,
      _concentration, _globalLinksPerRouter),
      threshold_(_threshold) {
  // every VC per hop
  assert(numVcs_ >= localDimWidths_.size() * (globalDimWidths_.size() + 1) + 1
                    + globalDimWidths_.size() + 1 + 1);
}

PAR::~PAR() {}

void PAR::processRequest(
    Flit* _flit, RoutingAlgorithm::Response* _response) {
  // ex: [c,1,...,m,1,...,n]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();
  const std::vector<u32>& routerAddress = router_->getAddress();
  Packet* packet = _flit->getPacket();
  u32 localDimensions = localDimWidths_.size();
  dbgprintf("Destination address is %s \n",
         strop::vecString<u32>(*destinationAddress).c_str());
  dbgprintf("Present address is %s \n",
         strop::vecString<u32>(routerAddress).c_str());
  std::unordered_set<u32> outputPorts;

  if (packet->getHopCount() == 1) {
    packet->setValiantMode(false);
    assert(packet->getGlobalHopCount() == 0);
  }

  // reset local dst after switching to Valiant mode in first group
  if (packet->getGlobalHopCount() == 0 &&
      packet->getValiantMode() == true) {
    std::vector<u32>* localRouter = new std::vector<u32>(localDimensions);
    for (u32 localDim = 0; localDim < localDimensions; localDim++) {
      localRouter->at(localDim) = routerAddress.at(localDim);
    }
    packet->setLocalDst(localRouter);
    u32 globalPort =  gSim->rnd.nextU64(0, globalLinksPerRouter_ - 1);
    std::vector<u32>* dstPort = new std::vector<u32>;
    dstPort->push_back(globalPort);
    packet->setLocalDstPort(dstPort);

    for (auto itr = dstPort->begin();
             itr != dstPort->end(); itr++) {
      u32 portBase = getPortBase();
      bool res = outputPorts.insert(portBase + *itr).second;
      (void)res;
      assert(res);
    }
  } else {
  // routing depends on mode
    outputPorts = PAR::routing(_flit, destinationAddress);
  }
  assert(outputPorts.size() >= 1);

  // reset localDst once in a new group
  if (*outputPorts.begin() >= getPortBase()) {
    packet->incrementGlobalHopCount();
    // delete local router
    packet->setLocalDst(nullptr);
    packet->setLocalDstPort(nullptr);
  }

  // figure out which VC set to use
  u32 vcSet;
  vcSet = packet->getHopCount() - 1;
  dbgprintf("vcset = %u\n", vcSet);

  // format the response
  for (auto it = outputPorts.cbegin(); it != outputPorts.cend(); ++it) {
    u32 outputPort = *it;
    if (outputPort < concentration_) {
      for (u32 vc = 0; vc < numVcs_; vc++) {
        _response->add(outputPort, vc);
      }
      assert(_response->size() > 0);
      packet->setLocalDst(nullptr);
      packet->setLocalDstPort(nullptr);
    } else {
      for (u32 vc = vcSet; vc < numVcs_; vc += localDimWidths_.size()
                    * (globalDimWidths_.size() + 1) + 1
                    + globalDimWidths_.size() + 1 + 1) {
        _response->add(outputPort, vc);
      }
    }
  }
  assert(_response->size() > 0);
}

std::unordered_set<u32> PAR::routing(
      Flit* _flit, const std::vector<u32>* destinationAddress) {
  // ex: [1,...,m,1,...,n]
  const std::vector<u32>& routerAddress = router_->getAddress();
  Packet* packet = _flit->getPacket();
  u32 globalDimensions = globalDimWidths_.size();
  u32 localDimensions = localDimWidths_.size();
  u32 numRoutersPerGlobalRouter = 1;
  for (u32 tmp = 0; tmp < localDimensions; tmp++) {
    numRoutersPerGlobalRouter *= localDimWidths_.at(tmp);
  }

  // determine if already at destination virtual global router
  u32 globalDim;
  u32 globalPortBase = 0;
  for (globalDim = 0; globalDim < globalDimensions; globalDim++) {
    if (routerAddress.at(localDimensions + globalDim)
        != destinationAddress->at(localDimensions + globalDim + 1)) {
      break;
    }
    globalPortBase += ((globalDimWidths_.at(globalDim) - 1)
                 * globalDimWeights_.at(globalDim));
  }

  std::vector<u32> globalOutputPorts;
  std::unordered_set<u32> outputPorts;

  // within first non-dst group
  if (packet->getGlobalHopCount() == 0 && globalDim != globalDimensions) {
    // choose a random local dst
    if (packet->getLocalDst() == nullptr) {
      setLocalDst(globalDim, globalPortBase,
                  destinationAddress, &globalOutputPorts, _flit);
    }
    const std::vector<u32>* localDst =
      reinterpret_cast<const std::vector<u32>*>(packet->getLocalDst());
    const std::vector<u32>* localDstPort =
      reinterpret_cast<const std::vector<u32>*>(packet->getLocalDstPort());

    // if router has a global link to destination global router
    if (std::equal(localDst->begin(), localDst->end(),
          routerAddress.begin())) {
      // in first group, determine if congested
      if (packet->getGlobalHopCount() == 0 &&
          packet->getValiantMode() == false) {
        for (auto itr = localDstPort->begin();
             itr != localDstPort->end(); itr++) {
          // make sure we are reffering to the right outputPort
          u32 outputPort = *itr + getPortBase();
          f64 availability = 0.0;
          u32 vcCount = 0;
          for (u32 vc = packet->getHopCount() - 1; vc < numVcs_;
               vc += localDimWidths_.size() * (globalDimWidths_.size() + 1) + 1
                    + globalDimWidths_.size() + 1 + 1) {
            u32 vcIdx = router_->vcIndex(outputPort, vc);
            f64 vcStatus = router_->congestionStatus(vcIdx);
            availability += vcStatus;
            vcCount++;
          }
          availability = availability / vcCount;
          if (availability <= threshold_) {
            dbgprintf("current router = %s \n",
              strop::vecString<u32>(routerAddress).c_str());
            dbgprintf("avaialability = %f\n", availability);
            // reset localdst for valiant
            packet->setLocalDst(nullptr);
            packet->setLocalDstPort(nullptr);
            packet->setValiantMode(true);
            dbgprintf("switched to valiant \n");
            // pick a random local port
            u32 localPort = gSim->rnd.nextU64(
                            concentration_, getPortBase() - 1);
            // send using the local port
            bool res = outputPorts.insert(localPort).second;
            (void)res;
            // assert(res);
          }
        }
        // not congested
        if (outputPorts.size() == 0) {
          for (auto itr = localDstPort->begin();
               itr != localDstPort->end(); itr++) {
            u32 portBase = getPortBase();
            bool res = outputPorts.insert(portBase + *itr).second;
            (void)res;
            assert(res);
          }
        }
      } else {
        // set output ports to those links
        for (auto itr = localDstPort->begin();
             itr != localDstPort->end(); itr++) {
          u32 portBase = getPortBase();
          bool res = outputPorts.insert(portBase + *itr).second;
          (void)res;
          assert(res);
        }
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
      assert(outputPorts.size() >= 1);
    }
  } else {
    // not in first group, just use dimension order
    return GlobalDimOrderRoutingAlgorithm::routing(
           _flit, destinationAddress);
  }
  assert(outputPorts.size() >= 1);
  return outputPorts;
}

u32 PAR::getPortBase() {
  u32 localDimensions = localDimWidths_.size();
  u32 portBase = concentration_;
  for (u32 i = 0; i < localDimensions; i++) {
    portBase += ((localDimWidths_.at(i) - 1) * localDimWeights_.at(i));
  }
  return portBase;
}

void PAR::globalPortToLocalAddress(
          u32 globalPort, std::vector<u32>* localAddress,
          u32* localPortWithoutBase) {
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
  dbgprintf("Connected local router address is %s \n",
            strop::vecString<u32>(*localAddress).c_str());
}

void PAR::setLocalDst(
        u32 globalDim, u32 globalPortBase,
        const std::vector<u32>* destinationAddress,
        std::vector<u32>* globalOutputPorts, Flit* _flit) {
  const std::vector<u32>& routerAddress = router_->getAddress();
  Packet* packet = _flit->getPacket();
  u32 localDimensions = localDimWidths_.size();
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
    globalOutputPorts->push_back(globalPort);
  }
  assert(globalOutputPorts->size() > 0);

  bool hasGlobalLinkToDst = false;
  std::vector<u32>* dstPort = new std::vector<u32>;

  // set local dst to self if has global link
  for (auto itr = globalOutputPorts->begin();
       itr != globalOutputPorts->end(); itr++) {
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
    u32 globalPort = globalOutputPorts->at(gSim->rnd.nextU64(
                      0, globalOutputPorts->size() - 1));

    // translate global router port number to local router
    std::vector<u32>* localRouter = new std::vector<u32>(localDimensions);
    u32 connectedPort;
    globalPortToLocalAddress(globalPort, localRouter, &connectedPort);
    dstPort->push_back(connectedPort);
    packet->setLocalDst(localRouter);
    packet->setLocalDstPort(dstPort);
  }
}

}  // namespace HierarchicalHyperX
