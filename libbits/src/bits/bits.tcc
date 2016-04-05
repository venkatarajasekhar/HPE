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
#ifndef BITS_BITS_H_
#error "do not include this file, use the .h instead"
#else  // BITS_BITS_H_

#include <cassert>

namespace bits {

template <typename T>
T pow2(T _uint) {
  assert(_uint < (sizeof(T) * CHAR_BIT));
  return (T)1 << _uint;
}

template <typename T>
bool isPow2(T _uint) {
  return _uint && !(_uint & (_uint - (T)1));
}

template <typename T>
T floorLog2(T _uint) {
  T r = 0;
  while (_uint >>= (T)1) {
    r++;
  }
  return r;
}

template <typename T>
T ceilLog2(T _uint) {
  // top end bail out
  T e = sizeof(T) * CHAR_BIT;
  if (_uint >= ((T)1 << (e - 1))) {
    return e;
  }

  // all but the highest case
  e = 0;
  while (((T)1 << e) < _uint) {
    e++;
  }
  return e;
}

template <typename T>
T reverse(T _v, u8 _bits) {
  const u8 MAX_BITS = sizeof(T) * CHAR_BIT;
  assert(_bits <= MAX_BITS);
  if (_bits != MAX_BITS) {
    assert(_v < pow2<T>(_bits));
  }

  // modified from: http://graphics.stanford.edu/~seander/bithacks.html
  T r = _v;
  T s = MAX_BITS - 1;
  for (_v >>= 1; _v; _v >>= 1) {
    r <<= 1;
    r |= _v & 1;
    s--;
  }
  r <<= s;  // shift when v's highest bits are zero
  r >>= MAX_BITS - _bits;  // limit bit range
  return r;
}

template <typename T>
T rotateRight(T _v, u8 _bits) {
  const u8 MAX_BITS = sizeof(T) * CHAR_BIT;
  assert(_bits <= MAX_BITS);
  if (_bits != MAX_BITS) {
    assert(_v < pow2<T>(_bits));
  }

  T w = _v & 0x1;  // detect wrap
  T r = (_v >> 1) | (w << (_bits - 1));
  return r;
}

template <typename T>
T rotateLeft(T _v, u8 _bits) {
  const u8 MAX_BITS = sizeof(T) * CHAR_BIT;
  assert(_bits <= MAX_BITS);
  if (_bits != MAX_BITS) {
    assert(_v < pow2<T>(_bits));
  }

  T t = 1 << (_bits - 1);  // find top bit location
  bool w = _v & t;  // detect wrap
  T r = ((_v << 1) & ((1 << _bits) - 1)) | w;
  return r;
}

}  // namespace bits

#endif  // BITS_BITS_H_
