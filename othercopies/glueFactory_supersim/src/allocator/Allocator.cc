/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "allocator/Allocator.h"

#include <cassert>

Allocator::Allocator(const std::string& _name, const Component* _parent,
                     u32 _numClients, u32 _numResources)
    : Component(_name, _parent), numClients_(_numClients),
      numResources_(_numResources) {
  assert(numClients_ > 0);
  assert(numResources_ > 0);
}

Allocator::~Allocator() {}

u32 Allocator::numClients() const {
  return numClients_;
}

u32 Allocator::numResources() const {
  return numResources_;
}
