/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "stats/ChannelLog.h"

ChannelLog::ChannelLog(Json::Value _settings)
    : FileLog(_settings) {
  write("name,utilization\n");
  ss_.precision(6);
  ss_.setf(std::ios::fixed, std::ios::floatfield);
}

ChannelLog::~ChannelLog() {}

void ChannelLog::logChannel(const Channel* _channel) {
  ss_ << _channel->fullName() << ',' << _channel->utilization() << std::endl;
  write(ss_.str());
  ss_.str("");
  ss_.clear();
}
