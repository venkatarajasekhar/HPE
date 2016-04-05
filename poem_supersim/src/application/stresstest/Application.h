/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_STRESSTEST_APPLICATION_H_
#define APPLICATION_STRESSTEST_APPLICATION_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "application/Application.h"

namespace StressTest {

class Application : public ::Application {
 public:
  Application(const std::string& _name, const Component* _parent,
              Json::Value _settings);
  ~Application();
  void terminalWarmed(u32 _id);
  void terminalSaturated(u32 _id);
  void terminalComplete(u32 _id);
  f64 percentComplete() const override;

 private:
  u32 warmedTerminals_;
  u32 saturatedTerminals_;
  bool warmupComplete_;
  f64 warmupThreshold_;
  u32 completedTerminals_;
};

}  // namespace StressTest

#endif  // APPLICATION_STRESSTEST_APPLICATION_H_
