/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <json/json.h>
#include <gtest/gtest.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "arbiter/Arbiter.h"
#include "arbiter/RandomArbiter.h"
#include "random/Random.h"

#include "arbiter/Arbiter_TEST.h"
#include "test/TestSetup_TEST.h"

TEST(Arbiter, Random) {
  TestSetup testSetup(1, 123);

  for (u32 size = 1; size < 100; size++) {
    bool* request = new bool[size];
    u64* metadata = new u64[size];
    bool* grant = new bool[size];
    do {
      for (u32 idx = 0; idx < size; idx++) {
        request[idx] = gRandom->randomBool();
        metadata[idx] = 0;
      }
    } while (hotCount(request, size) <= size/2);
    /*putc('=', stdout);
      for (u32 idx = 0; idx < size; idx++) {
      putc(request[idx] ? '1' : '0', stdout);
      }
      putc('\n', stdout);*/

    Arbiter* arb = new RandomArbiter("Arb", nullptr, size, Json::Value());
    assert(arb->size() == size);
    for (u32 idx = 0; idx < size; idx++) {
      arb->setRequest(idx, &request[idx]);
      arb->setMetadata(idx, &metadata[idx]);
      arb->setGrant(idx, &grant[idx]);
    }

    // do more arbitrations
    while (hotCount(request, size) > 0) {
      memset(grant, false, size);
      arb->arbitrate();

      ASSERT_EQ(hotCount(grant, size), 1u);
      u32 awinner = winnerId(grant, size);
      (void)awinner;  // can't predict this for testing

      // turn off the winner
      ASSERT_TRUE(request[awinner]);
      request[awinner] = false;
    }

    // zero requests input test
    ASSERT_EQ(hotCount(request, size), 0u);
    memset(grant, false, size);
    arb->arbitrate();
    ASSERT_EQ(hotCount(grant, size), 0u);

    // cleanup
    delete[] request;
    delete[] metadata;
    delete[] grant;
    delete arb;
  }
}
