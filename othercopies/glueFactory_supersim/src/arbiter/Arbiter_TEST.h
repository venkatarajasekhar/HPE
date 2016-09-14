/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ARBITER_ARBITER_TEST_H_
#define ARBITER_ARBITER_TEST_H_

#include <prim/prim.h>

#include <vector>

#include "arbiter/Arbiter.h"

u32 hotCount(bool* _bools, u32 _len);
u32 winnerId(bool* _bools, u32 _len);

#endif  // ARBITER_ARBITER_TEST_H_
