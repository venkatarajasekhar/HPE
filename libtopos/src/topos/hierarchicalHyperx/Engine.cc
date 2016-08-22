/*
 * Copyright (c) 2015, Nic McDonald
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
#include "topos/hierarchicalHyperx/Engine.h"

#include <strop/strop.h>

#include <cassert>

#include <algorithm>
#include <stdexcept>

namespace topos {
namespace hierarchicalHyperx {

static const u8 HSE_DEBUG = 0;

CostFunction::CostFunction() {}
CostFunction::~CostFunction() {}

bool Comparator::operator()(const HHyperx& _lhs,
                            const HHyperx& _rhs) const {
  return _rhs.cost > _lhs.cost;
}

Engine::Engine(u64 _minLocalDim, u64 _maxLocalDim, u64 _minGlobalDim,
               u64 _maxGlobalDim, u64 _minRadix,
               u64 _maxRadix, u64 _minConcentration, u64 _maxConcentration,
               u64 _minTerminals, u64 _maxTerminals, f64 _minBandwidth,
               bool _fixedWidth, bool _fixedWeight, u64 _maxResults,
               const CostFunction* _costFunction)
    : minLocalDim_(_minLocalDim),
      maxLocalDim_(_maxLocalDim),
      minGlobalDim_(_minGlobalDim),
      maxGlobalDim_(_maxGlobalDim),
      minRadix_(_minRadix),
      maxRadix_(_maxRadix),
      minConcentration_(_minConcentration),
      maxConcentration_(_maxConcentration),
      minTerminals_(_minTerminals),
      maxTerminals_(_maxTerminals),
      minBandwidth_(_minBandwidth),
      fixedWidth_(_fixedWidth),
      fixedWeight_(_fixedWeight),
      maxResults_(_maxResults),
      costFunction_(_costFunction) {
  if (minLocalDim_ < 1 || minGlobalDim_ < 1) {
    throw std::runtime_error("mindimensions must be greater than 0");
  } else if (maxLocalDim_ < minLocalDim_ || maxGlobalDim_ < minGlobalDim_) {
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
  }
}

Engine::~Engine() {}

void Engine::run() {
  hHyperx_ = HHyperx();
  results_.clear();

  stage1();
}

const std::deque<HHyperx>& Engine::results() const {
  return results_;
}

void Engine::stage1() {
  /*
   * loop over the number of local and global dimensions
   */
  for (hHyperx_.globalDim = minGlobalDim_;
       hHyperx_.globalDim <= maxGlobalDim_;
       hHyperx_.globalDim++) {
    for (hHyperx_.localDim = minLocalDim_;
         hHyperx_.localDim <= maxLocalDim_;
         hHyperx_.localDim++) {
      for (hHyperx_.concentration = minConcentration_;
           hHyperx_.concentration <= maxConcentration_;
           hHyperx_.concentration++) {
        hHyperx_.localLinks = ceil(2 * hHyperx_.localDim *
                                     hHyperx_.concentration * minBandwidth_
                                     * (hHyperx_.globalDim + 1));
        hHyperx_.globalLinks = ceil(2 * hHyperx_.globalDim *
                                      hHyperx_.concentration * minBandwidth_);
        hHyperx_.routerRadix = hHyperx_.localLinks + hHyperx_.globalLinks
                               + hHyperx_.concentration;

        // check radix requirment
        if (hHyperx_.routerRadix >= minRadix_ &&
            hHyperx_.routerRadix <= maxRadix_) {
          /*
           * generate possible dimension widths
          */
          hHyperx_.localWidths.clear();
          hHyperx_.localWidths.resize(hHyperx_.localDim, 2);

          // find the maximum width of local dimension
          u64 maxLocalWidth = hHyperx_.localLinks - hHyperx_.localDim + 2;
          if (maxLocalWidth < 2) {
            break;
          }

          stage2();
        }
      }  // end of concentration loop
    }  // end of localDim loop
  }  // end of globalDim loop
}

