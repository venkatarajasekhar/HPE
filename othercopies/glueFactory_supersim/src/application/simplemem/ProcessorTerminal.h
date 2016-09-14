/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SIMPLEMEM_PROCESSORTERMINAL_H_
#define APPLICATION_SIMPLEMEM_PROCESSORTERMINAL_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "application/Terminal.h"

namespace SimpleMem {

class ProcessorTerminal : public Terminal {
 public:
  ProcessorTerminal(const std::string& _name, const Component* _parent,
                    u32 _id, std::vector<u32> _address, Json::Value _settings);
  ~ProcessorTerminal();
  void processEvent(void* _event, s32 _type) override;
  void handleMessage(Message* _message) override;
  void messageEnteredInterface(Message* _message) override;
  void messageExitedNetwork(Message* _message) override;
  f64 percentComplete() const;

 private:
  enum class eState {kProcessing, kWaitingForReadResp, kWaitingForWriteResp,
      kDone};

  void startProcessing();
  void startNextMemoryAccess();

  u32 totalMemory_;
  u32 memorySlice_;
  u32 blockSize_;

  u32 latency_;
  u32 numMemoryAccesses_;
  u32 remainingAccesses_;

  eState fsm_;
};

}  // namespace SimpleMem

#endif  // APPLICATION_SIMPLEMEM_PROCESSORTERMINAL_H_
