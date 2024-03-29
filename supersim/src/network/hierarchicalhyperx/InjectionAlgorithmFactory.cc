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
#include "network/hierarchicalhyperx/InjectionAlgorithmFactory.h"

#include <cassert>

#include "network/hierarchicalhyperx/AnyInjectionAlgorithm.h"
#include "network/InjectionAlgorithm.h"

namespace HierarchicalHyperX {

InjectionAlgorithmFactory::InjectionAlgorithmFactory(
    u32 _numVcs, Json::Value _settings)
    : ::InjectionAlgorithmFactory(), numVcs_(_numVcs), settings_(_settings) {}

InjectionAlgorithmFactory::~InjectionAlgorithmFactory() {}

InjectionAlgorithm* InjectionAlgorithmFactory::createInjectionAlgorithm(
    const std::string& _name, const Component* _parent, Interface* _interface) {
  std::string algorithm = settings_["algorithm"].asString();
  u32 latency = settings_["latency"].asUInt();

  if (algorithm == "dimension_order") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "valiant") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "global_random") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "global_local_random") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "min_adaptive") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "progressive_adaptive") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else if (algorithm == "progressive_adaptive_g") {
    return new HierarchicalHyperX::AnyInjectionAlgorithm(
        _name, _parent, _interface, latency, numVcs_);
  } else {
    fprintf(stderr, "Unknown injection algorithm: '%s'\n", algorithm.c_str());
    assert(false);
  }
}

}  // namespace HierarchicalHyperX
