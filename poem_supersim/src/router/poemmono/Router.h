/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_ROUTER_H_
#define ROUTER_POEMMONO_ROUTER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/Channel.h"
#include "network/RoutingFunction.h"
#include "network/RoutingFunctionFactory.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"
#include "router/common/FlitDistributor.h"
#include "router/poemmono/CrossbarScheduler.h"
#include "router/poemmono/CrossbarSchedulerFactory.h"
#include "router/Router.h"
#include "types/Control.h"
#include "types/Credit.h"
#include "types/Flit.h"

namespace PoemMono {

class InputQueue;
class OutputQueue;
class Ejector;

class Router : public ::Router {
 public:
  Router(const std::string& _name, const Component* _parent,
         RoutingFunctionFactory* _routingFunctionFactory,
         Json::Value _settings);
  ~Router();

  // Network
  void setInputChannel(u32 _index, Channel* _channel) override;
  void setOutputChannel(u32 _index, Channel* _channel) override;

  void receiveFlit(u32 _port, Flit* _flit) override;
  void receiveControl(u32 _port, Control* _control) override;

  void sendCredit(u32 _port, u32 _vc);  // called by InputQueue
  void sendFlit(u32 _port, Flit* _flit);  // called by Ejector

  u32 vcIndex(u32 _port, u32 _vc) const;
  void vcIndexRev(u32 _index, u32* _port, u32* _vc) const;

 private:
  bool outputSpeedup_;

  std::vector<FlitDistributor*> inputFlitDistributors_;
  std::vector<InputQueue*> inputQueues_;
  std::vector<RoutingFunction*> routingFunctions_;
  Crossbar* crossbar_;
  CrossbarScheduler* crossbarScheduler_;

  std::vector<FlitDistributor*> outputFlitDistributors_;
  std::vector<OutputQueue*> outputQueues_;
  std::vector<::CrossbarScheduler*> outputCrossbarSchedulers_;
  std::vector<Crossbar*> outputCrossbars_;
  std::vector<Ejector*> ejectors_;

  std::vector<Channel*> inputChannels_;
  std::vector<Channel*> outputChannels_;
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_ROUTER_H_
