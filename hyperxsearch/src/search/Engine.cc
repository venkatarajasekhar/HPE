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
#include "search/Engine.h"

#include <strop/strop.h>

#include <cassert>

#include <algorithm>
#include <stdexcept>

static const u8 HSE_DEBUG = 0;

CostFunction::CostFunction() {}
CostFunction::~CostFunction() {}

bool Comparator::operator()(const Hyperx& _lhs, const Hyperx& _rhs) const {
  return _rhs.cost > _lhs.cost;
}

Engine::Engine(u64 _minDimensions, u64 _maxDimensions, u64 _minRadix,
               u64 _maxRadix, u64 _minConcentration, u64 _maxConcentration,
               u64 _minTerminals, u64 _maxTerminals, f64 _minBandwidth,
               f64 _maxBandwidth, bool _fixedWidth, bool _fixedWeight,
               u64 _maxResults, const CostFunction* _costFunction)
    : minDimensions_(_minDimensions),
      maxDimensions_(_maxDimensions),
      minRadix_(_minRadix),
      maxRadix_(_maxRadix),
      minConcentration_(_minConcentration),
      maxConcentration_(_maxConcentration),
      minTerminals_(_minTerminals),
      maxTerminals_(_maxTerminals),
      minBandwidth_(_minBandwidth),
      maxBandwidth_(_maxBandwidth),
      fixedWidth_(_fixedWidth),
      fixedWeight_(_fixedWeight),
      maxResults_(_maxResults),
      costFunction_(_costFunction) {
  if (minDimensions_ < 1) {
    throw std::runtime_error("mindimensions must be greater than 0");
  } else if (maxDimensions_ < minDimensions_) {
    throw std::runtime_error("maxdimensions must be greater than or equal to "
                             "mindimensions");
  } else if (minRadix_ < 2) {
    throw std::runtime_error("minradix must be greater than 1");
  } else if (maxRadix_ < minRadix_) {
    throw std::runtime_error("maxradix must be greater than or equal to "
                             "minradix");
  } else if (maxConcentration_ < minConcentration_) {
    throw std::runtime_error("maxconcentration must be greater than or equal "
                             "to minconcentration");
  } else if (minTerminals_ < minRadix_) {
    throw std::runtime_error("minterminals must be greater than or equal to "
                             "minradix");
  } else if (maxTerminals_ < minTerminals_) {
    throw std::runtime_error("maxterminals must be greater than or equal to "
                             "minterminals");
  } else if (minBandwidth_ <= 0) {
    throw std::runtime_error("minbandwidth must be greater than 0.0");
  } else if (maxBandwidth_ < minBandwidth_) {
    throw std::runtime_error("maxbandwidth must be greater than or equal to "
                             "minbandwidth");
  }
}

Engine::~Engine() {}

void Engine::run() {
  hyperx_ = Hyperx();
  results_.clear();

  stage1();
}

const std::deque<Hyperx>& Engine::results() const {
  return results_;
}

void Engine::stage1() {
  /*
   * loop over the number of dimensions
   */
  for (hyperx_.dimensions = minDimensions_;
       hyperx_.dimensions <= maxDimensions_;
       hyperx_.dimensions++) {
    // find the maximum width of any one dimension
    u64 maxWidth;
    if (!fixedWidth_) {
      // HyperX
      maxWidth = maxRadix_ - (hyperx_.dimensions - 1);
    } else {
      // FbFly
      maxWidth = ((maxRadix_ - 1) / hyperx_.dimensions) + 1;
    }

    if (maxWidth < 2) {
      break;
    }

    if (HSE_DEBUG >= 6) {
      printf("1: dimensions=%lu maxWidth=%lu\n",
             hyperx_.dimensions, maxWidth);
    }

    /*
     * generate possible dimension widths (S)
     */
    hyperx_.widths.clear();
    hyperx_.widths.resize(hyperx_.dimensions, 2);
    while (true) {
      // determine the number of routers
      hyperx_.routers = 1;
      u64 baseRadix = 1;  // minimum current radix
      for (u64 d = 0; d < hyperx_.widths.size(); d++) {
        hyperx_.routers *= hyperx_.widths.at(d);
        baseRadix += hyperx_.widths.at(d) - 1;
      }

      // find reasons to skip this case
      //  expr 1: at minimum, there would be 1 terminal per router
      //  expr 2: check minimum current router radix
      if ((hyperx_.routers <= maxTerminals_) &&
          (baseRadix <= maxRadix_)) {
        // if this configuration appears to work so far, use it
        stage2();
      } else if (HSE_DEBUG >= 7) {
        printf("1s: SKIPPING S=%s\n",
               strop::vecString<u64>(hyperx_.widths).c_str());
      }

      // detect when done
      if (hyperx_.widths.at(0) == maxWidth) {
        break;
      }

      // find the next widths configuration
      if (!fixedWidth_) {
        // HyperX
        u64 ndim = U64_MAX;  // next dimension to increment
        for (u64 invdim = 0; invdim < hyperx_.dimensions; invdim++) {
          ndim = hyperx_.dimensions - invdim - 1;
          if (hyperx_.widths.at(ndim) == maxWidth) {
            continue;
          } else {
            break;
          }
        }

        // increment this dimension
        hyperx_.widths.at(ndim)++;

        // all inferior dimensions reset
        for (u64 d = ndim + 1; d < hyperx_.dimensions; d++) {
          hyperx_.widths.at(d) = hyperx_.widths.at(ndim);
        }
      } else {
        // FbFly
        for (u64 d = 0; d < hyperx_.dimensions; d++) {
          hyperx_.widths.at(d)++;
        }
      }
    }
  }
}

