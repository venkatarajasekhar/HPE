/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "allocator/RcSeparableAllocator.h"

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

TEST(Allocator, RcSeparable_LSLP) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "lslp";
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["client_arbiter"] = arbSettings;
  allocSettings["iterations"] = 3;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "rc_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, false);
}

TEST(Allocator, RcSeparable_Greater) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "comparing";
  arbSettings["greater"] = true;
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["client_arbiter"] = arbSettings;
  allocSettings["iterations"] = 2;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "rc_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, false);
}

TEST(Allocator, RcSeparable_Lesser) {
  // create the allocator settings
  Json::Value arbSettings;
  arbSettings["type"] = "comparing";
  arbSettings["greater"] = false;
  Json::Value allocSettings;
  allocSettings["resource_arbiter"] = arbSettings;
  allocSettings["client_arbiter"] = arbSettings;
  allocSettings["iterations"] = 1;
  allocSettings["slip_latch"] = true;
  allocSettings["type"] = "rc_separable";
  // printf("%s\n", Settings::toString(&allocSettings).c_str());

  // test
  AllocatorTest(allocSettings, verify, false);
}
