/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_STRESSTEST_BLASTTERMINAL_H_
#define APPLICATION_STRESSTEST_BLASTTERMINAL_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "traffic/TrafficPattern.h"
#include "application/Terminal.h"

namespace StressTest {

class Application;

class BlastTerminal : public Terminal {
 public:
  BlastTerminal(const std::string& _name, const Component* _parent,
                u32 _id, std::vector<u32> _address,
                Application* _app, Json::Value _settings);
  ~BlastTerminal();
  void processEvent(void* _event, s32 _type) override;
  void handleMessage(Message* _message) override;
  void messageEnteredInterface(Message* _message) override;
  void messageExitedNetwork(Message* _message) override;
  f64 percentComplete() const;
  void startLogging();
  void stopSending();

 private:
  void sendNextMessage();

  Application* app_;
  TrafficPattern* trafficPattern_;

  u32 numMessages_;
  u32 remainingMessages_;
  u32 minMessageSize_;  // flits
  u32 maxMessageSize_;  // flits
  u32 maxPacketSize_;  // flits

  // warmup/saturation detector
  bool warmupEnable_;
  u32 warmupInterval_;  // messages received
  u32 warmupFlitsReceived_;
  u32 warmupWindow_;
  u32 maxWarmupAttempts_;
  u32 warmupAttempts_;
  std::vector<u64> enrouteSampleTimes_;
  std::vector<u64> enrouteSampleValues_;
  u32 enrouteSamplePos_;
  u32 fastFailSample_;

  // message generator
  u64 lastSendTime_;

  // logging and message generation
  bool enableLogging_;
  bool enableSending_;
  std::unordered_set<u32> messagesToLog_;
  u32 loggableEnteredCount_;
  u32 loggableExitedCount_;
};

}  // namespace StressTest

#endif  // APPLICATION_STRESSTEST_BLASTTERMINAL_H_
