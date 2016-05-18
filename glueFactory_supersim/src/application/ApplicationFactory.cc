/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/ApplicationFactory.h"

#include <cassert>

#include "application/Application.h"
#include "application/stresstest/Application.h"
#include "application/simplemem/Application.h"
#include "application/singlestream/Application.h"

Application* ApplicationFactory::createApplication(
    const std::string& _name, const Component* _parent,
    Json::Value _settings) {
  std::string type = _settings["type"].asString();
  if (type == "stress_test") {
    return new StressTest::Application(_name, _parent, _settings);
  } else if (type == "simple_mem") {
    return new SimpleMem::Application(_name, _parent, _settings);
  } else if (type == "single_stream") {
    return new SingleStream::Application(_name, _parent, _settings);
  } else {
    fprintf(stderr, "unknown application type: %s\n", type.c_str());
    assert(false);
  }
}
