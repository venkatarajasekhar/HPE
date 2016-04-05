/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "stats/FileLog.h"

#include <prim/prim.h>

#include <cassert>
#include <cstdio>

FileLog::FileLog(Json::Value _settings) {
  std::string filename = _settings["file"].asString();
  u64 namelen = filename.size();
  assert(namelen > 0);
  compress_ = filename.substr(namelen-3) == ".gz";

  bool fail;
  if (compress_) {
    gzFile_ = gzopen(filename.c_str(), "wb");
    fail = (gzFile_ == nullptr);
  } else {
    regFile_ = fopen(filename.c_str(), "wb");
    fail = (regFile_ == nullptr);
  }

  if (fail) {
    fprintf(stderr, "ERROR: couldn't open file '%s'\n", filename.c_str());
    exit(-1);
  }
}

FileLog::~FileLog() {
  if (compress_) {
    gzclose(gzFile_);
  } else {
    fclose(regFile_);
  }
}

void FileLog::write(const std::string& _text) {
  const void* cstr = reinterpret_cast<const void*>(_text.c_str());
  size_t len = _text.size();

  if (compress_) {
    assert(gzwrite(gzFile_, cstr, len) == (s64)len);
  } else {
    assert(fwrite(cstr, sizeof(char), len, regFile_) == len);
  }
}
