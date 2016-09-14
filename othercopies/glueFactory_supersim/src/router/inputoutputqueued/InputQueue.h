/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_INPUTOUTPUTQUEUED_INPUTQUEUE_H_
#define ROUTER_INPUTOUTPUTQUEUED_INPUTQUEUE_H_

#include <prim/prim.h>

#include <queue>
#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"
#include "router/common/VcScheduler.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace InputOutputQueued {

class Router;

class InputQueue : public Component, public FlitReceiver,
                   public RoutingFunction::Client,
                   public VcScheduler::Client,
                   public CrossbarScheduler::Client {
 public:
  InputQueue(const std::string& _name, const Component* _parent,
             Router* _router, u32 _depth, u32 _port, u32 _numVcs, u32 _vc,
             RoutingFunction* _routingFunction, VcScheduler* _vcScheduler,
             u32 _vcSchedulerIndex, CrossbarScheduler* _crossbarScheduler,
             u32 _crossbarSchedulerIndex, Crossbar* _crossbar,
             u32 _crossbarIndex);
  ~InputQueue();

  // called by next higher router (FlitReceiver)
  void receiveFlit(u32 _port, Flit* _flit) override;

  // event system (Component)
  void processEvent(void* _event, s32 _type) override;

  // response from routing function
  void routingFunctionResponse(RoutingFunction::Response* _response) override;

  // response from VcScheduler
  void vcSchedulerResponse(u32 _vc) override;

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

  // external devices
  Router* router_;
  RoutingFunction* routingFunction_;
  VcScheduler* vcScheduler_;
  u32 vcSchedulerIndex_;
  CrossbarScheduler* crossbarScheduler_;
  u32 crossbarSchedulerIndex_;
  Crossbar* crossbar_;
  u32 crossbarIndex_;

  // single flit per clock input limit assurance
  u64 lastReceivedTime_;

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
  std::queue<Flit*> buffer_;

  // routing function execution [rfe_] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
    // results
    RoutingFunction::Response route;
  } rfe_;

  // VC allocation [vca] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
    RoutingFunction::Response route;
    // results
    u32 allocatedPort;
    u32 allocatedVc;
  } vca_;

  // Switch allocation [swa_] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
    u32 allocatedPort;
    u32 allocatedVc;
  } swa_;

  // Crossbar traversal [xtr_] stage (no state needed)
};

}  // namespace InputOutputQueued

#endif  // ROUTER_INPUTOUTPUTQUEUED_INPUTQUEUE_H_
