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
#ifndef PRIM_PRIM_H_
#define PRIM_PRIM_H_

#include <cstdint>
#include <climits>
#include <limits>

typedef std::int8_t s8;
const s8 S8_MIN = INT8_MIN;
const s8 S8_MAX = INT8_MAX;

typedef std::uint8_t u8;
const u8 U8_MAX = UINT8_MAX;

typedef std::int16_t s16;
const s16 S16_MIN = INT16_MIN;
const s16 S16_MAX = INT16_MAX;

typedef std::uint16_t u16;
const u16 U16_MAX = UINT16_MAX;

typedef std::int32_t s32;
const s32 S32_MIN = INT32_MIN;
const s32 S32_MAX = INT32_MAX;

typedef std::uint32_t u32;
const u32 U32_MAX = UINT32_MAX;

typedef std::int64_t s64;
const s64 S64_MIN = INT64_MIN;
const s64 S64_MAX = INT64_MAX;

typedef std::uint64_t u64;
const u64 U64_MAX = UINT64_MAX;

typedef float f32;
const f32 F32_EPS = std::numeric_limits<f32>::epsilon();
const f32 F32_MIN = std::numeric_limits<f32>::min();
const f32 F32_MAX = std::numeric_limits<f32>::max();
const f32 F32_NAN = std::numeric_limits<f32>::quiet_NaN();
const f32 F32_POS_INF = std::numeric_limits<f32>::infinity();
const f32 F32_NEG_INF = -1.0 * F32_POS_INF;

typedef double f64;
const f64 F64_EPS = std::numeric_limits<f64>::epsilon();
const f64 F64_MIN = std::numeric_limits<f64>::min();
const f64 F64_MAX = std::numeric_limits<f64>::max();
const f64 F64_NAN = std::numeric_limits<f64>::quiet_NaN();
const f64 F64_POS_INF = std::numeric_limits<f64>::infinity();
const f64 F64_NEG_INF = -1.0 * F64_POS_INF;

#endif  // PRIM_PRIM_H_
