/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <json/json.h>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include "application/Application.h"
#include "application/ApplicationFactory.h"
#include "application/Terminal.h"
#include "event/Simulator.h"
#include "event/VectorQueue.h"
#include "network/Network.h"
#include "network/NetworkFactory.h"
#include "random/Random.h"
#include "settings/Settings.h"

static void usage(const char* _exe, const char* _error) {
  if (_error != nullptr) {
    printf("ERROR: %s\n", _error);
  }
  printf(
      "usage:\n"
      "  %s <file> [overrides] ...\n"
      "\n"
      "  file      : JSON formated settings file expressing configuration\n"
      "              (see examples)\n"
      "  override  : a descriptor of a settings override\n"
      "              <path_description>=<type>=<value>\n"
      "              type may be uint, float, string, or bool\n"
      "              examples:\n"
      "              simulator.cycle_time=uint=1200\n"
      "              network.dimensions[3]=uint=10\n"
      "              stats.data_log.compress=bool=false\n"
      "\n", _exe);
}

s32 main(s32 _argc, char** _argv) {
  // turn off buffered output on stdout and stderr
  setbuf(stdout, nullptr);
  setbuf(stderr, nullptr);

  // check for -h or --help
  for (s32 i = 1; i < _argc; i++) {
    if ((strcmp(_argv[i], "-h") == 0) ||
        (strcmp(_argv[i], "--help") == 0)) {
      usage(_argv[0], nullptr);
      return 0;
    }
  }

  // check minimum number of args
  if (_argc < 2) {
    usage(_argv[0], "please specify a settings file");
    return -1;
  }

  // read in the settings
  Json::Value settings;
  Settings::initFile(_argv[1], &settings);

  // apply command line updates to the settings
  {
    std::vector<std::string> settingsUpdates;
    for (s64 idx = 2; idx < _argc; idx++) {
      settingsUpdates.push_back(std::string(_argv[idx]));
    }
    Settings::update(&settings, settingsUpdates);
  }
  printf("%s\n", Settings::toString(&settings).c_str());

  /*
   * enable debugging on select components
   */
  for (u32 i = 0; i < settings["debug"].size(); i++) {
    std::string componentName = settings["debug"][i].asString();
    Component::addDebugName(componentName);
  }

  /*
   * Initialize all global components
   */

  // initialize the discrete event simulator
  //  we will assume that time is on the picosecond scale
  gSim = new VectorQueue(settings["simulator"]);

  // create a random number generator
  gRandom = new Random(settings["random"]);

  /*
   * Create a network
   */
  Network* network = NetworkFactory::createNetwork(
      "Network", nullptr, settings["network"]);
  gSim->setNetwork(network);
  u32 numInterfaces = network->numInterfaces();
  u32 numRouters = network->numRouters();
  u32 routerRadix = network->getRouter(0)->numPorts();
  u32 numVcs = network->numVcs();
  u64 numComponents = Component::numComponents();

  printf("Endpoints:    %u\n"
         "Routers:      %u\n"
         "Router radix: %u\n"
         "VCs:          %u\n"
         "Components:   %lu\n\n",
         numInterfaces,
         numRouters,
         routerRadix,
         numVcs,
         numComponents);

  // create an application
  Application* application = ApplicationFactory::createApplication(
      "Application", nullptr, settings["application"]);
  gSim->setApplication(application);
  assert(application->numTerminals() == numInterfaces);

  // check that all debug names were authentic
  Component::debugCheck();

  // run the simulation!
  printf("Simulation beginning\n");
  gSim->simulate();
  printf("Simulation complete\n");

  // cleanup the elements created here
  delete network;
  delete application;

  // cleanup the global simulator components
  delete gRandom;
  delete gSim;

  return 0;
}
