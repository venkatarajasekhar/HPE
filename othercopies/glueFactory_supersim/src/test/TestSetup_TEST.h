/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TEST_TESTSETUP_TEST_H_
#define TEST_TESTSETUP_TEST_H_

#include <prim/prim.h>

class TestSetup {
 public:
  TestSetup(u64 _cycleTime, u64 _randomSeed);
  ~TestSetup();
};

#endif  // TEST_TESTSETUP_TEST_H_
