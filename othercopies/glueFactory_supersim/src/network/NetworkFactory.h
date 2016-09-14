/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_NETWORKFACTORY_H_
#define NETWORK_NETWORKFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "network/Network.h"
#include "event/Component.h"

class NetworkFactory {
 public:
  static Network* createNetwork(
      const std::string& _name, const Component* _parent,
      Json::Value _settings);
};

#endif  // NETWORK_NETWORKFACTORY_H_
