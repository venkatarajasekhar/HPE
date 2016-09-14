/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_POEM_MESSAGEREASSEMBLER_H_
#define INTERFACE_POEM_MESSAGEREASSEMBLER_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "event/Component.h"
#include "types/Packet.h"
#include "types/Message.h"

namespace Poem {

class MessageReassembler : public Component {
 public:
  MessageReassembler(const std::string& _name, const Component* _parent);
  ~MessageReassembler();
  Message* receivePacket(Packet* _packet);

 private:
  struct MessageData {
    Message* message;
    std::vector<bool> packetsReceived;
    u32 receivedCount;
  };
  std::unordered_map<u64, MessageData> messages_;
};

}  // namespace Poem

#endif  // INTERFACE_POEM_MESSAGEREASSEMBLER_H_
