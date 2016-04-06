/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_FLITRECEIVER_H_
#define TYPES_FLITRECEIVER_H_

#include <prim/prim.h>

#include "types/Flit.h"

class FlitReceiver {
 public:
  FlitReceiver();
  virtual ~FlitReceiver();
  virtual void receiveFlit(u32 _port, Flit* _flit) = 0;
};

#endif  // TYPES_FLITRECEIVER_H_
