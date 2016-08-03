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
#ifndef TRAFFIC_DRAGONFLYWC_H_
#define TRAFFIC_DRAGONFLYWC_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "traffic/TrafficPattern.h"

class DragonflyWC : public TrafficPattern {
 public:
  DragonflyWC(const std::string& _name, const Component* _parent,
                            u32 _numTerminals, u32 _self,
                            Json::Value _settings);
  ~DragonflyWC();
  u32 nextDestination() override;
 protected:
  u32 groupSize;
  u32 groupNumber;
  u32 concentration;
};

#endif  // TRAFFIC_DRAGONFLYWC_H_
