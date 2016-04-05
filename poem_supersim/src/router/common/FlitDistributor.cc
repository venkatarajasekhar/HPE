/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/common/FlitDistributor.h"

#include <cassert>

FlitDistributor::FlitDistributor(
    const std::string& _name, const Component* _parent, u32 _outputs)
    : Component(_name, _parent) {
  receivers_.resize(_outputs, nullptr);
}

FlitDistributor::~FlitDistributor() {}

void FlitDistributor::setReceiver(u32 _vc, FlitReceiver* _receiver) {
  receivers_.at(_vc) = _receiver;
}

void FlitDistributor::receiveFlit(u32 _port, Flit* _flit) {
  assert(_port == 0);
  u32 vc = _flit->getVc();
  receivers_.at(vc)->receiveFlit(0, _flit);
}
