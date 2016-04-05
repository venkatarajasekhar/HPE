/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/simplemem/MemoryOp.h"

namespace SimpleMem {

MemoryOp::MemoryOp(MemoryOp::eOp _op, u32 _address)
    : MemoryOp(_op, _address, 0) {}

MemoryOp::MemoryOp(MemoryOp::eOp _op, u32 _address, u32 _blockSize)
    : op_(_op), address_(_address), blockSize_(_blockSize), block_(nullptr) {
  if (_blockSize > 0) {
    block_ = new u8[_blockSize];
  }
}

MemoryOp::~MemoryOp() {
  if (block_ != nullptr) {
    delete[] block_;
  }
}

MemoryOp::eOp MemoryOp::op() const {
  return op_;
}

u32 MemoryOp::address() const {
  return address_;
}

u32 MemoryOp::blockSize() const {
  return blockSize_;
}

u8* MemoryOp::block() const {
  return block_;
}

}  // namespace SimpleMem
