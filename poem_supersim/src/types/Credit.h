/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_CREDIT_H_
#define TYPES_CREDIT_H_

#include <prim/prim.h>

#include "types/Control.h"

class Credit : public Control {
 public:
  explicit Credit(u32 _nums);
  ~Credit();
  bool more() const;
  void putNum(u32 _num);
  u32 getNum();

 private:
  u32 numNums_;
  u32 putPos_;
  u32 getPos_;
  u32* nums_;
};

#endif  // TYPES_CREDIT_H_
