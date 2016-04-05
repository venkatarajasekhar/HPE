/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemnoc2/InputQueue.h"

#include <cassert>

#include <algorithm>

#include "random/Random.h"
#include "router/poemnoc2/Router.h"
#include "types/Packet.h"
#include "types/Message.h"

namespace PoemNoc2 {

InputQueue::InputQueue(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _depth, u32 _port, u32 _numVcs, u32 _vc, bool _outputSpeedup,
    RoutingFunction* _routingFunction,
    CrossbarScheduler* _crossbarScheduler,
    u32 _crossbarSchedulerIndex, Crossbar* _crossbar, u32 _crossbarIndex)
    : Component(_name, _parent), depth_(_depth), port_(_port), numVcs_(_numVcs),
      vc_(_vc), outputSpeedup_(_outputSpeedup), router_(_router),
      routingFunction_(_routingFunction),
      crossbarScheduler_(_crossbarScheduler),
      crossbarSchedulerIndex_(_crossbarSchedulerIndex),
      crossbar_(_crossbar), crossbarIndex_(_crossbarIndex),
      lastReceivedTime_(0), lastReceivedCount_(0) {
  // ensure the buffer is empty
  assert(buffer_.size() == 0);

  rfe_.fsm = ePipelineFsm::kEmpty;
  rfe_.flit = nullptr;
  rfe_.route.clear();

  swa_.fsm = ePipelineFsm::kEmpty;
  swa_.flit = nullptr;
  swa_.outputPort = U32_MAX;
  swa_.outputVc = U32_MAX;
  swa_.outputVcId = U32_MAX;

  eventTime_ = U64_MAX;
}

InputQueue::~InputQueue() {}

InputQueue::DeadlineComparator::DeadlineComparator() {}

InputQueue::DeadlineComparator::~DeadlineComparator() {}

bool InputQueue::DeadlineComparator::operator()(
    const Flit* _lhs, const Flit* _rhs) const {
  return (_lhs->getPacket()->getDeadline() >
          _rhs->getPacket()->getDeadline());
}

void InputQueue::receiveFlit(u32 _port, Flit* _flit) {
  /*dbgprintf("received flit (msgId=%u)",
    _flit->getPacket()->getMessage()->getId());*/

  assert(_flit->isHead() && _flit->isTail());

  // 'port' is unused
  assert(_port == 0);

  // make sure this is the right VC
  assert(_flit->getVc() == vc_);

  // record and check the buffer input bandwidth
  u64 now = gSim->time();
  if (lastReceivedTime_ < now) {
    lastReceivedTime_ = now;
    lastReceivedCount_ = 0;
  }
  lastReceivedCount_++;
  assert(lastReceivedCount_ <= 2);

  // push flit into corresponding buffer
  buffer_.push(_flit);
  assert(buffer_.size() <= depth_);  // overflow check

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::processEvent(void* _event, s32 _type) {
  processPipeline();
}

void InputQueue::routingFunctionResponse(RoutingFunction::Response* _response) {
  assert(rfe_.fsm == ePipelineFsm::kWaitingForResponse);
  rfe_.fsm = ePipelineFsm::kReadyToAdvance;

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::crossbarSchedulerResponse(u32 _port, u32 _vc) {
  assert(swa_.fsm == ePipelineFsm::kWaitingForResponse);
  swa_.fsm = ePipelineFsm::kReadyToAdvance;
  swa_.success = (_port != U32_MAX);
  if (swa_.success) {
    assert(_vc == swa_.outputVcId);
  }

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void InputQueue::setPipelineEvent() {
  if (eventTime_ == U64_MAX) {
    if (gSim->epsilon() < 2) {
      eventTime_ = gSim->time();
    } else {
      eventTime_ = gSim->futureCycle(1);
    }
    addEvent(eventTime_, 2, nullptr, 0);
  }
}

void InputQueue::processPipeline() {
  assert(gSim->epsilon() == 2);

  // clear the eventTime_ variable to indicate no more events are set
  eventTime_ = U64_MAX;

  /*
   * attempt to load the crossbar from the head of the SWA pipeline
   */
  assert(swa_.fsm != ePipelineFsm::kWaitingToRequest);
  assert(swa_.fsm != ePipelineFsm::kWaitingForResponse);
  if (swa_.fsm == ePipelineFsm::kReadyToAdvance) {
    if (swa_.success) {
      /*dbgprintf("loading crossbar (msgId=%u)",
        swa_.flit->getPacket()->getMessage()->getId());*/

      // send the flit on the crossbar, consume a credit
      swa_.flit->setVc(swa_.outputVc);
      crossbar_->inject(swa_.flit, crossbarIndex_, swa_.outputPort);
      crossbarScheduler_->decrementCreditCount(swa_.outputVcId);

      // send a credit back
      router_->sendCredit(port_, vc_);
    } else {
      /*dbgprintf("recycling flit (msgId=%u)",
        swa_.flit->getPacket()->getMessage()->getId());*/
      // put the flit back in the buffer
      receiveFlit(0, swa_.flit);
    }

    // clear SWA info
    swa_.fsm = ePipelineFsm::kEmpty;
    swa_.flit = nullptr;
    swa_.outputPort = U32_MAX;
    swa_.outputVc = U32_MAX;
    swa_.outputVcId = U32_MAX;
  }

  /*
   * attempt to load SWA tail pipeline stage from the RFE stage
   *  also submit the SWA request
   */
  // ensure VCA is ready to advance
  assert(swa_.fsm == ePipelineFsm::kEmpty);
  if (rfe_.fsm == ePipelineFsm::kReadyToAdvance) {
    /*dbgprintf("loading SWA (msgId=%u)",
      rfe_.flit->getPacket()->getMessage()->getId());*/

    // ensure SWA is empty
    assert(swa_.flit == nullptr);
    assert(swa_.outputPort == U32_MAX);
    assert(swa_.outputVc == U32_MAX);
    assert(swa_.outputVcId == U32_MAX);

    // set SWA info
    swa_.flit = rfe_.flit;
    u64 r = gRandom->randomU64(0, rfe_.route.size() - 1);
    u32 port, vc;
    rfe_.route.get(r, &port, &vc);
    swa_.outputVcId = router_->vcIndex(port, vc);
    swa_.outputPort = outputSpeedup_ ? swa_.outputVcId : port;
    swa_.outputVc = vc;

    // submit SWA request
    crossbarScheduler_->request(
        crossbarSchedulerIndex_, swa_.outputPort, swa_.outputVcId,
        swa_.flit->getPacket()->getDeadline());
    swa_.fsm = ePipelineFsm::kWaitingForResponse;

    // clear RFE info
    rfe_.fsm = ePipelineFsm::kEmpty;
    rfe_.flit = nullptr;
    rfe_.route.clear();
  }

  /*
   * attempt to load RFE stage
   */
  if ((rfe_.fsm == ePipelineFsm::kEmpty) && (buffer_.empty() == false)) {
    /*dbgprintf("loading RFE (msgId=%u)",
      buffer_.top()->getPacket()->getMessage()->getId());*/

    // ensure RFE is empty
    assert(rfe_.flit == nullptr);

    // pull out the front flit
    Flit* flit = buffer_.top();
    buffer_.pop();

    // put it in the routing pipeline stage
    assert(rfe_.flit == nullptr);
    rfe_.flit = flit;

    // set state as ready to request routing function
    rfe_.fsm = ePipelineFsm::kWaitingToRequest;
  }

  /*
   * attempt to submit a routing request
   */
  if (rfe_.fsm == ePipelineFsm::kWaitingToRequest) {
    // dbgprintf("[RFE], head flit");

    // submit request
    routingFunction_->request(this, rfe_.flit, &rfe_.route);

    // set state machine
    rfe_.fsm = ePipelineFsm::kWaitingForResponse;
  }

  /*
   * there are a few reasons that the next cycle should be processed:
   *  1. more flits in the queue, need to pull one out
   * if any of these cases are true, create an event to handle the next cycle
   */
  if (buffer_.size() > 0) {   // more flits in buffer
    // set a pipeline event for the next cycle
    setPipelineEvent();
  }
}

}  // namespace PoemNoc2
