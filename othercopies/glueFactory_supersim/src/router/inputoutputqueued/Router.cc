/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/inputoutputqueued/Router.h"

#include <cassert>

#include "network/RoutingFunctionFactory.h"
#include "router/inputoutputqueued/InputQueue.h"
#include "router/inputoutputqueued/OutputQueue.h"
#include "router/inputoutputqueued/Ejector.h"
#include "types/Credit.h"

namespace InputOutputQueued {

Router::Router(
    const std::string& _name, const Component* _parent,
    RoutingFunctionFactory* _routingFunctionFactory,
    Json::Value _settings)
    : ::Router(_name, _parent, _settings) {
  u32 inputQueueDepth = _settings["input_queue_depth"].asUInt();
  assert(inputQueueDepth > 0);
  u32 outputQueueDepth = _settings["output_queue_depth"].asUInt();
  assert(outputQueueDepth > 0);

  // create crossbar and schedulers
  crossbar_ = new Crossbar(
      "Crossbar", this, numPorts_ * numVcs_, numPorts_ * numVcs_,
      _settings["crossbar"]);
  vcScheduler_ = new VcScheduler(
      "VcScheduler", this, numPorts_ * numVcs_, numPorts_ * numVcs_,
      _settings["vc_scheduler"]);
  crossbarScheduler_ = new CrossbarScheduler(
      "CrossbarScheduler", this, numPorts_ * numVcs_, numPorts_ * numVcs_,
      numPorts_ * numVcs_, _settings["crossbar_scheduler"]);

  // create routing functions, input queues, link to routing function,
  //  crossbar, and schedulers
  routingFunctions_.resize(numPorts_ * numVcs_);
  inputQueues_.resize(numPorts_ * numVcs_, nullptr);
  for (u32 port = 0; port < numPorts_; port++) {
    for (u32 vc = 0; vc < numVcs_; vc++) {
      // initialize the credit count in the CrossbarScheduler
      crossbarScheduler_->initCreditCount(vcIndex(port, vc), outputQueueDepth);

      // create the name suffix
      std::string nameSuffix = "_" + std::to_string(port) + "_" +
                               std::to_string(vc);

      // routing function
      std::string rfname = "RoutingFunction" + nameSuffix;
      RoutingFunction* rf = _routingFunctionFactory->createRoutingFunction(
          rfname, this, this, port, _settings["routing"]);
      routingFunctions_.at(vcIndex(port, vc)) = rf;

      // compute the client index (same for VC alloc, SW alloc, and Xbar)
      u32 clientIndex = vcIndex(port, vc);

      // input queue
      std::string iqName = "InputQueue" + nameSuffix;
      InputQueue* iq = new InputQueue(
          iqName, this, this, inputQueueDepth, port, numVcs_, vc, rf,
          vcScheduler_, clientIndex, crossbarScheduler_, clientIndex, crossbar_,
          clientIndex);
      inputQueues_.at(vcIndex(port, vc)) = iq;

      // register the input queue with VC and crossbar schedulers
      vcScheduler_->setClient(clientIndex, iq);
      crossbarScheduler_->setClient(clientIndex, iq);
    }
  }

  // output queues, schedulers, and crossbar
  outputQueues_.resize(numPorts_ * numVcs_, nullptr);
  outputCrossbarSchedulers_.resize(numPorts_, nullptr);
  outputCrossbars_.resize(numPorts_, nullptr);
  ejectors_.resize(numPorts_, nullptr);
  for (u32 port = 0; port < numPorts_; port++) {
    // output port switch allocator
    std::string outputCrossbarSchedulerName =
        "OutputCrossbarScheduler_" + std::to_string(port);
    outputCrossbarSchedulers_.at(port) = new CrossbarScheduler(
        outputCrossbarSchedulerName, this, numVcs_, numVcs_, 1,
        _settings["output_crossbar_scheduler"]);

    // output crossbar
    std::string outputCrossbarName = "OutputCrossbar_" + std::to_string(port);
    outputCrossbars_.at(port) = new Crossbar(
        outputCrossbarName, this, numVcs_, 1,
        _settings["output_crossbar"]);

    // ejector
    std::string ejName = "Ejector_" + std::to_string(port);
    ejectors_.at(port) = new Ejector(ejName, this, port);
    outputCrossbars_.at(port)->setReceiver(0, ejectors_.at(port), 0);

    // queues
    for (u32 vc = 0; vc < numVcs_; vc++) {
      // initialize the credit count in the OutputCrossbarScheduler
      outputCrossbarSchedulers_.at(port)->initCreditCount(vc, inputQueueDepth);

      // create the name suffix
      std::string nameSuffix = "_" + std::to_string(port) + "_" +
                               std::to_string(vc);

      // compute the client indexes
      u32 clientIndexOut = vc;  // sw alloc and output crossbar
      u32 clientIndexMain = vcIndex(port, vc);  // main crossbar

      // output queue
      std::string oqName = "OutputQueue" + nameSuffix;
      OutputQueue* oq = new OutputQueue(
          oqName, this, outputQueueDepth, port, vc,
          outputCrossbarSchedulers_.at(port), clientIndexOut,
          outputCrossbars_.at(port), clientIndexOut,
          crossbarScheduler_, clientIndexMain);
      outputQueues_.at(clientIndexMain) = oq;

      // register the output queue with switch allocator
      outputCrossbarSchedulers_.at(port)->setClient(clientIndexOut, oq);

      // register the output queue as the main crossbar receiver
      crossbar_->setReceiver(clientIndexMain, oq, 0);
    }
  }

  // allocate slots for I/O channels
  inputChannels_.resize(numPorts_, nullptr);
  outputChannels_.resize(numPorts_, nullptr);
}

Router::~Router() {
  delete vcScheduler_;
  delete crossbarScheduler_;
  delete crossbar_;
  for (u32 vc = 0; vc < (numPorts_ * numVcs_); vc++) {
    delete routingFunctions_.at(vc);
    delete inputQueues_.at(vc);
    delete outputQueues_.at(vc);
  }
  for (u32 port = 0; port < numPorts_; port++) {
    delete outputCrossbarSchedulers_.at(port);
    delete outputCrossbars_.at(port);
    delete ejectors_.at(port);
  }
}

void Router::setInputChannel(u32 _index, Channel* _channel) {
  inputChannels_.at(_index) = _channel;
  _channel->setSink(this, _index);
}

void Router::setOutputChannel(u32 _index, Channel* _channel) {
  outputChannels_.at(_index) = _channel;
  _channel->setSource(this, _index);
}

void Router::receiveFlit(u32 _port, Flit* _flit) {
  u32 vc = _flit->getVc();
  InputQueue* iq = inputQueues_.at(vcIndex(_port, vc));
  iq->receiveFlit(0, _flit);
}

void Router::receiveControl(u32 _port, Control* _control) {
  Credit* cred = dynamic_cast<Credit*>(_control);
  assert(cred);
  while (cred->more()) {
    u32 vc = cred->getNum();
    outputCrossbarSchedulers_.at(_port)->incrementCreditCount(vc);
  }
  delete _control;
}

void Router::sendCredit(u32 _port, u32 _vc) {
  // ensure there is an outgoing credit for the next time slot
  Control* ctrl = inputChannels_.at(_port)->getNextControl();
  Credit* cred;
  if (ctrl == nullptr) {
    cred = new Credit(numVcs_);
    inputChannels_.at(_port)->setNextControl(cred);
  } else {
    cred = dynamic_cast<Credit*>(ctrl);
    assert(cred);
  }

  // mark the credit with the specified VC
  cred->putNum(_vc);
}

void Router::sendFlit(u32 _port, Flit* _flit) {
  assert(outputChannels_.at(_port)->getNextFlit() == nullptr);
  outputChannels_.at(_port)->setNextFlit(_flit);
}

u32 Router::vcIndex(u32 _port, u32 _vc) const {
  return (_port * numVcs_) + _vc;
}

void Router::vcIndexRev(u32 _index, u32* _port, u32* _vc) const {
  assert(_index < (numPorts_ * numVcs_));
  *_port = _index / numVcs_;
  *_vc = _index % numVcs_;
}

}  // namespace InputOutputQueued
