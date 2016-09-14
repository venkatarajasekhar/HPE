/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_POEM_INTERFACE_H_
#define INTERFACE_POEM_INTERFACE_H_

#include <json/json.h>
#include <string>
#include <tuple>
#include <vector>

#include "interface/Interface.h"
#include "network/Channel.h"
#include "router/common/Crossbar.h"
#include "router/common/CrossbarScheduler.h"
#include "types/Control.h"
#include "types/ControlReceiver.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"
#include "types/Message.h"
#include "types/MessageReceiver.h"

namespace Poem {

class InputQueue;
class Ejector;
class PacketReassembler;
class MessageReassembler;

class Interface : public ::Interface {
 public:
  Interface(const std::string& _name, const Component* _parent, u32 _id,
            Json::Value _settings);
  ~Interface();
  void setInputChannel(Channel* _channel) override;
  void setOutputChannel(Channel* _channel) override;
  void receiveMessage(Message* _message) override;
  void receiveFlit(u32 _port, Flit* _flit) override;
  void receiveControl(u32 _port, Control* _control) override;
  void sendFlit(Flit* _flit);

 private:
  Channel* inputChannel_;
  Channel* outputChannel_;

  std::vector<InputQueue*> inputQueues_;
  Crossbar* crossbar_;
  CrossbarScheduler* crossbarScheduler_;
  Ejector* ejector_;

  std::vector<PacketReassembler*> packetReassemblers_;
  MessageReassembler* messageReassembler_;
};

}  // namespace Poem

#endif  // INTERFACE_POEM_INTERFACE_H_
