/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ALLOCATOR_ALLOCATOR_H_
#define ALLOCATOR_ALLOCATOR_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"
#include "event/Component.h"

class Allocator : public Component {
 public:
  Allocator(const std::string& _name, const Component* _parent,
            u32 _numClients, u32 _numResources);
  virtual ~Allocator();

  // returns number of clients
  u32 numClients() const;
  // returns number of resources
  u32 numResources() const;
  // maps the request to the specified client and resource
  virtual void setRequest(u32 _client, u32 _resource,
                          bool* _request) = 0;
  // maps the metadata to the specified client and resource
  virtual void setMetadata(u32 _client, u32 _resource,
                           u64* _metadata) = 0;
  // maps the grant to the specified client and resource
  virtual void setGrant(u32 _client, u32 _resource, bool* _grant) = 0;

  // computes the allocation logic, must be overridden
  //  should only set grants true (logically OR)
  virtual void allocate() = 0;

 protected:
  const u32 numClients_;
  const u32 numResources_;
};

#endif  // ALLOCATOR_ALLOCATOR_H_
