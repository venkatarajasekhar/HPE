/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_CONTROLRECEIVER_H_
#define TYPES_CONTROLRECEIVER_H_

#include <prim/prim.h>

#include "types/Control.h"

class ControlReceiver {
 public:
  ControlReceiver();
  virtual ~ControlReceiver();
  virtual void receiveControl(u32 _port, Control* _control) = 0;
};

#endif  // TYPES_CONTROLRECEIVER_H_
