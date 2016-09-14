/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/uno/DirectRoutingFunction.h"

#include <strop/strop.h>

#include <cassert>

#include "types/Packet.h"
#include "types/Message.h"

namespace Uno {

DirectRoutingFunction::DirectRoutingFunction(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs, u32 _concentration, bool _allVcs)
    : RoutingFunction(_name, _parent, _latency), router_(_router),
      numVcs_(_numVcs), concentration_(_concentration), allVcs_(_allVcs) {
}

DirectRoutingFunction::~DirectRoutingFunction() {}

void DirectRoutingFunction::processRequest(
    Flit* _flit, RoutingFunction::Response* _response) {
  // direct route to destination
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();
  u32 outputPort = destinationAddress->at(0);
  assert(outputPort < concentration_);

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

}  // namespace Uno
