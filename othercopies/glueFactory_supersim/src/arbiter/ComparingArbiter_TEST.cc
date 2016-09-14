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
#include "arbiter/ComparingArbiter.h"
#include "random/Random.h"

#include "arbiter/Arbiter_TEST.h"
#include "test/TestSetup_TEST.h"

TEST(Arbiter, Comparing) {
  TestSetup testSetup(1, 123);

  for (u32 size = 1; size < 100; size++) {
    for (u32 dir = 0; dir < 2; dir++) {
      bool greater = dir == 1;

      bool* request = new bool[size];
      u64* metadata = new u64[size];
      bool* grant = new bool[size];
      do {
        for (u32 idx = 0; idx < size; idx++) {
          request[idx] = gRandom->randomBool();
          metadata[idx] = 10000 + idx;
        }
      } while (hotCount(request, size) <= size/2);
      /*putc('=', stdout);
        for (u32 idx = 0; idx < size; idx++) {
        putc(request[idx].enable ? '1' : '0', stdout);
        }
        putc('\n', stdout);*/

      Json::Value asettings;
      asettings["greater"] = greater;
      Arbiter* arb = new ComparingArbiter("Arb", nullptr, size, asettings);
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
        u32 ewinner = U32_MAX;
        u64 emeta = U64_MAX;
        for (u32 idx = 0; idx < size; idx++) {
          if (request[idx]) {
            if (ewinner == U32_MAX) {
              ewinner = idx;
              emeta = metadata[idx];
            } else {
              ASSERT_NE(metadata[idx], emeta);
              if ((greater) && (metadata[idx] > emeta)) {
                ewinner = idx;
                emeta = metadata[idx];
              } else if ((!greater) && (metadata[idx] < emeta)) {
                ewinner = idx;
                emeta = metadata[idx];
              }
            }
          }
        }
        ASSERT_NE(ewinner, U32_MAX);
        ASSERT_NE(emeta, U64_MAX);
        ASSERT_EQ(awinner, ewinner);

        // turn off the winner
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
}
