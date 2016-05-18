/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_INTERFACE_H_
#define INTERFACE_INTERFACE_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <vector>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/FlitReceiver.h"
#include "types/Control.h"
#include "types/ControlReceiver.h"
#include "types/Message.h"
#include "types/MessageReceiver.h"
#include "network/Channel.h"

class PacketReassembler;
class MessageReassembler;

class Interface : public Component, public FlitReceiver,
                  public ControlReceiver, public MessageReceiver {
 public:
  Interface(const std::string& _name, const Component* _parent, u32 _id,
            Json::Value _settings);
  virtual ~Interface();
  u32 getId() const;
  u32 numVcs() const;
  void setMessageReceiver(MessageReceiver* _receiver);
  MessageReceiver* getMessageReceiver() const;
  virtual void setInputChannel(Channel* _channel) = 0;
  virtual void setOutputChannel(Channel* _channel) = 0;

 protected:
  const u32 id_;
  const u32 numVcs_;

 private:
  MessageReceiver* messageReceiver_;
  std::vector<PacketReassembler*> packetReassemblers_;
  MessageReassembler* messageReassembler_;
};

#endif  // INTERFACE_INTERFACE_H_
