/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_ARBITERFACTORY_H_
#define ARBITER_ARBITERFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"

class ArbiterFactory {
 public:
  static Arbiter* createArbiter(
      const std::string& _name, const Component* _parent,
      u32 _size, Json::Value _settings);
};

#endif  // ARBITER_ARBITERFACTORY_H_
