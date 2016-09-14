/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "types/Credit.h"

#include <cassert>

Credit::Credit(u32 _nums) {
  assert(_nums > 0);
  numNums_ = _nums;
  putPos_ = 0;
  getPos_ = 0;
  nums_ = new u32[_nums];
}

Credit::~Credit() {
  delete[] nums_;
}

bool Credit::more() const {
  return getPos_ < putPos_;
}

void Credit::putNum(u32 _num) {
  assert(putPos_ < numNums_);
  nums_[putPos_++] = _num;
}

u32 Credit::getNum() {
  assert(getPos_ < numNums_);
  return nums_[getPos_++];
}
