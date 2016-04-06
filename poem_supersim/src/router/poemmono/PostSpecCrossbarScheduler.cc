/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemmono/PostSpecCrossbarScheduler.h"

#include <cassert>
#include <cstring>

#include "allocator/AllocatorFactory.h"

namespace PoemMono {

PostSpecCrossbarScheduler::PostSpecCrossbarScheduler(
    const std::string& _name, const Component* _parent, u32 _numClients,
    u32 _numVcs, u32 _numPorts, Json::Value _settings)
    : CrossbarScheduler(_name, _parent, _numClients, _numVcs,
                        _numPorts, _settings) {
  // create the credit counters
  credits_.resize(numVcs_, 0);
  maxCredits_.resize(numVcs_, 0);

  // create arrays for allocator inputs and outputs
  requests_ = new bool[numPorts_ * numClients_];
  memset(requests_, 0, sizeof(bool) * numPorts_ * numClients_);
  metadatas_ = new u64[numPorts_ * numClients_];
  grants_ = new bool[numPorts_ * numClients_];

  // create client requested flags
  clients_.resize(numClients_, nullptr);
  clientRequestPorts_.resize(numClients_, U32_MAX);
  clientRequestVcs_.resize(numClients_, U32_MAX);

  // create the allocator
  allocator_ = AllocatorFactory::createAllocator(
      "Allocator", this, numClients_, numPorts_, _settings["allocator"]);

  // map inputs and outputs to allocator
  for (u32 c = 0; c < numClients_; c++) {
    for (u32 p = 0; p < numPorts_; p++) {
      allocator_->setRequest(c, p, &requests_[index(c, p)]);
      allocator_->setMetadata(c, p, &metadatas_[index(c, p)]);
      allocator_->setGrant(c, p, &grants_[index(c, p)]);
    }
  }
}

PostSpecCrossbarScheduler::~PostSpecCrossbarScheduler() {
  for (u32 vc = 0; vc < numVcs_; vc++) {
    assert(credits_.at(vc) == maxCredits_.at(vc));
  }

  delete[] requests_;
  delete[] metadatas_;
  delete[] grants_;
  delete allocator_;
}

void PostSpecCrossbarScheduler::request(u32 _client, u32 _port, u32 _vc,
                                        u32 _metadata) {
  // validate inputs
  assert(gSim->epsilon() >= 1);
  assert(_client < numClients_);
  assert(_vc < numVcs_);
  assert(_port < numPorts_);

  // create pipeline
  u64 currentTime = gSim->time();
  u64 allocationTime = gSim->futureCycle(latency_);
  Pipeline& pipeline = pipelines_[currentTime];

  // add input to future
  pipeline.requests.push_back({_client, _port, _vc, _metadata});

  // make sure there is a allocation event set
  u8& allocationTodo = events_[allocationTime];
  if (allocationTodo == NONE) {
    // dbgprintf("adding allocation event for %lu", allocationTime);
    addEvent(allocationTime, 0, nullptr, 0);
  }
  allocationTodo |= ALLOCATE;
}

void PostSpecCrossbarScheduler::initCreditCount(u32 _vc, u32 _credits) {
  assert(_vc < numVcs_);
  credits_[_vc] = _credits;
  maxCredits_[_vc] = _credits;
}

void PostSpecCrossbarScheduler::incrementCreditCount(u32 _vc) {
  assert(gSim->epsilon() >= 1);
  assert(_vc < numVcs_);

  // add increment value to VC
  incrCredits_[_vc]++;

  // make sure there is a credit increment event set
  u64 creditTime = gSim->futureCycle(1);
  u8& creditTodo = events_[creditTime];
  if (creditTodo == NONE) {
    // dbgprintf("adding credit event for %lu", creditTime);
    addEvent(creditTime, 0, nullptr, 0);
  }
  creditTodo |= CREDITS;
}

u32 PostSpecCrossbarScheduler::getCreditCount(u32 _vc) const {
  assert(_vc < numVcs_);
  return credits_[_vc];
}

void PostSpecCrossbarScheduler::processEvent(void* _event, s32 _type) {
  assert(gSim->epsilon() == 0);

  // figure out what needs to be done on this cycle
  u64 now = gSim->time();
  u8 todo = events_.at(now);
  u64 res = events_.erase(now);
  (void)res;
  assert(res == 1);
  assert(todo != NONE);

  // apply all credit incrementations needed
  if ((todo & CREDITS) != 0) {
    // dbgprintf("incrementing credit counts");
    for (auto it = incrCredits_.cbegin(); it != incrCredits_.cend(); ++it) {
      u32 vc = it->first;
      u32 incr = it->second;
      assert(vc < numVcs_);
      credits_[vc] += incr;
      assert(credits_[vc] <= maxCredits_[vc]);
    }
    incrCredits_.clear();
  }
  assert(incrCredits_.size() == 0);

  // if required, run the allocator
  if ((todo & ALLOCATE) != 0) {
    // dbgprintf("allocating");

    // retrieve the pipeline for this processing time
    u64 startTime = gSim->time() - (gSim->cycleTime() * latency_);
    Pipeline& pipeline = pipelines_.at(startTime);

    // apply all requests
    for (const Request& req : pipeline.requests) {
      assert(clientRequestPorts_[req.client] == U32_MAX);
      assert(clientRequestVcs_[req.client] == U32_MAX);
      clientRequestPorts_[req.client] = req.port;
      clientRequestVcs_[req.client] = req.vc;
      u64 idx = index(req.client, req.port);
      requests_[idx] = true;
      metadatas_[idx] = req.metadata;
    }

    // clear the grants (must do before allocate() call)
    memset(grants_, false, sizeof(bool) * numClients_ * numPorts_);

    // run the allocator
    allocator_->allocate();

    // deliver responses, reset requests
    for (u32 c = 0; c < numClients_; c++) {
      if (clientRequestPorts_[c] != U32_MAX) {
        u32 port = clientRequestPorts_[c];
        clientRequestPorts_[c] = U32_MAX;
        u32 vc = clientRequestVcs_[c];
        clientRequestVcs_[c] = U32_MAX;
        u64 idx = index(c, port);

        u32 granted = U32_MAX;
        if (grants_[idx] && credits_[vc] == 0) {
          // printf("%s no credits\n", fullName().c_str());
        }
        if (grants_[idx] && credits_[vc] > 0) {
          granted = port;
          assert(credits_[vc] > 0);
          credits_[vc]--;
        }
        requests_[idx] = false;

        clients_[c]->crossbarSchedulerResponse(granted, vc);
      }
    }

    // remove the future
    u64 res = pipelines_.erase(startTime);
    (void)res;
    assert(res == 1);
  }
}

u64 PostSpecCrossbarScheduler::index(u64 _client, u64 _port) const {
  // this indexing contiguously places resources
  return (numPorts_ * _client) + _port;
}

}  // namespace PoemMono
