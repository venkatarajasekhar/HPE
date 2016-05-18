/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "event/Component.h"

#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <utility>

#include "event/Simulator.h"

// this is some weird C++ syntax declaration of previously declared
//  static member variables.
std::unordered_map<std::string, Component*> Component::components_;
std::unordered_set<std::string> Component::toBeDebugged_;

Component::Component(const std::string& _name, const Component* _parent)
    : debug_(false), name_(_name), parent_(_parent) {
  if (components_.insert({fullName(), this}).second == false) {
    fprintf(stderr, "duplicate component name detected: %s\n",
            fullName().c_str());
    assert(false);
  }
  if (toBeDebugged_.count(fullName()) == 1) {
    setDebug(true);
    assert(toBeDebugged_.erase(fullName()) == 1);
  }
}

Component::~Component() {
  assert(Component::components_.erase(fullName()) == 1);
}

void Component::setName(const std::string& _name) {
  name_ = _name;
}

void Component::prependName(std::string _prefix) {
  name_ = _prefix + name_;
}

void Component::appendName(std::string _postfix) {
  name_ = name_ + _postfix;
}

std::string Component::name() const {
  return name_;
}

std::string Component::fullName() const {
  if (parent_) {
    return parent_->fullName() + "." + name_;
  } else {
    return name_;
  }
}

void Component::setParent(const Component* _parent) {
  parent_ = _parent;
}

const Component* Component::getParent() const {
  return parent_;
}

void Component::addEvent(u64 _time, u8 _epsilon, void* _event, s32 _type) {
  gSim->addEvent(_time, _epsilon, this, _event, _type);
}

void Component::processEvent(void* _event, s32 _type) {
  assert(false);  // this function should be overridden if it is to be used
}

void Component::setDebug(bool _debug) {
  debug_ = _debug;
}

s32 Component::debugPrint(const char* _func, s32 _line, const char* _name,
                          u64 _time, u8 _epsilon, const char* _format,
                          ...) const {
  printf("[%lu:%u] %s Func=%s Line=%i: ", _time, _epsilon, _name, _func, _line);
  va_list args;
  va_start(args, _format);
  vprintf(_format, args);
  printf("\n");
  va_end(args);
  return 0;
}

Component* Component::findComponentByName(std::string _fullName) {
  auto iter = components_.find(_fullName);
  if (iter == components_.end()) {
    return nullptr;
  }
  return iter->second;
}

u64 Component::numComponents() {
  return components_.size();
}

void Component::addDebugName(std::string _fullname) {
  assert(toBeDebugged_.insert(_fullname).second);
}

void Component::debugCheck() {
  for (auto it = toBeDebugged_.begin(); it != toBeDebugged_.end(); ++it) {
    fprintf(stderr, "%s is an invalid component name\n", it->c_str());
  }
  assert(toBeDebugged_.size() == 0);
  toBeDebugged_.reserve(0);
}
