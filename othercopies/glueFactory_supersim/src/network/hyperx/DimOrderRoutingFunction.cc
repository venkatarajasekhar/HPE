/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/hyperx/DimOrderRoutingFunction.h"

#include <cassert>

#include <unordered_set>

#include "types/Packet.h"
#include "types/Message.h"

namespace HyperX {

DimOrderRoutingFunction::DimOrderRoutingFunction(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs, const std::vector<u32>& _dimensionWidths,
    const std::vector<u32>& _dimensionWeights, u32 _concentration, bool _allVcs)
    : RoutingFunction(_name, _parent, _latency), router_(_router),
      numVcs_(_numVcs), dimensionWidths_(_dimensionWidths),
      dimensionWeights_(_dimensionWeights), concentration_(_concentration),
      allVcs_(_allVcs) {
}

DimOrderRoutingFunction::~DimOrderRoutingFunction() {}

void DimOrderRoutingFunction::processRequest(
    Flit* _flit, RoutingFunction::Response* _response) {
  std::unordered_set<u32> outputPorts;

  // ex: [x,y,z]
  const std::vector<u32>& routerAddress = router_->getAddress();
  // ex: [c,x,y,z]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();
  assert(routerAddress.size() == (destinationAddress->size() - 1));

  // determine the next dimension to work on
  u32 dim;
  u32 portBase = concentration_;
  for (dim = 0; dim < routerAddress.size(); dim++) {
    if (routerAddress.at(dim) != destinationAddress->at(dim+1)) {
      break;
    }
    portBase += ((dimensionWidths_.at(dim) - 1) * dimensionWeights_.at(dim));
  }

  // test if already at destination router
  if (dim == routerAddress.size()) {
    bool res = outputPorts.insert(destinationAddress->at(0)).second;
    (void)res;
    assert(res);
  } else {
    // more router-to-router hops needed
    u32 src = routerAddress.at(dim);
    u32 dst = destinationAddress->at(dim+1);
    if (dst < src) {
      dst += dimensionWidths_.at(dim);
    }
    u32 offset = (dst - src - 1) * dimensionWeights_.at(dim);
    // add all ports where the two routers are connecting
    for (u32 weight = 0; weight < dimensionWeights_.at(dim); weight++) {
      bool res = outputPorts.insert(portBase + offset + weight).second;
      (void)res;
      assert(res);
    }
  }

  assert(outputPorts.size() > 0);
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

}  // namespace HyperX
