/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <json/json.h>
#include <gtest/gtest.h>
#include <prim/prim.h>
#include <strop/strop.h>

#include <algorithm>
#include <string>
#include <tuple>

#include "event/Component.h"
#include "random/Random.h"
#include "router/common/CrossbarScheduler.h"

#include "test/TestSetup_TEST.h"

class CrossbarSchedulerTestClient : public CrossbarScheduler::Client,
                                    public Component {
 public:
  static const s32 RequestPortEvent = 123;
  static const s32 GiveBackCreditEvent = 456;
  enum class Fsm : u8 { REQUESTING = 1, WAITING = 2, DONE = 3 };

  CrossbarSchedulerTestClient(u32 _id, CrossbarScheduler* _xbarSch, u32 _numVcs,
                              u32 _numPorts, u32 _allocs)
      : Component("TestClient_" + std::to_string(_id), nullptr),
        id_(_id), xbarSch_(_xbarSch), numVcs_(_numVcs), numPorts_(_numPorts),
        fsm_(Fsm::REQUESTING),
        request_(std::make_tuple(U32_MAX, U32_MAX, U64_MAX)),
        remaining_(_allocs) {
    debug_ = !true;
    xbarSch_->setClient(id_, this);
    addEvent(gSim->time(), 1, nullptr, RequestPortEvent);
  }

  ~CrossbarSchedulerTestClient() {
    assert(remaining_ == 0);
    assert(fsm_ == Fsm::DONE);
  }

  void processEvent(void* _event, s32 _type) {
    u32* vcPtr;
    switch (_type) {
      case RequestPortEvent:
        requestPort();
        break;
      case GiveBackCreditEvent:
        vcPtr = reinterpret_cast<u32*>(_event);
        assert(*vcPtr < numVcs_);
        xbarSch_->incrementCreditCount(*vcPtr);
        delete vcPtr;
        break;
      default:
        assert(false);
    }
  }

  void requestPort() {
    assert(fsm_ == Fsm::REQUESTING);

    if (std::get<0>(request_) == U32_MAX) {
      // new request
      u32 vc = gRandom->randomU64(0, numVcs_ - 1);
      u32 port = vc / (numVcs_ / numPorts_);
      u64 metadata = gRandom->randomU64(1000, 2000 - 1);
      dbgprintf("requesting %u %u %lu", port, vc, metadata);
      request_ = std::make_tuple(port, vc, metadata);
      xbarSch_->request(id_, port, vc, metadata);
    } else {
      // re-request
      u32 port = std::get<0>(request_);
      u32 vc = std::get<1>(request_);
      u64 metadata = std::get<2>(request_);
      xbarSch_->request(id_, port, vc, metadata);
    }

    fsm_ = Fsm::WAITING;
  }

  void crossbarSchedulerResponse(u32 _port, u32 _vc) override {
    assert(remaining_ > 0);
    assert(std::get<0>(request_) != U32_MAX);

    if (_port != U32_MAX) {
      u32 port = std::get<0>(request_);
      u32 vc = std::get<1>(request_);
      std::get<0>(request_) = U32_MAX;

      assert(port == _port);
      assert(vc == _vc);

      // use credit
      dbgprintf("using %u (via port %u)", vc, port);
      xbarSch_->decrementCreditCount(vc);
      remaining_--;

      // release later
      u64 releaseTime = gSim->futureCycle(gRandom->randomU64(2, 6));
      u32* vcPtr = new u32;
      *vcPtr = vc;
      addEvent(releaseTime, 1, vcPtr, GiveBackCreditEvent);
    }

    if (remaining_ > 0) {
      addEvent(gSim->time(), gSim->epsilon() + 1, nullptr, RequestPortEvent);
      fsm_ = Fsm::REQUESTING;
    } else {
      fsm_ = Fsm::DONE;
    }
  }

 private:
  u32 id_;
  CrossbarScheduler* xbarSch_;
  u32 numVcs_;
  u32 numPorts_;
  Fsm fsm_;
  std::tuple<u32, u32, u64> request_;
  u32 remaining_;
};

TEST(CrossbarScheduler, basic) {
  const u32 ALLOCS_PER_CLIENT = 100;

  for (u32 C = 1; C < 16; C++) {
    for (u32 P = 1; P < 16; P++) {
      for (u32 Vd = 1; Vd < 4; Vd++) {
        u32 V = P * Vd;

        // setup
        TestSetup testSetup(12, 0x1234567890abcdf);
        Json::Value arbSettings;
        arbSettings["type"] = "random";
        Json::Value allocSettings;
        allocSettings["type"] = "r_separable";
        allocSettings["resource_arbiter"] = arbSettings;
        allocSettings["slip_latch"] = true;
        Json::Value schSettings;
        schSettings["allocator"] = allocSettings;
        CrossbarScheduler* xbarSch =
            new CrossbarScheduler("XbarSch", nullptr, C, V, P, schSettings);
        assert(xbarSch->numClients() == C);
        assert(xbarSch->numVcs() == V);
        assert(xbarSch->numPorts() == P);
        for (u32 v = 0; v < V; v++) {
          xbarSch->initCreditCount(v, 3);
        }

        std::vector<CrossbarSchedulerTestClient*> clients(C);
        for (u32 c = 0; c < C; c++) {
          clients[c] = new CrossbarSchedulerTestClient(c, xbarSch, V, P,
                                                       ALLOCS_PER_CLIENT);
        }

        // run the simulator
        gSim->simulate();

        // tear down
        delete xbarSch;
        for (u32 c = 0; c < C; c++) {
          delete clients[c];
        }
      }
    }
  }
}
