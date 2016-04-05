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
#include "rnd/Random.h"

#include <cassert>

namespace rnd {

Random::Random() {}

Random::Random(u64 _seed) {
  seed(_seed);
}

Random::~Random() {}

void Random::seed(u64 _seed) {
  std::seed_seq seq = {(u32)((_seed >> 32) & 0xFFFFFFFFlu),
                       (u32)((_seed >>  0) & 0xFFFFFFFFlu)};
  prng_.seed(seq);
}

u64 Random::nextU64() {
  return intDist_(prng_);
}

u64 Random::nextU64(u64 _min, u64 _max) {
  assert(_max >= _min);
  if (_min == _max) {
    return _min;
  }
  u64 rand = intDist_(prng_);
  u64 span = _max - _min;
  if (span == U64_MAX) {
    return rand;
  } else {
    return (rand % (span + 1)) + _min;
  }
}

f64 Random::nextF64() {
  return realDist_(prng_);
}

f64 Random::nextF64(f64 _min, f64 _max) {
  assert(_max >= _min);
  f64 r = realDist_(prng_);
  r *= (_max - _min);
  r += _min;
  return r;
}

bool Random::nextBool() {
  return static_cast<bool>(intDist_(prng_) & 0x1);
}

}  // namespace rnd