void Engine::stage2() {
  u64 maxLocalWidth = hHyperx_.localLinks - hHyperx_.localDim + 2;
  // loop through local widths
  while (true) {
    // sanity check of local link number
    u64 totalLocalLinks = 0;
    for (u64 localD = 0; localD < hHyperx_.localDim; localD++) {
      totalLocalLinks += hHyperx_.localWidths.at(localD) - 1;
    }
    if (totalLocalLinks <= hHyperx_.localLinks) {
      // generate possible global widths
      hHyperx_.globalWidths.clear();
      hHyperx_.globalWidths.resize(hHyperx_.globalDim, 2);

      // determine the number of routers
      hHyperx_.routersPerGroup = 1;
      for (u64 localD = 0; localD < hHyperx_.localDim; localD++) {
        hHyperx_.routersPerGroup *= hHyperx_.localWidths.at(localD);
      }

      // find maximum width of global dimension
      u64 maxGlobalWidth = hHyperx_.globalLinks * hHyperx_.routersPerGroup
                           - hHyperx_.globalDim + 2;
      if (maxGlobalWidth < 2) {
        break;
      }
      stage3();
    }

    // detect when done
    if (hHyperx_.localWidths.at(0) == maxLocalWidth) {
      break;
    }
    // find the next widths configuration
    u64 nLocalDim = U64_MAX;  // next dimension to increment
    for (u64 invdim = 0; invdim < hHyperx_.localDim; invdim++) {
      nLocalDim = hHyperx_.localDim - invdim - 1;
      if (hHyperx_.localWidths.at(nLocalDim) == maxLocalWidth) {
        continue;
      } else {
        break;
      }
    }
    // increment this dimension
    hHyperx_.localWidths.at(nLocalDim)++;
    // all inferior dimensions reset
    for (u64 d = nLocalDim + 1; d < hHyperx_.localDim; d++) {
      hHyperx_.localWidths.at(d) = hHyperx_.localWidths.at(nLocalDim);
    }
  }  // end of local widths loop
}

void Engine::stage3() {
  u64 maxGlobalWidth = hHyperx_.globalLinks * hHyperx_.routersPerGroup
                       - hHyperx_.globalDim + 2;

  // loop through global widths
  while (true) {
    // sanity check of global link number
    u64 totalGlobalLinks = 0;
    for (u64 globalD = 0; globalD < hHyperx_.globalDim; globalD++) {
      totalGlobalLinks += hHyperx_.globalWidths.at(globalD) - 1;
    }
    if (totalGlobalLinks <= hHyperx_.globalLinks * hHyperx_.routersPerGroup) {
      hHyperx_.routers = hHyperx_.routersPerGroup;
      for (u64 globalD = 0; globalD < hHyperx_.globalDim; globalD++) {
        hHyperx_.routers *= hHyperx_.globalWidths.at(globalD);
      }
      hHyperx_.terminals = hHyperx_.routers * hHyperx_.concentration;

      // check the number of terminals
      if ((hHyperx_.terminals <= maxTerminals_) &&
          (hHyperx_.terminals >= minTerminals_)) {
        // if this configuration appears to work so far, use it
        stage4();
      }
    }
    // detect when done
    if (hHyperx_.globalWidths.at(0) == maxGlobalWidth) {
      break;
    }
    // find the next widths configuration
    u64 nGlobalDim = U64_MAX;  // next dimension to increment
    for (u64 invdim = 0; invdim < hHyperx_.globalDim; invdim++) {
      nGlobalDim = hHyperx_.globalDim - invdim - 1;
      if (hHyperx_.globalWidths.at(nGlobalDim) == maxGlobalWidth) {
        continue;
      } else {
        break;
      }
    }
    // increment this dimension
    hHyperx_.globalWidths.at(nGlobalDim)++;
    // all inferior dimensions reset
    for (u64 d = nGlobalDim + 1; d < hHyperx_.globalDim; d++) {
      hHyperx_.globalWidths.at(d) = hHyperx_.globalWidths.at(nGlobalDim);
    }
  }  // end of global width loop
}

void Engine::stage4() {
  u64 maxLocalWidth = hHyperx_.localLinks - hHyperx_.localDim + 2;
  // find appropriate local weights
  hHyperx_.localWeights.clear();
  hHyperx_.localWeights.resize(hHyperx_.localDim, 1);
  u64 maxLocalWeight = maxLocalWidth - 1;

  // loop through local weights
  while (true) {
    // printf("In local weight loop \n");
    // sanity check of local link number
    u64 totalLocalLinks = 0;
    for (u64 localD = 0; localD < hHyperx_.localDim; localD++) {
      totalLocalLinks += (hHyperx_.localWidths.at(localD) - 1)
                         * hHyperx_.localWeights.at(localD);
    }
    if (totalLocalLinks == hHyperx_.localLinks) {
      if (hHyperx_.localDim > 1 && hHyperx_.globalDim == 1) {
        // fatter link horizontally as for DOR
        if (hHyperx_.localWeights.at(0) == hHyperx_.localWidths.at(1)
                                         * hHyperx_.localWeights.at(1)) {
          stage5();
         }
      } else if (hHyperx_.globalDim == 2  && hHyperx_.localDim == 2) {
        u64 firstDimLinks = (hHyperx_.localWidths.at(0) - 1) *
                             hHyperx_.localWeights.at(0);
        u64 secondDimLinks = (hHyperx_.localWidths.at(1) - 1) *
                             hHyperx_.localWeights.at(1);
        if (firstDimLinks == secondDimLinks) {
          stage5();
        }
      } else {
        stage5();
      }
    }

    // detect when done
    if (hHyperx_.localWeights.at(0) == maxLocalWeight) {
      break;
    }
    // find the next widths configuration
    u64 nLocalWeight = U64_MAX;  // next weight to increment
    for (u64 invdim = 0; invdim < hHyperx_.localDim; invdim++) {
      nLocalWeight = hHyperx_.localDim - invdim - 1;
      if (hHyperx_.localWeights.at(nLocalWeight) == maxLocalWeight) {
        continue;
      } else {
        break;
      }
    }
    // increment this dimension
    hHyperx_.localWeights.at(nLocalWeight)++;
    // all inferior dimensions reset
    for (u64 d = nLocalWeight + 1; d < hHyperx_.localDim; d++) {
      hHyperx_.localWeights.at(d) = 1;
    }
  }  // end of local weight loop
}


