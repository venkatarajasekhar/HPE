/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef EVENT_VECTORQUEUE_H_
#define EVENT_VECTORQUEUE_H_

#include <json/json.h>
#include <prim/prim.h>

#include <queue>
#include <vector>

#include "event/Simulator.h"
#include "event/Component.h"

class VectorQueue : public Simulator {
 public:
  explicit VectorQueue(Json::Value _settings);
  ~VectorQueue();
  void addEvent(u64 _time, u8 _epsilon, Component* _component, void* _event,
                s32 _type) override;
  u64 queueSize() const override;

 protected:
  void runNextEvent() override;

 private:
  class EventBundle {
   public:
    u64 time;
    u8 epsilon;
    Component* component;
    void* event;
    s32 type;
  };

  class EventBundleComparator {
   public:
    EventBundleComparator();
    ~EventBundleComparator();
    bool operator()(const EventBundle _lhs, const EventBundle _rhs) const;
  };

  std::priority_queue<EventBundle, std::vector<EventBundle>,
                      EventBundleComparator> eventQueue_;
};

#endif  // EVENT_VECTORQUEUE_H_
