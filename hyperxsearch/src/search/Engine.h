/*
 * Copyright (c) 2016, Nic McDonald
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
#ifndef SEARCH_ENGINE_H_
#define SEARCH_ENGINE_H_

#include <prim/prim.h>

#include <deque>
#include <vector>

struct Hyperx {
  u64 dimensions;  // L
  std::vector<u64> widths;  // S
  u64 routers;  // P
  u64 concentration;  // T
  u64 terminals;  // N
  std::vector<u64> weights;  // K
  u64 routerRadix;  // R
  std::vector<f64> bisections;  // B
  u64 channels;
  f64 cost;
};

class CostFunction {
 public:
  CostFunction();
  virtual ~CostFunction();
  virtual f64 cost(const Hyperx& _hyperx) const = 0;
};

class Comparator {
 public:
  bool operator()(const Hyperx& _lhs, const Hyperx& _rhs) const;
};

class Engine {
 public:
  Engine(u64 _minDimensions, u64 _maxDimensions, u64 _minRadix, u64 _maxRadix,
         u64 _minConcentration, u64 _maxConcentration, u64 _minTerminals,
         u64 _maxTerminals, f64 _minBandwidth, f64 _maxBandwidth,
         bool _fixedWidth, bool _fixedWeight, u64 _maxResults,
         const CostFunction* _costFunction);
  ~Engine();

  void run();
  const std::deque<Hyperx>& results() const;

 private:
  u64 minDimensions_;
  u64 maxDimensions_;
  u64 minRadix_;
  u64 maxRadix_;
  u64 minConcentration_;
  u64 maxConcentration_;
  u64 minTerminals_;
  u64 maxTerminals_;
  f64 minBandwidth_;
  f64 maxBandwidth_;
  bool fixedWidth_;
  bool fixedWeight_;
  u64 maxResults_;
  const CostFunction* costFunction_;
  Comparator comparator_;
  Hyperx hyperx_;
  std::deque<Hyperx> results_;

  void stage1();
  void stage2();
  void stage3();
  void stage4();
  void stage5();
};

#endif  // SEARCH_ENGINE_H_