void Engine::stage5() {
  u64 maxGlobalWidth = hHyperx_.globalLinks * hHyperx_.routersPerGroup
                       - hHyperx_.globalDim + 2;
  // find appropriate global weights
  hHyperx_.globalWeights.clear();
  hHyperx_.globalWeights.resize(hHyperx_.globalDim, 1);
  u64 maxGlobalWeight = maxGlobalWidth - 1;

  // loop through global weights
  while (true) {
    // printf("In global weight loop \n");
    // sanity check of global link number
    u64 totalGlobalLinks = 0;
    for (u64 globalD = 0; globalD < hHyperx_.globalDim; globalD++) {
      totalGlobalLinks += (hHyperx_.globalWidths.at(globalD) - 1)
                          * hHyperx_.globalWeights.at(globalD);
    }
    if (totalGlobalLinks == hHyperx_.globalLinks * hHyperx_.routersPerGroup) {
      if (hHyperx_.globalDim > 1 && hHyperx_.localDim == 1) {
        // compensate for stronger demands horizontally
        if (hHyperx_.globalWeights.at(0) == hHyperx_.globalWidths.at(1)
                                         * hHyperx_.globalWeights.at(1)) {
          stage6();
        }
      } else if (hHyperx_.globalDim == 2  && hHyperx_.localDim == 2) {
        u64 firstDimLinks = (hHyperx_.globalWidths.at(0) - 1) *
                            hHyperx_.globalWeights.at(0);
        u64 secondDimLinks = (hHyperx_.globalWidths.at(1) - 1) *
                             hHyperx_.globalWeights.at(1);
        // optimize for random dim routing
        if (firstDimLinks == secondDimLinks) {
            // if (  // ((hHyperx_.globalWidths.at(0) - 1)
            // * hHyperx_.globalWeights.at(0))
            // % hHyperx_.routersPerGroup == 0 &&
            // fatter global link horizontally
            //  hHyperx_.globalWeights.at(0) == hHyperx_.globalWidths.at(1)
            // * hHyperx_.globalWeights.at(1)) {
          stage6();
        }
      } else {
        stage6();
      }
    }

    // detect when done
    if (hHyperx_.globalWeights.at(0) == maxGlobalWeight) {
      break;
    }
    // find the next configuration
    u64 nGlobalWeight = U64_MAX;  // next weight to increment
    for (u64 invdim = 0; invdim < hHyperx_.globalDim; invdim++) {
      nGlobalWeight = hHyperx_.globalDim - invdim - 1;
      if (hHyperx_.globalWeights.at(nGlobalWeight) == maxGlobalWeight) {
        continue;
      } else {
        break;
      }
    }
    // increment this dimension
    hHyperx_.globalWeights.at(nGlobalWeight)++;
    // all inferior dimensions reset
    for (u64 d = nGlobalWeight + 1; d < hHyperx_.globalDim; d++) {
      hHyperx_.globalWeights.at(d) = 1;
    }
  }  // end of global weight loop
}

void Engine::stage6() {
  for (u64 localD = 1; localD < hHyperx_.localDim; localD++) {
    assert(hHyperx_.localWidths.at(localD) >=
      hHyperx_.localWidths.at(localD-1));
  }
  for (u64 globalD = 1; globalD < hHyperx_.globalDim; globalD++) {
    assert(hHyperx_.globalWidths.at(globalD) >=
      hHyperx_.globalWidths.at(globalD-1));
  }

  // compute the number of channels
  hHyperx_.channels = hHyperx_.terminals;
  hHyperx_.localChannels = hHyperx_.terminals * hHyperx_.localLinks;
  hHyperx_.globalChannels = hHyperx_.terminals * hHyperx_.globalLinks;
  hHyperx_.channels += hHyperx_.localChannels + hHyperx_.globalChannels;

  hHyperx_.cost = costFunction_->cost(hHyperx_);

  results_.push_back(hHyperx_);
  std::sort(results_.begin(), results_.end(), comparator_);

  if (results_.size() > maxResults_) {
    results_.pop_back();
  }
}
}  // namespace hierarchicalHyperx
}  // namespace topos
