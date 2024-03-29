/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/inputoutputqueued/InputQueue.h"

#include <cassert>

#include <algorithm>

#include "router/inputoutputqueued/Router.h"
#include "types/Packet.h"

// event types
#define PROCESS_PIPELINE (0xB7)

namespace InputOutputQueued {

InputQueue::InputQueue(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _depth, u32 _port, u32 _numVcs, u32 _vc,
    RoutingFunction* _routingFunction, VcScheduler* _vcScheduler,
    u32 _vcSchedulerIndex, CrossbarScheduler* _crossbarScheduler,
    u32 _crossbarSchedulerIndex, Crossbar* _crossbar, u32 _crossbarIndex)
    : Component(_name, _parent), depth_(_depth), port_(_port), numVcs_(_numVcs),
      vc_(_vc), router_(_router), routingFunction_(_routingFunction),
      vcScheduler_(_vcScheduler),
      vcSchedulerIndex_(_vcSchedulerIndex),
      crossbarScheduler_(_crossbarScheduler),
      crossbarSchedulerIndex_(_crossbarSchedulerIndex),
      crossbar_(_crossbar), crossbarIndex_(_crossbarIndex),
      lastReceivedTime_(U64_MAX) {
  // ensure the buffer is empty
  assert(buffer_.size() == 0);

  // initialize the entry
  rfe_.fsm = ePipelineFsm::kEmpty;
  rfe_.flit = nullptr;
  rfe_.route.clear();

  vca_.fsm = ePipelineFsm::kEmpty;
  vca_.flit = nullptr;
  vca_.route.clear();
  vca_.allocatedPort = U32_MAX;
  vca_.allocatedVc = U32_MAX;

  swa_.fsm = ePipelineFsm::kEmpty;
  swa_.flit = nullptr;
  swa_.allocatedPort = U32_MAX;
  swa_.allocatedVc = U32_MAX;

  // no event is set to trigger
  eventTime_ = U64_MAX;
}

InputQueue::~InputQueue() {}

void InputQueue::receiveFlit(u32 _port, Flit* _flit) {
  // 'port' is unused
  assert(_port == 0);

  // make sure this is the right VC
  assert(_flit->getVc() == vc_);

  // we can only receive one flit per cycle
  assert((lastReceivedTime_ == U64_MAX) ||
         (lastReceivedTime_ < gSim->time()));
  lastReceivedTime_ = gSim->time();

  // push flit into corresponding buffer
  buffer_.push(_flit);
  assert(buffer_.size() <= depth_);  // overflow check

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::processEvent(void* _event, s32 _type) {
  switch (_type) {
    case (PROCESS_PIPELINE):
      assert(gSim->epsilon() == 2);
      processPipeline();
      break;

    default:
      assert(false);
  }
}

void InputQueue::routingFunctionResponse(RoutingFunction::Response* _response) {
  assert(rfe_.fsm == ePipelineFsm::kWaitingForResponse);
  rfe_.fsm = ePipelineFsm::kReadyToAdvance;

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::vcSchedulerResponse(u32 _vc) {
  assert(vca_.flit->isHead());
  assert(vca_.fsm == ePipelineFsm::kWaitingForResponse);

  if (_vc != U32_MAX) {
    // granted
    vca_.fsm = ePipelineFsm::kReadyToAdvance;
    router_->vcIndexRev(_vc, &vca_.allocatedPort, &vca_.allocatedVc);
    // dbgprintf("VcSch p=%u v=%u", vca_.allocatedPort, vca_.allocatedVc);
  } else {
    // denied
    vca_.fsm = ePipelineFsm::kWaitingToRequest;
  }

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::crossbarSchedulerResponse(u32 _port, u32 _vc) {
  assert(swa_.fsm == ePipelineFsm::kWaitingForResponse);

  if (_port != U32_MAX) {
    // granted
    swa_.fsm = ePipelineFsm::kReadyToAdvance;
  } else {
    // denied
    swa_.fsm = ePipelineFsm::kWaitingToRequest;
  }

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::setPipelineEvent() {
  if (eventTime_ == U64_MAX) {
    eventTime_ = gSim->time();
    addEvent(gSim->time(), 2, nullptr, PROCESS_PIPELINE);
  }
}

void InputQueue::processPipeline() {
  /*
   * attempt to load the crossbar
   */
  if (swa_.fsm == ePipelineFsm::kReadyToAdvance) {
    // dbgprintf("loading crossbar");

    // send the flit on the crossbar, consume a credit
    crossbar_->inject(swa_.flit, crossbarIndex_, swa_.allocatedPort);
    crossbarScheduler_->decrementCreditCount(swa_.allocatedVc);

    // if this is a tail flit, release the VC
    /** NOTE: this causes a stall when there are back-to-back
        packets in the same VC going to the same VC **/
    if (swa_.flit->isTail()) {
      vcScheduler_->releaseVc(swa_.allocatedVc);
    }

    // clear SWA info
    swa_.fsm = ePipelineFsm::kEmpty;
    swa_.flit = nullptr;
    swa_.allocatedPort = U32_MAX;
    swa_.allocatedVc = U32_MAX;
  }

  /*
   * attempt to load SWA stage
   */
  // ensure VCA is ready to advance
  if ((swa_.fsm == ePipelineFsm::kEmpty) &&
      (vca_.fsm == ePipelineFsm::kReadyToAdvance)) {
    // dbgprintf("loading SWA");

    // ensure SWA is empty
    assert(swa_.flit == nullptr);
    assert(swa_.allocatedPort == U32_MAX);
    assert(swa_.allocatedVc == U32_MAX);

    // set SWA info
    swa_.flit = vca_.flit;
    swa_.flit->setVc(vca_.allocatedVc);
    swa_.allocatedPort = router_->vcIndex(vca_.allocatedPort, vca_.allocatedVc);
    swa_.allocatedVc = swa_.allocatedPort;
    swa_.fsm = ePipelineFsm::kWaitingToRequest;

    // clear VCA info
    vca_.fsm = ePipelineFsm::kEmpty;
    vca_.flit = nullptr;
    vca_.route.clear();
    if (swa_.flit->isTail()) {
      // clear the allocated info only on tail flit
      vca_.allocatedPort = U32_MAX;
      vca_.allocatedVc = U32_MAX;
    }
  }

  /*
   * Attempt to submit a SWA request
   */
  if (swa_.fsm == ePipelineFsm::kWaitingToRequest) {
    crossbarScheduler_->request(
        crossbarSchedulerIndex_, swa_.allocatedPort, swa_.allocatedVc,
        swa_.flit->getPacket()->getDeadline());
    swa_.fsm = ePipelineFsm::kWaitingForResponse;
  }

  /*
   * attempt to load VCA stage
   */
  if ((vca_.fsm == ePipelineFsm::kEmpty) &&
      (rfe_.fsm == ePipelineFsm::kReadyToAdvance)) {
    // dbgprintf("loading VCA");

    // ensure VCA is empty
    assert(vca_.flit == nullptr);
    if (rfe_.flit->isHead()) {
      // U32_MAX means cleared
      assert(vca_.allocatedPort == U32_MAX);
      assert(vca_.allocatedVc == U32_MAX);
    } else {
      // these should still be valid from the head flit
      assert(vca_.allocatedPort != U32_MAX);
      assert(vca_.allocatedVc != U32_MAX);
    }

    // set VCA info
    vca_.flit = rfe_.flit;
    vca_.route = rfe_.route;
    if (vca_.flit->isHead()) {
      // dbgprintf("[VCA], head flit");
      vca_.fsm = ePipelineFsm::kWaitingToRequest;
    } else {
      // dbgprintf("[VCA], body flit");
      vca_.fsm = ePipelineFsm::kReadyToAdvance;
    }

    // clear RFE info
    rfe_.fsm = ePipelineFsm::kEmpty;
    rfe_.flit = nullptr;
    rfe_.route.clear();
  }

  /*
   * attempt to submit VCA requests
   */
  if (vca_.fsm == ePipelineFsm::kWaitingToRequest) {
    assert(vca_.flit->isHead());

    // set state machine as waiting for response from VC alloc
    vca_.fsm = ePipelineFsm::kWaitingForResponse;

    // request everything of the VC alloc
    u32 responseSize = vca_.route.size();
    assert(responseSize > 0);
    for (u32 r = 0; r < responseSize; r++) {
      u32 requestPort, requestVc;
      vca_.route.get(r, &requestPort, &requestVc);
      u32 vcIndex = router_->vcIndex(requestPort, requestVc);
      u32 metadata = vca_.flit->getPacket()->getDeadline();
      vcScheduler_->request(vcSchedulerIndex_, vcIndex, metadata);
    }
  }

  /*
   * attempt to load RFE stage
   */
  if ((rfe_.fsm == ePipelineFsm::kEmpty) && (buffer_.empty() == false)) {
    // dbgprintf("loading RFE");

    // ensure RFE is empty
    assert(rfe_.flit == nullptr);

    // pull out the front flit
    Flit* flit = buffer_.front();
    buffer_.pop();

    // put it in the routing pipeline stage
    assert(rfe_.flit == nullptr);
    rfe_.flit = flit;

    // send a credit back
    router_->sendCredit(port_, vc_);

    // set state as ready to request routing function
    rfe_.fsm = ePipelineFsm::kWaitingToRequest;
  }

  /*
   * attempt to submit a routing request
   */
  if (rfe_.fsm == ePipelineFsm::kWaitingToRequest) {
    // if this is a head flit, submit a routing request
    if (rfe_.flit->isHead()) {
      // dbgprintf("[RFE], head flit");

      // submit request
      routingFunction_->request(this, rfe_.flit, &rfe_.route);

      // set state machine
      rfe_.fsm = ePipelineFsm::kWaitingForResponse;
    } else {
      // not a head flit, set as ready to advance, queue event for this stage
      // dbgprintf("[RFE], body flit");
      rfe_.fsm = ePipelineFsm::kReadyToAdvance;
    }
  }

  // clear the eventTime_ variable to indicate no more events are set
  eventTime_ = U64_MAX;

  /*
   * there are a few reasons that the next cycle should be processed:
   *  1. VCA body flit, made progress, needs to continue
   *  2. RFE body flit, made progress, needs to continue
   *  3. more flits in the queue, need to pull one out
   * if any of these cases are true, create an event to handle the next cycle
   */
  if ((vca_.fsm == ePipelineFsm::kReadyToAdvance) ||     // body flit
      (rfe_.fsm == ePipelineFsm::kReadyToAdvance) ||    // body flit
      (buffer_.size() > 0)) {   // more flits in buffer
    // set a pipeline event for the next cycle
    eventTime_ = gSim->futureCycle(1);
    addEvent(eventTime_, 2, nullptr, PROCESS_PIPELINE);
  }
}

}  // namespace InputOutputQueued
