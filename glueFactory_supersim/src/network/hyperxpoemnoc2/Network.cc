/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/hyperxpoemnoc2/Network.h"

#include <strop/strop.h>

#include <cassert>
#include <cmath>

#include "interface/InterfaceFactory.h"
#include "network/hyperxpoemnoc2/RoutingFunctionFactory.h"
#include "router/poemnoc2/Router.h"
#include "util/DimensionIterator.h"

namespace HyperXPoemNoc2 {

Network::Network(const std::string& _name, const Component* _parent,
                 Json::Value _settings)
    : ::Network(_name, _parent, _settings) {
  // dimensions and concentration
  assert(_settings["dimension_widths"].isArray());
  dimensions_ = _settings["dimension_widths"].size();
  assert(_settings["dimension_weights"].size() == dimensions_);
  dimensionWidths_.resize(dimensions_);
  for (u32 i = 0; i < dimensions_; i++) {
    dimensionWidths_.at(i) = _settings["dimension_widths"][i].asUInt();
    assert(dimensionWidths_.at(i) >= 2);
  }
  dimensionWeights_.resize(dimensions_);
  for (u32 i = 0; i < dimensions_; i++) {
    dimensionWeights_.at(i) = _settings["dimension_weights"][i].asUInt();
    assert(dimensionWeights_.at(i) >= 1);
  }
  concentration_ = _settings["concentration"].asUInt();
  assert(concentration_ > 0);
  dbgprintf("dimensions_ = %u", dimensions_);
  dbgprintf("dimensionWidths_ = %s",
            strop::vecString<u32>(dimensionWidths_).c_str());
  dbgprintf("dimensionWeights_ = %s",
            strop::vecString<u32>(dimensionWeights_).c_str());
  dbgprintf("concentration_ = %u", concentration_);

  // NOC attributes
  nocWidth_ = _settings["noc_width"].asUInt();
  assert(nocWidth_ > 0);
  nocWeight_ = _settings["noc_weight"].asUInt();
  assert(nocWeight_ > 0);
  nocConcentration_ = _settings["noc_concentration"].asUInt();
  assert(nocConcentration_ > 0);

  // router radix
  assert(_settings["router"].isMember("num_ports") == false);
  assert(_settings["router"].isMember("num_vcs") == false);
  assert(_settings["interface"].isMember("num_vcs") == false);
  u32 nocRadix = concentration_;
  for (u32 i = 0; i < dimensions_; i++) {
    nocRadix += ((dimensionWidths_.at(i) - 1) * dimensionWeights_.at(i));
  }
  u32 nocRouterRadix = nocConcentration_ + (nocWeight_ * (nocWidth_ - 1));
  assert((nocConcentration_ * nocWidth_) >= nocRadix);
  _settings["router"]["num_ports"] = Json::Value(nocRouterRadix);
  _settings["router"]["concentration"] = Json::Value(nocConcentration_);
  _settings["router"]["num_vcs"] = Json::Value(numVcs_);
  _settings["interface"]["num_vcs"] = Json::Value(numVcs_);

  // create a routing function factory to give to the routers
  RoutingFunctionFactory* routingFunctionFactory = new RoutingFunctionFactory(
      numVcs_, dimensionWidths_, dimensionWeights_, concentration_,
      nocWidth_, nocWeight_, nocConcentration_);

  // create a vector of dimension widths that contains the NOC width
  std::vector<u32> allDimensionWidths(1);
  allDimensionWidths.at(0) = nocWidth_;
  allDimensionWidths.insert(allDimensionWidths.begin() + 1,
                            dimensionWidths_.cbegin(),
                            dimensionWidths_.cend());

  // setup a router iterator for looping over the router dimensions
  DimensionIterator routerIterator(allDimensionWidths);
  std::vector<u32> routerAddress(allDimensionWidths.size());

  // create the routers (loop over allDimensionWidths)
  routerIterator.reset();
  routers_.setSize(allDimensionWidths);
  while (routerIterator.next(&routerAddress)) {
    std::string routerName = "Router_" + strop::vecString<u32>(routerAddress);

    // use the router factory to create a router
    routers_.at(routerAddress) = new PoemNoc2::Router(
        routerName, this, routingFunctionFactory, _settings["router"]);

    // set the router's address
    routers_.at(routerAddress)->setAddress(routerAddress);
  }
  delete routingFunctionFactory;

  // link routers via internal channels (loop over dimensionWidths_)
  DimensionIterator nocIterator(dimensionWidths_);
  std::vector<u32> nocAddress(dimensionWidths_.size());
  while (nocIterator.next(&nocAddress)) {
    u32 portBase = concentration_;
    for (u32 dim = 0; dim < dimensions_; dim++) {
      u32 dimWidth = dimensionWidths_.at(dim);
      u32 dimWeight = dimensionWeights_.at(dim);
      dbgprintf("dim=%u width=%u weight=%u\n", dim, dimWidth, dimWeight);

      for (u32 offset = 1; offset < dimWidth; offset++) {
        // determine the source NOC
        std::vector<u32> sourceAddress(nocAddress);

        // determine the destination NOC
        std::vector<u32> destinationAddress(sourceAddress);
        destinationAddress.at(dim) = (sourceAddress.at(dim) + offset) %
                                      dimWidth;

        for (u32 weight = 0; weight < dimWeight; weight++) {
          // create the channel
          std::string channelName = "InternalChannel_" +
              strop::vecString<u32>(sourceAddress) + "-to-" +
              strop::vecString<u32>(destinationAddress) +
              "-" + std::to_string(weight);
          Channel* channel = new Channel(channelName, this,
                                         _settings["internal_channel"]);
          internalChannels_.push_back(channel);

          // determine the port numbers
          u32 sourcePort = portBase + ((offset - 1) * dimWeight) + weight;
          u32 destinationPort = portBase + ((dimWidth - 1) * dimWeight) -
              (offset * dimWeight) + weight;
          dbgprintf("linking %s:%u to %s:%u with %s",
                    strop::vecString<u32>(sourceAddress).c_str(), sourcePort,
                    strop::vecString<u32>(destinationAddress).c_str(),
                    destinationPort,
                    channelName.c_str());

          // translate to NOC routers with port
          u32 nocSourceRouterIndex = sourcePort % nocWidth_;
          u32 nocSourceRouterPort = sourcePort / nocWidth_;
          assert(nocSourceRouterPort < nocConcentration_);
          std::vector<u32> nocSourceRouterAddress(1);
          nocSourceRouterAddress.at(0) = nocSourceRouterIndex;
          nocSourceRouterAddress.insert(
              nocSourceRouterAddress.begin() + 1,
              sourceAddress.cbegin(),
              sourceAddress.cend());
          u32 nocDestinationRouterIndex = destinationPort % nocWidth_;
          u32 nocDestinationRouterPort = destinationPort / nocWidth_;
          assert(nocDestinationRouterPort < nocConcentration_);
          std::vector<u32> nocDestinationRouterAddress(1);
          nocDestinationRouterAddress.at(0) = nocDestinationRouterIndex;
          nocDestinationRouterAddress.insert(
              nocDestinationRouterAddress.begin() + 1,
              destinationAddress.cbegin(),
              destinationAddress.cend());

          // link the routers from source to destination
          routers_.at(nocSourceRouterAddress)->setOutputChannel(
              nocSourceRouterPort, channel);
          routers_.at(nocDestinationRouterAddress)->setInputChannel(
              nocDestinationRouterPort, channel);
        }
      }
      portBase += ((dimWidth - 1) * dimWeight);
    }
  }

  // link NOC routers via NOC channels (loop over dimensionWidths_)
  nocIterator.reset();
  while (nocIterator.next(&nocAddress)) {
    for (u32 nocRouterIndex = 0; nocRouterIndex < nocWidth_; nocRouterIndex++) {
      // get the source address
      std::vector<u32> nocSourceRouterAddress(1);
      nocSourceRouterAddress.at(0) = nocRouterIndex;
      nocSourceRouterAddress.insert(
          nocSourceRouterAddress.begin() + 1,
          nocAddress.cbegin(),
          nocAddress.cend());

      // connect to all other routers in this NOC
      for (u32 offset = 1; offset < nocWidth_; offset++) {
        // get the destination address
        std::vector<u32> nocDestinationRouterAddress(nocSourceRouterAddress);
        nocDestinationRouterAddress.at(0) =
            ((nocDestinationRouterAddress.at(0) + offset) % nocWidth_);

        // make 'nocWeight_' connections between each router
        for (u32 weight = 0; weight < nocWeight_; weight++) {
          // make 'numVcs_' connections for each weight connection
          for (u32 vc = 0; vc < numVcs_; vc++) {
            // create the channel
            std::string channelName = "NocChannel_" +
                strop::vecString<u32>(nocSourceRouterAddress) + "-to-" +
                strop::vecString<u32>(nocDestinationRouterAddress) +
                "-" + std::to_string(weight) + "-" + std::to_string(vc);
            Channel* channel = new Channel(channelName, this,
                                           _settings["noc_channel"]);
            nocChannels_.push_back(channel);

            // determine port numbers (regular ports)
            u32 nocSourceRouterPort =
                nocConcentration_ + ((offset - 1) * nocWeight_) + weight;
            u32 nocDestinationRouterPort =
                nocConcentration_ + ((nocWidth_ - 1) * nocWeight_) -
                (offset * nocWeight_) + weight;

            // translate to VC ports
            nocSourceRouterPort = nocConcentration_ +
                ((nocSourceRouterPort - nocConcentration_) * numVcs_) + vc;
            nocDestinationRouterPort = nocConcentration_ +
                ((nocDestinationRouterPort - nocConcentration_) * numVcs_) + vc;

            // link the routers from source to destination
            routers_.at(nocSourceRouterAddress)->setOutputChannel(
                nocSourceRouterPort, channel);
            routers_.at(nocDestinationRouterAddress)->setInputChannel(
                nocDestinationRouterPort, channel);
          }
        }
      }
    }
  }

  // create a vector of dimension widths that contains the concentration
  std::vector<u32> fullDimensionWidths(1);
  fullDimensionWidths.at(0) = concentration_;
  fullDimensionWidths.insert(fullDimensionWidths.begin() + 1,
                             dimensionWidths_.cbegin(),
                             dimensionWidths_.cend());

  // create interfaces and link them with the routers
  interfaces_.setSize(fullDimensionWidths);
  u32 interfaceId = 0;
  nocIterator.reset();
  while (nocIterator.next(&nocAddress)) {
    // loop over concentration
    for (u32 conc = 0; conc < concentration_; conc++) {
      // create a vector for the Interface address
      std::vector<u32> interfaceAddress(1);
      interfaceAddress.at(0) = conc;
      interfaceAddress.insert(interfaceAddress.begin() + 1,
                              nocAddress.cbegin(),
                              nocAddress.cend());

      // create an interface name
      std::string interfaceName = "Interface_" +
          strop::vecString<u32>(interfaceAddress);

      // create the interface
      Interface* interface = InterfaceFactory::createInterface(
          interfaceName, this, interfaceId, _settings["interface"]);
      interfaces_.at(interfaceAddress) = interface;
      interfaceId++;

      // create I/O channels
      std::string inChannelName = "ExternalChannel_" +
          strop::vecString<u32>(interfaceAddress) + "-to-" +
          strop::vecString<u32>(nocAddress);
      std::string outChannelName = "ExternalChannel_" +
          strop::vecString<u32>(nocAddress) + "-to-" +
          strop::vecString<u32>(interfaceAddress);
      Channel* inChannel = new Channel(inChannelName, this,
                                       _settings["external_channel"]);
      Channel* outChannel = new Channel(outChannelName, this,
                                       _settings["external_channel"]);
      externalChannels_.push_back(inChannel);
      externalChannels_.push_back(outChannel);

      // translate to NOC router with port
      u32 nocRouterIndex = conc % nocWidth_;
      u32 nocRouterPort = conc / nocWidth_;
      assert(nocRouterPort < nocConcentration_);
      std::vector<u32> nocRouterAddress(1);
      nocRouterAddress.at(0) = nocRouterIndex;
      nocRouterAddress.insert(
          nocRouterAddress.begin() + 1,
          nocAddress.cbegin(),
          nocAddress.cend());

      // link with router
      routers_.at(nocRouterAddress)->setInputChannel(nocRouterPort,
                                                     inChannel);
      interface->setOutputChannel(inChannel);
      routers_.at(nocRouterAddress)->setOutputChannel(nocRouterPort,
                                                      outChannel);
      interface->setInputChannel(outChannel);
    }
  }
}

Network::~Network() {
  // delete routers
  for (u32 id = 0; id < routers_.size(); id++) {
    delete routers_.at(id);
  }

  // delete interfaces
  for (u32 id = 0; id < interfaces_.size(); id++) {
    delete interfaces_.at(id);
  }

  // delete channels
  for (auto it = nocChannels_.begin();
       it != nocChannels_.end(); ++it) {
    delete *it;
  }
  for (auto it = internalChannels_.begin();
       it != internalChannels_.end(); ++it) {
    delete *it;
  }
  for (auto it = externalChannels_.begin();
       it != externalChannels_.end(); ++it) {
    delete *it;
  }
}

u32 Network::numRouters() const {
  return routers_.size();
}

u32 Network::numInterfaces() const {
  return interfaces_.size();
}

Router* Network::getRouter(u32 _id) const {
  return routers_.at(_id);
}

Interface* Network::getInterface(u32 _id) const {
  return interfaces_.at(_id);
}

void Network::translateIdToAddress(u32 _id, std::vector<u32>* _address) const {
  _address->resize(dimensions_ + 1);
  // addresses are in little endian format
  u32 mod, div;
  mod = _id % concentration_;
  div = _id / concentration_;
  _address->at(0) = mod;
  for (u32 dim = 0; dim < dimensions_; dim++) {
    u32 dimWidth = dimensionWidths_.at(dim);
    mod = div % dimWidth;
    div = div / dimWidth;
    _address->at(dim + 1) = mod;
  }
}

void Network::collectChannels(std::vector<Channel*>* _channels) {
  for (auto it = externalChannels_.begin();
       it != externalChannels_.end(); ++it) {
    _channels->push_back(*it);
  }
  for (auto it = internalChannels_.begin();
       it != internalChannels_.end(); ++it) {
    _channels->push_back(*it);
  }
  for (auto it = nocChannels_.begin();
       it != nocChannels_.end(); ++it) {
    _channels->push_back(*it);
  }
}

}  // namespace HyperXPoemNoc2
