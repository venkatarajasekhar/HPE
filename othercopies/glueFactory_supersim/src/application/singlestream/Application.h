/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SINGLESTREAM_APPLICATION_H_
#define APPLICATION_SINGLESTREAM_APPLICATION_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "application/Application.h"

namespace SingleStream {

class Application : public ::Application {
 public:
  Application(const std::string& _name, const Component* _parent,
              Json::Value _settings);
  ~Application();
  f64 percentComplete() const override;
  u32 getSource() const;
  u32 getDestination() const;
  void receivedFirst(u32 _id);
  void sentLast(u32 _id);

 private:
  u32 sourceTerminal_;
  u32 destinationTerminal_;
  bool doMonitoring_;
};

}  // namespace SingleStream

#endif  // APPLICATION_SINGLESTREAM_APPLICATION_H_