void Engine::stage2() {
  for (u64 dim = 1; dim < hyperx_.dimensions; dim++) {
    assert(hyperx_.widths.at(dim) >= hyperx_.widths.at(dim-1));
  }

  if (HSE_DEBUG >= 5) {
    printf("2: S=%s P=%lu\n",
           strop::vecString<u64>(hyperx_.widths).c_str(), hyperx_.routers);
  }

  // compute the baseRadix (no terminals)
  u64 baseRadix = 0;
  for (u64 dim = 1; dim < hyperx_.dimensions; dim++) {
    baseRadix += hyperx_.widths.at(dim) - 1;
  }

  // try possible values for terminals per router ratio
  for (hyperx_.concentration = minConcentration_;
       hyperx_.concentration <= maxConcentration_;
       hyperx_.concentration++) {
    hyperx_.terminals = hyperx_.routers * hyperx_.concentration;
    u64 baseRadix2 = baseRadix + hyperx_.concentration;
    if ((hyperx_.terminals >= minTerminals_) &&
        (hyperx_.terminals <= maxTerminals_) &&
        (baseRadix2 <= maxRadix_)) {
      stage3();
    } else {
      if (HSE_DEBUG >= 7) {
        printf("2s: SKIPPING S=%s P=%lu T=%lu\n",
               strop::vecString<u64>(hyperx_.widths).c_str(), hyperx_.routers,
               hyperx_.concentration);
      }
    }
    if ((hyperx_.terminals > maxTerminals_) ||
        (baseRadix2 > maxRadix_)) {
      break;
    }
  }
}

