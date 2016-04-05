/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/Arbiter.h"

#include <cassert>

Arbiter::Arbiter(
    const std::string& _name, const Component* _parent, u32 _size)
    : Component(_name, _parent), size_(_size) {
  assert(size_ > 0);
  requests_.resize(size_, nullptr);
  metadatas_.resize(size_, nullptr);
  grants_.resize(size_, nullptr);
}

Arbiter::~Arbiter() {}

u32 Arbiter::size() const {
  return size_;
}

void Arbiter::setRequest(u32 _port, const bool* _request) {
  requests_.at(_port) = _request;
}

void Arbiter::setMetadata(u32 _port, const u64* _metadata) {
  metadatas_.at(_port) = _metadata;
}

void Arbiter::setGrant(u32 _port, bool* _grant) {
  grants_.at(_port) = _grant;
}

void Arbiter::latch() {}
