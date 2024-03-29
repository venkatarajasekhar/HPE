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
#ifndef ROUTER_COMMON_CONGESTION_CONGESTIONSTATUS_H_
#define ROUTER_COMMON_CONGESTION_CONGESTIONSTATUS_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"

class Router;

class CongestionStatus : public Component {
 public:
  CongestionStatus(const std::string& _name, const Component* _parent,
                   Router* _router, Json::Value _settings);
  virtual ~CongestionStatus();

  // configuration
  void initCredits(u32 _port, u32 _vc, u32 _credits);

  // operation
  void incrementCredit(u32 _port, u32 _vc);  // a credit came from downstream
  void decrementCredit(u32 _port, u32 _vc);  // a credit was consumed locally

  // this returns congestion status (i.e. 0=empty 1=congested)
  f64 status(u32 _port, u32 _vc) const;  // (must be epsilon >= 1)

  void processEvent(void* _event, s32 _type);

  // this class NEEDS to use these two event types. subclasses also using
  //  events must pass these two types on to this class.
  static const s32 INCR = 0x50;
  static const s32 DECR = 0xAF;

 protected:
  Router* router_;
  const u32 numPorts_;
  const u32 numVcs_;

  virtual void performInitCredits(u32 _port, u32 _vc, u32 _credits) = 0;
  virtual void performIncrementCredit(u32 _port, u32 _vc) = 0;
  virtual void performDecrementCredit(u32 _port, u32 _vc) = 0;

  // this MUST return a value >= 0.0 and <= 1.0
  virtual f64 computeStatus(u32 _port, u32 _vc) const = 0;

 private:
  void createEvent(u32 _port, u32 _vc, s32 _type);

  const u32 latency_;
  const u32 granularity_;
};

#endif  // ROUTER_COMMON_CONGESTION_CONGESTIONSTATUS_H_
