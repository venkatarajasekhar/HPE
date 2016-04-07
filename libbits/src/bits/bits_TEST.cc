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
#include <cstdio>

#include <map>
#include <random>

#include "bits/bits.h"

TEST(pow2, u32) {
  std::map<u32, u32> pow2s =
      {{0, 1u}, {1, 2u}, {2, 4u}, {3, 8u}, {4, 16u}, {5, 32u}, {6, 64u},
       {7, 128u}, {8, 256u}, {9, 512u}, {10, 1024u}, {11, 2048u}, {12, 4096u},
       {13, 8192u}, {14, 16384u}, {15, 32768u}, {16, 65536u}, {17, 131072u},
       {18, 262144u}, {19, 524288u}, {20, 1048576u}, {21, 2097152u},
       {22, 4194304u}, {23, 8388608u}, {24, 16777216u}, {25, 33554432u},
       {26, 67108864u}, {27, 134217728u}, {28, 268435456u}, {29, 536870912u},
       {30, 1073741824u}, {31, 2147483648u}};

  for (auto& p : pow2s) {
    ASSERT_EQ(bits::pow2<u32>(p.first), p.second);
  }
}

TEST(pow2, u64) {
  std::map<u64, u64> pow2s =
      {{0, 1lu}, {1, 2lu}, {2, 4lu}, {3, 8lu}, {4, 16lu}, {5, 32lu}, {6, 64lu},
       {7, 128lu}, {8, 256lu}, {9, 512lu}, {10, 1024lu}, {11, 2048lu},
       {12, 4096lu}, {13, 8192lu}, {14, 16384lu}, {15, 32768lu}, {16, 65536lu},
       {17, 131072lu}, {18, 262144lu}, {19, 524288lu}, {20, 1048576lu},
       {21, 2097152lu}, {22, 4194304lu}, {23, 8388608lu}, {24, 16777216lu},
       {25, 33554432lu}, {26, 67108864lu}, {27, 134217728lu}, {28, 268435456lu},
       {29, 536870912lu}, {30, 1073741824lu}, {31, 2147483648lu},
       {32, 4294967296lu}, {33, 8589934592lu}, {34, 17179869184lu},
       {35, 34359738368lu}, {36, 68719476736lu}, {37, 137438953472lu},
       {38, 274877906944lu}, {39, 549755813888lu}, {40, 1099511627776lu},
       {41, 2199023255552lu}, {42, 4398046511104lu}, {43, 8796093022208lu},
       {44, 17592186044416lu}, {45, 35184372088832lu}, {46, 70368744177664lu},
       {47, 140737488355328lu}, {48, 281474976710656lu},
       {49, 562949953421312lu}, {50, 1125899906842624lu},
       {51, 2251799813685248lu}, {52, 4503599627370496lu},
       {53, 9007199254740992lu}, {54, 18014398509481984lu},
       {55, 36028797018963968lu}, {56, 72057594037927936lu},
       {57, 144115188075855872lu}, {58, 288230376151711744lu},
       {59, 576460752303423488lu}, {60, 1152921504606846976lu},
       {61, 2305843009213693952lu}, {62, 4611686018427387904lu},
       {63, 9223372036854775808lu}};

  for (auto& p : pow2s) {
    ASSERT_EQ(bits::pow2<u64>(p.first), p.second);
  }
}


TEST(isPow2, u32) {
  ASSERT_FALSE(bits::isPow2<u32>(0u));

  for (u32 b = 0; b < 32; b++) {
    u32 v = (u64)1 << b;
    ASSERT_TRUE(bits::isPow2<u32>(v));
  }

  std::mt19937_64 prng;
  prng.seed(123245678);

  for (u32 cnt = 0; cnt < 1000000; cnt++) {
    u32 b0, b1;
    do {
      b0 = prng() % 32;
      b1 = prng() % 32;
    } while ((b0 == 0) ||
             (b1 == 0) ||
             (b0 == b1));
    u32 val = (1 << b0) | (1 << b1);
    ASSERT_FALSE(bits::isPow2<u32>(val));
  }
}

