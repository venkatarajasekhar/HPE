/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_RANDOMARBITER_H_
#define ARBITER_RANDOMARBITER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"
#include "event/Component.h"

// compares the metadata value, random tie-breaker
class RandomArbiter : public Arbiter {
 public:
  RandomArbiter(const std::string& _name, const Component* _parent,
                u32 _size, Json::Value _settings);
  ~RandomArbiter();

  u32 arbitrate() override;
};


#endif  // ARBITER_RANDOMARBITER_H_
