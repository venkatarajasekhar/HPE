/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_NULLTERMINAL_H_
#define APPLICATION_NULLTERMINAL_H_

#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "application/Terminal.h"

class NullTerminal : public Terminal {
 public:
  NullTerminal(const std::string& _name, const Component* _parent, u32 _id,
               std::vector<u32> _address);
  virtual ~NullTerminal();
  void receiveMessage(Message* _message) override;
  void messageEnteredInterface(Message* _message) override;
  void messageExitedNetwork(Message* _message) override;
};

#endif  // APPLICATION_NULLTERMINAL_H_
