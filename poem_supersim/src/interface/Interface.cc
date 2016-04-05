/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "interface/Interface.h"

#include <cassert>

Interface::Interface(const std::string& _name, const Component* _parent,
                     u32 _id, Json::Value _settings)
    : Component(_name, _parent), id_(_id),
      numVcs_(_settings["num_vcs"].asUInt()) {
  assert(numVcs_ > 0);
}

Interface::~Interface() {}

u32 Interface::getId() const {
  return id_;
}

u32 Interface::numVcs() const {
  return numVcs_;
}

void Interface::setMessageReceiver(MessageReceiver* _receiver) {
  messageReceiver_ = _receiver;
}

MessageReceiver* Interface::getMessageReceiver() const {
  return messageReceiver_;
}
