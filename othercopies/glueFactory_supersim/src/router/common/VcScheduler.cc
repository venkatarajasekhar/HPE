/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/common/VcScheduler.h"

#include <cassert>
#include <cstring>

#include "allocator/AllocatorFactory.h"

VcScheduler::Client::Client() {}

VcScheduler::Client::~Client() {}

const s32 kAllocEvent = 123;

VcScheduler::VcScheduler(const std::string& _name, const Component* _parent,
                         u32 _numClients, u32 _numVcs, Json::Value _settings)
    : Component(_name, _parent), numClients_(_numClients), numVcs_(_numVcs) {
  assert(numClients_ > 0 && numClients_ != U32_MAX);
  assert(numVcs_ > 0 && numVcs_ != U32_MAX);

  // create Client pointers and requested flags
  clients_.resize(numClients_, nullptr);
  clientRequested_.resize(numClients_, false);

  // create the VC used flags
  vcTaken_.resize(numVcs_, false);

  // create arrays for allocator inputs and outputs
  requests_ = new bool[numVcs_ * numClients_];
  memset(requests_, 0, sizeof(bool) * numVcs_ * numClients_);
  metadatas_ = new u64[numVcs_ * numClients_];
  grants_ = new bool[numVcs_ * numClients_];

  // create the allocator
  allocator_ = AllocatorFactory::createAllocator(
      "Allocator", this, numClients_, numVcs_, _settings["allocator"]);

  // map inputs and outputs to allocator
  for (u32 c = 0; c < numClients_; c++) {
    for (u32 v = 0; v < numVcs_; v++) {
      allocator_->setRequest(c, v, &requests_[index(c, v)]);
      allocator_->setMetadata(c, v, &metadatas_[index(c, v)]);
      allocator_->setGrant(c, v, &grants_[index(c, v)]);
    }
  }

  // initialize state variables
  allocEventSet_ = false;
}

VcScheduler::~VcScheduler() {
  delete[] requests_;
  delete[] metadatas_;
  delete[] grants_;
  delete allocator_;
}

u32 VcScheduler::numClients() const {
  return numClients_;
}

u32 VcScheduler::numVcs() const {
  return numVcs_;
}

void VcScheduler::setClient(u32 _id, Client* _client) {
  assert(clients_.at(_id) == nullptr);
  clients_.at(_id) = _client;
}

void VcScheduler::request(u32 _client, u32 _vc, u64 _metadata) {
  assert(gSim->epsilon() >= 1);
  assert(_client < numClients_);
  assert(_vc < numVcs_);

  // set the request
  u64 idx = index(_client, _vc);
  requests_[idx] = true;
  metadatas_[idx] = _metadata;
  clientRequested_[_client] = true;

  // ensure there is an event set to perform scheduling
  if (!allocEventSet_) {
    allocEventSet_ = true;
    addEvent(gSim->futureCycle(1), 0, nullptr, kAllocEvent);
  }
}

void VcScheduler::releaseVc(u32 _vc) {
  assert(gSim->epsilon() >= 1);
  assert(vcTaken_.at(_vc) == true);
  vcTaken_.at(_vc) = false;
}

void VcScheduler::processEvent(void* _event, s32 _type) {
  assert(_type == kAllocEvent);
  assert(gSim->epsilon() == 0);
  allocEventSet_ = false;

  // check VC availability, make out unavailable VC requests
  for (u32 c = 0; c < numClients_; c++) {
    if (clientRequested_[c]) {
      for (u32 v = 0; v < numVcs_; v++) {
        u64 idx = index(c, v);
        if (requests_[idx] && vcTaken_[v]) {
          requests_[idx] = false;
        }
      }
    }
  }

  // clear the grants (must do before allocate() call)
  memset(grants_, false, sizeof(bool) * numVcs_ * numClients_);

  // run the allocator
  allocator_->allocate();

  // deliver responses, mark used VCs, reset requests
  for (u32 c = 0; c < numClients_; c++) {
    if (clientRequested_[c]) {
      clientRequested_[c] = false;
      u32 granted = U32_MAX;
      for (u32 v = 0; v < numVcs_; v++) {
        u64 idx = index(c, v);
        if ((granted == U32_MAX) && (grants_[idx])) {
          granted = v;
          assert(vcTaken_[v] == false);
          vcTaken_[v] = true;
        } else {
          // multiple grants to the same client? BAD
          assert(!((granted != U32_MAX) && (grants_[idx])));
        }
        requests_[idx] = false;
      }
      clients_[c]->vcSchedulerResponse(granted);
    }
  }
}

u64 VcScheduler::index(u64 _client, u64 _vc) const {
  // this indexing contiguously places resources
  return (numVcs_ * _client) + _vc;
}
