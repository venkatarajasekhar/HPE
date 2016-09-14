/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/NullTerminal.h"

#include <cassert>

NullTerminal::NullTerminal(const std::string& _name, const Component* _parent,
                           u32 _id, std::vector<u32> _address)
    : Terminal(_name, _parent, _id, _address) {}

NullTerminal::~NullTerminal() {}

void NullTerminal::receiveMessage(Message* _message) {
  assert(false);
}

void NullTerminal::messageEnteredInterface(Message* _message) {
  assert(false);
}

void NullTerminal::messageExitedNetwork(Message* _message) {
  assert(false);
}
