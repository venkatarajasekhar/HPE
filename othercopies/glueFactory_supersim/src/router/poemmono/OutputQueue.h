/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_OUTPUTQUEUE_H_
#define ROUTER_POEMMONO_OUTPUTQUEUE_H_

#include <prim/prim.h>

#include <string>
#include <queue>
#include <vector>

#include "event/Component.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"
#include "router/poemmono/CrossbarScheduler.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"

namespace PoemMono {

class Router;

class OutputQueue : public Component, public FlitReceiver,
                    public ::CrossbarScheduler::Client {
 public:
  OutputQueue(const std::string& _name, const Component* _parent,
              u32 _depth, u32 _port, u32 _vc,
              ::CrossbarScheduler* _outputCrossbarScheduler,
              u32 _crossbarSchedulerIndex,
              Crossbar* _crossbar, u32 _crossbarIndex,
              CrossbarScheduler* _mainCrossbarScheduler,
              u32 _mainCrossbarVcId);
  ~OutputQueue();

  // called by main router crossbar
  void receiveFlit(u32 _port, Flit* _flit) override;

  // event system (Component)
  void processEvent(void* _event, s32 _type) override;

  // response from ::CrossbarScheduler
  void crossbarSchedulerResponse(u32 _port, u32 _vc) override;

 private:
  void setPipelineEvent();
  void processPipeline();

  // attributes
  u32 depth_;
  u32 port_;
  u32 vc_;

  // external devices
  ::CrossbarScheduler* outputCrossbarScheduler_;
  u32 crossbarSchedulerIndex_;
  Crossbar* crossbar_;
  u32 crossbarIndex_;
  CrossbarScheduler* mainCrossbarScheduler_;
  u32 mainCrossbarVcId_;

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

  // Switch allocation [swa_] pipeline stage
  struct {
    ePipelineFsm fsm;
    Flit* flit;
    bool success;
  } swa_;

  // Crossbar traversal [xtr_] stage (no state needed)
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_OUTPUTQUEUE_H_
