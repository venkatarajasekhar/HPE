/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#ifndef NETWORK_HIERARCHYHYPERX_NETWORK_H_
#define NETWORK_HIERARCHYHYPERX_NETWORK_H_

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

namespace HierarchyHyperX {

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
  u32 hierarchy_;
  // global hyperX parameters
  u32 globalDimensions_;
  std::vector<u32> globalDimensionWidths_;
  std::vector<u32> globalDimensionWeights_;
  // local hyperX parameters
  u32 localDimensions_;
  std::vector<u32> localDimensionWidths_;
  std::vector<u32> localDimensionWeights_;
  u32 concentration_;
  // user specifed number of global links per router
  u32 globalLinksPerRouter_;

  DimensionalArray<Router*> routers_;
  DimensionalArray<Interface*> interfaces_;
  std::vector<Channel*> localChannels_;
  std::vector<Channel*> globalChannels_;
  std::vector<Channel*> externalChannels_;
};

}  // namespace HierarchyHyperX

#endif  // NETWORK_HIERARCHYHYPERX_NETWORK_H_
