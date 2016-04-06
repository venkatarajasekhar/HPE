/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef STATS_MESSAGELOG_H_
#define STATS_MESSAGELOG_H_

#include <json/json.h>

#include "stats/FileLog.h"
#include "types/Message.h"

class MessageLog : public FileLog {
 public:
  explicit MessageLog(Json::Value _settings);
  ~MessageLog();
  void logMessage(const Message* _message);
};

#endif  // STATS_MESSAGELOG_H_
