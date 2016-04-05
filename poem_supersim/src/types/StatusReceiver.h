/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_STATUSRECEIVER_H_
#define TYPES_STATUSRECEIVER_H_

#include <prim/prim.h>

class StatusReceiver {
 public:
  StatusReceiver();
  virtual ~StatusReceiver();
  virtual void receiveStatus(u32 _clientId, f64 _percentComplete) = 0;
};

#endif  // TYPES_STATUSRECEIVER_H_
