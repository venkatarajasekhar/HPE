/*
 * Copyright (c) 2012-2015, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef RND_QUEUE_H_
#error "Do not include this .tcc file directly, use the .h file instead"
#else

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

namespace rnd {

template <typename T>
Queue<T>::Queue(Random* _random)
    : random_(_random) {}

template <typename T>
Queue<T>::~Queue() {}

template <typename T>
void Queue<T>::add(T _item) {
  values_.insert(_item);
}

template <typename T>
void Queue<T>::add(T _start, T _stop) {
  if (_stop >= _start) {
    T current = _start;
    while (true) {
      bool last = (current == _stop);
      values_.insert(current);
      current++;
      if (last) {
        break;
      }
    }
  }
}

template <typename T>
void Queue<T>::add(const std::vector<T>& _values) {
  for (auto it = _values.cbegin(); it != _values.cend(); ++it) {
    values_.insert(*it);
  }
}

template <typename T>
void Queue<T>::add(const std::set<T>& _values) {
  for (auto it = _values.cbegin(); it != _values.cend(); ++it) {
    values_.insert(*it);
  }
}

template <typename T>
void Queue<T>::clear() {
  values_.clear();
}

template <typename T>
size_t Queue<T>::size() const {
  return values_.size();
}

template <typename T>
T Queue<T>::pop() {
  u64 idx = random_->nextU64(0, values_.size() - 1);
  auto it = values_.begin();
  std::advance(it, idx);
  T val = *it;
  values_.erase(it);
  return val;
}

template <typename T>
u64 Queue<T>::erase(T _item) {
  return values_.erase(_item);
}

}  // namespace rnd

#endif  // COMMON_RANDOM_RANDOMSET_H_
