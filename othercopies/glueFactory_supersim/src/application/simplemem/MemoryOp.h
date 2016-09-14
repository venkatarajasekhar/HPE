/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SIMPLEMEM_MEMORYOP_H_
#define APPLICATION_SIMPLEMEM_MEMORYOP_H_

#include <prim/prim.h>

#include "application/simplemem/MemoryOp.h"

namespace SimpleMem {

class MemoryOp {
 public:
  enum class eOp {kReadReq, kReadResp, kWriteReq, kWriteResp};

  MemoryOp(eOp _op, u32 _address);
  MemoryOp(eOp _op, u32 _address, u32 _blockSize);
  ~MemoryOp();

  eOp op() const;
  u32 address() const;
  u32 blockSize() const;
  u8* block() const;

 private:
  eOp op_;
  u32 address_;
  u32 blockSize_;
  u8* block_;
};

}  // namespace SimpleMem

#endif  // APPLICATION_SIMPLEMEM_MEMORYOP_H_
