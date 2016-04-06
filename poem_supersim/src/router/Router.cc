/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/Router.h"

#include <cassert>

Router::Router(const std::string& _name, const Component* _parent,
               Json::Value _settings)
    : Component(_name, _parent),
      numPorts_(_settings["num_ports"].asUInt()),
      numVcs_(_settings["num_vcs"].asUInt()) {
  assert(numPorts_ > 0);
  assert(numVcs_ > 0);
}

Router::~Router() {}

u32 Router::numPorts() const {
  return numPorts_;
}

u32 Router::numVcs() const {
  return numVcs_;
}

void Router::setAddress(const std::vector<u32>& _address) {
  address_ = _address;
}

const std::vector<u32>& Router::getAddress() const {
  return address_;
}
