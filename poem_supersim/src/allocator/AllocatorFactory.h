/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ALLOCATOR_ALLOCATORFACTORY_H_
#define ALLOCATOR_ALLOCATORFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "allocator/Allocator.h"

class AllocatorFactory {
 public:
  static Allocator* createAllocator(
      const std::string& _name, const Component* _parent,
      u32 _numClients, u32 _numResources, Json::Value _settings);
};

#endif  // ALLOCATOR_ALLOCATORFACTORY_H_
