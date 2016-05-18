/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_HYPERXPOEMNOC1_NETWORK_H_
#define NETWORK_HYPERXPOEMNOC1_NETWORK_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "interface/Interface.h"
#include "network/Network.h"
#include "network/Channel.h"
#include "router/Router.h"
#include "util/DimensionalArray.h"

namespace HyperXPoemNoc1 {

class Network : public ::Network {
 public:
  Network(const std::string& _name, const Component* _parent,
          Json::Value _settings);
  ~Network();

  // Network
  u32 numRouters() const override;
  u32 numInterfaces() const override;
  Router* getRouter(u32 _id) const override;
  Interface* getInterface(u32 _id) const override;
  void translateIdToAddress(u32 _id, std::vector<u32>* _address) const override;

 protected:
  void collectChannels(std::vector<Channel*>* _channels) override;

 private:
  u32 dimensions_;
  std::vector<u32> dimensionWidths_;
  std::vector<u32> dimensionWeights_;
  u32 concentration_;
  u32 nocWidth_;
  u32 nocWeight_;
  u32 nocConcentration_;
  DimensionalArray<Router*> routers_;
  DimensionalArray<Interface*> interfaces_;
  std::vector<Channel*> nocChannels_;
  std::vector<Channel*> internalChannels_;
  std::vector<Channel*> externalChannels_;
};

}  // namespace HyperXPoemNoc1

#endif  // NETWORK_HYPERXPOEMNOC1_NETWORK_H_
