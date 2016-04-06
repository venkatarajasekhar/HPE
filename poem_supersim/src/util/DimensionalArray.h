/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef UTIL_DIMENSIONALARRAY_H_
#define UTIL_DIMENSIONALARRAY_H_

#include <prim/prim.h>

#include <vector>

template <typename T>
class DimensionalArray {
 public:
  DimensionalArray();
  ~DimensionalArray();

  // Warning: this clears all data
  void setSize(const std::vector<u32>& _dimensionWidths);

  u32 size() const;
  u32 dimensionSize(u32 _dimension) const;

  u32 index(const std::vector<u32>& _index) const;
  T& at(const std::vector<u32>& _index);
  T& at(u32 _index);
  const T& at(u32 _index) const;

 private:
  std::vector<T> elements_;
  std::vector<u32> dimensionWidths_;
  std::vector<u32> dimensionScale_;
};

#include "util/DimensionalArray.tcc"

#endif  // UTIL_DIMENSIONALARRAY_H_
