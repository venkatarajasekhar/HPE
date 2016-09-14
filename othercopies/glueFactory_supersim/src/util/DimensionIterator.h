/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef UTIL_DIMENSIONITERATOR_H_
#define UTIL_DIMENSIONITERATOR_H_

#include <prim/prim.h>

#include <vector>

class DimensionIterator {
 public:
  explicit DimensionIterator(std::vector<u32> _widths);
  ~DimensionIterator();
  bool next(std::vector<u32>* _address);
  void reset();

 private:
  std::vector<u32> widths_;
  std::vector<u32> state_;
  bool more_;
};

#endif  // UTIL_DIMENSIONITERATOR_H_
