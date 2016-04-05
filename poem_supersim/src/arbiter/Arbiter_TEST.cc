/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "arbiter/Arbiter_TEST.h"

u32 hotCount(bool* _bools, u32 _len) {
  u32 cnt = 0;
  for (u32 idx = 0; idx < _len; idx++) {
    if (_bools[idx]) {
      cnt++;
    }
  }
  return cnt;
}

u32 winnerId(bool* _bools, u32 _len) {
  for (u32 idx = 0; idx < _len; idx++) {
    if (_bools[idx]) {
      return idx;
    }
  }
  return U32_MAX;
}
