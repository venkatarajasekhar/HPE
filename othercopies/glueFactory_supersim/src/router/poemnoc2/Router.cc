/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "router/poemnoc2/Router.h"

#include <cassert>

#include "network/RoutingFunctionFactory.h"
#include "router/poemnoc2/InputQueue.h"
#include "router/poemnoc2/OutputQueue.h"
#include "router/poemnoc2/Ejector.h"
#include "types/Credit.h"

namespace PoemNoc2 {

Router::Router(
    const std::string& _name, const Component* _parent,
    RoutingFunctionFactory* _routingFunctionFactory,
    Json::Value _settings)
    : ::Router(_name, _parent, _settings) {
  // determine attributes of NOC router
  inputQueueDepth_ = _settings["input_queue_depth"].asUInt();
  assert(inputQueueDepth_ > 0);
  localQueueDepth_ = _settings["local_queue_depth"].asUInt();
  assert(localQueueDepth_ > 0);
  outputQueueDepth_ = _settings["output_queue_depth"].asUInt();
  assert(outputQueueDepth_ > 0);
  concentration_ = _settings["concentration"].asUInt();
  assert(concentration_ > 0);

  // create each subrouter
  u32 crossbarInputs = numPorts_ * numVcs_;
  u32 crossbarOutputs = numPorts_ * numVcs_;
  crossbar_ = new Crossbar(
      "Crossbar", this, crossbarInputs, crossbarOutputs, _settings["crossbar"]);
  crossbarScheduler_ = new CrossbarScheduler(
      "CrossbarScheduler", this, crossbarInputs, crossbarInputs,
      crossbarOutputs, _settings["crossbar_scheduler"]);

  // make space for flit distributors (global ports only)
  inputFlitDistributors_.resize(concentration_, NULL);

  // create routing functions, input queues, link to routing function,
  //  crossbar, and schedulers
  routingFunctions_.resize(crossbarInputs);
  inputQueues_.resize(crossbarInputs, nullptr);
  for (u32 port = 0; port < numPorts_; port++) {
    bool portIsGlobal = port < concentration_;

    // input flit distributor
    if (portIsGlobal) {
      std::string fdname = "InputFlitDistributer_" + std::to_string(port);
      inputFlitDistributors_.at(port) = new FlitDistributor(fdname, this,
                                                            numVcs_);
    }

    // create each input pipeline
    for (u32 vc = 0; vc < numVcs_; vc++) {
      // create the name suffix
      std::string nameSuffix = "_" + std::to_string(port) + "_" +
                               std::to_string(vc);

      // routing function
      std::string rfname = "RoutingFunction" + nameSuffix;
      RoutingFunction* rf = _routingFunctionFactory->createRoutingFunction(
          rfname, this, this, port, _settings["routing"]);
      routingFunctions_.at(vcIndex(port, vc)) = rf;

      // input queue
      std::string iqName = (portIsGlobal ? "InputQueue" : "LocalQueue")
          + nameSuffix;
      u32 queueDepth = portIsGlobal ? inputQueueDepth_ : localQueueDepth_;
      InputQueue* iq = new InputQueue(
          iqName, this, this, queueDepth, port, numVcs_, vc,
          true, rf, crossbarScheduler_, vcIndex(port, vc), crossbar_,
          vcIndex(port, vc));
      inputQueues_.at(vcIndex(port, vc)) = iq;

      // initialize the credit count in the CrossbarScheduler
      u32 availableCredits = portIsGlobal ? outputQueueDepth_ :
          localQueueDepth_;
      crossbarScheduler_->initCreditCount(vcIndex(port, vc), availableCredits);

      // register the input queue with crossbar scheduler
      crossbarScheduler_->setClient(vcIndex(port, vc), iq);

      // register the input queue with the flit distributor
      if (portIsGlobal) {
        inputFlitDistributors_.at(port)->setReceiver(vc, iq);
      }
    }
  }

  u32 numChannels = concentration_ + ((numPorts_ - concentration_) * numVcs_);

  // output queues, allocators, crossbars, and ejectors
  outputQueues_.resize(concentration_ * numVcs_, nullptr);
  outputCrossbarSchedulers_.resize(concentration_, nullptr);
  outputCrossbars_.resize(concentration_, nullptr);
  ejectors_.resize(numChannels, nullptr);
  for (u32 port = 0; port < numPorts_; port++) {
    bool portIsGlobal = port < concentration_;

    // create output structure for global ports
    if (portIsGlobal) {
      // output port crossbar scheduler
      std::string outputCrossbarSchedulerName =
          "OutputCrossbarScheduler_" + std::to_string(port);
      outputCrossbarSchedulers_.at(port) = new ::CrossbarScheduler(
          outputCrossbarSchedulerName, this, numVcs_, numVcs_, 1,
          _settings["output_crossbar_scheduler"]);

      // output crossbar
      std::string outputCrossbarName = "OutputCrossbar_" + std::to_string(port);
      outputCrossbars_.at(port) = new Crossbar(
          outputCrossbarName, this, numVcs_, 1,
          _settings["output_crossbar"]);

      // ejector
      std::string ejName = "GlobalEjector_" + std::to_string(port);
      ejectors_.at(port) = new Ejector(ejName, this);
      outputCrossbars_.at(port)->setReceiver(0, ejectors_.at(port), 0);

      // queues
      for (u32 vc = 0; vc < numVcs_; vc++) {
        // initialize the credit count in the OutputCrossbarScheduler
        outputCrossbarSchedulers_.at(port)->initCreditCount(vc,
                                                            inputQueueDepth_);

        // create the name suffix
        std::string nameSuffix = "_" + std::to_string(port) + "_" +
            std::to_string(vc);

        // output queue
        std::string oqName = "OutputQueue" + nameSuffix;
        OutputQueue* oq = new OutputQueue(
            oqName, this, outputQueueDepth_, port, vc,
            outputCrossbarSchedulers_.at(port), vc,
            outputCrossbars_.at(port), vc,
            crossbarScheduler_, vcIndex(port, vc));
        outputQueues_.at(vcIndex(port, vc)) = oq;

        // register the output queue with switch allocator
        outputCrossbarSchedulers_.at(port)->setClient(vc, oq);

        // register the output queue with the flit distributor
        crossbar_->setReceiver(vcIndex(port, vc), oq, 0);
      }
    } else {
      // create output structure for local ports
      for (u32 vc = 0; vc < numVcs_; vc++) {
        std::string ejName = "LocalEjector_" + std::to_string(port) + "_" +
            std::to_string(vc);
        u32 ejId = concentration_ + vcIndex(port - concentration_, vc);
        ejectors_.at(ejId) = new Ejector(ejName, this);
        crossbar_->setReceiver(vcIndex(port, vc), ejectors_.at(ejId), 0);
      }
    }
  }

  // allocate slots for I/O channels
  inputChannels_.resize(numChannels, nullptr);
  outputChannels_.resize(numChannels, nullptr);
}

Router::~Router() {
  delete crossbarScheduler_;
  delete crossbar_;
  for (auto rf : routingFunctions_) {
    delete rf;
  }
  for (auto iq : inputQueues_) {
    delete iq;
  }
  for (auto oq : outputQueues_) {
    delete oq;
  }
  for (auto ocs : outputCrossbarSchedulers_) {
    delete ocs;
  }
  for (auto oc : outputCrossbars_) {
    delete oc;
  }
  for (auto ej : ejectors_) {
    delete ej;
  }
  for (auto ifd : inputFlitDistributors_) {
    delete ifd;
  }
}

void Router::setInputChannel(u32 _port, Channel* _channel) {
  assert(inputChannels_.at(_port) == nullptr);
  inputChannels_.at(_port) = _channel;

  bool portIsGlobal = _port < concentration_;
  if (portIsGlobal) {
    // global ports connect to flit distributors
    _channel->setSink(inputFlitDistributors_.at(_port), 0);
  } else {
    // local channels connect directly to input queues
    u32 qId = (_port - concentration_) + (concentration_ * numVcs_);
    _channel->setSink(inputQueues_.at(qId), 0);
  }
}

void Router::setOutputChannel(u32 _port, Channel* _channel) {
  assert(outputChannels_.at(_port) == NULL);
  outputChannels_.at(_port) = _channel;
  _channel->setSource(this, _port);
  ejectors_.at(_port)->setChannel(outputChannels_.at(_port));
}

void Router::receiveFlit(u32 _port, Flit* _flit) {
  assert(false);  // given directly to subcomponents
}

void Router::receiveControl(u32 _port, Control* _control) {
  Credit* cred = dynamic_cast<Credit*>(_control);
  assert(cred);
  bool portIsGlobal = _port < concentration_;
  while (cred->more()) {
    u32 vc = cred->getNum();
    if (portIsGlobal) {
      outputCrossbarSchedulers_.at(_port)->incrementCreditCount(vc);
    } else {
      u32 port = concentration_ + ((_port - concentration_) / numVcs_);
      u32 vc = (_port - concentration_) % numVcs_;
      crossbarScheduler_->incrementCreditCount(vcIndex(port, vc));
    }
  }
  delete _control;
}

void Router::sendCredit(u32 _port, u32 _vc) {
  // ensure there is an outgoing credit for the next time slot
  bool portIsGlobal = _port < concentration_;
  u32 chId;
  if (portIsGlobal) {
    chId = _port;
  } else {
    chId = concentration_ + vcIndex(_port - concentration_, _vc);
  }

  Channel* channel = inputChannels_.at(chId);
  Control* ctrl = channel->getNextControl();
  Credit* cred;
  if (ctrl == nullptr) {
    cred = new Credit(numVcs_);
    channel->setNextControl(cred);
  } else {
    cred = dynamic_cast<Credit*>(ctrl);
    assert(cred);
  }

  // mark the credit with the specified VC
  cred->putNum(_vc);
}

u32 Router::vcIndex(u32 _port, u32 _vc) const {
  return (_port * numVcs_) + _vc;
}

}  // namespace PoemNoc2
