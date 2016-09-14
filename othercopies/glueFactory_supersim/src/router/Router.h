/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_ROUTER_H_
#define ROUTER_ROUTER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/Channel.h"
#include "types/FlitReceiver.h"
#include "types/ControlReceiver.h"

class Router : public Component, public FlitReceiver, public ControlReceiver {
 public:
  Router(const std::string& _name, const Component* _parent,
         Json::Value _settings);
  virtual ~Router();

  u32 numPorts() const;
  u32 numVcs() const;
  void setAddress(const std::vector<u32>& _address);
  const std::vector<u32>& getAddress() const;

  virtual void setInputChannel(u32 _port, Channel* _channel) = 0;
  virtual void setOutputChannel(u32 port, Channel* _channel) = 0;

 protected:
  const u32 numPorts_;
  const u32 numVcs_;
  std::vector<u32> address_;
};

#endif  // ROUTER_ROUTER_H_
