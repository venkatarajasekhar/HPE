/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_COMPARINGARBITER_H_
#define ARBITER_COMPARINGARBITER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"
#include "event/Component.h"

// compares the metadata value, random tie-breaker
class ComparingArbiter : public Arbiter {
 public:
  ComparingArbiter(const std::string& _name, const Component* _parent,
                   u32 _size, Json::Value _settings);
  ~ComparingArbiter();

  u32 arbitrate() override;

 private:
  bool greater_;
};


#endif  // ARBITER_COMPARINGARBITER_H_
