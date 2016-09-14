/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "allocator/RSeparableAllocator.h"

#include <json/json.h>
#include <gtest/gtest.h>
#include <prim/prim.h>

#include "random/Random.h"
#include "settings/Settings.h"

#include "allocator/Allocator_TEST.h"

static void verify(u32 _numClients, u32 _numResources, const bool* _request,
                   const u64* _metadata, const bool* _grant) {
  // TODO(nic): find a good test for this
}

TEST(Allocator, RSeparable_LSLP) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "lslp";
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "r_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, true);
}

TEST(Allocator, RSeparable_Greater) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "comparing";
  arbSettings["greater"] = true;
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "r_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, true);
}

TEST(Allocator, RSeparable_Lesser) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "comparing";
  arbSettings["greater"] = false;
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "r_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, true);
}
