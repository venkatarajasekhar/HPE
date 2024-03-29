/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <prim/prim.h>

#include <vector>
#include "util/DimensionalArray.h"
#include "util/DimensionIterator.h"

#include "gtest/gtest.h"

TEST(DimensionalArray, index) {
  DimensionalArray<u64>* da = new DimensionalArray<u64>();

  std::vector<u32> dimWidths{3, 2, 3};
  da->setSize(dimWidths);

  u32 idx;

  std::vector<u32> index1{1, 1, 1};
  idx = da->index(index1);
  ASSERT_EQ(idx, 10u);

  std::vector<u32> index2{0, 0, 1};
  idx = da->index(index2);
  ASSERT_EQ(idx, 6u);

  std::vector<u32> index3{2, 0, 0};
  idx = da->index(index3);
  ASSERT_EQ(idx, 2u);

  std::vector<u32> index4{0, 1, 2};
  idx = da->index(index4);
  ASSERT_EQ(idx, 15u);

  delete da;
}

TEST(DimensionalArray, store) {
  DimensionalArray<u64>* da = new DimensionalArray<u64>();

  std::vector<u32> dimWidths{3, 2, 3};
  da->setSize(dimWidths);

  std::vector<u32> index{0, 0, 0};
  DimensionIterator it(dimWidths);
  u64 count = 1000;
  while (it.next(&index)) {
    da->at(index) = count;
    count++;
  }

  it.reset();
  count = 1000;
  while (it.next(&index)) {
    ASSERT_EQ(da->at(index), count);
    count++;
  }

  delete da;
}

TEST(DimensionalArray, order1) {
  DimensionalArray<u64>* da = new DimensionalArray<u64>();

  std::vector<u32> dimWidths{2, 3, 4};
  da->setSize(dimWidths);

  for (u32 i = 0; i < da->size(); i++) {
    da->at(i) = i;
  }

  std::vector<u32> index{0, 0, 0};
  DimensionIterator it(dimWidths);
  for (u32 i = 0; it.next(&index); i++) {
    ASSERT_EQ(da->at(index), da->at(i));
    ASSERT_EQ(da->at(index), i);
  }

  delete da;
}