void Engine::stage3() {
  if (HSE_DEBUG >= 4) {
    printf("3: S=%s T=%lu N=%lu P=%lu\n",
           strop::vecString<u64>(hyperx_.widths).c_str(),
           hyperx_.concentration, hyperx_.terminals,
           hyperx_.routers);
  }

  // find the base radix
  u64 baseRadix = hyperx_.concentration;
  for (u64 dim = 1; dim < hyperx_.dimensions; dim++) {
    baseRadix += hyperx_.widths.at(dim) - 1;
  }
  u64 deltaRadix = maxRadix_ - baseRadix;

  // find the amount of weighting that is within maximum bounds
  std::vector<u64> maxWeights(hyperx_.dimensions, 2);  // minimize to 2
  u64 maxWeight = 0;
  for (u64 dim = 0; dim < hyperx_.dimensions; dim++) {
    u64 m = 1 + (deltaRadix / (hyperx_.widths.at(dim) - 1));
    if (m > maxWeights.at(dim)) {
      maxWeights.at(dim) = m;
    }
    if (maxWeights.at(dim) > maxWeight) {
      maxWeight = maxWeights.at(dim);
    }
  }

  // try finding acceptable weights
  hyperx_.weights.clear();
  hyperx_.weights.resize(hyperx_.dimensions, 1);
  u64 ldim = 0;  // last incremented dimension
  while (true) {
    // compute router radix
    hyperx_.routerRadix = hyperx_.concentration;
    for (u64 dim = 0; dim < hyperx_.dimensions; dim++) {
      hyperx_.routerRadix += ((hyperx_.widths.at(dim) - 1) *
                              hyperx_.weights.at(dim));
    }

    bool tooSmallRadix = (hyperx_.routerRadix < minRadix_);
    bool tooBigRadix = (hyperx_.routerRadix > maxRadix_);

    // test router radix
    if ((tooSmallRadix || tooBigRadix) &&
        (HSE_DEBUG >= 6)) {
      printf("3s: SKIPPING S=%s T=%lu N=%lu P=%lu K=%s R=%lu\n",
             strop::vecString<u64>(hyperx_.widths).c_str(),
             hyperx_.concentration, hyperx_.terminals,
             hyperx_.routers, strop::vecString<u64>(hyperx_.weights).c_str(),
             hyperx_.routerRadix);
    }

    // if not already skipped, compute bisection bandwidth
    bool tooSmallBandwidth = false;
    bool tooBigBandwidth = false;
    if (!tooSmallRadix && !tooBigRadix) {
      hyperx_.bisections.clear();
      hyperx_.bisections.resize(hyperx_.dimensions, 0.0);
      f64 smallestBandwidth = F64_POS_INF;
      f64 largestBandwidth = F64_NEG_INF;
      for (u64 dim = 0; dim < hyperx_.dimensions; dim++) {
        hyperx_.bisections.at(dim) =
            (hyperx_.widths.at(dim) * hyperx_.weights.at(dim)) /
            (2.0 * hyperx_.concentration);
        if (hyperx_.bisections.at(dim) < smallestBandwidth) {
          smallestBandwidth = hyperx_.bisections.at(dim);
        }
        if (hyperx_.bisections.at(dim) > largestBandwidth) {
          largestBandwidth = hyperx_.bisections.at(dim);
        }
      }
      if (smallestBandwidth < minBandwidth_) {
        tooSmallBandwidth = true;
        if (HSE_DEBUG >= 7) {
          printf("3s: SKIPPING S=%s T=%lu N=%lu P=%lu K=%s R=%lu B=%s\n",
                 strop::vecString<u64>(hyperx_.widths).c_str(),
                 hyperx_.concentration, hyperx_.terminals, hyperx_.routers,
                 strop::vecString<u64>(hyperx_.weights).c_str(),
                 hyperx_.routerRadix,
                 strop::vecString<f64>(hyperx_.bisections).c_str());
        }
      } else if (largestBandwidth > maxBandwidth_) {
        tooBigBandwidth = true;
        if (HSE_DEBUG >= 7) {
          printf("3s: SKIPPING S=%s T=%lu N=%lu P=%lu K=%s R=%lu B=%s\n",
                 strop::vecString<u64>(hyperx_.widths).c_str(),
                 hyperx_.concentration, hyperx_.terminals, hyperx_.routers,
                 strop::vecString<u64>(hyperx_.weights).c_str(),
                 hyperx_.routerRadix,
                 strop::vecString<f64>(hyperx_.bisections).c_str());
        }
      }
    }

    // if passed all tests, send to next stage
    if (!tooSmallRadix && !tooBigBandwidth &&
        !tooBigRadix && !tooSmallBandwidth) {
      stage4();
    }

    // detect when done, if the last dimension was incremented then
    //  subsequentally skipped due to too large of router radix
    if ((tooBigRadix) && (ldim == (hyperx_.dimensions - 1))) {
      break;
    }

    // find the next weights configuration
    if (!fixedWeight_) {
      // HyperX
      u64 ndim = U64_MAX;  // next dimension to increment
      for (ndim = 0; ndim < hyperx_.dimensions; ndim++) {
        if (hyperx_.weights.at(ndim) == maxWeights.at(ndim)) {
          continue;
        } else {
          break;
        }
      }
      hyperx_.weights.at(ndim)++;
      ldim = ndim;
      for (u64 d = 0; ndim != 0 && d < ndim - 1; d++) {
        hyperx_.weights.at(d) = hyperx_.weights.at(ndim);
      }
    } else {
      // FbFly
      for (u64 d = 0; d < hyperx_.dimensions; d++) {
        hyperx_.weights.at(d)++;
      }
      ldim = hyperx_.dimensions - 1;
    }
  }
}

void Engine::stage4() {
  for (u64 dim = 1; dim < hyperx_.dimensions; dim++) {
    assert(hyperx_.weights.at(dim) <= hyperx_.weights.at(dim-1));
  }

  if (HSE_DEBUG >= 3) {
    printf("4: S=%s T=%lu N=%lu K=%s B=%s\n",
           strop::vecString<u64>(hyperx_.widths).c_str(),
           hyperx_.concentration, hyperx_.terminals,
           strop::vecString<u64>(hyperx_.weights).c_str(),
           strop::vecString<f64>(hyperx_.bisections).c_str());
  }

  // compute the number of channels
  hyperx_.channels = hyperx_.terminals;
  for (u64 dim = 0; dim < hyperx_.dimensions; dim++) {
    u64 triNum = hyperx_.widths.at(dim);
    triNum = (triNum * (triNum - 1)) / 2;
    u64 dimChannels = hyperx_.weights.at(dim) * triNum;
    for (u64 dim2 = 0; dim2 < hyperx_.dimensions; dim2++) {
      if (dim2 != dim) {
        dimChannels *= hyperx_.widths.at(dim2);
      }
    }
    hyperx_.channels += dimChannels;
  }

  stage5();
}

void Engine::stage5() {
  if (HSE_DEBUG >= 2) {
    printf("5: S=%s T=%lu N=%lu P=%lu K=%s R=%lu B=%s\n",
           strop::vecString<u64>(hyperx_.widths).c_str(),
           hyperx_.concentration, hyperx_.terminals, hyperx_.routers,
           strop::vecString<u64>(hyperx_.weights).c_str(),
           hyperx_.routerRadix,
           strop::vecString<f64>(hyperx_.bisections).c_str());
  }

  hyperx_.cost = costFunction_->cost(hyperx_);

  results_.push_back(hyperx_);
  std::sort(results_.begin(), results_.end(), comparator_);

  if (results_.size() > maxResults_) {
    results_.pop_back();
  }
}
