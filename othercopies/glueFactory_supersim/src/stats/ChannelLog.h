/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef STATS_CHANNELLOG_H_
#define STATS_CHANNELLOG_H_

#include <json/json.h>
#include <prim/prim.h>

#include <sstream>
#include <string>

#include "network/Channel.h"
#include "stats/FileLog.h"

class ChannelLog : public FileLog {
 public:
  explicit ChannelLog(Json::Value _settings);
  ~ChannelLog();
  void logChannel(const Channel* _channel);

 private:
  std::stringstream ss_;
};

#endif  // STATS_CHANNELLOG_H_
