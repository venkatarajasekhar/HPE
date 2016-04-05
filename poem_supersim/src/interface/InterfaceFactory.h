/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_INTERFACEFACTORY_H_
#define INTERFACE_INTERFACEFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "interface/Interface.h"

class InterfaceFactory {
 public:
  static Interface* createInterface(
      const std::string& _name, const Component* _parent, u32 _id,
      Json::Value _settings);
};

#endif  // INTERFACE_INTERFACEFACTORY_H_
