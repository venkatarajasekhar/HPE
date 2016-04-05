/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_UNO_NETWORK_H_
#define NETWORK_UNO_NETWORK_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "interface/Interface.h"
#include "network/Network.h"
#include "network/Channel.h"
#include "router/Router.h"

namespace Uno {

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
  u32 concentration_;
  Router* router_;

  std::vector<Channel*> externalChannels_;
  std::vector<Interface*> interfaces_;
};

}  // namespace Uno

#endif  // NETWORK_UNO_NETWORK_H_
