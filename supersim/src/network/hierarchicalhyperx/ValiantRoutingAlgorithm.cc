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

namespace HierarchicalHyperX {

ValiantRoutingAlgorithm::ValiantRoutingAlgorithm(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs,
    const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _concentration, u32 _globalLinksPerRouter)
  : DimOrderRoutingAlgorithm(_name, _parent,
    _latency, _router, _numVcs, _globalDimensionWidths,
    _globalDimensionWeights, _localDimensionWidths, _localDimensionWeights,
    _concentration, _globalLinksPerRouter) {
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

  // create the routing extension if needed
  if (packet->getRoutingExtension() == nullptr) {
    // should be first router encountered
    assert(packet->getHopCount() == 1);

    // create routing extension header
    //  the extension is a vector with one dummy element then the address of the
    //  intermediate router
    std::vector<u32>* re = new std::vector<u32>(1 + routerAddress.size());
    re->at(0) = U32_MAX;  // dummy
    packet->setRoutingExtension(re);

    // random intermediate address
    for (u32 idx = 0; idx < localDimWidths_.size(); idx++) {
      re->at(idx + 1) = gSim->rnd.nextU64(0, localDimWidths_.at(idx) - 1);
    }
    for (u32 idx = 0; idx < globalDimWidths_.size(); idx++) {
      re->at(idx + localDimWidths_.size() + 1) =
        gSim->rnd.nextU64(0, globalDimWidths_.at(idx) - 1);
    }
  }

  // intermediate address info
  const std::vector<u32>* intermediateAddress =
      reinterpret_cast<const std::vector<u32>*>(packet->getRoutingExtension());

  dbgprintf("VAL:Router address is %s \n",
         strop::vecString<u32>(routerAddress).c_str());
  dbgprintf("VAL: final dst address is %s \n",
         strop::vecString<u32>(*destinationAddress).c_str());
  dbgprintf("Intermediate address is %s \n",
         strop::vecString<u32>(*intermediateAddress).c_str());
  assert(routerAddress.size() == destinationAddress->size() - 1);
  assert(routerAddress.size() == intermediateAddress->size() - 1);

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
    delete intermediateAddress;
    packet->setRoutingExtension(nullptr);
  } else {
    // update intermediate info for Valiant
    const std::vector<u32> intermediateRouter(intermediateAddress->begin() +
      localDimWidths_.size() + 1, intermediateAddress->end());
    if (std::equal(routerAddress.begin() + localDimWidths_.size(),
                   routerAddress.end(), intermediateRouter.begin())) {
      _flit->setIntermediate(true);
    }
    std::unordered_set<u32> outputPorts;
    // first stage of valiant
    if (_flit->getIntermediateDone() == false) {
      outputPorts = DimOrderRoutingAlgorithm::routing(
                    _flit, intermediateAddress);
      assert(_flit->getGlobalHopCount() < globalDimWidths_.size() + 1);
     } else {
      outputPorts = DimOrderRoutingAlgorithm::routing(
                    _flit, destinationAddress);
     }

    assert(outputPorts.size() > 0);
    // figure out which VC set to use
    u32 vcSet = _flit->getGlobalHopCount();
    dbgprintf("using vcset %u \n", vcSet);
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

}  // namespace HierarchicalHyperX