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
#ifndef FIO_INFILE_H_
#define FIO_INFILE_H_

#include <prim/prim.h>
#include <zlib.h>

#include <sstream>
#include <string>

namespace fio {

const char kDefaultDelim = '\n';
const u64 kDefaultBlockSize = 8192;

class InFile {
 public:
  enum class Status : u8 {OK, END, ERROR};

  InFile(const char* _filepath,
         char _delim = kDefaultDelim,
         u64 _blockSize = kDefaultBlockSize);
  InFile(const std::string& _filepath,
         char _delim = kDefaultDelim,
         u64 _blockSize = kDefaultBlockSize);
  virtual ~InFile();

  bool compressed() const;
  Status getLine(std::string* _line);

 private:
  bool error_;
  bool compress_;
  bool stdin_;
  FILE* regFile_;
  gzFile gzFile_;
  const char delim_;
  const u64 blockSize_;
  char* buf_;
  u64 count_;  // chars in buffer
  u64 start_;  // beginning of line
  u64 next_;  // char after line
  bool eof_;
  std::stringstream ss_;
};

}  // namespace fio

#endif  // FIO_INFILE_H_
