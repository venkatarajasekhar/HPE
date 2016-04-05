/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemnoc1/OutputQueue.h"

#include <cassert>

#include <string>
#include <queue>
#include <algorithm>

#include "router/poemnoc1/Router.h"
#include "types/Message.h"
#include "types/Packet.h"

// event types
#define PROCESS_PIPELINE (0xB7)

namespace PoemNoc1 {

OutputQueue::OutputQueue(
    const std::string& _name, const Component* _parent, u32 _depth, u32 _port,
    u32 _vc, ::CrossbarScheduler* _outputCrossbarScheduler,
    u32 _crossbarSchedulerIndex, Crossbar* _crossbar, u32 _crossbarIndex,
    CrossbarScheduler* _mainCrossbarScheduler, u32 _mainCrossbarVcId)
    : Component(_name, _parent), depth_(_depth), port_(_port), vc_(_vc),
      outputCrossbarScheduler_(_outputCrossbarScheduler),
      crossbarSchedulerIndex_(_crossbarSchedulerIndex), crossbar_(_crossbar),
      crossbarIndex_(_crossbarIndex),
      mainCrossbarScheduler_(_mainCrossbarScheduler),
      mainCrossbarVcId_(_mainCrossbarVcId),
      lastReceivedTime_(0), lastReceivedCount_(0) {
  // ensure the buffer is empty
  assert(buffer_.size() == 0);

  // initialize the entry
  swa_.fsm = ePipelineFsm::kEmpty;
  swa_.flit = nullptr;

  // no event is set to trigger
  eventTime_ = U64_MAX;
}

OutputQueue::~OutputQueue() {}

OutputQueue::DeadlineComparator::DeadlineComparator() {}

OutputQueue::DeadlineComparator::~DeadlineComparator() {}

bool OutputQueue::DeadlineComparator::operator()(
    const Flit* _lhs, const Flit* _rhs) const {
  return (_lhs->getPacket()->getDeadline() >
          _rhs->getPacket()->getDeadline());
}

void OutputQueue::receiveFlit(u32 _port, Flit* _flit) {
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

void OutputQueue::processEvent(void* _event, s32 _type) {
  switch (_type) {
    case (PROCESS_PIPELINE):
      assert(gSim->epsilon() == 2);
      processPipeline();
      break;

    default:
      assert(false);
  }
}

void OutputQueue::crossbarSchedulerResponse(u32 _port, u32 _vc) {
  assert(swa_.fsm == ePipelineFsm::kWaitingForResponse);

  swa_.fsm = ePipelineFsm::kReadyToAdvance;
  swa_.success = _port != U32_MAX;

  // ensure an event is set to process the pipeline
  setPipelineEvent();
}

void OutputQueue::setPipelineEvent() {
  if (eventTime_ == U64_MAX) {
    eventTime_ = gSim->time();
    addEvent(gSim->time(), 2, nullptr, PROCESS_PIPELINE);
  }
}

void OutputQueue::processPipeline() {
  /*
   * attempt to load the crossbar
   */
  if (swa_.fsm == ePipelineFsm::kReadyToAdvance) {
    if (swa_.success) {
      /*dbgprintf("loading crossbar (msgId=%u)",
        swa_.flit->getPacket()->getMessage()->getId());*/

      // send the flit on the crossbar
      crossbar_->inject(swa_.flit, crossbarIndex_, 0);
      outputCrossbarScheduler_->decrementCreditCount(vc_);

      // send a credit back
      mainCrossbarScheduler_->incrementCreditCount(mainCrossbarVcId_);
    } else {
      /*dbgprintf("recycling flit  (msgId=%u)",
        swa_.flit->getPacket()->getMessage()->getId());*/

      // put the flit back in the buffer
      receiveFlit(0, swa_.flit);
    }

    // clear SWA info
    swa_.fsm = ePipelineFsm::kEmpty;
    swa_.flit = nullptr;
  }

  /*
   * attempt to load SWA stage
   */
  if ((swa_.fsm == ePipelineFsm::kEmpty) && (buffer_.empty() == false)) {
    // dbgprintf("loading SWA");

    // ensure SWA is empty
    assert(swa_.flit == nullptr);

    // pull out the front flit
    Flit* flit = buffer_.top();
    buffer_.pop();

    // put it in this pipeline stage
    swa_.flit = flit;

    // set SWA info
    swa_.fsm = ePipelineFsm::kWaitingToRequest;
  }

  /*
   * Attempt to submit a SWA request
   */
  if (swa_.fsm == ePipelineFsm::kWaitingToRequest) {
    swa_.fsm = ePipelineFsm::kWaitingForResponse;
    u64 metadata = swa_.flit->getPacket()->getDeadline();
    outputCrossbarScheduler_->request(crossbarSchedulerIndex_, 0, vc_,
                                      metadata);
  }

  // clear the eventTime_ variable to indicate no more events are set
  eventTime_ = U64_MAX;

  /*
   * there are a few reasons that the next cycle should be processed:
   *  1. no credits were available for crossing the switch, try again
   *  2. more flits in the queue, need to pull one out
   * if any of these cases are true, create and expect an event the next cycle
   */
  if ((swa_.fsm == ePipelineFsm::kWaitingToRequest) ||  // no credits
      (buffer_.size() > 0)) {   // more flits in buffer
    // set a pipeline event for the next cycle
    eventTime_ = gSim->futureCycle(1);
    addEvent(eventTime_, 2, nullptr, PROCESS_PIPELINE);
  }
}

}  // namespace PoemNoc1
