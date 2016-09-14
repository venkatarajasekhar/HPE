/*
 * Copyright (c) 2016, Nic McDonald
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
#include "fio/InFile.h"

#include <prim/prim.h>

#include <cassert>

namespace fio {

InFile::InFile(const char* _filepath, char _delim, u64 _blockSize)
    : InFile(std::string(_filepath), _delim, _blockSize) {}

InFile::InFile(const std::string& _filepath, char _delim, u64 _blockSize)
    : delim_(_delim), blockSize_(_blockSize) {
  u64 namelen = _filepath.size();
  assert(namelen > 0);
  compress_ = _filepath.substr(namelen-3) == ".gz";

  error_ = false;
  if (compress_) {
    gzFile_ = gzopen(_filepath.c_str(), "rb");
    error_ = (gzFile_ == nullptr);
  } else {
    stdin_ = _filepath == "-";
    if (stdin_) {
      regFile_ = stdin;
    } else {
      regFile_ = fopen(_filepath.c_str(), "rb");
    }
    error_ = (regFile_ == nullptr);
  }

  buf_ = new char[blockSize_ + 1];
  buf_[blockSize_] = '\0';
  count_ = 0;
  start_ = 0;
  next_ = 0;
  eof_ = false;
}

InFile::~InFile() {
  if (compress_) {
    gzclose(gzFile_);
  } else if (!stdin_) {
    fclose(regFile_);
  }
  delete[] buf_;
}

bool InFile::compressed() const {
  return compress_;
}

InFile::Status InFile::getLine(std::string* _line) {
  if (eof_) {
    return Status::END;
  }

  assert(count_ < blockSize_);

  // read until full or delim found
  u64 end = U64_MAX;
  bool done = false;
  while (!done) {
    // search for the delimiter in the buffer
    for (u64 cnt = 0; cnt < count_; cnt++) {
      u64 idx = (start_ + cnt) % blockSize_;
      if (buf_[idx] == delim_) {
        buf_[idx] = '\0';
        end = idx;
        done = true;
        break;
      }
    }
    if (done || eof_) {
      break;
    }

    // determine how much data to ask for
    u64 ask = (next_ < start_) ? (blockSize_ - count_) : (blockSize_ - next_);
    assert(ask > 0);

    // read data from file
    u64 read;
    if (compress_) {
      s64 bytesRead = gzread(gzFile_, &buf_[next_], ask);
      assert(bytesRead > 0 || (bytesRead == 0 && gzeof(gzFile_)));
      read = (u64)bytesRead;
    } else {
      u64 bytesRead = fread(&buf_[next_], 1, ask, regFile_);
      assert(bytesRead > 0 || feof(regFile_));
      read = bytesRead;
    }

    // handle buffered data
    if (read == 0) {
      eof_ = true;
      buf_[next_] = '\n';
      count_++;
      done = count_ == 0;
    } else {
      // advance buffer
      count_ += read;
      next_ = (next_ + read) % blockSize_;
    }
  }

  // copy out string
  if (end == U64_MAX && eof_ == true) {
    return Status::END;
  } else {
    assert(end < blockSize_);
    ss_.str("");
    ss_ << &buf_[start_];
    if (end < start_) {
      ss_ << &buf_[0];
    }
    _line->clear();
    *_line = ss_.str();
    count_ -= (_line->size() + 1);
    start_ = (end + 1) % blockSize_;
    return Status::OK;
  }
}

}  // namespace fio
