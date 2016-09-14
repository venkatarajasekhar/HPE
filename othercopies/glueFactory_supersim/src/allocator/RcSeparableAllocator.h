/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ALLOCATOR_RCSEPARABLEALLOCATOR_H_
#define ALLOCATOR_RCSEPARABLEALLOCATOR_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "allocator/Allocator.h"
#include "arbiter/Arbiter.h"
#include "event/Component.h"

class RcSeparableAllocator : public Allocator {
 public:
  RcSeparableAllocator(const std::string& _name, const Component* _parent,
                       u32 _numClients, u32 _numResources,
                       Json::Value _settings);
  ~RcSeparableAllocator();

  void setRequest(u32 _client, u32 _resource,
                  bool* _request) override;
  void setMetadata(u32 _client, u32 _resource,
                   u64* _metadata) override;
  void setGrant(u32 _client, u32 _resource, bool* _grant) override;

  void allocate() override;

 private:
  std::vector<Arbiter*> resourceArbiters_;
  std::vector<Arbiter*> clientArbiters_;

  bool** requests_;
  u64** metadatas_;
  bool* intermediates_;
  bool** grants_;

  u32 iterations_;
  bool slipLatch_;  // iSLIP selective priority latching

  u64 index(u64 _client, u64 _resource) const;
};

#endif  // ALLOCATOR_RCSEPARABLEALLOCATOR_H_
