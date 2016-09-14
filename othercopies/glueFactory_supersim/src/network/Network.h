/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_NETWORK_H_
#define NETWORK_NETWORK_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "interface/Interface.h"
#include "network/Channel.h"
#include "router/Router.h"
#include "stats/ChannelLog.h"

class Network : public Component {
 public:
  Network(const std::string& _name, const Component* _parent,
          Json::Value _settings);
  virtual ~Network();
  virtual u32 numRouters() const = 0;
  virtual u32 numInterfaces() const = 0;
  virtual Router* getRouter(u32 _id) const = 0;
  virtual Interface* getInterface(u32 _id) const = 0;
  virtual void translateIdToAddress(u32 _id,
                                    std::vector<u32>* _address) const = 0;
  u32 numVcs() const;
  void startMonitoring();
  void endMonitoring();

 protected:
  virtual void collectChannels(std::vector<Channel*>* _channels) = 0;

  u32 numVcs_;

 private:
  ChannelLog* channelLog_;
};

#endif  // NETWORK_NETWORK_H_
