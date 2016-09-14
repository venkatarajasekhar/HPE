/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "allocator/AllocatorFactory.h"

#include <cassert>

#include "allocator/CrSeparableAllocator.h"
#include "allocator/RcSeparableAllocator.h"
#include "allocator/RSeparableAllocator.h"

Allocator* AllocatorFactory::createAllocator(
    const std::string& _name, const Component* _parent,
    u32 _numClients, u32 _numResources, Json::Value _settings) {
  std::string type = _settings["type"].asString();

  if (type == "cr_separable") {
    return new CrSeparableAllocator(_name, _parent, _numClients, _numResources,
                                    _settings);
  } else if (type == "rc_separable") {
    return new RcSeparableAllocator(_name, _parent, _numClients, _numResources,
                                    _settings);
  } else if (type == "r_separable") {
    return new RSeparableAllocator(_name, _parent, _numClients, _numResources,
                                   _settings);
  } else {
    fprintf(stderr, "unknown Allocator 'type': %s\n", type.c_str());
    assert(false);
  }
}
