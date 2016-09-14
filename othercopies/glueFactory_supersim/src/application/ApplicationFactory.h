/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_APPLICATIONFACTORY_H_
#define APPLICATION_APPLICATIONFACTORY_H_

#include <json/json.h>
#include <string>

#include "event/Component.h"

class Application;

class ApplicationFactory {
 public:
  static Application* createApplication(
      const std::string& _name, const Component* _parent,
      Json::Value _settings);
};

#endif  // APPLICATION_APPLICATIONFACTORY_H_
