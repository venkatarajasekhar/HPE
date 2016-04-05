/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef STATS_FILELOG_H_
#define STATS_FILELOG_H_

#include <json/json.h>
#include <zlib.h>

#include <string>

class FileLog {
 public:
  explicit FileLog(Json::Value _settings);
  virtual ~FileLog();

  void write(const std::string& _text);

 private:
  bool compress_;
  FILE* regFile_;
  gzFile gzFile_;
};

#endif  // STATS_FILELOG_H_
