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
#include "fio/OutFile.h"

#include <prim/prim.h>

#include <cassert>

namespace fio {

OutFile::OutFile(const char* _filepath)
    : OutFile(std::string(_filepath)) {}

OutFile::OutFile(const std::string& _filepath) {
  u64 namelen = _filepath.size();
  assert(namelen > 0);
  compress_ = _filepath.substr(namelen-3) == ".gz";

  error_ = false;
  if (compress_) {
    gzFile_ = gzopen(_filepath.c_str(), "wb");
    error_ = (gzFile_ == nullptr);
  } else {
    regFile_ = fopen(_filepath.c_str(), "wb");
    error_ = (regFile_ == nullptr);
  }
}

OutFile::~OutFile() {
  if (compress_) {
    gzclose(gzFile_);
  } else {
    fclose(regFile_);
  }
}

bool OutFile::compressed() const {
  return compress_;
}

void OutFile::write(const std::string& _text) {
  const void* cstr = reinterpret_cast<const void*>(_text.c_str());
  size_t len = _text.size();

  if (compress_) {
    assert(gzwrite(gzFile_, cstr, len) == (s64)len);
  } else {
    assert(fwrite(cstr, sizeof(char), len, regFile_) == len);
  }
}

}  // namespace fio
