/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_PRESPECCROSSBARSCHEDULER_H_
#define ROUTER_POEMMONO_PRESPECCROSSBARSCHEDULER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "allocator/Allocator.h"
#include "event/Component.h"
#include "router/poemmono/CrossbarScheduler.h"

namespace PoemMono {

class PreSpecCrossbarScheduler : public CrossbarScheduler {
 public:
  // constructor and destructor
  PreSpecCrossbarScheduler(
      const std::string& _name, const Component* _parent, u32 _numClients,
      u32 _numVcs, u32 _numPorts, Json::Value _settings);
  ~PreSpecCrossbarScheduler();

  // requests to send a flit to a VC
  void request(u32 _client, u32 _port, u32 _vc, u32 _metadata) override;

  // credit counts
  void initCreditCount(u32 _vc, u32 _credits) override;
  void incrementCreditCount(u32 _vc) override;
  u32 getCreditCount(u32 _vc) const override;

  // event processing
  void processEvent(void* _event, s32 _type) override;

 private:
  std::vector<u32> credits_;
  std::vector<u32> maxCredits_;
  std::unordered_map<u32, u32> incrCredits_;  // vc -> count

  struct Request {
    u32 client;
    u32 port;
    u32 vc;
    u64 metadata;
    bool enable;
  };
  struct Pipeline {
    std::vector<Request> requests;
    std::unordered_set<u32> vcs;
  };
  std::unordered_map<u64, Pipeline> pipelines_;

  bool* requests_;
  u64* metadatas_;
  u32* vcs_;
  bool* grants_;
  std::vector<u32> clientRequestPorts_;
  std::vector<u32> clientRequestVcs_;
  Allocator* allocator_;

  static const u8 NONE      = 0;       // nothing (yet)
  static const u8 CREDITS   = 1 << 0;  // increment credits from last cycle
  static const u8 SPECULATE = 1 << 1;  // speculate for future allocation
  static const u8 ALLOCATE  = 1 << 2;  // perform allocation, fix speculations
  std::unordered_map<u64, u8> events_;

  // this creates an index for requests_, metadatas_, vcs_, and grants_
  u64 index(u64 _client, u64 _port) const;
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_PRESPECCROSSBARSCHEDULER_H_
