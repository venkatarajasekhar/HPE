/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

#include <json/json.h>
#include <string>
#include <vector>

class Settings {
 public:
  static void initFile(const char* _configFile, Json::Value* _settings);
  static void initString(const char* _config, Json::Value* _settings);
  static std::string toString(const Json::Value* _settings);
  static void update(Json::Value* _settings,
                     const std::vector<std::string>& _updates);
};

#endif  // SETTINGS_SETTINGS_H_