TEST(isPow2, u64) {
  ASSERT_FALSE(bits::isPow2<u64>(0lu));

  for (u64 b = 0; b < 64; b++) {
    u64 v = (u64)1 << b;
    ASSERT_TRUE(bits::isPow2<u64>(v));
  }

  std::mt19937_64 prng;
  prng.seed(123245678);

  for (u32 cnt = 0; cnt < 1000000; cnt++) {
    u64 b0, b1;
    do {
      b0 = prng() % 64;
      b1 = prng() % 64;
    } while ((b0 == 0) ||
             (b1 == 0) ||
             (b0 == b1));
    u64 val = ((u64)1 << b0) | ((u64)1 << b1);
    ASSERT_FALSE(bits::isPow2<u64>(val));
  }
}

TEST(floorLog2, u32) {
  std::mt19937_64 prng;
  prng.seed(123245678);

  for (u32 cnt = 0; cnt < 1000000; cnt++) {
    u32 rnd = prng();
    ASSERT_EQ(bits::floorLog2<u32>(rnd), (u32)std::floor(std::log2((f64)rnd)));
  }
}

TEST(ceilLog2, u32) {
  std::mt19937_64 prng;
  prng.seed(123245678);

  for (u32 cnt = 0; cnt < 1000000; cnt++) {
    u32 rnd = prng();
    ASSERT_EQ(bits::ceilLog2<u32>(rnd), (u32)std::ceil(std::log2((f64)rnd)));
  }
}

TEST(reverse, u32_full_range) {
  ASSERT_EQ(bits::reverse<u32>(0u), 0u);
  ASSERT_EQ(bits::reverse<u32>(1u), 2147483648u);
  ASSERT_EQ(bits::reverse<u32>(2904892549u), 2703533237u);
  ASSERT_EQ(bits::reverse<u32>(2703533237u), 2904892549u);
  ASSERT_EQ(bits::reverse<u32>(2147483648u), 1u);
}

TEST(reverse, u32_limited_range) {
  ASSERT_EQ(bits::reverse<u32>(0u, 8), 0u);
  ASSERT_EQ(bits::reverse<u32>(1u, 4), 8u);
  ASSERT_EQ(bits::reverse<u32>(8u, 4), 1u);
  ASSERT_EQ(bits::reverse<u32>(1u, 8), 128u);
  ASSERT_EQ(bits::reverse<u32>(128u, 8), 1u);
  ASSERT_EQ(bits::reverse<u32>(2u, 8), 64u);
  ASSERT_EQ(bits::reverse<u32>(64u, 8), 2u);
  ASSERT_EQ(bits::reverse<u32>(5u, 8), 160u);
  ASSERT_EQ(bits::reverse<u32>(160u, 8), 5u);
}

TEST(rotateRight, u32) {
  ASSERT_EQ(bits::rotateRight<u32>(1u), 2147483648u);
  ASSERT_EQ(bits::rotateRight<u32>(1u, 32), 2147483648u);
  ASSERT_EQ(bits::rotateRight<u32>(1u, 16), 32768u);
  ASSERT_EQ(bits::rotateRight<u32>(32768u, 16), 16384u);
  ASSERT_EQ(bits::rotateRight<u32>(16384u, 16), 8192u);
  ASSERT_EQ(bits::rotateRight<u32>(43008u, 16), 21504u);
}

TEST(rotateLeft, u32) {
  ASSERT_EQ(bits::rotateLeft<u32>(2147483648u), 1u);
  ASSERT_EQ(bits::rotateLeft<u32>(2147483648u, 32), 1u);;
  ASSERT_EQ(bits::rotateLeft<u32>(32768u, 16), 1u);
  ASSERT_EQ(bits::rotateLeft<u32>(16384u, 16), 32768u);
  ASSERT_EQ(bits::rotateLeft<u32>(8192u, 16), 16384u);
  ASSERT_EQ(bits::rotateLeft<u32>(21504u, 16), 43008u);
}
