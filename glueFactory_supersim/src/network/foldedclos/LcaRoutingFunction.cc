/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/foldedclos/LcaRoutingFunction.h"

#include <cassert>
#include <vector>

#include "random/Random.h"
#include "types/Packet.h"
#include "types/Message.h"

namespace FoldedClos {

LcaRoutingFunction::LcaRoutingFunction(
    const std::string& _name, const Component* _parent, u64 _latency,
    Router* _router, u32 _numVcs, u32 _numPorts, u32 _numLevels, u32 _level,
    u32 _inputPort, bool _allVcs)
    : RoutingFunction(_name, _parent, _latency), router_(_router),
      numVcs_(_numVcs), numPorts_(_numPorts), numLevels_(_numLevels),
      level_(_level), inputPort_(_inputPort), allVcs_(_allVcs) {}

LcaRoutingFunction::~LcaRoutingFunction() {}

void LcaRoutingFunction::processRequest(Flit* _flit,
                                        RoutingFunction::Response* _response) {
  u32 outputPort;

  bool atTopLevel = (level_ == (numLevels_ - 1));
  bool movingUpward = (inputPort_ < (numPorts_ / 2));

  // up facing ports of top level are disconnected, make sure there is no funny
  //  business going on.
  assert(!(atTopLevel && !movingUpward));

  const std::vector<u32>& routerAddress = router_->getAddress();
  assert(routerAddress.size() == numLevels_);
  const std::vector<u32>* destinationAddress =
      _flit->getPacket()->getMessage()->getDestinationAddress();
  assert(destinationAddress->size() == numLevels_);

  // when moving up the tree, packets turn around at the least common router
  //  ancester
  if (movingUpward) {
    // determine if this router is an ancester of the destination
    bool isAncester = true;
    for (u32 i = 0; i < (numLevels_-level_-1); i++) {
      u32 index = numLevels_ - 1 - i;
      if (routerAddress.at(index) != destinationAddress->at(index)) {
        isAncester = false;
        break;
      }
    }

    if (isAncester) {
      // the packet needs to turn downward,
      //  let the downward logic below handle it
      movingUpward = false;
    } else {
      // the packet needs continue upward,
      //  just choose a random upward output port
      outputPort = gRandom->randomU64(numPorts_ / 2, numPorts_ - 1);
    }
  }

  // when moving down the tree, output port is a simple lookup into the
  //  destination address vector
  if (!movingUpward) {
    outputPort = destinationAddress->at(level_);
  }

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

}  // namespace FoldedClos
