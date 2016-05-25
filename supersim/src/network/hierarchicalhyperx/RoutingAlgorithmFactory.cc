/*
 * Copyright 2016 Hewlett Packard Enterprise Development LP
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "network/hierarchicalhyperx/RoutingAlgorithmFactory.h"

#include <cassert>

#include "network/hierarchicalhyperx/DimOrderRoutingAlgorithm.h"
#include "network/hierarchicalhyperx/ValiantRoutingAlgorithm.h"
#include "network/hierarchicalhyperx/MinAdaptiveRoutingAlgorithm.h"
#include "network/RoutingAlgorithm.h"

namespace HierarchicalHyperX {

RoutingAlgorithmFactory::RoutingAlgorithmFactory(
    u32 _numVcs, const std::vector<u32>& _globalDimensionWidths,
    const std::vector<u32>& _globalDimensionWeights,
    const std::vector<u32>& _localDimensionWidths,
    const std::vector<u32>& _localDimensionWeights,
    u32 _globalLinksPerRouter, u32 _concentration,
    Json::Value _settings)
    : ::RoutingAlgorithmFactory(), numVcs_(_numVcs),
      globalDimensionWidths_(_globalDimensionWidths),
      globalDimensionWeights_(_globalDimensionWeights),
      localDimensionWidths_(_localDimensionWidths),
      localDimensionWeights_(_localDimensionWeights),
      globalLinksPerRouter_(_globalLinksPerRouter),
      concentration_(_concentration), settings_(_settings) {}

RoutingAlgorithmFactory::~RoutingAlgorithmFactory() {}

RoutingAlgorithm* RoutingAlgorithmFactory::createRoutingAlgorithm(
    const std::string& _name, const Component* _parent, Router* _router,
    u32 _inputPort) {
  std::string algorithm = settings_["algorithm"].asString();
  u32 latency = settings_["latency"].asUInt();
  // bool allVcs = settings_["all_vcs"].asBool();

  if (algorithm == "dimension_order") {
    return new HierarchicalHyperX::DimOrderRoutingAlgorithm(
        _name, _parent, latency, _router, numVcs_, globalDimensionWidths_,
        globalDimensionWeights_, localDimensionWidths_, localDimensionWeights_,
        concentration_, globalLinksPerRouter_);
  } else if (algorithm == "valiant") {
    return new HierarchicalHyperX::ValiantRoutingAlgorithm(
        _name, _parent, latency, _router,  numVcs_, globalDimensionWidths_,
        globalDimensionWeights_, localDimensionWidths_, localDimensionWeights_,
        concentration_, globalLinksPerRouter_);
  } else if (algorithm == "min_adaptive") {
    return new HierarchicalHyperX::MinAdaptiveRoutingAlgorithm(
        _name, _parent, latency, _router,  numVcs_, globalDimensionWidths_,
        globalDimensionWeights_, localDimensionWidths_, localDimensionWeights_,
        concentration_, globalLinksPerRouter_);
  } else {
    fprintf(stderr, "Unknown routing algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace HierarchicalHyperX