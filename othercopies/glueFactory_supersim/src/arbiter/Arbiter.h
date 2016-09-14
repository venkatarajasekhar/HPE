/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_ARBITER_H_
#define ARBITER_ARBITER_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"

class Arbiter : public Component {
 public:
  // constructor
  Arbiter(const std::string& _name, const Component* _parent, u32 _size);
  virtual ~Arbiter();

  // returns number of inputs & outputs
  u32 size() const;
  // maps the request to the specified port
  void setRequest(u32 _port, const bool* _request);
  // maps the metadata to the specified port
  void setMetadata(u32 _port, const u64* _metadata);
  // maps the output to the specified port
  void setGrant(u32 _port, bool* _grant);

  // latches any state from the last cycle
  //  override when necessary
  virtual void latch();

  // computes the arbitration logic, must be overridden
  //  should only set grants true (logically OR)
  //  returns the winner, or U32_MAX when nothing granted
  virtual u32 arbitrate() = 0;

 protected:
  std::vector<const bool*> requests_;
  std::vector<const u64*> metadatas_;
  std::vector<bool*> grants_;
  const u32 size_;
};

#endif  // ARBITER_ARBITER_H_
