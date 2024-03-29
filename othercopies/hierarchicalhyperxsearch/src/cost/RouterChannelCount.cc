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
#include "cost/RouterChannelCount.h"

namespace calc {

RouterChannelCount::RouterChannelCount() {}

RouterChannelCount::~RouterChannelCount() {}

f64 RouterChannelCount::cost(const topos::hierarchicalHyperx::HHyperx&
                             _hHyperx) const {
  f64 globalCost = 0.000001;
  f64 localCost =  0.00000001;
  f64 cost = _hHyperx.routers + _hHyperx.globalChannels * globalCost
             + _hHyperx.localChannels * localCost;
  // if (_hHyperx.globalDim > 1) {
  // cost += ((_hHyperx.globalWidths.at(0) - 1) * _hHyperx.globalWeights.at(0) -
  //         (_hHyperx.globalWidths.at(1) - 1) * _hHyperx.globalWeights.at(1)) *
  //          globalCost;
  // }
  // if (_hHyperx.localDim > 1) {
  // cost += ((_hHyperx.localWidths.at(0) - 1) * _hHyperx.localWeights.at(0) -
  //           (_hHyperx.localWidths.at(1) - 1) * _hHyperx.localWeights.at(1)) *
  //          localCost;
  // }
  return cost;
}

}  // namespace calc
