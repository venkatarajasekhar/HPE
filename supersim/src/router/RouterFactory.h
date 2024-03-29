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
#ifndef ROUTER_ROUTERFACTORY_H_
#define ROUTER_ROUTERFACTORY_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "network/RoutingAlgorithmFactory.h"
#include "router/Router.h"

class RouterFactory {
 public:
  static Router* createRouter(
      const std::string& _name, const Component* _parent,
      const std::vector<u32>& _address,
      RoutingAlgorithmFactory* _routingAlgorithmFactory,
      MetadataHandler* _metadataHandler, Json::Value _settings);
};

#endif  // ROUTER_ROUTERFACTORY_H_
