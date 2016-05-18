/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef STATS_RATELOG_H_
#define STATS_RATELOG_H_

#include <json/json.h>
#include <prim/prim.h>

#include <sstream>
#include <string>

#include "stats/FileLog.h"

class RateLog : public FileLog {
 public:
  explicit RateLog(Json::Value _settings);
  ~RateLog();
  void logRates(u32 _terminalId, const std::string& _terminalName,
                f64 _supplyRate, f64 _injectionRate, f64 _ejectionRate);

 private:
  std::stringstream ss_;
};

#endif  // STATS_RATELOG_H_
