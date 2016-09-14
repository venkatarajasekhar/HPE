/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include <stdexcept>
#include <vector>

template <typename T>
DimensionalArray<T>::DimensionalArray() {}

template <typename T>
DimensionalArray<T>::~DimensionalArray() {}

template <typename T>
void DimensionalArray<T>::setSize(const std::vector<u32>& _dimensionWidths) {
  // determine the total number of element
  u32 totalSize = 0;
  if (_dimensionWidths.size() > 0) {
    totalSize++;
    for (auto it = _dimensionWidths.begin(); it != _dimensionWidths.end();
         ++it) {
      totalSize *= *it;
    }
  }

  // clear and resize
  elements_.clear();
  elements_.resize(totalSize);

  // copy dimension widths
  dimensionWidths_ = _dimensionWidths;

  // set the dimension scale
  dimensionScale_.resize(dimensionWidths_.size());
  u32 scale = totalSize;
  for (s32 i = dimensionWidths_.size() - 1; i >= 0; i--) {
    scale /= dimensionWidths_.at(i);
    dimensionScale_.at(i) = scale;
  }
}

template <typename T>
u32 DimensionalArray<T>::size() const {
  return elements_.size();
}

template <typename T>
u32 DimensionalArray<T>::dimensionSize(u32 _dimension) const {
  return dimensionWidths_.at(_dimension);
}

template <typename T>
u32 DimensionalArray<T>::index(const std::vector<u32>& _index) const {
  if (elements_.size() == 0) {
    throw std::out_of_range("there are no elements in the DimensionalArray");
  }
  if (_index.size() != dimensionWidths_.size()) {
    throw std::out_of_range("'index' size must match number of dimensions");
  }

  u32 element = 0;
  for (s32 i = _index.size() - 1; i >= 0; --i) {
    if (_index.at(i) >= dimensionWidths_.at(i)) {
      throw std::out_of_range("'index' out of bounds");
    }
    u32 dimScale = dimensionScale_.at(i);
    u32 dimIndex = _index.at(i);
    element += dimScale * dimIndex;
  }
  return element;
}

template <typename T>
T& DimensionalArray<T>::at(const std::vector<u32>& _index) {
  return elements_.at(index(_index));
}

template <typename T>
T& DimensionalArray<T>::at(u32 _index) {
  return elements_.at(_index);
}

template <typename T>
const T& DimensionalArray<T>::at(u32 _index) const {
  return elements_.at(_index);
}

/*

  18
  z - 111111222222333333 - 3 - 6
  y - 111222111222111222 - 2 - 3
  x - 123123123123123123 - 3 - 1

 */
