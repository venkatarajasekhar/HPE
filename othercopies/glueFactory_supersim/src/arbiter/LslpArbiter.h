/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_LSLPARBITER_H_
#define ARBITER_LSLPARBITER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"
#include "event/Component.h"

// last served -> lowest priority (LSLP) aka round-robin
class LslpArbiter : public Arbiter {
 public:
  LslpArbiter(const std::string& _name, const Component* _parent,
              u32 _size, Json::Value _settings);
  ~LslpArbiter();

  void latch() override;
  u32 arbitrate() override;

 private:
  u32 priority_;
  u32 prevPriority_;
};

#endif  // ARBITER_LSLPARBITER_H_
