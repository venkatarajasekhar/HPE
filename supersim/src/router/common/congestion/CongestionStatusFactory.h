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
#ifndef ROUTER_COMMON_CONGESTION_CONGESTIONSTATUSFACTORY_H_
#define ROUTER_COMMON_CONGESTION_CONGESTIONSTATUSFACTORY_H_

#include <json/json.h>

#include <string>

#include "event/Component.h"
#include "router/common/congestion/CongestionStatus.h"

class CongestionStatusFactory {
 public:
  static CongestionStatus* createCongestionStatus(
      const std::string& _name, const Component* _parent, Router* _router,
      Json::Value _settings);
};

#endif  // ROUTER_COMMON_CONGESTION_CONGESTIONSTATUSFACTORY_H_
