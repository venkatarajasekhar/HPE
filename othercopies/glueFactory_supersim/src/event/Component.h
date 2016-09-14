/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef EVENT_COMPONENT_H_
#define EVENT_COMPONENT_H_

#include <prim/prim.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "event/Simulator.h"

class Component {
 public:
  Component(const std::string& _name, const Component* _parent);
  virtual ~Component();
  void setName(const std::string& _name);
  void prependName(std::string _prefix);
  void appendName(std::string _postfix);
  std::string name() const;
  std::string fullName() const;
  void setParent(const Component* _parent);
  const Component* getParent() const;
  virtual void processEvent(void* _event, s32 _type);
  void setDebug(bool _debug);

  static Component* findComponentByName(std::string _fullName);
  static u64 numComponents();
  static void addDebugName(std::string _fullName);
  static void debugCheck();

 protected:
  void addEvent(u64 _time, u8 _epsilon, void* _event, s32 _type);
  s32 debugPrint(const char* _func, s32 _line, const char* _name,
                 u64 _time, u8 _epsilon, const char* _format, ...) const;
  bool debug_;

 private:
  std::string name_;
  const Component* parent_;

  static std::unordered_map<std::string, Component*> components_;
  static std::unordered_set<std::string> toBeDebugged_;
};

#define dbgprintf(...) (                                                \
    (this->debug_) ?                                                    \
    (this->debugPrint(__func__,                                         \
                      __LINE__,                                         \
                      this->fullName().c_str(),                         \
                      gSim->time(),                                     \
                      gSim->epsilon(),                                  \
                      __VA_ARGS__)) : (0))

#endif  // EVENT_COMPONENT_H_
