/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SIMPLEMEM_MEMORYTERMINAL_H_
#define APPLICATION_SIMPLEMEM_MEMORYTERMINAL_H_

#include <json/json.h>
#include <prim/prim.h>

#include <queue>
#include <string>
#include <vector>

#include "event/Component.h"
#include "application/Terminal.h"

namespace SimpleMem {

class MemoryTerminal : public Terminal {
 public:
  MemoryTerminal(const std::string& _name, const Component* _parent,
                 u32 _id, std::vector<u32> _address, u32 _memorySlice,
                 Json::Value _settings);
  ~MemoryTerminal();
  void processEvent(void* _event, s32 _type) override;
  void handleMessage(Message* _message) override;
  void messageEnteredInterface(Message* _message) override;
  void messageExitedNetwork(Message* _message) override;

 private:
  enum class eState {kWaiting, kAccessing};

  void startMemoryAccess();
  void sendMemoryResponse();

  u32 memoryOffset_;
  u8* memory_;
  u32 latency_;

  std::queue<Message*> messages_;
  eState fsm_;
};

}  // namespace SimpleMem

#endif  // APPLICATION_SIMPLEMEM_MEMORYTERMINAL_H_
