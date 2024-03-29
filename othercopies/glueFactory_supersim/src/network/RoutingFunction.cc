/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/RoutingFunction.h"

#include <cassert>

#include "event/Simulator.h"

/* RoutingFunction::Response class */

RoutingFunction::Response::Response() {}

RoutingFunction::Response::~Response() {}

void RoutingFunction::Response::clear() {
  response_.clear();
}

void RoutingFunction::Response::add(u32 _port, u32 _vc) {
  response_.push_back({_port, _vc});
}

u32 RoutingFunction::Response::size() const {
  return response_.size();
}

void RoutingFunction::Response::get(u32 _index, u32* _port, u32* _vc) const {
  const std::pair<u32, u32>& val = response_.at(_index);
  *_port = val.first;
  *_vc   = val.second;
}

/* RoutingFunction::Client class */

RoutingFunction::Client::Client() {}

RoutingFunction::Client::~Client() {}

/* RoutingFunction class */

RoutingFunction::RoutingFunction(const std::string& _name,
                                 const Component* _parent, u32 _latency)
    : Component(_name, _parent), latency_(_latency) {
  assert(latency_ > 0);
}

RoutingFunction::~RoutingFunction() {}

u32 RoutingFunction::latency() const {
  return latency_;
}

void RoutingFunction::request(Client* _client, Flit* _flit,
                              Response* _response) {
  u64 respTime = gSim->futureCycle(latency_);
  EventPackage* evt = new EventPackage();
  evt->client = _client;
  evt->flit = _flit;
  evt->response = _response;
  addEvent(respTime, 0, evt, 0);
}

void RoutingFunction::processEvent(void* _event, s32 _type) {
  EventPackage* evt = reinterpret_cast<EventPackage*>(_event);
  processRequest(evt->flit, evt->response);
  evt->client->routingFunctionResponse(evt->response);
  delete evt;
}
