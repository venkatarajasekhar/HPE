/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#include "network/hierarchicalhyperx/ValiantRoutingAlgorithm.h"

#include <strop/strop.h>
#include <cassert>
#include <unordered_set>
#include <set>
#include "types/Packet.h"
#include "types/Message.h"
#include "network/hierarchicalhyperx/util.h"

namespace HierarchicalHyperX {

ValiantRoutingAlgorithm::ValiantRoutingAlgorithm(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs,
    const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _concentration, u32 _globalLinksPerRouter, bool _randomGroup)
  : DimOrderRoutingAlgorithm(_name, _parent,
    _latency, _router, _numVcs, _globalDimensionWidths,
    _globalDimensionWeights, _localDimensionWidths, _localDimensionWeights,
    _concentration, _globalLinksPerRouter), randomGroup_(_randomGroup) {
  assert(numVcs_ >= 2 * globalDimWidths_.size() + 2);
}

ValiantRoutingAlgorithm::~ValiantRoutingAlgorithm() {}

void ValiantRoutingAlgorithm::processRequest(
    Flit* _flit, RoutingAlgorithm::Response* _response) {
  // ex: [1,...,m,1,...,n]
  const std::vector<u32>& routerAddress = router_->getAddress();
  Packet* packet = _flit->getPacket();
  Message* message = packet->getMessage();
  // ex: [c,1,...,m,1,...,n]
  const std::vector<u32>* destinationAddress = message->getDestinationAddress();

  // if at destination
  if (std::equal(routerAddress.begin(), routerAddress.end(),
                 destinationAddress->begin() + 1)) {
    u32 outputPort = destinationAddress->at(0);
    // on ejection, any dateline VcSet is ok within any stage VcSet
    for (u32 vc = 0; vc < numVcs_; vc++) {
      _response->add(outputPort, vc);
    }
    assert(_response->size() > 0);
    // delete the routing extension
    packet->setRoutingExtension(nullptr);
  } else {
    std::unordered_set<u32> outputPorts = routing(_flit, destinationAddress);
    assert(outputPorts.size() > 0);
    RoutingInfo* ri = reinterpret_cast<RoutingInfo*>(
                      packet->getRoutingExtension());
    if (*outputPorts.begin() >= getPortBase(concentration_, localDimWidths_,
                                            localDimWeights_)) {
      ri->globalHopCount++;
      // delete local router
      ri->localDst = nullptr;
      ri->localDstPort = nullptr;
      packet->setRoutingExtension(ri);
    }

    // figure out which VC set to use
    u32 vcSet = ri->globalHopCount;
    assert(vcSet <= 2 * globalDimWidths_.size() + 2);

    // format the response
    for (auto it = outputPorts.cbegin(); it != outputPorts.cend(); ++it) {
      u32 outputPort = *it;
      // select VCs in the corresponding set
      for (u32 vc = vcSet; vc < numVcs_;
           vc += 2 * globalDimWidths_.size() + 2) {
        _response->add(outputPort, vc);
      }
    }
    assert(_response->size() > 0);
  }
}

std::unordered_set<u32> ValiantRoutingAlgorithm::routing(Flit* _flit,
  const std::vector<u32>* destinationAddress) {
  const std::vector<u32>& routerAddress = router_->getAddress();
  Packet* packet = _flit->getPacket();

  // create the routing extension if needed
  if (packet->getRoutingExtension() == nullptr) {
    // create routing extension header
    //  the extension is a vector with one dummy element then the address of the
    //  intermediate router
    std::vector<u32>* re = new std::vector<u32>(1 + routerAddress.size());
    re->at(0) = U32_MAX;  // dummy

    // random intermediate address
    for (u32 idx = 0; idx < localDimWidths_.size(); idx++) {
      re->at(idx + 1) = gSim->rnd.nextU64(0, localDimWidths_.at(idx) - 1);
    }
    for (u32 idx = 0; idx < globalDimWidths_.size(); idx++) {
      re->at(idx + localDimWidths_.size() + 1) =
        gSim->rnd.nextU64(0, globalDimWidths_.at(idx) - 1);
    }
    RoutingInfo* ri = new RoutingInfo();
    ri->intermediateAddress = re;
    ri->localDst = nullptr;
    ri->localDstPort = nullptr;
    ri->localDerouteCount = 0;
    ri->globalHopCount = 0;
    ri->intermediateDone = false;
    ri->valiantMode = false;
    packet->setRoutingExtension(ri);
  }

  RoutingInfo* ri = reinterpret_cast<RoutingInfo*>(
                    packet->getRoutingExtension());
  if (ri->intermediateAddress == nullptr) {
    std::vector<u32>* re = new std::vector<u32>(1 + routerAddress.size());
    re->at(0) = U32_MAX;  // dummy

    // random intermediate address
    for (u32 idx = 0; idx < localDimWidths_.size(); idx++) {
      re->at(idx + 1) = gSim->rnd.nextU64(0, localDimWidths_.at(idx) - 1);
    }
    for (u32 idx = 0; idx < globalDimWidths_.size(); idx++) {
      re->at(idx + localDimWidths_.size() + 1) =
        gSim->rnd.nextU64(0, globalDimWidths_.at(idx) - 1);
    }
    ri->intermediateAddress = re;
  }
  // intermediate address info
  const std::vector<u32>* intermediateAddress =
      reinterpret_cast<const std::vector<u32>*>(ri->intermediateAddress);
  assert(routerAddress.size() == destinationAddress->size() - 1);
  assert(routerAddress.size() == intermediateAddress->size() - 1);

  // update intermediate info for Valiant
  if (ri->intermediateDone == false) {
    if (randomGroup_ == true) {
      const std::vector<u32> intermediateGroup(intermediateAddress->begin() +
                      localDimWidths_.size() + 1, intermediateAddress->end());
      if (std::equal(routerAddress.begin() + localDimWidths_.size(),
                     routerAddress.end(), intermediateGroup.begin())) {
        ri->intermediateDone = true;
        packet->setRoutingExtension(ri);
      }
    } else {
      const std::vector<u32> intermediateNode(intermediateAddress->begin() +
                                          + 1, intermediateAddress->end());
      if (std::equal(routerAddress.begin(),
                     routerAddress.end(), intermediateNode.begin())) {
        ri->intermediateDone = true;
        packet->setRoutingExtension(ri);
      }
    }
  }

  std::unordered_set<u32> outputPorts;
  // first stage of valiant
  if (ri->intermediateDone == false) {
    outputPorts = DimOrderRoutingAlgorithm::routing(
                  _flit, intermediateAddress);
  } else {
    outputPorts = DimOrderRoutingAlgorithm::routing(
                  _flit, destinationAddress);
  }
  return outputPorts;
}

}  // namespace HierarchicalHyperX
