/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SINGLESTREAM_TERMINAL_H_
#define APPLICATION_SINGLESTREAM_TERMINAL_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "traffic/TrafficPattern.h"
#include "application/Terminal.h"

namespace SingleStream {

class Application;

class Terminal : public ::Terminal {
 public:
  Terminal(const std::string& _name, const Component* _parent,
           u32 _id, std::vector<u32> _address, Application* _app,
           Json::Value _settings);
  ~Terminal();
  void processEvent(void* _event, s32 _type) override;
  void handleMessage(Message* _message) override;
  void messageEnteredInterface(Message* _message) override;
  void messageExitedNetwork(Message* _message) override;
  f64 percentComplete() const;

 private:
  void sendNextMessage();

  Application* app_;

  u32 numMessages_;
  u32 remainingMessages_;
  u32 minMessageSize_;  // flits
  u32 maxMessageSize_;  // flits
  u32 maxPacketSize_;  // flits
  u32 recvdMessages_;

  // message generator
  u64 lastSendTime_;
};

}  // namespace SingleStream

#endif  // APPLICATION_SINGLESTREAM_TERMINAL_H_
