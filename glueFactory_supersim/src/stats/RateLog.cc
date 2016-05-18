/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "stats/RateLog.h"

RateLog::RateLog(Json::Value _settings)
    : FileLog(_settings) {
  write("id,name,supply,injection,ejection\n");
  ss_.precision(6);
  ss_.setf(std::ios::fixed, std::ios::floatfield);
}

RateLog::~RateLog() {}

void RateLog::logRates(u32 _terminalId, const std::string& _terminalName,
                       f64 _supplyRate, f64 _injectionRate, f64 _ejectionRate) {
  ss_ << _terminalId << ',' << _terminalName << ',' << _supplyRate << ',' <<
      _injectionRate << ',' << _ejectionRate << '\n';
  write(ss_.str());
  ss_.str("");
  ss_.clear();
}
