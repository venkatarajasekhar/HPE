/*
 * Copyright (c) 2012-2015, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <gtest/gtest.h>
#include <prim/prim.h>

#include <cmath>
#include <ctime>

#include <vector>

#include "rnd/Random.h"


TEST(Random, seed) {
  const u64 kRands = 1000;

  // initialize a bunch of Random objects
  std::vector<rnd::Random*> rands;
  for (u64 i = 0; i < kRands; i++) {
    rands.push_back(new rnd::Random((i+1) << 32));
  }

  // get the first value of each random
  std::vector<u64> values;
  for (u64 i = 0; i < kRands; i++) {
    values.push_back(rands[i]->nextU64(0, U64_MAX));
  }

  // re-seed each random, verify the random produces the same value
  for (u64 n = 0; n < 5; n++) {
    for (u64 i = 0; i < kRands; i++) {
      rands[i]->seed((i+1) << 32);
      u64 value = rands.at(i)->nextU64(0, U64_MAX);
      ASSERT_EQ(value, values.at(i));
    }
  }

  // cleanup the Random objects
  for (u64 i = 0; i < kRands; i++) {
    delete rands[i];
  }
}

TEST(Random, u64) {
  const u64 kBkts = 1000;
  const u64 kRounds = 10000000;
  std::vector<u64> buckets(kBkts, 0);
  rnd::Random rand(0xDEADBEEF12345678lu);

  for (u64 r = 0; r < kRounds; r++) {
    u64 value = rand.nextF64(0, kBkts);
    buckets.at(value)++;
  }

  f64 sum = 0;
  for (u64 b = 0; b < kBkts; b++) {
    sum += buckets.at(b);
    // printf("[%lu] = %lu\n", b, buckets.at(b));
  }
  f64 mean = sum / kBkts;

  sum = 0;
  for (u64 b = 0; b < kBkts; b++) {
    f64 c = (static_cast<f64>(buckets.at(b)) - mean);
    c *= c;
    sum += c;
  }
  f64 stdDev = std::sqrt(sum / kBkts);
  // printf("stdDev = %f\n", stdDev);
  stdDev /= kRounds;
  // printf("relStdDev = %f\n", stdDev);
  ASSERT_LE(stdDev, 0.00009);
}

TEST(Random, f64) {
  const u64 kBkts = 1000;
  const u64 kRounds = 10000000;
  std::vector<u64> buckets(kBkts, 0);
  rnd::Random rand(0xDEADBEEF12345678lu);

  for (u64 r = 0; r < kRounds; r++) {
    f64 value = rand.nextF64(0, 1000);
    buckets.at(static_cast<u64>(value))++;
  }

  f64 sum = 0;
  for (u64 b = 0; b < kBkts; b++) {
    sum += buckets.at(b);
    // printf("[%lu] = %lu\n", b, buckets.at(b));
  }
  f64 mean = sum / kBkts;

  sum = 0;
  for (u64 b = 0; b < kBkts; b++) {
    f64 c = (static_cast<f64>(buckets.at(b)) - mean);
    c *= c;
    sum += c;
  }
  f64 stdDev = std::sqrt(sum / kBkts);
  // printf("stdDev = %f\n", stdDev);
  stdDev /= kRounds;
  // printf("relStdDev = %f\n", stdDev);
  ASSERT_LE(stdDev, 0.00009);
}

TEST(Random, bool) {
  const u64 kRounds = 100000000;

  // use a non-deterministic RNG to seed our deterministic PRNG
  srand(time(nullptr));
  for (u64 t = 0; t < 10; t++) {
    u64 seed = 0;
    for (u64 b = 0; b < sizeof(u64); b++) {
      u64 byte = rand() % 0xFF;  // NOLINT
      seed = (seed << 8) | byte;
    }

    rnd::Random rand(seed);

    u64 count = 0;
    for (u64 r = 0; r < kRounds; r++) {
      if (rand.nextBool()) {
        count++;
      }
    }

    f64 ratio = static_cast<f64>(count) / kRounds;
    f64 dev = std::abs(0.5 - ratio);
    // printf("ratio %.15f\n", ratio);
    // printf("dev %.15f\n", dev);
    ASSERT_LE(dev, 0.0002);
  }
}
