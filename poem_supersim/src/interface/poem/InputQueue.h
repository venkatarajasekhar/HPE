/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_POEM_INPUTQUEUE_H_
#define INTERFACE_POEM_INPUTQUEUE_H_

#include <prim/prim.h>

#include <string>
#include <queue>
#include <vector>

#include "event/Component.h"
#include "types/Flit.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"

namespace Poem {

class Interface;

class InputQueue : public Component, public CrossbarScheduler::Client {
 public:
  InputQueue(const std::string& _name, Interface* _interface,
             CrossbarScheduler* _crossbarScheduler, u32 _crossbarSchedulerIndex,
             Crossbar* _crossbar, u32 _crossbarIndex, u32 _vc);
  ~InputQueue();

  void receiveFlit(Flit* _flit);
  void crossbarSchedulerResponse(u32 _port, u32 _vc) override;

  void processEvent(void* _event, s32 _type) override;

 private:
  CrossbarScheduler* crossbarScheduler_;
  u32 crossbarSchedulerIndex_;
  Crossbar* crossbar_;
  u32 crossbarIndex_;
  Interface* interface_;
  u32 vc_;

  // buffer
  class DeadlineComparator {
   public:
    DeadlineComparator();
    ~DeadlineComparator();
    bool operator()(const Flit* _lhs, const Flit* _rhs) const;
  };
  std::priority_queue<Flit*, std::vector<Flit*>, DeadlineComparator> buffer_;
};

}  // namespace Poem

#endif  // INTERFACE_POEM_INPUTQUEUE_H_
