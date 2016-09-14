/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_INPUTOUTPUTQUEUED_OUTPUTQUEUE_H_
#define ROUTER_INPUTOUTPUTQUEUED_OUTPUTQUEUE_H_

#include <prim/prim.h>

#include <string>
#include <queue>
#include <vector>

#include "event/Component.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace InputOutputQueued {

class Router;

class OutputQueue : public Component, public FlitReceiver,
                    public CrossbarScheduler::Client {
 public:
  OutputQueue(const std::string& _name, const Component* _parent,
              u32 _depth, u32 _port, u32 _vc,
              CrossbarScheduler* _outputCrossbarScheduler,
              u32 _crossbarSchedulerIndex,
              Crossbar* _crossbar, u32 _crossbarIndex,
              CrossbarScheduler* _mainCrossbarScheduler,
              u32 _mainCrossbarVcId);
  ~OutputQueue();

  // called by main router crossbar
  void receiveFlit(u32 _port, Flit* _flit) override;

  // event system (Component)
  void processEvent(void* _event, s32 _type) override;

  // response from CrossbarScheduler
  void crossbarSchedulerResponse(u32 _port, u32 _vc) override;

 private:
  void setPipelineEvent();
  void processPipeline();

  // attributes
  u32 depth_;
  u32 port_;
  u32 vc_;

  // external devices
  CrossbarScheduler* outputCrossbarScheduler_;
  u32 crossbarSchedulerIndex_;
  Crossbar* crossbar_;
  u32 crossbarIndex_;
  CrossbarScheduler* mainCrossbarScheduler_;
  u32 mainCrossbarVcId_;

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
  std::queue<Flit*> buffer_;  // insertion time & inserted flit

  // Switch allocation [swa_] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
  } swa_;

  // Crossbar traversal [xtr_] stage (no state needed)
};

}  // namespace InputOutputQueued

#endif  // ROUTER_INPUTOUTPUTQUEUED_OUTPUTQUEUE_H_
