/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemmono/HybridSpecCrossbarScheduler.h"

#include <cassert>
#include <cstring>

#include <utility>

#include "allocator/AllocatorFactory.h"

namespace PoemMono {

HybridSpecCrossbarScheduler::HybridSpecCrossbarScheduler(
    const std::string& _name, const Component* _parent, u32 _numClients,
    u32 _numVcs, u32 _numPorts, Json::Value _settings)
    : CrossbarScheduler(_name, _parent, _numClients, _numVcs,
                        _numPorts, _settings),
      maxSpec_(_settings["max_speculation"].asUInt()) {
  // create the credit counters
  credits_.resize(numVcs_, 0);
  maxCredits_.resize(numVcs_, 0);

  // create the speculation allowance counters
  specAllowance_.resize(numVcs_, maxSpec_);

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

HybridSpecCrossbarScheduler::~HybridSpecCrossbarScheduler() {
  for (u32 vc = 0; vc < numVcs_; vc++) {
    assert(credits_.at(vc) == maxCredits_.at(vc));
  }
  for (u32 vc = 0; vc < numVcs_; vc++) {
    assert(specAllowance_.at(vc) == maxSpec_);
  }

  delete[] requests_;
  delete[] metadatas_;
  delete[] grants_;
  delete allocator_;
}

void HybridSpecCrossbarScheduler::request(u32 _client, u32 _port, u32 _vc,
                                          u32 _metadata) {
  // validate inputs
  assert(gSim->epsilon() >= 1);
  assert(_client < numClients_);
  assert(_vc < numVcs_);
  assert(_port < numPorts_);

  // create pipeline
  u64 currentTime = gSim->time();
  u64 speculationTime = gSim->futureCycle(1);
  u64 allocationTime = gSim->futureCycle(latency_);
  Pipeline& pipeline = pipelines_[currentTime];

  // add input to future
  pipeline.requests.push_back({_client, _port, _vc, _metadata, true});
  pipeline.vcs.insert({_vc, false});

  // make sure there is a speculation event set
  u8& speculationTodo = events_[speculationTime];
  if (speculationTodo == NONE) {
    addEvent(speculationTime, 0, nullptr, 0);
  }
  speculationTodo |= SPECULATE;

  // make sure there is a allocation event set
  u8& allocationTodo = events_[allocationTime];
  if (allocationTodo == NONE) {
    addEvent(allocationTime, 0, nullptr, 0);
  }
  allocationTodo |= ALLOCATE;
}

void HybridSpecCrossbarScheduler::initCreditCount(u32 _vc, u32 _credits) {
  assert(_vc < numVcs_);
  credits_[_vc] = _credits;
  maxCredits_[_vc] = _credits;
}

void HybridSpecCrossbarScheduler::incrementCreditCount(u32 _vc) {
  assert(gSim->epsilon() >= 1);
  assert(_vc < numVcs_);

  // add increment value to VC
  incrCredits_[_vc]++;

  // make sure there is a credit increment event set
  u64 creditTime = gSim->futureCycle(1);
  u8& creditTodo = events_[creditTime];
  if (creditTodo == NONE) {
    addEvent(creditTime, 0, nullptr, 0);
  }
  creditTodo |= CREDITS;
}

u32 HybridSpecCrossbarScheduler::getCreditCount(u32 _vc) const {
  assert(_vc < numVcs_);
  return credits_[_vc];
}

void HybridSpecCrossbarScheduler::processEvent(void* _event, s32 _type) {
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

  // perform the speculation phase
  if ((todo & SPECULATE) != 0) {
    // dbgprintf("speculating");

    // retrieve the pipeline for this speculation time
    u64 startTime = gSim->time() - gSim->cycleTime();
    Pipeline& pipeline = pipelines_.at(startTime);

    // determine whether or not each VC is being speculated or not.
    //  if speculated, make sure there is enough speculation allowance left.
    for (auto it = pipeline.vcs.begin(); it != pipeline.vcs.end(); ) {
      const u32& vc = it->first;
      bool& spec = it->second;

      if (credits_[vc] > 0) {
        // enough credits, no speculation required
        //  allow requests to proceed unspeculatively
        spec = false;
        ++it;
      } else if (specAllowance_[vc] > 0) {
        // not enough credits, speculation required and allowance exists
        //  allow requests to proceed speculatively
        spec = true;
        specAllowance_[vc]--;
        ++it;
      } else {
        // not enough credits, speculation required but no allowance left
        //  block all requests
        it = pipeline.vcs.erase(it);
      }
    }

    // filter the requests individually
    for (auto it = pipeline.requests.begin();
         it != pipeline.requests.end(); ++it) {
      if (pipeline.vcs.count(it->vc) == 0) {
        // VC doesn't have enough credits or speculation allowance
        it->enable = false;
      }
    }
  }

  // if required, run the allocator
  if ((todo & ALLOCATE) != 0) {
    // dbgprintf("allocating");

    // retrieve the pipeline for this processing time
    u64 startTime = gSim->time() - (gSim->cycleTime() * latency_);
    Pipeline& pipeline = pipelines_.at(startTime);

    // apply all requests, check enable bit
    for (const Request& req : pipeline.requests) {
      assert(clientRequestPorts_[req.client] == U32_MAX);
      assert(clientRequestVcs_[req.client] == U32_MAX);
      clientRequestPorts_[req.client] = req.port;
      clientRequestVcs_[req.client] = req.vc;
      if (req.enable) {
        u64 idx = index(req.client, req.port);
        requests_[idx] = true;
        metadatas_[idx] = req.metadata;
      }
    }

    // clear the grants (must do before allocate() call)
    memset(grants_, false, sizeof(bool) * numClients_ * numPorts_);

    // run the allocator
    allocator_->allocate();

    // deliver responses, reset requests
    for (u32 c = 0; c < numClients_; c++) {
      if (clientRequestPorts_[c] != U32_MAX) {
        // get the client's original request
        u32 port = clientRequestPorts_[c];
        u32 vc = clientRequestVcs_[c];
        clientRequestPorts_[c] = U32_MAX;
        clientRequestVcs_[c] = U32_MAX;

        // check for a grant
        u64 idx = index(c, port);
        u32 granted = U32_MAX;
        if (grants_[idx]) {
          // grant was given, now check current credit count
          if (credits_[vc] == 0) {
            // no more credits, don't give grant
          } else {
            // credit available to consume, give grant
            granted = port;
            credits_[vc]--;
          }
        }
        requests_[idx] = false;

        clients_[c]->crossbarSchedulerResponse(granted, vc);
      }
    }

    // for all VC request batches that were speculative, give back the
    //  speculation allowance that was consumed
    for (const std::pair<u32, bool>& vcSpec : pipeline.vcs) {
      if (vcSpec.second) {
        specAllowance_[vcSpec.first]++;
      }
    }

    // remove the future
    u64 res = pipelines_.erase(startTime);
    (void)res;
    assert(res == 1);
  }
}

u64 HybridSpecCrossbarScheduler::index(u64 _client, u64 _port) const {
  // this indexing contiguously places resources
  return (numPorts_ * _client) + _port;
}

}  // namespace PoemMono
