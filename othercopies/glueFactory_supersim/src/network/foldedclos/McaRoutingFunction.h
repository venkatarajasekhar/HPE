/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef NETWORK_FOLDEDCLOS_MCAROUTINGFUNCTION_H_
#define NETWORK_FOLDEDCLOS_MCAROUTINGFUNCTION_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "network/RoutingFunction.h"
#include "router/Router.h"

namespace FoldedClos {

class McaRoutingFunction : public RoutingFunction {
 public:
  McaRoutingFunction(const std::string& _name, const Component* _parent,
                     u64 _latency, Router* _router, u32 _numVcs, u32 _numPorts,
                     u32 _numLevels, u32 _level, u32 _inputPort, bool _allVcs);
  ~McaRoutingFunction();

 protected:
  void processRequest(
      Flit* _flit, RoutingFunction::Response* _response) override;

 private:
  Router* router_;
  u32 numVcs_;
  u32 numPorts_;
  u32 numLevels_;
  u32 level_;
  u32 inputPort_;
  bool allVcs_;
};

}  // namespace FoldedClos

#endif  // NETWORK_FOLDEDCLOS_MCAROUTINGFUNCTION_H_
