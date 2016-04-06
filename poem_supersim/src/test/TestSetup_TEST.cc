/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "test/TestSetup_TEST.h"

#include <json/json.h>
#include <string>

#include "settings/Settings.h"
#include "event/Simulator.h"
#include "event/VectorQueue.h"
#include "random/Random.h"

TestSetup::TestSetup(u64 _cycleTime, u64 _randomSeed) {
  std::string str =
      std::string("{\n") +
      "  \"simulator\": {\n" +
      "     \"cycle_time\": " + std::to_string(_cycleTime) + ",\n" +
      "     \"print_progress\": false\n" +
      "  },\n" +
      "  \"random\": {\n" +
      "     \"seed\": " + std::to_string(_randomSeed) + "\n" +
      "  }\n" +
      "}\n" +
      std::string();

  Json::Value settings;
  Settings::initString(str.c_str(), &settings);

  gSim = new VectorQueue(settings["simulator"]);
  gRandom = new Random(settings["random"]);
}

TestSetup::~TestSetup() {
  delete gSim;
  gSim = nullptr;
  delete gRandom;
  gRandom = nullptr;
}
