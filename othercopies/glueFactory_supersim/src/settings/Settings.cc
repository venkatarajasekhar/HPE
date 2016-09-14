/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "settings/Settings.h"

#include <strop/strop.h>

#include <cassert>
#include <cstdio>
#include <fstream>  // NOLINT
#include <sstream>

void Settings::initFile(const char* _configFile, Json::Value* _settings) {
  // create an ifstream for the JSON reader
  std::ifstream is(_configFile, std::ifstream::binary);
  if (!is) {
    fprintf(stderr, "Settings error: could not open file '%s'\n",
            _configFile);
    exit(-1);
  }

  // read in the config file
  Json::Reader reader;
  bool success = reader.parse(is, *_settings, true);
  is.close();

  if (!success) {
    fprintf(stderr, "Settings error: failed to parse JSON file '%s'\n%s\n",
            _configFile, reader.getFormattedErrorMessages().c_str());
    exit(-1);
  }
}

void Settings::initString(const char* _config, Json::Value* _settings) {
  // read in the config file
  Json::Reader reader;
  bool success = reader.parse(_config, *_settings, true);
  if (!success) {
    fprintf(stderr, "Settings error: failed to parse JSON string:\n%s\n%s\n",
            _config, reader.getFormattedErrorMessages().c_str());
    exit(-1);
  }
}

std::string Settings::toString(const Json::Value* _settings) {
  Json::StyledWriter writer;
  std::stringstream ss;
  ss << writer.write(*_settings);
  return ss.str();
}

void Settings::update(Json::Value* _settings,
                      const std::vector<std::string>& _updates) {
  for (auto it = _updates.cbegin(); it != _updates.cend(); ++it) {
    const std::string& overwrite = *it;

    u64 equals1 = overwrite.find_first_of('=');
    u64 equals2 = overwrite.find_last_of('=');
    if ((equals1 == std::string::npos) ||
        (equals2 == std::string::npos) ||
        (equals2 <= equals1 + 1)) {
      fprintf(stderr, "invalid setting overwrite spec: %s\n",
              overwrite.c_str());
      assert(false);
    }

    std::string pathStr = overwrite.substr(0, equals1);
    std::string varType = overwrite.substr(equals1 + 1,
                                           equals2 - equals1 - 1);
    std::string valueStr = overwrite.substr(equals2 + 1);

    Json::Path path(pathStr);
    Json::Value& setting = path.make(*_settings);
    if (varType == "int") {
      setting = Json::Value(std::stoll(valueStr));
    } else if (varType == "uint") {
      setting = Json::Value(std::stoull(valueStr));
    } else if (varType == "float") {
      setting = Json::Value(std::stod(valueStr));
    } else if (varType == "string") {
      setting = Json::Value(valueStr);
    } else if (varType == "bool") {
      if (valueStr == "true") {
        setting = Json::Value(true);
      } else if (valueStr == "false") {
        setting = Json::Value(false);
      } else {
        fprintf(stderr, "invalid bool: %s\n", valueStr.c_str());
        assert(false);
      }
    } else {
      fprintf(stderr, "invalid setting type: %s\n", varType.c_str());
      assert(false);
    }
  }
}
