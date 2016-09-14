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
#include "arbiter/LslpArbiter.h"
#include "random/Random.h"

#include "arbiter/Arbiter_TEST.h"
#include "test/TestSetup_TEST.h"

TEST(Arbiter, Lslp) {
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
      putc(request[idx].enable ? '1' : '0', stdout);
      }
      putc('\n', stdout);*/

    Arbiter* arb = new LslpArbiter("Arb", nullptr, size, Json::Value());
    assert(arb->size() == size);
    for (u32 idx = 0; idx < size; idx++) {
      arb->setRequest(idx, &request[idx]);
      arb->setMetadata(idx, &metadata[idx]);
      arb->setGrant(idx, &grant[idx]);
    }

    // first arbitration to over come random initialization
    memset(grant, false, size);
    arb->arbitrate();
    ASSERT_EQ(hotCount(grant, size), 1u);
    u32 pwinner = winnerId(grant, size);
    arb->latch();

    // do more arbitrations
    const u32 ARBS = 100;
    for (u32 a = 0; a < ARBS; a++) {
      memset(grant, false, size);
      arb->arbitrate();

      ASSERT_EQ(hotCount(grant, size), 1u);
      u32 awinner = winnerId(grant, size);
      u32 ewinner = (pwinner + 1) % size;
      while (request[ewinner] == false) {
        ewinner = (ewinner + 1) % size;
      }
      // printf("p=%u a=%u e=%u\n", pwinner, awinner, ewinner);
      ASSERT_EQ(awinner, ewinner);
      ASSERT_TRUE(request[awinner]);

      // only latch the priority sometimes
      if (gRandom->randomBool()) {
        arb->latch();
        pwinner = awinner;
      }
    }

    // zero requests input test
    for (u32 idx = 0; idx < size; idx++) {
      request[idx] = false;
    }
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
