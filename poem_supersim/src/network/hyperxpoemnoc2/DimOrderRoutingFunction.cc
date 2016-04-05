/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/hyperxpoemnoc2/DimOrderRoutingFunction.h"

#include <strop/strop.h>

#include <cassert>

#include <unordered_set>

#include "types/Packet.h"
#include "types/Message.h"

namespace HyperXPoemNoc2 {

DimOrderRoutingFunction::DimOrderRoutingFunction(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs, const std::vector<u32>& _dimensionWidths,
    const std::vector<u32>& _dimensionWeights, u32 _concentration,
    u32 _nocWidth, u32 _nocWeight, u32 _nocConcentration, bool _allVcs)
    : RoutingFunction(_name, _parent, _latency), router_(_router),
      numVcs_(_numVcs), dimensionWidths_(_dimensionWidths),
      dimensionWeights_(_dimensionWeights), concentration_(_concentration),
      nocWidth_(_nocWidth), nocWeight_(_nocWeight),
      nocConcentration_(_nocConcentration), allVcs_(_allVcs) {
}

DimOrderRoutingFunction::~DimOrderRoutingFunction() {}

void DimOrderRoutingFunction::processRequest(
    Flit* _flit, RoutingFunction::Response* _response) {
  // ex: [n,x,y,z]
  const std::vector<u32>& nocRouterAddress = router_->getAddress();
  // ex: [c,x,y,z]
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();
  assert(nocRouterAddress.size() == destinationAddress->size());

  // determine the next dimension to work on
  u32 dim;
  u32 portBase = concentration_;
  for (dim = 0; dim < nocRouterAddress.size() - 1; dim++) {
    if (nocRouterAddress.at(dim+1) != destinationAddress->at(dim+1)) {
      break;
    }
    portBase += ((dimensionWidths_.at(dim) - 1) * dimensionWeights_.at(dim));
  }

  // first perform routing at the NOC level
  std::unordered_set<u32> nocOutputPorts;

  // test if already at destination NOC
  if (dim == dimensionWidths_.size()) {
    bool res = nocOutputPorts.insert(destinationAddress->at(0)).second;
    (void)res;
    assert(res);
  } else {
    // more router-to-router hops needed
    u32 src = nocRouterAddress.at(dim+1);
    u32 dst = destinationAddress->at(dim+1);
    if (dst < src) {
      dst += dimensionWidths_.at(dim);
    }
    u32 offset = (dst - src - 1) * dimensionWeights_.at(dim);

    // add all ports where the two routers are connecting
    for (u32 weight = 0; weight < dimensionWeights_.at(dim); weight++) {
      u32 port = portBase + offset + weight;
      bool res = nocOutputPorts.insert(port).second;
      (void)res;
      assert(res);
    }
  }
  assert(nocOutputPorts.size() > 0);

  // now that routing is done at the NOC level, translate that into NOC hops
  std::unordered_set<u32> outputPorts;

  // find the minimum amount of hops across the NOC
  u32 minimumNocHops = U32_MAX;
  for (u32 nocOutputPort : nocOutputPorts) {
    u32 nocDestinationRouterIndex = nocOutputPort % nocWidth_;
    u32 nocHops = (nocDestinationRouterIndex == nocRouterAddress.at(0)) ? 0 : 1;
    if (nocHops < minimumNocHops) {
      minimumNocHops = nocHops;
    }
  }

  // select all minimal paths across the NOC
  for (u32 nocOutputPort : nocOutputPorts) {
    u32 nocDestinationRouterIndex = nocOutputPort % nocWidth_;
    u32 nocDestinationRouterPort = nocOutputPort / nocWidth_;
    u32 nocHops = (nocDestinationRouterIndex == nocRouterAddress.at(0)) ? 0 : 1;
    if (nocHops == minimumNocHops) {
      // test if already at NOC output router
      if (nocDestinationRouterIndex == nocRouterAddress.at(0)) {
        // add single output link
        bool res = outputPorts.insert(nocDestinationRouterPort).second;
        (void)res;
        assert(res);
      } else {
        // a NOC router to NOC router hop is needed
        u32 src = nocRouterAddress.at(0);
        u32 dst = nocDestinationRouterIndex;
        if (dst < src) {
          dst += nocWidth_;
        }
        u32 offset = (dst - src - 1) * nocWeight_;

        // add all ports where the two routers are connecting
        for (u32 weight = 0; weight < nocWeight_; weight++) {
          u32 port = nocConcentration_ + offset + weight;
          bool res = outputPorts.insert(port).second;
          (void)res;
          assert(res);
        }
      }
    }
  }
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

}  // namespace HyperXPoemNoc2
