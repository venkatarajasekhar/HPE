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
#include "traffic/DragonflyWorstCaseTrafficPattern.h"
#include <cassert>

DragonflyWorstCaseTrafficPattern::DragonflyWorstCaseTrafficPattern(
    const std::string& _name, const Component* _parent, u32 _numTerminals,
    u32 _self, Json::Value _settings)
    : TrafficPattern(_name, _parent, _numTerminals, _self) {
  groupSize = _settings["group_size"].asUInt();
  groupNumber = _settings["group_num"].asUInt();
  concentration = _settings["concentration"].asUInt();
}

DragonflyWorstCaseTrafficPattern::~DragonflyWorstCaseTrafficPattern() {}

u32 DragonflyWorstCaseTrafficPattern::nextDestination() {
  u32 dest;
  u32 currentGroup = self/ (groupSize * concentration);
  dest = gSim->rnd.nextU64(0, groupSize - 1);
  assert(currentGroup <= groupNumber - 1);
  if (currentGroup < groupNumber - 1) {
    dest += groupSize * (currentGroup + 1);
  }
  dest *= concentration;
  return dest;
}
