/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_INPUTQUEUE_H_
#define ROUTER_POEMMONO_INPUTQUEUE_H_

#include <prim/prim.h>

#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/common/Crossbar.h"
#include "router/poemmono/CrossbarScheduler.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace PoemMono {

class Router;

class InputQueue : public Component, public FlitReceiver,
                   public RoutingFunction::Client,
                   public CrossbarScheduler::Client {
 public:
  InputQueue(const std::string& _name, const Component* _parent,
             Router* _router, u32 _depth, u32 _port, u32 _numVcs, u32 _vc,
             bool _outputSpeedup, RoutingFunction* _routingFunction,
             CrossbarScheduler* _crossbarScheduler,
             u32 _crossbarSchedulerIndex, Crossbar* _crossbar,
             u32 _crossbarIndex);
  ~InputQueue();

  // called by next higher router (FlitReceiver)
  void receiveFlit(u32 _port, Flit* _flit) override;

  // event system (Component)
  void processEvent(void* _event, s32 _type) override;

  // response from routing function
  void routingFunctionResponse(RoutingFunction::Response* _response) override;

  // response from CrossbarScheduler
  void crossbarSchedulerResponse(u32 _port, u32 _vc) override;

 private:
  void setPipelineEvent();
  void processPipeline();

  // attributes
  u32 depth_;
  u32 port_;
  u32 numVcs_;  // in system, not this module
  u32 vc_;
  bool outputSpeedup_;

  // external devices
  Router* router_;
  RoutingFunction* routingFunction_;
  CrossbarScheduler* crossbarScheduler_;
  u32 crossbarSchedulerIndex_;
  Crossbar* crossbar_;
  u32 crossbarIndex_;

  // check buffer input bandwidth
  u64 lastReceivedTime_;
  u32 lastReceivedCount_;

  // state machine to represent a generic pipeline stage
  enum class ePipelineFsm {
    kEmpty,
    kWaitingToRequest,
    kWaitingForResponse,
    kReadyToAdvance
  };

  // remembers if an event is set to process the pipeline
  u64 eventTime_;

  // The following variables represent the pipeline registers

  // buffer
  class DeadlineComparator {
   public:
    DeadlineComparator();
    ~DeadlineComparator();
    bool operator()(const Flit* _lhs, const Flit* _rhs) const;
  };
  std::priority_queue<Flit*, std::vector<Flit*>, DeadlineComparator> buffer_;

  // routing function execution [rfe_] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
    // results
    RoutingFunction::Response route;
  } rfe_;

  // Switch allocation [swa_] pipeline stage
  struct Swa {
    ePipelineFsm fsm;
    Flit* flit;
    u32 outputPort;
    u32 outputVc;
    u32 outputVcId;
    bool success;
  };
  std::vector<Swa> swa_;

  // Crossbar traversal [xtr_] stage (no state needed)
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_INPUTQUEUE_H_
