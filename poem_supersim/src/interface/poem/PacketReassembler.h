/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef INTERFACE_POEM_PACKETREASSEMBLER_H_
#define INTERFACE_POEM_PACKETREASSEMBLER_H_

#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "types/Flit.h"
#include "types/Packet.h"

namespace Poem {

class PacketReassembler : public Component {
 public:
  PacketReassembler(const std::string& _name, const Component* _parent);
  ~PacketReassembler();
  Packet* receiveFlit(Flit* _flit);

 private:
  u32 expSourceId_;
  u32 expPacketId_;
  u32 expFlitId_;
};

}  // namespace Poem

#endif  // INTERFACE_POEM_PACKETREASSEMBLER_H_
