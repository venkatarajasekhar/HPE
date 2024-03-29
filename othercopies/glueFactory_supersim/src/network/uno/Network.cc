/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/uno/Network.h"

#include <cassert>
#include <cmath>

#include "interface/InterfaceFactory.h"
#include "network/uno/RoutingFunctionFactory.h"
#include "router/RouterFactory.h"

namespace Uno {

Network::Network(const std::string& _name, const Component* _parent,
                 Json::Value _settings)
    : ::Network(_name, _parent, _settings) {
  // dimensions and concentration
  concentration_ = _settings["concentration"].asUInt();
  assert(concentration_ > 0);
  dbgprintf("concentration_ = %u", concentration_);

  // router radix
  assert(_settings["router"].isMember("num_ports") == false);
  u32 routerRadix = concentration_;
  _settings["router"]["num_ports"] = Json::Value(routerRadix);
  _settings["router"]["num_vcs"] = Json::Value(numVcs_);
  _settings["interface"]["num_vcs"] = Json::Value(numVcs_);

  // create a routing function factory to give to the routers
  RoutingFunctionFactory* routingFunctionFactory = new RoutingFunctionFactory(
      numVcs_, concentration_);

  // create the router
  router_ = RouterFactory::createRouter(
      "Router", this, routingFunctionFactory, _settings["router"]);
  router_->setAddress(std::vector<u32>());  // empty
  delete routingFunctionFactory;

  // create the interfaces and external channels
  interfaces_.resize(concentration_, nullptr);
  for (u32 id = 0; id < concentration_; id++) {
    // create the interface
    std::string interfaceName = "Interface_" + std::to_string(id);
    Interface* interface = InterfaceFactory::createInterface(
        interfaceName, this, id, _settings["interface"]);
    interfaces_.at(id) = interface;

    // create the channels
    std::string inChannelName = "InChannel_" + std::to_string(id);
    Channel* inChannel = new Channel(inChannelName, this,
                                     _settings["external_channel"]);
    externalChannels_.push_back(inChannel);
    std::string outChannelName = "OutChannel_" + std::to_string(id);
    Channel* outChannel = new Channel(outChannelName, this,
                                      _settings["external_channel"]);
    externalChannels_.push_back(outChannel);

    // link interfaces to router via channels
    router_->setInputChannel(id, inChannel);
    router_->setOutputChannel(id, outChannel);
    interface->setInputChannel(outChannel);
    interface->setOutputChannel(inChannel);
  }
}

Network::~Network() {
  delete router_;
  for (auto it = interfaces_.begin(); it != interfaces_.end(); ++it) {
    Interface* interface = *it;
    delete interface;
  }
  for (auto it = externalChannels_.begin(); it != externalChannels_.end();
       ++it) {
    Channel* channel = *it;
    delete channel;
  }
}

u32 Network::numRouters() const {
  return 1;
}

u32 Network::numInterfaces() const {
  return concentration_;
}

Router* Network::getRouter(u32 _id) const {
  assert(_id == 0);
  return router_;
}

Interface* Network::getInterface(u32 _id) const {
  return interfaces_.at(_id);
}

void Network::translateIdToAddress(u32 _id, std::vector<u32>* _address) const {
  _address->resize(1);
  _address->at(0) = _id;
}

void Network::collectChannels(std::vector<Channel*>* _channels) {
  for (auto it = externalChannels_.begin(); it != externalChannels_.end();
       ++it) {
    Channel* channel = *it;
    _channels->push_back(channel);
  }
}

}  // namespace Uno
